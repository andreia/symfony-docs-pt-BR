.. index::
   single: Segurança; Voters para Permissão de Dados

Como Usar Voters para Verificar as Permissões do Usuário
========================================================

No Symfony2 você pode verificar a permissão para acessar dados usando o
:doc:`módulo ACL </cookbook/security/acl>`, entretanto ele pode ser demasiado
para muitas aplicações. Uma solução muito mais fácil é trabalhar com voters personalizados,
que são como instruções condicionais simples.

.. seealso::

    Os voters também podem ser utilizados de outras formas, como, por exemplo, para lista negra de
    endereços IP de toda a aplicação: :doc:`/cookbook/security/voters`.

.. tip::

    Verifique o capítulo
    :doc:`autorização </components/security/authorization>`
    para uma compreensão mais aprofundada sobre os voters.

Como o Symfony Utiliza os Voters
--------------------------------

Para utilizar os voters, você precisa entender como o Symfony trabalha com eles.
Todos os voters são chamados cada vez que você usa o método ``isGranted()`` no contexto
de segurança do Symfony (ou seja, o serviço ``security.context``). Cada um decide
se o usuário atual deve ter acesso a algum recurso.

Finalmente, o Symfony utiliza uma de três diferentes abordagens sobre o que fazer
com o feedback de todos os voters: afirmativa, consenso e unanimidade.

Para mais informações, leia
:ref:`a seção sobre gerentes de decisões acesso <components-security-access-decision-manager>`.

A Interface Voter
-----------------

Um voter personalizado deve implementar
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`,
que tem esta estrutura:

.. include:: /cookbook/security/voter_interface.rst.inc

Neste exemplo, o voter irá verificar se o usuário tem acesso a um objeto específico
de acordo com as suas condições personalizadas (ex., eles devem ser proprietários do
objeto). Se a condição falhar, você vai retornar
``VoterInterface::ACCESS_DENIED``, caso contrário você vai retornar
``VoterInterface::ACCESS_GRANTED``. No caso da responsabilidade por essa decisão
não pertencer a este voter, ele retornará ``VoterInterface::ACCESS_ABSTAIN``.

Criando o Voter Personalizado
-----------------------------

O objetivo é criar um voter que verifica se o usuário tem acesso para visualizar ou
editar um objeto particular. Aqui está um exemplo de implementação::

    // src/Acme/DemoBundle/Security/Authorization/Voter/PostVoter.php
    namespace Acme\DemoBundle\Security\Authorization\Voter;

    use Symfony\Component\Security\Core\Exception\InvalidArgumentException;
    use Symfony\Component\Security\Core\Authorization\Voter\VoterInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\User\UserInterface;

    class PostVoter implements VoterInterface
    {
        const VIEW = 'view';
        const EDIT = 'edit';

        public function supportsAttribute($attribute)
        {
            return in_array($attribute, array(
                self::VIEW,
                self::EDIT,
            ));
        }

        public function supportsClass($class)
        {
            $supportedClass = 'Acme\DemoBundle\Entity\Post';

            return $supportedClass === $class || is_subclass_of($class, $supportedClass);
        }

        /**
         * @var \Acme\DemoBundle\Entity\Post $post
         */
        public function vote(TokenInterface $token, $post, array $attributes)
        {
            // check if class of this object is supported by this voter
            if (!$this->supportsClass(get_class($post))) {
                return VoterInterface::ACCESS_ABSTAIN;
            }

            // check if the voter is used correct, only allow one attribute
            // this isn't a requirement, it's just one easy way for you to
            // design your voter
            if(1 !== count($attributes)) {
                throw new InvalidArgumentException(
                    'Only one attribute is allowed for VIEW or EDIT'
                );
            }

            // set the attribute to check against
            $attribute = $attributes[0];

            // get current logged in user
            $user = $token->getUser();

            // check if the given attribute is covered by this voter
            if (!$this->supportsAttribute($attribute)) {
                return VoterInterface::ACCESS_ABSTAIN;
            }

            // make sure there is a user object (i.e. that the user is logged in)
            if (!$user instanceof UserInterface) {
                return VoterInterface::ACCESS_DENIED;
            }

            switch($attribute) {
                case 'view':
                    // the data object could have for example a method isPrivate()
                    // which checks the Boolean attribute $private
                    if (!$post->isPrivate()) {
                        return VoterInterface::ACCESS_GRANTED;
                    }
                    break;

                case 'edit':
                    // we assume that our data object has a method getOwner() to
                    // get the current owner user entity for this data object
                    if ($user->getId() === $post->getOwner()->getId()) {
                        return VoterInterface::ACCESS_GRANTED;
                    }
                    break;
            }
        }
    }

É isso! O voter está pronto. O próximo passo é injetar o voter na
camada de segurança.

Declarando o Voter como um Serviço
----------------------------------

Para injetar o voter na camada de segurança, você deve declará-lo como um serviço
e adicionar a tag ``security.voter``:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/DemoBundle/Resources/config/services.yml
        services:
            security.access.post_voter:
                class:      Acme\DemoBundle\Security\Authorization\Voter\PostVoter
                public:     false
                tags:
                   - { name: security.voter }

    .. code-block:: xml

        <!-- src/Acme/DemoBundle/Resources/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <service id="security.access.post_document_voter"
                    class="Acme\DemoBundle\Security\Authorization\Voter\PostVoter"
                    public="false">
                    <tag name="security.voter" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // src/Acme/DemoBundle/Resources/config/services.php
        $container
            ->register(
                    'security.access.post_document_voter',
                    'Acme\DemoBundle\Security\Authorization\Voter\PostVoter'
            )
            ->addTag('security.voter')
        ;

Como usar o Voter em um Controlador
-----------------------------------

O voter registrado será então sempre solicitado assim que o método ``isGranted()``
do contexto de segurança é chamado.

.. code-block:: php

    // src/Acme/DemoBundle/Controller/PostController.php
    namespace Acme\DemoBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    class PostController extends Controller
    {
        public function showAction($id)
        {
            // get a Post instance
            $post = ...;
            
            // keep in mind, this will call all registered security voters
            if (false === $this->get('security.context')->isGranted('view', $post)) {
                throw new AccessDeniedException('Unauthorised access!');
            }

            return new Response('<h1>'.$post->getName().'</h1>');
        }
    }

É assim fácil!
