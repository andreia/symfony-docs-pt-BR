.. index::
   single: Segurança; Personalizando o formulário de login

Como personalizar o seu Formulário de Login
===========================================

Usar um :ref:`formulário de login<book-security-form-login>` para autenticação é
é um método comum e flexível para lidar com a autenticação no Symfony2. Praticamente
todos os aspectos do formulário de login podem ser personalizados. A configuração padrão
completa é mostrada na próxima seção.

Configuração de Referência do Formulário de Login
-------------------------------------------------

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    form_login:
                        # the user is redirected here when he/she needs to login
                        login_path:                     /login

                        # if true, forward the user to the login form instead of redirecting
                        use_forward:                    false

                        # submit the login form here
                        check_path:                     /login_check

                        # by default, the login form *must* be a POST, not a GET
                        post_only:                      true

                        # login success redirecting options (read further below)
                        always_use_default_target_path: false
                        default_target_path:            /
                        target_path_parameter:          _target_path
                        use_referer:                    false

                        # login failure redirecting options (read further below)
                        failure_path:                   null
                        failure_forward:                false

                        # field names for the username and password fields
                        username_parameter:             _username
                        password_parameter:             _password

                        # csrf token options
                        csrf_parameter:                 _csrf_token
                        intention:                      authenticate

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <form-login
                    check_path="/login_check"
                    login_path="/login"
                    use_forward="false"
                    always_use_default_target_path="false"
                    default_target_path="/"
                    target_path_parameter="_target_path"
                    use_referer="false"
                    failure_path="null"
                    failure_forward="false"
                    username_parameter="_username"
                    password_parameter="_password"
                    csrf_parameter="_csrf_token"
                    intention="authenticate"
                    post_only="true"
                />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('form_login' => array(
                    'check_path'                     => '/login_check',
                    'login_path'                     => '/login',
                    'user_forward'                   => false,
                    'always_use_default_target_path' => false,
                    'default_target_path'            => '/',
                    'target_path_parameter'          => _target_path,
                    'use_referer'                    => false,
                    'failure_path'                   => null,
                    'failure_forward'                => false,
                    'username_parameter'             => '_username',
                    'password_parameter'             => '_password',
                    'csrf_parameter'                 => '_csrf_token',
                    'intention'                      => 'authenticate',
                    'post_only'                      => true,
                )),
            ),
        ));

Redirecionando após Sucesso
---------------------------

Você pode alterar o local para onde o formulário de login redireciona após um login bem-sucedido 
usando opções de configuração diferentes. Por padrão, o formulário irá redirecionar para a URL
solicitada pelo usuário (ou seja, a URL que acionou o formulário de login que está sendo exibido).
Por exemplo, se o usuário solicitou ``http://www.example.com/admin/post/18/edit``, então, após 
o login bem-sucedido, ele será enviado de volta para ``http://www.example.com/admin/post/18/edit`` 
. Isto é feito através do armazenamento da URL solicitada 
em sessão. Se nenhuma URL estiver presente na sessão (talvez o usuário acessou
diretamente a página de login), então, o usuário é redirecionado para a página padrão,
que é ``/`` (ou seja, a homepage). Você pode alterar esse comportamento
de várias formas.

.. note::

    Como mencionado, por padrão, o usuário é redirecionado para a página que ele solicitou
    originalmente. Às vezes, isso pode causar problemas, como, por exemplo, uma requisição AJAX 
    em background "aparece" sendo a última URL visitada, fazendo com que o usuário seja
    redirecionado para lá. Para informações sobre como controlar esse comportamento, consulte
    :doc:`/cookbook/security/target_path`.

Alterando a Página Padrão
~~~~~~~~~~~~~~~~~~~~~~~~~

Primeiro, a página padrão pode ser definida (ou seja, a página para a qual o usuário será 
redirecionado se nenhuma página anterior foi armazenada em sessão). Para configurá-la para 
``/admin`` use a seguinte configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    form_login:
                        # ...
                        default_target_path: /admin

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <form-login
                    default_target_path="/admin"                    
                />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('form_login' => array(
                    ...,
                    'default_target_path' => '/admin',
                )),
            ),
        ));

Agora, quando não houver uma URL definida na sessão, os usuários serão enviados para ``/admin``.

Sempre Redirecionar para a Página Padrão
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você pode fazer com que os usuários sejam sempre redirecionados para a página padrão,
independentemente da URL que eles tenham solicitado anteriormente, definindo a 
a opção ``always_use_default_target_path`` para true:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    form_login:
                        # ...
                        always_use_default_target_path: true
                        
    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <form-login
                    always_use_default_target_path="true"
                />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('form_login' => array(
                    ...,
                    'always_use_default_target_path' => true,
                )),
            ),
        ));

Usando a URL de referência (Referring URL)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No caso de nenhuma URL anterior estar armazenada em sessão, você pode desejar usar
o ``HTTP_REFERER`` no lugar, pois este, muitas vezes, é o mesmo. Você pode fazer
isso configurando o ``use_referer`` para true (o padrão é false): 

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    form_login:
                        # ...
                        use_referer:        true

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <form-login
                    use_referer="true"
                />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('form_login' => array(
                    ...,
                    'use_referer' => true,
                )),
            ),
        ));

.. versionadded:: 2.1
    A partir da versão 2.1, se o referer for igual à opção ``login_path``, 
    o usuário será redirecionado para ``default_target_path``.

Controlando a URL de redirecionamento dentro do Formulário
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode sobrescrever para onde o usuário será redirecionado, através do próprio 
formulário incluindo um campo oculto com o nome ``_target_path``. Por exemplo, para
redirecionar para a URL definida por rota ``account``, use o seguinte:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/SecurityBundle/Resources/views/Security/login.html.twig #}
        {% if error %}
            <div>{{ error.message }}</div>
        {% endif %}

        <form action="{{ path('login_check') }}" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="{{ last_username }}" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <input type="hidden" name="_target_path" value="account" />

            <input type="submit" name="login" />
        </form>

    .. code-block:: html+php

        <!-- src/Acme/SecurityBundle/Resources/views/Security/login.html.php -->
        <?php if ($error): ?>
            <div><?php echo $error->getMessage() ?></div>
        <?php endif; ?>

        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="<?php echo $last_username ?>" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <input type="hidden" name="_target_path" value="account" />
            
            <input type="submit" name="login" />
        </form>

Agora, o usuário será redirecionado para o valor do campo oculto do formulário. O
valor do atributo pode ser um caminho relativo, uma URL absoluta, ou um nome de rota. 
Você pode até mesmo alterar o nome do campo oculto do formulário, alterando a opção 
``target_path_parameter`` para um outro valor.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    form_login:
                        target_path_parameter: redirect_url

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <form-login
                    target_path_parameter="redirect_url"
                />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('form_login' => array(
                    'target_path_parameter' => redirect_url,
                )),
            ),
        ));

Redirecionando quando o Login Falhar
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Além de redirecionar o usuário após um login bem-sucedido, você também pode definir
a URL que o usuário deve ser redirecionado após uma falha de login (por exemplo, quando um
nome de usuário ou senha inválida foi submetida). Por padrão, o usuário é redirecionado
de volta ao formulário de login. Você pode definir isso para uma URL diferente com a
seguinte configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    form_login:
                        # ...
                        failure_path: /login_failure
                        
    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <form-login
                    failure_path="login_failure"
                />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('form_login' => array(
                    ...,
                    'failure_path' => login_failure,
                )),
            ),
        ));
