.. index::
    single: Segurança; Authenticator de Requisição Personalizado

Como autenticar usuários com chaves de API
==========================================

Atualmente, é bastante comum autenticar o usuário através de uma chave de API (ao desenvolver
um web service, por exemplo). A chave de API é fornecida para cada requisição e é
passada como uma query string ou através de um cabeçalho HTTP.

O autenticador da chave de API
------------------------------

.. versionadded:: 2.4
    A interface ``SimplePreAuthenticatorInterface`` foi introduzida no Symfony 2.4.

A autenticação de um usuário com base nas informações da Requisição deve ser feita através de um
mecanismo de pré-autenticação. A :class:`Symfony\\Component\\Security\\Core\\Authentication\\SimplePreAuthenticatorInterface`
permite implementar esse esquema muito facilmente.

Sua situação pode divergir, mas neste exemplo, um token é lido
a partir de um parâmetro query ``apikey``, o username adequado é carregado a partir desse
valor e, em seguida, um objeto User é criado::

    // src/Acme/HelloBundle/Security/ApiKeyAuthenticator.php
    namespace Acme\HelloBundle\Security;

    use Symfony\Component\Security\Core\Authentication\SimplePreAuthenticatorInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\Authentication\Token\PreAuthenticatedToken;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Symfony\Component\Security\Core\Exception\BadCredentialsException;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface
    {
        protected $userProvider;

        public function __construct(ApiKeyUserProvider $userProvider)
        {
            $this->userProvider = $userProvider;
        }

        public function createToken(Request $request, $providerKey)
        {
            if (!$request->query->has('apikey')) {
                throw new BadCredentialsException('No API key found');
            }

            return new PreAuthenticatedToken(
                'anon.',
                $request->query->get('apikey'),
                $providerKey
            );
        }

        public function authenticateToken(TokenInterface $token, UserProviderInterface $userProvider, $providerKey)
        {
            $apiKey = $token->getCredentials();
            $username = $this->userProvider->getUsernameForApiKey($apiKey);

            if (!$username) {
                throw new AuthenticationException(
                    sprintf('API Key "%s" does not exist.', $apiKey)
                );
            }

            $user = $this->userProvider->loadUserByUsername($username);

            return new PreAuthenticatedToken(
                $user,
                $apiKey,
                $providerKey,
                $user->getRoles()
            );
        }

        public function supportsToken(TokenInterface $token, $providerKey)
        {
            return $token instanceof PreAuthenticatedToken && $token->getProviderKey() === $providerKey;
        }
    }

Uma vez que você :ref:`configurou <cookbook-security-api-key-config>` tudo,
poderá autenticar adicionando um parâmetro apikey à query
string, como ``http://example.com/admin/foo?apikey=37b51d194a7513e45b56f6524f2d51f2``.

O processo de autenticação possui várias etapas, e sua implementação
provavelmente irá diferir:

1. createToken
~~~~~~~~~~~~~~

Logo no início do ciclo da requisição, o Symfony chama ``createToken()``. Seu trabalho aqui
é o de criar um objeto token que contém todas as informações da
requisição que você precisa para autenticar o usuário (por exemplo, o parâmetro query 
``apikey``). Se essa informação estiver faltando, lance uma exceção
:class:`Symfony\\Component\\Security\\Core\\Exception\\BadCredentialsException`
que fará a autenticação falhar.

2. supportsToken
~~~~~~~~~~~~~~~~

.. include:: _supportsToken.rst.inc

3. authenticateToken
~~~~~~~~~~~~~~~~~~~~

Se ``supportsToken()`` retornar ``true``, o Symfony agora vai chamar ``authenticateToken()``.
Uma parte importante é o ``$userProvider``, que é uma classe externa que ajuda
a carregar informações sobre o usuário. Você aprenderá mais sobre isso em seguida.

Nesse exemplo específico, acontece o seguinte em ``authenticateToken()``:

#. Primeiro, você usa o ``$userProvider`` para, de alguma forma, procurar o ``$username`` que
   corresponde à ``$apiKey``;
