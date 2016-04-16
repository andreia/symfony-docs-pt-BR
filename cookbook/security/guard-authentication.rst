.. index::
    single: Segurança; Autenticação Personalizada

Como Criar um Sistema de Autenticação Personalizado com o Guard
===============================================================

Quer você precise construir um formulário de login tradicional, um sistema de autenticação com token para
uma API ou integrar com algum sistema proprietário single-sign-on, o componente
Guard pode tornar isso fácil... e divertido!

Neste exemplo, você vai construir um sistema de autenticação via token para API e aprender como
trabalhar com o Guard.

Criar um Usuário e um Provedor de Usuário
-----------------------------------------

Independentemente de como você se autentica, você precisa criar uma classe User que implementa ``UserInterface``
e configurar um :doc:`provedor de usuário </cookbook/security/custom_provider>`. Nesse
exemplo, os usuários são armazenados no banco de dados via Doctrine, e cada usuário tem uma propriedade 
``apiKey`` que usam para acessar sua conta através da API::

    // src/AppBundle/Entity/User.php
    namespace AppBundle\Entity;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity
     * @ORM\Table(name="user")
     */
    class User implements UserInterface
    {
        /**
         * @ORM\Id
         * @ORM\GeneratedValue(strategy="AUTO")
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string", unique=true)
         */
        private $username;

        /**
         * @ORM\Column(type="string", unique=true)
         */
        private $apiKey;

        public function getUsername()
        {
            return $this->username;
        }

        public function getRoles()
        {
            return ['ROLE_USER'];
        }

        public function getPassword()
        {
        }
        public function getSalt()
        {
        }
        public function eraseCredentials()
        {
        }

        // more getters/setters
    }

.. tip::

    Esse User não tem uma senha, mas você pode adicionar uma propriedade ``password`` se
    você também quiser permitir que esse usuário faça o login com uma senha (ex., através de um formulário de login).

Sua classe ``User`` não precisa ser armazenada via Doctrine: você pode fazer qualquer coisa que precisar.
Em seguida, verifique se você configurou um "provedor de usuário" para o usuário:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            providers:
                your_db_provider:
                    entity:
                        class: AppBundle:User

            # ...

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <provider name="your_db_provider">
                    <entity class="AppBundle:User" />
                </provider>

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'providers' => array(
                'your_db_provider' => array(
                    'entity' => array(
                        'class' => 'AppBundle:User',
                    ),
                ),
            ),

            // ...
        ));

É isso! Precisando de mais informações sobre esse passo, consulte:

* :doc:`/cookbook/security/entity_provider`
* :doc:`/cookbook/security/custom_provider`

Passo 1) Criar a classe Authenticator
-------------------------------------

Suponha que você tenha uma API onde seus clientes vão enviar um cabeçalho ``X-AUTH-TOKEN``
em cada requisição com o token deles da API. Seu trabalho é ler isso e encontrar o
usuário associado (se houver).

Para criar um sistema de autenticação personalizado, basta criar uma classe e fazer ela implementar
:class:`Symfony\\Component\\Security\\Guard\\GuardAuthenticatorInterface`. Ou, estender
a mais simples :class:`Symfony\\Component\\Security\\Guard\\AbstractGuardAuthenticator`.
Isso requer que você implemente seis métodos::

    // src/AppBundle/Security/TokenAuthenticator.php
    namespace AppBundle\Security;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\JsonResponse;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Guard\AbstractGuardAuthenticator;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Doctrine\ORM\EntityManager;

    class TokenAuthenticator extends AbstractGuardAuthenticator
    {
        private $em;

        public function __construct(EntityManager $em)
        {
            $this->em = $em;
        }

        /**
         * Called on every request. Return whatever credentials you want,
         * or null to stop authentication.
         */
        public function getCredentials(Request $request)
        {
            if (!$token = $request->headers->get('X-AUTH-TOKEN')) {
                // no token? Return null and no other methods will be called
                return;
            }

            // What you return here will be passed to getUser() as $credentials
            return array(
                'token' => $token,
            );
        }

        public function getUser($credentials, UserProviderInterface $userProvider)
        {
            $apiKey = $credentials['token'];

            // if null, authentication will fail
            // if a User object, checkCredentials() is called
            return $this->em->getRepository('AppBundle:User')
                ->findOneBy(array('apiKey' => $apiKey));
        }

        public function checkCredentials($credentials, UserInterface $user)
        {
            // check credentials - e.g. make sure the password is valid
            // no credential check is needed in this case

            // return true to cause authentication success
            return true;
        }

        public function onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)
        {
            // on success, let the request continue
            return null;
        }

        public function onAuthenticationFailure(Request $request, AuthenticationException $exception)
        {
            $data = array(
                'message' => strtr($exception->getMessageKey(), $exception->getMessageData())

                // or to translate this message
                // $this->translator->trans($exception->getMessageKey(), $exception->getMessageData())
            );

            return new JsonResponse($data, 403);
        }

        /**
         * Called when authentication is needed, but it's not sent
         */
        public function start(Request $request, AuthenticationException $authException = null)
        {
            $data = array(
                // you might translate this message
                'message' => 'Authentication Required'
            );

            return new JsonResponse($data, 401);
        }

        public function supportsRememberMe()
        {
            return false;
        }
    }

