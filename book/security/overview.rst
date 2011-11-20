.. index::
   single: Security

Segurança
========

Symfony2 já possui uma camada de segurança embutida. Ela atua disponibilizando para
 seu aplicativo autenticação e autorização.

*Autenticação* é o nome dado ao processo que garante que o usuário é que ele diz ser.
*Autorização* se refere ao processo de decidir se o usuário tem permissão para
executar determinada ação ou não. A autenticação é sempre executada antes da autorização.

Este documento é uma rápida introdução sobre esses dois principais conceitos, mas
é ainda muito pouco sobre o assunto. Se você realmente quiser saber mais sobre
o sistema de segurança do Symfony2, leia também os documentos seguintes:
:doc:`Users </book/security/users>`,
:doc:`Authentication </book/security/authentication>`, and
:doc:`Authorization </book/security/authorization>`.

.. index::
   pair: Security; Configuration

Configuração
-------------

Para a maioria dos casos, pode-se configurar a segurança do Symfony2 a partir do
 arquivo principal de configuração. A seguir está este arquivo com uma configuração
 típica:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            encoders:
                Symfony\Component\Security\Core\User\User: plaintext

            providers:
                main:
                    users:
                        foo: { password: testing, roles: ROLE_USER }

            firewalls:
                main:
                    pattern:    /.*
                    http_basic: true
                    logout:     true

            access_control:
                - { path: /.*, role: ROLE_USER }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <security:config>
            <security:encoder class="Symfony\Component\Security\Core\User\User" algorithm="plaintext" />

            <security:provider>
                <security:user name="foo" password="testing" roles="ROLE_USER" />
            </security:provider>

            <security:firewall pattern="/.*">
                <security:http-basic />
                <security:logout />
            </security:firewall>

            <security:access-control>
                <security:rule path="/.*" role="ROLE_USER" />
            </security:access-control>
        </security:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => 'plaintext',
            ),
            'providers' => array(
                'main' => array(
                    'users' => array(
                        'foo' => array('password' => 'testing', 'roles' => 'ROLE_USER'),
                )),
            ),
            'firewalls' => array(
                'main' => array('pattern' => '/.*', 'http_basic' => true, 'logout' => true),
            ),
            'access_control' => array(
                array('path' => '/.*', 'role' => 'ROLE_USER'),
            ),
        ));

Na maioria das vezes, é mais conveniente separar todas as configurações relacionadas
a seguança em um aruqivo externo. Se você utiliza XML, o arquivo externo pode usar
 como nome de escopo (namespace) o utilizado como padrão para facilitar a leitura:

.. code-block:: xml

        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <provider>
                    <password-encoder hash="sha1" />
                    <user name="foo" password="0beec7b5ea3f0fdbc95d0dd47f3c5bc275da8a33" roles="ROLE_USER" />
                </provider>

                <firewall pattern="/.*">
                    <http-basic />
                    <logout />
                </firewall>

                <access-control>
                    <rule path="/.*" role="ROLE_USER" />
                </access-control>
            </config>
        </srv:container>

.. note::

    Todos os exemplos neste documento assume que você esteja utilizando um arquivo
    externo com o nome de escopo (namespace) de segurança padrão como acima.

Como pode observar, a configuração é composta de quatro seções:

* *encoder*: Um encoder é utilizado para gerar o hash (código alfanumérico obtido através de uma função hash) das senhas dos usuários;

* *provider*: Um provider sabe como buscar usuários;

* *firewall*: Um firewall define o mecanismo de autenticação para toda aplicação ou para apenas parte dela;

* *access-control*: São as regras de controle de acesso de sua aplicação através dos diferentes perfis (roles).

Em resumo, o firewall autentica o cliente baseado nas credenciais enviadas (tipicamente login/senha)
e o usuário é criado na sessão pelo provedor de usuários (user provider). Finalmente o controle de acesso
é utilizado para proteger partes específicas da ou toda a aplicação.

Autenticação
--------------