#. Segundo, você usa o ``$userProvider`` novamente para carregar ou criar um objeto
   ``User`` para o ``$username``;
#. Finalmente, você cria um *token autenticado* (ou seja, um token com pelo menos um
   papel), que tem os papéis apropriados e o objeto User ligado a ele.

O objetivo é, em última análise, usar a ``$apiKey`` para encontrar ou criar um objeto
``User``. *Como* você faz isso (por exemplo, consultar um banco de dados) e a classe exata para
seu objeto ``User``, pode variar. Essas diferenças serão mais óbvias em seu
provedor de usuário.

O Provedor de Usuário
~~~~~~~~~~~~~~~~~~~~~

O ``$userProvider`` pode ser qualquer provedor de usuário (veja :doc:`/cookbook/security/custom_provider`).
Neste exemplo, a ``$apiKey`` é usada para encontrar, de alguma forma, o username
do usuário. Esse trabalho é feito em um método ``getUsernameForApiKey()``, que
é criado inteiramente personalizado para esse caso de uso (ou seja, esse não é um método que é
usado pelo sistema provedor de usuário das classes do Symfony).

O ``$userProvider`` pode parecer com o seguinte::

    // src/Acme/HelloBundle/Security/ApiKeyUserProvider.php
    namespace Acme\HelloBundle\Security;

    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\User\User;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Exception\UnsupportedUserException;

    class ApiKeyUserProvider implements UserProviderInterface
    {
        public function getUsernameForApiKey($apiKey)
        {
            // Look up the username based on the token in the database, via
            // an API call, or do something entirely different
            $username = ...;

            return $username;
        }

        public function loadUserByUsername($username)
        {
            return new User(
                $username,
                null,
                // the roles for the user - you may choose to determine
                // these dynamically somehow based on the user
                array('ROLE_USER')
            );
        }

        public function refreshUser(UserInterface $user)
        {
            // this is used for storing authentication in the session
            // but in this example, the token is sent in each request,
            // so authentication can be stateless. Throwing this exception
            // is proper to make things stateless
            throw new UnsupportedUserException();
        }

        public function supportsClass($class)
        {
            return 'Symfony\Component\Security\Core\User\User' === $class;
        }
    }

.. note::

    Leia o artigo dedicado para aprender
    :doc:`como criar um provedor de usuário personalizado </cookbook/security/custom_provider>`.

A lógica dentro de ``getUsernameForApiKey()`` é com você. Você pode de alguma forma transformar
a chave da API (ex., ``37b51d``) em um username (ex., ``jondoe``), pesquisando
algumas informações em uma tabela de banco de dados "token".

O mesmo é verdadeiro para ``loadUserByUsername()``. Nesse exemplo, a classe do Symfony
:class:`Symfony\\Component\\Security\\Core\\User\\User` é simplesmente criada.
Isso faz sentido se você não precisa armazenar qualquer informação extra no seu
objeto User (ex., ``firstName``). Mas, se você fizer isso, pode, ao invés, ter a sua *própria*
classe de usuário que você irá criar e preencher aqui consultando um banco de dados. Isso
permite que você tenha dados personalizados no objeto ``User``.

Finalmente, certifique-se apenas que ``supportsClass()`` retorna ``true`` para objetos
User com a mesma classe que qualquer usuário que você retornar em ``loadUserByUsername()``.
Se a sua autenticação é stateless, como nesse exemplo (ou seja, você espera
que o usuário envie a chave de API em cada requisição para que você não tenha que salvar o
login em sessão), então você pode simplesmente lançar a exceção ``UnsupportedUserException``
em ``refreshUser()``.

.. note::

    Se você *quer* armazenar os dados de autenticação na sessão para que
    a chave não precise ser enviada em cada requisição, veja :ref:`cookbook-security-api-key-session`.

Tratamento de falhas de autenticação
------------------------------------

