Segurança
=========

Autenticação e Firewalls (ou seja, obter as Credenciais do Usuário)
-------------------------------------------------------------------

Você pode configurar o Symfony para autenticar os usuários usando qualquer método que
desejar e carregar as informações do usuário a partir de qualquer fonte. Esse é um tema complexo,
mas a `Seção sobre Segurança do Cookbook`_ possui muitas informações sobre isso.

Independentemente das suas necessidades, a autenticação é configurada no ``security.yml``,
principalmente sob a chave ``firewalls``.

.. best-practice::

    A menos que você tenha dois sistemas de autenticação e usuários legitimamente diferentes
    (por exemplo, formulário de login para o site principal e um sistema de token somente para sua
    API), recomendamos ter apenas *uma* entrada firewall com a chave ``anonymous``
    habilitada.

A maioria das aplicações tem somente um sistema de autenticação e um conjunto de usuários.
Por essa razão, você só precisa de *uma* entrada firewall. Há exceções
claro, especialmente se você tiver seções web e API separadas no seu
site. Mas o ponto é manter as coisas simples.

Além disso, você deve usar a chave ``anonymous`` no seu firewall. Se
você precisa exigir que os usuários estejam logados em diferentes seções do seu
site (ou talvez quase *todas* as seções), use a área ``access_control``.

.. best-practice::

    Use o encoder ``bcrypt`` para codificar as senhas de seus usuários.

Se os usuários tiverem uma senha, então recomendamos codificá-la usando o encoder ``bcrypt``
em vez do codificador de hash tradicional SHA-512. As principais vantagens
do ``bcrypt`` são a inclusão de um valor *salt* para proteger contra ataques
de rainbow table, e a sua natureza adaptativa, que permite tornar mais lenta para
permanecer resistente a ataques de força bruta.

Com isso em mente, aqui está a configuração de autenticação da nossa aplicação,
que utiliza um formulário de login para carregar usuários do banco de dados:

.. code-block:: yaml

    security:
        encoders:
            AppBundle\Entity\User: bcrypt

        providers:
            database_users:
                entity: { class: AppBundle:User, property: username }

        firewalls:
            secured_area:
                pattern: ^/
                anonymous: true
                form_login:
                    check_path: security_login_check
                    login_path: security_login_form

                logout:
                    path: security_logout
                    target: homepage

    # ... access_control exists, but is not shown here

.. tip::

    O código fonte para o nosso projeto contém comentários que explicam cada parte.

Autorização (ou seja, Negar Acesso)
-----------------------------------

O Symfony oferece várias maneiras para impor a autorização, incluindo a configuração ``access_control``
no `security.yml`_, a :ref:`anotação @Security <best-practices-security-annotation>`
e usar :ref:`isGranted <best-practices-directy-isGranted>` no serviço ``security.context``
diretamente.

.. best-practice::

    * Para proteger os padrões de URL gerais, use ``access_control``;
    * Sempre que possível, use a anotação ``@Security``;
    * Verifique a segurança diretamente no serviço ``security.context`` sempre
      que você tiver uma situação mais complexa.

Há também diferentes formas de centralizar a sua lógica de autorização, como
utilizar um voter de segurança personalizado ou ACL.

.. best-practice::

    * Para as restrições de granulação fina, defina um voter de segurança personalizado;
    * Para retringir o acesso a *qualquer* objeto para *qualquer * usuário através de uma interface administrativa,
      use o ACL do Symfony.

.. _best-practices-security-annotation:

A Anotação @Security
--------------------

Para controlar o acesso em uma base controlador-a-controlador, use a anotação
``@Security`` sempre que possível. É fácil de ler e é colocado de forma consistente
acima de cada ação.

Em nossa aplicação, você precisa do ``ROLE_ADMIN`` a fim de criar um novo post.
Usando ``@Security``, parecerá com:

.. code-block:: php

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;
    // ...

    /**
     * Displays a form to create a new Post entity.
     *
     * @Route("/new", name="admin_post_new")
     * @Security("has_role('ROLE_ADMIN')")
     */
    public function newAction()
    {
        // ...
    }

Usando expressões para restrições de segurança complexas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se a sua lógica de segurança for um pouco mais complexa, você pode usar uma `expressão`_
dentro de ``@Security``. No exemplo a seguir, um usuário só pode acessar o
controlador se o seu e-mail corresponde ao valor retornado pelo método
``getAuthorEmail`` do objeto ``Post``:

.. code-block:: php

    use AppBundle\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("user.getEmail() == post.getAuthorEmail()")
     */
    public function editAction(Post $post)
    {
        // ...
    }

Note que isso requer o uso do `ParamConverter`_, que automaticamente
consulta o objeto ``Post`` e coloca ele no argumento ``$post``. Isso
é o que torna possível utilizar a variável ``post`` na expressão.

Mas isso tem uma grande desvantagem: uma expressão em uma anotação não pode facilmente
ser reutilizada em outras partes da aplicação. Imagine que você deseja adicionar
um link em um template que somente será visto pelos autores. Agora, você terá
que repetir o código de expressão usando a sintaxe Twig:

.. code-block:: html+jinja

    {% if app.user and app.user.email == post.authorEmail %}
        <a href=""> ... </a>
    {% endif %}

A solução mais fácil - se a sua lógica for bastante simples - é adicionar um novo método,
na entidade ``Post``, que verifica se um determinado usuário é o seu autor:

.. code-block:: php

    // src/AppBundle/Entity/Post.php
    // ...

    class Post
    {
        // ...

        /**
         * Is the given User the author of this Post?
         *
         * @return bool
         */
        public function isAuthor(User $user = null)
        {
            return $user && $user->getEmail() == $this->getAuthorEmail();
        }
    }