Symfony2 suporta de fábrica vários mecanismos de autenticação e outros podem ser adicionados facilmente
se necessário. Os principais que já estão disponíveis estão listados abaixo.

* HTTP Basic;
* HTTP Digest;
* Form based authentication;
* X.509 certificates.

Abaixo está um exemplo de configuração de como você pode adicionar autenticação por HTTP Basic a sua aplicação.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    http_basic: true

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <http-basic />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('http_basic' => true),
            ),
        ));

É possível utilizar vários firewalls trabalhando em conjunto se necessário para diferentes partes
de sua aplicação. Veja um exemplo a seguir.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                backend:
                    pattern: /admin/.*
                    http_basic: true
                public:
                    pattern:  /.*
                    security: false

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall pattern="/admin/.*">
                <http-basic />
            </firewall>

            <firewall pattern="/.*" security="false" />
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'backend' => array('pattern' => '/admin/.*', 'http-basic' => true),
                'public'  => array('pattern' => '/.*', 'security' => false),
            ),
        ));

.. tip::

    O HTTP Basic é o mais fácil mecanismo de autenticação de se utilizar. Certifique-se
    de ler o documento :doc:`Authentication </book/security/authentication>` para
    aprender como configurar outros mecanismos, como configurar autenticação sem sessão
    (stateless, que significa que nenhum cookie será criado pelo Symfony2), como se
    passar por outro usuário, como garantir a utilização do aplicativo através de HTTPS,
    e outras coisas.

Usuários
--------

Durante a autenticação, Symfony2 solicitar ao provedor de usuários (user provider)
para criar um objeto usuário que corresponda a solicitação do cliente (através de
credenciais como login e senha). Para iniciar rapidamente, você pode definir a
configuração de um provedor de usuários in-memory (as credenciais dos usuários do
sistema são definidas no próprio arquivo de configuração). Veja um exemplo a seguir.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            providers:
                main:
                    users:
                        foo: { password: foo }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <provider>
                <user name="foo" password="foo" />
            </provider>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'provider' => array(
                'main' => array('users' => array(
                    'foo' => array('password' => 'foo'),
                )),
            ),
        ));

The above configuration defines a 'foo' user with a 'foo' password. After
authentication, you can access the authenticated user via the security context
(the user is an instance of :class:`Symfony\\Component\\Security\\Core\\User\\User`)::

    $user = $container->get('security.context')->getToken()->getUser();

.. tip::

    Using the in-memory provider is a great way to easily secure your personal
    website backend, to create a prototype, or to provide fixtures for your
    tests. Read the :doc:`Users </book/security/users>` document to learn
    how to avoid the password to be in clear, how to use a Doctrine Entity as
    a user provider, how to define several providers, and much more.

Authorization
-------------

Authorization is optional but gives you a powerful way to restrict access to
your application resources based user roles:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            providers:
                main:
                    users:
                        foo: { password: foo, roles: ['ROLE_USER', 'ROLE_ADMIN'] }
            access_control:
                - { path: /.*, role: ROLE_USER }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <provider>
                <user name="foo" password="foo" roles="ROLE_USER,ROLE_ADMIN" />
            </provider>

            <access-control>
                <rule path="/.*" role="ROLE_USER" />
            </access-control>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'provider' => array(
                'main' => array('users' => array(
                    'foo' => array('password' => 'foo', 'roles' => array('ROLE_USER', 'ROLE_ADMIN')),
                )),
            ),

            'access_control' => array(
                array('path' => '/.*', 'role' => 'ROLE_USER'),
            ),
        ));

The above configuration defines a 'foo' user with the 'ROLE_USER' and
'ROLE_ADMIN' roles and it restricts access to the whole application to users
having the 'ROLE_USER' role.

.. tip::

    Read the :doc:`Authorization </book/security/authorization>` document to
    learn how to define a role hierarchy, how to customize your template based
    on roles, how to define access control rules based on request attributes,
    and much more.
