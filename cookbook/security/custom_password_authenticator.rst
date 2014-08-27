.. index::
    single: Segurança; Autenticator de Senha Personalizado

Como criar um Formulário Authenticator de Senha Personalizado
=============================================================

Imagine que você deseja permitir o acesso ao seu site apenas entre 2pm e 4pm
UTC. Antes do Symfony 2.4, você tinha que criar um token personalizado, factory, listener
e provedor. Nesse artigo, você vai aprender como fazer isso em um formulário de login
(ou seja, onde o usuário submete seu nome de usuário e senha).

O Authenticator de Senha
------------------------

.. versionadded:: 2.4
    A interface ``SimplePreAuthenticatorInterface`` foi introduzida no Symfony 2.4.

Primeiro, crie uma nova classe que implementa
:class:`Symfony\\Component\\Security\\Core\\Authentication\\SimpleFormAuthenticatorInterface`.
Eventualmente, isso permitirá criar uma lógica personalizada para autenticar
o usuário.

    // src/Acme/HelloBundle/Security/TimeAuthenticator.php
    namespace Acme\HelloBundle\Security;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Core\Authentication\SimpleFormAuthenticatorInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authentication\Token\UsernamePasswordToken;
    use Symfony\Component\Security\Core\Encoder\EncoderFactoryInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Symfony\Component\Security\Core\User\UserProviderInterface;

    class TimeAuthenticator implements SimpleFormAuthenticatorInterface
    {
        private $encoderFactory;

        public function __construct(EncoderFactoryInterface $encoderFactory)
        {
            $this->encoderFactory = $encoderFactory;
        }

        public function authenticateToken(TokenInterface $token, UserProviderInterface $userProvider, $providerKey)
        {
            try {
                $user = $userProvider->loadUserByUsername($token->getUsername());
            } catch (UsernameNotFoundException $e) {
                throw new AuthenticationException('Invalid username or password');
            }

            $encoder = $this->encoderFactory->getEncoder($user);
            $passwordValid = $encoder->isPasswordValid(
                $user->getPassword(),
                $token->getCredentials(),
                $user->getSalt()
            );

            if ($passwordValid) {
                $currentHour = date('G');
                if ($currentHour < 14 || $currentHour > 16) {
                    throw new AuthenticationException(
                        'You can only log in between 2 and 4!',
                        100
                    );
                }

                return new UsernamePasswordToken(
                    $user,
                    $user->getPassword(),
                    $providerKey,
                    $user->getRoles()
                );
            }

            throw new AuthenticationException('Invalid username or password');
        }

        public function supportsToken(TokenInterface $token, $providerKey)
        {
            return $token instanceof UsernamePasswordToken
                && $token->getProviderKey() === $providerKey;
        }

        public function createToken(Request $request, $username, $password, $providerKey)
        {
            return new UsernamePasswordToken($username, $password, $providerKey);
        }
    }

Como funciona
-------------

Excelente! Agora você só precisa configurar algumas :ref:`cookbook-security-password-authenticator-config`.
Mas, primeiro, você pode aprender mais sobre o que cada método faz nessa classe.

1) createToken
~~~~~~~~~~~~~~

Quando Symfony começa a lidar com um pedido, ``createToken()`` é chamado, onde
você cria um objeto :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface`
que contém todas as informações que você precisa em ``authenticateToken()``
para autenticar o usuário (por exemplo, o nome de usuário e senha).

Seja qual for o objeto token você criar aqui, ele será passado para você mais tarde em ``authenticateToken()``.

2) supportsToken
~~~~~~~~~~~~~~~~

.. include:: _supportsToken.rst.inc

3) authenticateToken
~~~~~~~~~~~~~~~~~~~~

Se ``supportsToken`` retornar ``true``, o Symfony irá chamar agora ``authenticateToken()``.
Seu trabalho aqui é verificar se é permitido o login do token primeiro
obtendo o objeto ``User`` através do provedor de usuário e, em seguida, verificando a senha
e a hora atual.

.. note::

    O "fluxo" de como obter o objeto ``User`` e determinar se o token é ou não
    válido (por exemplo, verificar a senha), pode variar de acordo com os seus
    requisitos.

Por fim, o seu trabalho é retornar um objeto token *novo* que é "autenticado"
(ou seja, ele tem pelo menos um papel - role - definido) e que tem o objeto ``User``
em seu interior.

Dentro deste método, um encoder é necessário para verificar a validade da senha::

        $encoder = $this->encoderFactory->getEncoder($user);
        $passwordValid = $encoder->isPasswordValid(
            $user->getPassword(),
            $token->getCredentials(),
            $user->getSalt()
        );

Esse é um serviço que já está disponível no Symfony e o algoritmo de senha
está definido na configuração de segurança (por exemplo, ``security.yml``) sob
a chave ``encoders``. Abaixo, você verá como injetar isso no ``TimeAuthenticator``.

.. _cookbook-security-password-authenticator-config:

Configuração
------------

Agora, configure o seu ``TimeAuthenticator`` como um serviço:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            # ...

            time_authenticator:
                class:     Acme\HelloBundle\Security\TimeAuthenticator
                arguments: ["@security.encoder_factory"]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- ... -->

                <service id="time_authenticator"
                    class="Acme\HelloBundle\Security\TimeAuthenticator"
                >
                    <argument type="service" id="security.encoder_factory" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;
        
        // ...

        $container->setDefinition('time_authenticator', new Definition(
            'Acme\HelloBundle\Security\TimeAuthenticator',
            array(new Reference('security.encoder_factory'))
        ));

Em seguida, ative-o na seção ``firewalls`` da configuração de segurança
utilizando a chave ``simple_form``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    pattern: ^/admin
                    # ...
                    simple_form:
                        authenticator: time_authenticator
                        check_path:    login_check
                        login_path:    login

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
                    >
                    <simple-form authenticator="time_authenticator"
                        check-path="login_check"
                        login-path="login"
                    />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ..

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area'    => array(
                    'pattern'     => '^/admin',
                    'simple_form' => array(
                        'provider'      => ...,
                        'authenticator' => 'time_authenticator',
                        'check_path'    => 'login_check',
                        'login_path'    => 'login',
                    ),
                ),
            ),
        ));

A chave ``simple_form`` tem as mesmas opções que a opção normal ``form_login``
, mas com a chave adicional ``authenticator`` que aponta para o
novo serviço. Para mais detalhes, veja :ref:`reference-security-firewall-form-login`.

Se criar um formulário de login em geral é novidade para você ou se não entende
as opções ``check_path`` ou ``login_path``, veja :doc:`/cookbook/security/form_login`.