Bom trabalho! Cada método é explicado abaixo em: :ref:`Os Métodos do Autenticador Guard<guard-auth-methods>`.

Passo 2) Configure o Autenticator
---------------------------------

Para finalizar, registre a classe como um serviço:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.token_authenticator:
                class: AppBundle\Security\TokenAuthenticator
                arguments: ['@doctrine.orm.entity_manager']

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <services>
            <service id="app.token_authenticator" class="AppBundle\Security\TokenAuthenticator">
                <argument type="service" id="doctrine.orm.entity_manager"/>
            </service>
        </services>

    .. code-block:: php

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->setDefinition('app.token_authenticator', new Definition(
            'AppBundle\Security\TokenAuthenticator',
            array(new Reference('doctrine.orm.entity_manager'))
        ));

E configure a chave ``firewalls`` no ``security.yml`` para usar esse autenticador:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                # ...

                main:
                    anonymous: ~
                    logout: ~

                    guard:
                        authenticators:
                            - app.token_authenticator

                    # if you want, disable storing the user in the session
                    # stateless: true

                    # maybe other things, like form_login, remember_me, etc
                    # ...

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <config>
                <!-- ... -->

                <firewall name="main"
                    pattern="^/"
                    anonymous="true"
                >
                    <logout />

                    <guard>
                        <authenticator>app.token_authenticator</authenticator>
                    </guard>

                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ..

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'       => array(
                    'pattern'        => '^/',
                    'anonymous'      => true,
                    'logout'         => true,
                    'guard'          => array(
                        'authenticators'  => array(
                            'app.token_authenticator'
                        ),
                    ),
                    // ...
                ),
            ),
        ));

Você conseguiu! Agora você tem um sistema de autenticação via token para API totalmente funcional. Se a sua
homepage requer ``ROLE_USER``, então você pode testá-la sob diferentes condições:

.. code-block:: bash

    # test with no token
    curl http://localhost:8000/
    # {"message":"Authentication Required"}

    # test with a bad token
    curl -H "X-AUTH-TOKEN: FAKE" http://localhost:8000/
    # {"message":"Username could not be found."}

    # test with a working token
    curl -H "X-AUTH-TOKEN: REAL" http://localhost:8000/
    # the homepage controller is executed: the page loads normally

Agora, aprenda mais sobre o que cada método faz.

.. _guard-auth-methods:

Os Métodos do Autenticator Guard
--------------------------------

Cada autenticador precisa dos seguintes métodos:

**getCredentials(Request $request)**
    Será chamado *a cada* requisição e seu trabalho é ler o token (ou
    qualquer que seja a sua informação de "autenticação") a partir da requisição e retornar ele.
    Se você retornar ``null``, o restante do processo de autenticação é ignorado. Caso contrário,
    ``getUser()`` será chamado e o valor de retorno é passado como o primeiro argumento.

**getUser($credentials, UserProviderInterface $userProvider)**
    Se ``getCredentials()`` retorna um valor não nulo, então esse método é chamado
    e o valor de retorno de ``getCredentials()`` é passado aqui como o argumento ``$credentials``.
    Seu trabalho é retornar um objeto que implementa ``UserInterface``. Se você fizer, então
    ``checkCredentials()`` será chamado. Se você retornar ``null`` (ou lançar uma
    :ref:`AuthenticationException <guard-customize-error>`)
    a autenticação irá falhar.

**checkCredentials($credentials, UserInterface $user)**
    Se ``getUser()`` retorna um objeto User, esse método é chamado. Seu trabalho é
    verificar se as credenciais estão corretas. Para um formulário de login, este é o lugar onde você
    verificaria se a senha está correta para o usuário. Para passar a autenticação, retorne
    ``true``. Se você retornar *qualquer outra coisa*
    (ou lançar uma :ref:`AuthenticationException <guard-customize-error>`),
    a autenticação irá falhar.

**onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)**
    É chamado após a autenticação bem sucedida e seu trabalho, é retornar
    um objeto :class:`Symfony\\Component\\HttpFoundation\\Response`
    que será enviado para o cliente ou ``null`` para continuar a requisição
    (ex., permitir que a rota/controlador seja chamado de forma normal). Uma vez que essa
    é uma API onde cada requisição se autentica, você quer retornar
    ``null``.

**onAuthenticationFailure(Request $request, AuthenticationException $exception)**
    É chamado se a autenticação falhar. Seu trabalho
    é retornar o objeto :class:`Symfony\\Component\\HttpFoundation\\Response`
    que deve ser enviado para o cliente. O ``$exception`` irá dizer
    *o que* ocorreu de errado durante a autenticação.

**start(Request $request, AuthenticationException $authException = null)**
    É chamado se o cliente acessa um URI/recurso que requer autenticação,
    mas não foram enviados detalhes de autenticação (ou seja, você retornou ``null`` a partir
    do ``getCredentials()``). Seu trabalho é retornar um objeto
    :class:`Symfony\\Component\\HttpFoundation\\Response` que ajuda
    a autenticação do usuário (ex., uma resposta 401 que diz "está faltando o token!").