Para que o seu ``ApiKeyAuthentication`` exiba corretamente um código de status
HTTP 403 quando as credenciais estiverem incorretas ou a autenticação falhar, você
precisa implementar a :class:`Symfony\Component\Security\Http\Authentication\AuthenticationFailureHandlerInterface` em seu
authenticator. Isso irá fornecer um método ``onAuthenticationFailure`` que
você pode usar para criar um erro ``Response``.

.. code-block:: php

    // src/Acme/HelloBundle/Security/ApiKeyAuthenticator.php
    namespace Acme\HelloBundle\Security;

    use Symfony\Component\Security\Core\Authentication\SimplePreAuthenticatorInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Http\Authentication\AuthenticationFailureHandlerInterface;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpFoundation\Request;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface, AuthenticationFailureHandlerInterface
    {
        //...

        public function onAuthenticationFailure(Request $request, AuthenticationException $exception)
        {
            return new Response("Authentication Failed.", 403);
        }
    }

.. _cookbook-security-api-key-config:

Configuração
------------

Uma vez que você tem a ``ApiKeyAuthentication`` configurada, você precisa registrá-la
como um serviço e usá-la em sua configuração de segurança (por exemplo, ``security.yml``).
Primeiro, registre ela como um serviço. Isso pressupõe que você já tenha configurado
seu provedor de usuário personalizado como um serviço chamado ``your_api_key_user_provider``
(veja :doc:`/cookbook/security/custom_provider`).

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            # ...

            apikey_authenticator:
                class:     Acme\HelloBundle\Security\ApiKeyAuthenticator
                arguments: ["@your_api_key_user_provider"]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="apikey_authenticator"
                    class="Acme\HelloBundle\Security\ApiKeyAuthenticator"
                >
                    <argument type="service" id="your_api_key_user_provider" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $container->setDefinition('apikey_authenticator', new Definition(
            'Acme\HelloBundle\Security\ApiKeyAuthenticator',
            array(new Reference('your_api_key_user_provider'))
        ));

Agora, ative-o na seção ``firewalls`` da sua configuração de segurança
usando a chave ``simple_preauth``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/admin
                    stateless: true
                    simple_preauth:
                        authenticator: apikey_authenticator

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

                <firewall name="secured_area"
                    pattern="^/admin"
                    stateless="true"
                >
                    <simple-preauth authenticator="apikey_authenticator" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ..

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area'       => array(
                    'pattern'        => '^/admin',
                    'stateless'      => true,
                    'simple_preauth' => array(
                        'authenticator'  => 'apikey_authenticator',
                    ),
                ),
            ),
        ));

É isso! Agora, sua ``ApiKeyAuthentication`` deve ser chamada no início
de cada requisição e o seu processo de autenticação será realizado.

O parâmetro de configuração ``stateless`` impede o Symfony de tentar
armazenar a informação de autenticação em sessão, o que não é necessário
uma vez que o cliente irá enviar a ``apikey`` em cada requisição. Se você *tem* necessidade
de armazenar a autenticação em sessão, continue lendo!

.. _cookbook-security-api-key-session:

Armazenando autenticação em sessão
----------------------------------

Até agora, esse artigo descreveu uma situação onde algum tipo de token de autenticação
é enviado em cada requisição. Mas, em algumas situações (como um fluxo OAuth),
o token pode ser enviado em apenas *uma* requisição. Nesse caso, você vai querer
autenticar o usuário e armazenar essa autenticação em sessão para que
o usuário seja conectado automaticamente em cada requisição subseqüente.

Para fazer isso, primeiro remova a chave ``stateless`` da sua configuração de firewall
ou configure-a para ``false``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/admin
                    stateless: false
                    simple_preauth:
                        authenticator: apikey_authenticator

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

                <firewall name="secured_area"
                    pattern="^/admin"
                    stateless="false"
                >
                    <simple-preauth authenticator="apikey_authenticator" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ..
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area'       => array(
                    'pattern'        => '^/admin',
                    'stateless'      => false,
                    'simple_preauth' => array(
                        'authenticator'  => 'apikey_authenticator',
                    ),
                ),
            ),
        ));

O armazenamento de informações de autenticação em sessão funciona assim:

#. No final de cada requisição, o Symfony serializa o objeto token (retornado
   do ``authenticateToken()``), que também serializa o objeto ``User``
   (uma vez que é definido em uma propriedade no token);
#. Na próxima requisição o token é desserializado e o objeto ``User``
   desserializado é passado para a função ``refreshUser()`` do provedor de usuário.

O segundo passo é o mais importante: o Symfony chama ``refreshUser()`` e passa
para ele o objeto de usuário que foi serializado na sessão. Se os usuários estão
armazenados em um banco de dados, então você pode desejar re-consultar para uma versão atualizada
do usuário, para ter certeza que não está desatualizado. Mas, independentemente de suas necessidades,
o ``refreshUser()`` deve agora retornar o objeto User::

    // src/Acme/HelloBundle/Security/ApiKeyUserProvider.php

    // ...
    class ApiKeyUserProvider implements UserProviderInterface
    {
        // ...

        public function refreshUser(UserInterface $user)
        {
            // $user is the User that you set in the token inside authenticateToken()
            // after it has been deserialized from the session

            // you might use $user to query the database for a fresh user
            // $id = $user->getId();
            // use $id to make a query

            // if you are *not* reading from a database and are just creating
            // a User object (like in this example), you can just return it
            return $user;
        }
    }

.. note::

    Você também deseja ter certeza de que o seu objeto ``User`` está sendo serializado
    corretamente. Se o seu objeto ``User`` possui propriedades privadas, o PHP não pode serializá-
    las. Nesse caso, você pode obter um objeto User que tem um valor ``null``
    para cada propriedade. Para um exemplo, veja :doc:`/cookbook/security/entity_provider`.

Autenticando apenas determinadas URLs
-------------------------------------

Esse artigo assumiu que você deseja verificar a autenticação ``apikey``
*em cada* requisição. Mas, em algumas situações (como um fluxo OAuth), você só
precisa realmente verificar a informação de autenticação uma vez que o usuário tenha atingido
uma determinada URL (por exemplo, a URL de redirecionamento em OAuth).

Felizmente, lidar com esta situação é fácil: basta verificar qual é
a URL atual antes de criar o token em ``createToken()``::

    // src/Acme/HelloBundle/Security/ApiKeyAuthenticator.php

    // ...
    use Symfony\Component\Security\Http\HttpUtils;
    use Symfony\Component\HttpFoundation\Request;

    class ApiKeyAuthenticator implements SimplePreAuthenticatorInterface
    {
        protected $userProvider;

        protected $httpUtils;

        public function __construct(ApiKeyUserProviderInterface $userProvider, HttpUtils $httpUtils)
        {
            $this->userProvider = $userProvider;
            $this->httpUtils = $httpUtils;
        }

        public function createToken(Request $request, $providerKey)
        {
            // set the only URL where we should look for auth information
            // and only return the token if we're at that URL
            $targetUrl = '/login/check';
            if (!$this->httpUtils->checkRequestPath($request, $targetUrl)) {
                return;
            }

            // ...
        }
    }

Isso usa a classe :class:`Symfony\\Component\\Security\\Http\\HttpUtils`
para verificar se a URL atual corresponde a que você está procurando. Nesse
caso, a URL (``/login/check``) foi codificada manualmente na classe, mas você
também poderia injetá-la como o terceiro argumento do construtor.

Em seguida, basta atualizar a configuração do serviço para injetar o serviço
``security.http_utils``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            # ...

            apikey_authenticator:
                class:     Acme\HelloBundle\Security\ApiKeyAuthenticator
                arguments: ["@your_api_key_user_provider", "@security.http_utils"]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="apikey_authenticator"
                    class="Acme\HelloBundle\Security\ApiKeyAuthenticator"
                >
                    <argument type="service" id="your_api_key_user_provider" />
                    <argument type="service" id="security.http_utils" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $container->setDefinition('apikey_authenticator', new Definition(
            'Acme\HelloBundle\Security\ApiKeyAuthenticator',
            array(
                new Reference('your_api_key_user_provider'),
                new Reference('security.http_utils')
            )
        ));

É isso! Divirta-se!
