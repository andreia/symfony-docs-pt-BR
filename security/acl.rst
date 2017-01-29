.. index::
   single: Security; Access Control Lists (ACLs)

Listas de controle de acesso (ACLs)
===================================

Em aplicativos complexos, comumente existem o problema que as decisões de permitir
ou negar acesso não podem ser tomadas somente baseada no usuário (``Token``)
solicitando acesso, mas também deve levar em consideração o objeto de domínio
que está tendo o acesso solicitado. É aí que o sistema ACL entra em ação.

Imagine que está projetando um sistema de blog onde seus usuário podem comentar
os textos (posts) publicados. Agora, você deseja que um usuário possa editar
seus próprios comentários, mas não os comentários dos outros usuários. Além
disso, você como administrador deseja pode editar todos os comentários. Neste
cenário, ``Comment`` seria seu objeto de domínio ao qual você quer restringir
acesso. Você poderia usar várias abordagens para conseguir o mesmo resultado.
Duas dessas seriam:

- *Impor segurança em seus métodos*: Basicamente, isso significa que deverá
  manter referências em cada ``Comment`` de todos os usuários que têm acesso
  e depois comparar com o usuário ``Token`` solicitando acesso.
- *Impor segurança com perfis*: Nesta abordagem, você adicionaria um perfil
  para cada objeto ``Comment``, isto é, ``ROLE_COMMENT_1``, ``ROLE_COMMENT_2``, etc.

Ambas abordagens são perfeitamnete válidas. Elas, porém, amarram sua lógica
de autorização de acesso com seu código, deixando-o mais difícil de reusar
em outros contextos. Também aumenta a dificuldade de criar testes unitários.
Além disso, pode-se ter problemas de performance caso muitos usuários tenham
acesso a um único objeto de domínio.

Felizmente, há uma maneira melhor que veremos a seguir.

Configuração
------------

Agora, antes de realmente começarmos, precisamos fazer algumas configurações.
Primeiramente, precisamos configurar a conexão de banco de dados queo sistema ACL
utilizará.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            acl:
                connection: default

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <acl>
            <connection>default</connection>
        </acl>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', 'acl', array(
            'connection' => 'default',
        ));


.. note::

    O sistema ACL requer que ao menos uma conexão Doctrine DBAL esteja configurada.
    Isto, porém, não significa que você tem que utilizar o Doctrine para mapear
    seus objetos de domínio. Você pode utilizar qualquer mapeamento que quiser
    para seus objetos, seja ele Doctrine ORM, Mongo ODM, Propel, ou SQL puro.
    A escolha é sua.

Depois de configurar a conexão, temos que importar a estrutura do banco de dados.
Felizmente, temos um comando para isto. Rode o seguinte comando.

.. code-block:: text

    php app/console init:acl

Começando
---------

Voltando ao nosso pequeno exemplo do início, vamos implementar o sistema
ACL dele.

Criando uma ACL, e adicionando uma entrada (ACE)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: php

    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    use Symfony\Component\Security\Acl\Domain\ObjectIdentity;
    use Symfony\Component\Security\Acl\Domain\UserSecurityIdentity;
    use Symfony\Component\Security\Acl\Permission\MaskBuilder;
    // ...
    
    // BlogController.php
    public function addCommentAction(Post $post)
    {
        $comment = new Comment();

        // setup $form, and bind data
        // ...

        if ($form->isValid()) {
            $entityManager = $this->get('doctrine.orm.default_entity_manager');
            $entityManager->persist($comment);
            $entityManager->flush();

            // creating the ACL
            $aclProvider = $this->get('security.acl.provider');
            $objectIdentity = ObjectIdentity::fromDomainObject($comment);
            $acl = $aclProvider->createAcl($objectIdentity);

            // retrieving the security identity of the currently logged-in user
            $securityContext = $this->get('security.context');
            $user = $securityContext->getToken()->getUser();
            $securityIdentity = UserSecurityIdentity::fromAccount($user);

            // grant owner access
            $acl->insertObjectAce($securityIdentity, MaskBuilder::MASK_OWNER);
            $aclProvider->updateAcl($acl);
        }
    }

Há algumas importantes decisões de implementação neste trecho de código. Por
enquanto, gostaria de destacar duas.

Primeiro, note que o método ``->createAcl()`` não aceita objetos de domínio
diretamente, mas somente implementações de ``ObjectIdentityInterface``. Este
passo adicional permite que trabalhe com ACLs mesmo quando não tiver uma instância
do objeto de domínio disponível. Isto será extremamente útil se você quiser
verificar permissões para um grande número de objetos sem realmente criar os
objetos.

Outra parte interessante é a chamada ``->insertObjectAce()``. Em nosso exemplo,
estamos concedendo ao usuário que está autenticado permissão de proprietário
do objeto Comment. ``MaskBuilder::MASK_OWNER`` é uma máscara (integer bitmask)
pré-definida. Não se preocupe que MaskBuilder abstrai a maior parte dos detalhes
técnicos, mas saiba que utilizando esta técnica é possível armazenar muitas
permissões diferentes em apenas uma linha do banco de dados, o que significa
uma considerável melhora na performance.

.. tip::

    A ordem em que as entradas de controle (ACE) são checadas é importante.
    Como regra geral, você deve colocar as entradas mais específicas no início.

Verificando o acesso
~~~~~~~~~~~~~~~~~~~~

.. code-block:: php

    // BlogController.php
    public function editCommentAction(Comment $comment)
    {
        $securityContext = $this->get('security.context');

        // check for edit access
        if (false === $securityContext->isGranted('EDIT', $comment))
        {
            throw new AccessDeniedException();
        }

        // retrieve actual comment object, and do your editing here
        // ...
    }

Neste exemplo, verificamos se o usuário tem permissão de edição (``EDIT``).
Internamente, Symfony2 mapea a permissão para várias máscaras (integer bitmasks)
e verifica se o usuário tem alguma delas.

.. note::

    Você pode definir até 32 permissões base (dependendo do seu SO,
    pode variar entre 30 e 32). Você ainda pode definir permisões cumulativas.

Permissões Cumulativas
----------------------

No nosso primeiro exemplo acima, nós concedemos somente a permissão base ``OWNER``.
Apesar disso significar que o usuário pode executar qualquer operação no
objeto de domínio tais como exibir, editar, etc, em alguns casos você pode
querer conceder essas permissões explicitamente.

O ``MaskBuilder`` pode ser usado para criar máscaras (bit masks) facilmente através
da combinação de várias permissões base.

.. code-block:: php

    $builder = new MaskBuilder();
    $builder
        ->add('view')
        ->add('edit')
        ->add('delete')
        ->add('undelete')
    ;
    $mask = $builder->get(); // int(15)

Este inteiro (integer bitmask) pode então ser usado para conceder a um usuário
todas as permissões base que você adicionou acima.

.. code-block:: php

    $acl->insertObjectAce(new UserSecurityIdentity('johannes'), $mask);

O usuário agora poderá exibir, editar, deletar e desfazer a deleção dos objetos.