**supportsRememberMe**
    Se você quiser fornecer suporte a funcionalidade "lembrar-me", retorne true a partir desse método.
    Você ainda precisará ativar ``remember_me`` sob seu firewall para que ela funcione.
    Uma vez que esta é uma API stateless, você não quer fornecer suporte a funcionalidade
    "lembrar-me" nesse exemplo.

.. _guard-customize-error:

Personalizando Mensagens de Erro
--------------------------------

Quando ``onAuthenticationFailure()`` é chamado, é passado uma ``AuthenticationException``
que descreve *como* a autenticação falhou através do seu método ``$e->getMessageKey()`` (e
``$e->getMessageData()``). A mensagem será diferente com base *onde* a
autenticação falhou (ou seja, ``getUser()`` versus ``checkCredentials()``).

Mas, você pode facilmente retornar uma mensagem personalizada, lançando uma
:class:`Symfony\\Component\\Security\\Core\\Exception\\CustomUserMessageAuthenticationException`.
Você pode lançar a partir do ``getCredentials()``, ``getUser()`` ou ``checkCredentials()``
para causar uma falha::

    // src/AppBundle/Security/TokenAuthenticator.php
    // ...

    use Symfony\Component\Security\Core\Exception\CustomUserMessageAuthenticationException;

    class TokenAuthenticator extends AbstractGuardAuthenticator
    {
        // ...

        public function getCredentials(Request $request)
        {
            // ...

            if ($token == 'ILuvAPIs') {
                throw new CustomUserMessageAuthenticationException(
                    'ILuvAPIs is not a real API key: it\'s just a silly phrase'
                );
            }

            // ...
        }

        // ...
    }

Neste caso, uma vez que "ILuvAPIs" é uma chave de API ridícula, você poderia incluir um easter
egg para retornar uma mensagem personalizada se alguém tentar isso:

.. code-block:: bash

    curl -H "X-AUTH-TOKEN: ILuvAPIs" http://localhost:8000/
    # {"message":"ILuvAPIs is not a real API key: it's just a silly phrase"}

Perguntas Frequentes
--------------------

**Posso ter Múltiplos Autenticadores?**
    Sim! Mas quando você tiver, terá que escolher apenas *um* autenticador para ser seu
    "ponto de entrada". Isso significa que você terá que escolher *qual* método ``start()`` do
    autenticador deve ser chamado quando um usuário anônimo tentar acessar um recurso protegido.
    Por exemplo, suponha que você tenha um ``app.form_login_authenticator`` que lida com
    um formulário de login tradicional. Quando um usuário acessar uma página protegida anonimamente,
    você quer usar o método ``start()`` do autenticador de formulário e redirecioná-los
    para a página de login (em vez de devolver uma resposta JSON):

    .. configuration-block::

        .. code-block:: yaml

            # app/config/security.yml
            security:
                # ...

                firewalls:
                    # ...

                    main:
                        anonymous: ~
                        logout: ~

                        guard:
                            authenticators:
                                - app.token_authenticator

                        # if you want, disable storing the user in the session
                        # stateless: true

                        # maybe other things, like form_login, remember_me, etc
                        # ...

        .. code-block:: xml

            <!-- app/config/security.xml -->
            <?xml version="1.0" encoding="UTF-8"?>
            <srv:container xmlns="http://symfony.com/schema/dic/security"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:srv="http://symfony.com/schema/dic/services"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">
                <config>
                    <!-- ... -->

                    <firewall name="main"
                        pattern="^/"
                        anonymous="true"
                    >
                        <logout />

                        <guard>
                            <authenticator>app.token_authenticator</authenticator>
                        </guard>

                        <!-- ... -->
                    </firewall>
                </config>
            </srv:container>

        .. code-block:: php

            // app/config/security.php

            // ..

            $container->loadFromExtension('security', array(
                'firewalls' => array(
                    'main'       => array(
                        'pattern'        => '^/',
                        'anonymous'      => true,
                        'logout'         => true,
                        'guard'          => array(
                            'authenticators'  => array(
                                'app.token_authenticator'
                            ),
                        ),
                        // ...
                    ),
                ),
            ));

**Posso usar junto com ``form_login``?**
    Sim! ``form_login`` é *um* caminho para autenticar um usuário, logo você pode usá-lo
    *e*, então, adicionar um ou mais autenticadores. Usar um autenticador guard não
    colide com outras maneiras de autenticar.

**Posso usar junto com o FOSUserBundle?**
    Sim! Na verdade, o FOSUserBundle não lida com a segurança: ele simplesmente fornece um
    objeto ``User`` e algumas rotas e controladores para ajudar com login, registo,
    esqueci a senha, etc. Quando você usa o FOSUserBundle, normalmente usa o ``form_login``
    na verdade para autenticar o usuário. Você pode continuar a fazer isso (veja a pergunta
    anterior) ou usar o objeto ``User`` do FOSUserBundle e criar seu próprio
    autenticador(es) (assim como nesse artigo).