Agora você pode reutilizar esse método tanto no template quanto na expressão de segurança:

.. code-block:: php

    use AppBundle\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("post.isAuthor(user)")
     */
    public function editAction(Post $post)
    {
        // ...
    }

.. code-block:: html+jinja

    {% if post.isAuthor(app.user) %}
        <a href=""> ... </a>
    {% endif %}

.. _best-practices-directy-isGranted:

Verificar as Permissões sem o @Security
---------------------------------------

O exemplo acima com o ``@Security`` só funciona porque nós estamos usando o
:ref:`ParamConverter <best-practices-paramconverter>`, que fornece à expressão
acesso a variável ``post``. Se você não usar isso, ou tem algum outro
caso de uso mais avançado, você sempre pode fazer a mesma verificação de segurança no PHP:

.. code-block:: php

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     */
    public function editAction($id)
    {
        $post = $this->getDoctrine()->getRepository('AppBundle:Post')
            ->find($id);

        if (!$post) {
            throw $this->createNotFoundException();
        }

        if (!$post->isAuthor($this->getUser())) {
            throw $this->createAccessDeniedException();
        }

        // ...
    }

Os Voters de Segurança
----------------------

Se a sua lógica de segurança é complexa e não pode ser centralizada num método
como ``isAuthor()``, você deve aproveitar os voters personalizados. Trata-se de uma ordem
de magnitude mais fácil do que `ACL's`_ e que fornecerá a flexibilidade que você precisa
em quase todos os casos.

Primeiro, crie uma classe voter. O exemplo a seguir mostra um voter que implementa
a mesma lógica do ``getAuthorEmail`` você usou acima:

.. code-block:: php

    namespace AppBundle\Security;

    use Symfony\Component\Security\Core\Authorization\Voter\AbstractVoter;
    use Symfony\Component\Security\Core\User\UserInterface;

    // AbstractVoter class requires Symfony 2.6 or higher version
    class PostVoter extends AbstractVoter
    {
        const CREATE = 'create';
        const EDIT   = 'edit';

        protected function getSupportedAttributes()
        {
            return array(self::CREATE, self::EDIT);
        }

        protected function getSupportedClasses()
        {
            return array('AppBundle\Entity\Post');
        }

        protected function isGranted($attribute, $post, $user = null)
        {
            if (!$user instanceof UserInterface) {
                return false;
            }

            if ($attribute === self::CREATE && in_array('ROLE_ADMIN', $user->getRoles(), true)) {
                return true;
            }

            if ($attribute === self::EDIT && $user->getEmail() === $post->getAuthorEmail()) {
                return true;
            }

            return false;
        }
    }

Para habilitar o voter de segurança na aplicação, defina um novo serviço:

.. code-block:: yaml

    # app/config/services.yml
    services:
        # ...
        post_voter:
            class:      AppBundle\Security\PostVoter
            public:     false
            tags:
               - { name: security.voter }

Agora, você pode usar o voter com a anotação ``@Security``:

.. code-block:: php

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("is_granted('edit', post)")
     */
    public function editAction(Post $post)
    {
        // ...
    }

Você também pode usar isso diretamente com o serviço ``security.context``, ou
através do atalho ainda mais fácil em um controller:

.. code-block:: php

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     */
    public function editAction($id)
    {
        $post = // query for the post ...

        if (!$this->get('security.context')->isGranted('edit', $post)) {
            throw $this->createAccessDeniedException();
        }
    }

Saiba mais
----------

O `FOSUserBundle`_, desenvolvido pela comunidade Symfony, adiciona suporte para um
sistema de usuário que utiliza banco de dados no Symfony2. Ele também lida com tarefas comuns como
registro de usuário e funcionalidade de esqueci senha.

Ative o `recurso Lembrar Me`_ para permitir que seus usuários continuem logados por
um longo período de tempo.

Ao fornecer suporte ao cliente, às vezes é necessário acessar a aplicação
como um *outro* usuário para que você possa reproduzir o problema. O Symfony fornece
a possibilidade de `personificar usuários`_.

Se a sua empresa utiliza um método de login de usuário que não é suportado pelo Symfony, você pode
desenvolver o `seu próprio provedor de usuário`_ e o `seu próprio provedor de autenticação`_.

.. _`Seção sobre Segurança do Cookbook`: http://symfony.com/doc/current/cookbook/security/index.html
.. _`security.yml`: http://symfony.com/doc/current/reference/configuration/security.html
.. _`ParamConverter`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`anotação @Security`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/security.html
.. _`security.yml`: http://symfony.com/doc/current/reference/configuration/security.html
.. _`voter de segurança`: http://symfony.com/doc/current/cookbook/security/voters_data_permission.html
.. _`Acces Control List`: http://symfony.com/doc/current/cookbook/security/acl.html
.. _`ACL's`: http://symfony.com/doc/current/cookbook/security/acl.html
.. _`expressão`: http://symfony.com/doc/current/components/expression_language/introduction.html
.. _`FOSUserBundle`: https://github.com/FriendsOfSymfony/FOSUserBundle
.. _`recurso Lembrar Me`: http://symfony.com/doc/current/cookbook/security/remember_me.html
.. _`personificar usuários`: http://symfony.com/doc/current/cookbook/security/impersonating_user.html
.. _`seu próprio provedor de usuário`: http://symfony.com/doc/current/cookbook/security/custom_provider.html
.. _`seu próprio provedor de autenticação`: http://symfony.com/doc/current/cookbook/security/custom_authentication_provider.html
