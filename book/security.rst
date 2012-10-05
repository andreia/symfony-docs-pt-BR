Segurança
=========

Segurança é um processo em dois passos principais. Seu objetivo é evitar que
um usuário tenha acesso a um recurso que ele não deveria ter.

No primeiro passo do processo, o sistema de segurança identifica quem o usuário é
exigindo que o mesmo envie algum tipo de identificação. Este primeiro passo é
chamado **autenticação** e signifcica que o sistema está tentando identificar
que é o usuário.

Uma vez que o sistema sabe quem está acessando, o próximo passo é determinar se o
usuário pode acessar determinado recurso. Este segundo passo é chamado
de **autorização** e significa que o sistema irá checar se o usuário tem permissão para
executar determinada ação.

.. image:: /images/book/security_authentication_authorization.png
   :align: center

Como a melhor maneira de aprender é com um exemplo, vamos para ele.

.. note::

    O `componente de segurança`_ do Symfony está disponível como uma biblioteca
    PHP podendo ser utilizada em qualquer projeto PHP.

Exemplo: Autenticação Básica HTTP
---------------------------------

O componente de segurança pode ser configurado através da configuração de
 sua aplicação. Na verdade, a maioria dos esquemas comuns de segurança podem
 ser conseguidos apenas configurando adequadamente sua aplicação. A configuração
 a seguir diz ao Symfony para proteger qualquer URL que satisfaça ``/admin/*``
 através da autenticação básica HTTP, solicitando do usuário credenciais (login/senha).

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    pattern:    ^/
                    anonymous: ~
                    http_basic:
                        realm: "Secured Demo Area"

            access_control:
                - { path: ^/admin, roles: ROLE_ADMIN }

            providers:
                in_memory:
                    memory:
                        users:
                            ryan:  { password: ryanpass, roles: 'ROLE_USER' }
                            admin: { password: kitten, roles: 'ROLE_ADMIN' }

            encoders:
                Symfony\Component\Security\Core\User\User: plaintext

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="secured_area" pattern="^/">
                    <anonymous />
                    <http-basic realm="Secured Demo Area" />
                </firewall>

                <access-control>
                    <rule path="^/admin" role="ROLE_ADMIN" />
                </access-control>

                <provider name="in_memory">
                    <memory>
                        <user name="ryan" password="ryanpass" roles="ROLE_USER" />
                        <user name="admin" password="kitten" roles="ROLE_ADMIN" />
                    </memory>
                </provider>

                <encoder class="Symfony\Component\Security\Core\User\User" algorithm="plaintext" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern' => '^/',
                    'anonymous' => array(),
                    'http_basic' => array(
                        'realm' => 'Secured Demo Area',
                    ),
                ),
            ),
            'access_control' => array(
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
            'providers' => array(
                'in_memory' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array('password' => 'ryanpass', 'roles' => 'ROLE_USER'),
                            'admin' => array('password' => 'kitten', 'roles' => 'ROLE_ADMIN'),
                        ),
                    ),
                ),
            ),
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => 'plaintext',
            ),
        ));

.. tip::

    A distribuição padrão do Symfony coloca a configuração de segurança em
    um arquivo separado (e.g. ``app/config/security.yml``). Se você não
    tem um arquivo separado para as configurações de segurança, pode colocar
    diretamente no arquivo de configuração principal (por exemplo, ``app/config/config.yml``).

O resultado final desta configuração é um completo sistema de segurança funcional
com as seguintes características:

* Há dois usuários no sistema (``ryan`` e ``admin``);
* Os usuários se autenticam através da janela de autenticação básica HTTP;
* Qualquer URL que comece com ``/admin/*`` será protegida e somente o usuário ``admin``
  terá acesso;
* Todas URLs que *não* comecem com ``/admin/*`` são acessíveis a todos usuários
  (e ao usuário nunca serão solicitadas as credenciais de acesso).

Vamos dar uma olhada como funciona a segurança e como cada parte da configuração influencia
no sistema.

Como funciona a segurança: Autenticação e Autorização
-----------------------------------------------------

O sistema de segurança do Symfony funciona determinando quem um usuário é (autenticação)
e depois checando se o usuário tem acesso ao recurso específico ou URL solicitado.

Firewalls (Autenticação)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando um usuário requisita uma URL que está protegida por um firewall,
o sistema de segurança é ativado. O trabalho do firewall é determinar
se o usuário precisa ou não ser autenticado. Se ele precisar, envia a resposta
de volta e inicia o processo de autenticação.

Um firewall será ativado quando a URL requisitada corresponda ao ``padrão de caracteres`` da
expressão regular configurada na configuração de segurança. Neste exemplo, o
``padrão de caracteres`` (``^/``) corresponde a qualquer solicitação. O fato do
firewall ser ativado *não* significa, porém, que a janela de autenticação básica HTTP
(solicitando login e senha) será exibida para todas requisições. Por exemplo,
qualquer usuário poderá acessar ``/foo`` sem que seja solicitada sua autenticação.

.. image:: /images/book/security_anonymous_user_access.png
   :align: center

Isto funciona primeiramente por que o firewall permite *usuários anônimos* através
do parâmetro ``anonymous`` da configuração. Em outras palavras,  o firewall não
exige que o usuário se autentique completamente. E por que nenhum ``perfil`` (``role``)
é necessário para acessar ``/foo`` (na seção ``access_control``), a solicitação
pode ser realizada sem que o usuário sequer se identifique.

Se você remover a chave ``anonymous``, o firewall *sempre* fará o usuário se identificar
por completo imediatamente.

Controles de acesso (Autorização)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se o usuário solicitar ``/admin/foo``, porém, o processo toma um rumo diferente.
Isto acontecerá por que a seção ``access_control`` da configuração indica
que qualquer URL que se encaixe no padrão de caracteres ``^/admin`` (isto é, ``/admin``
ou qualquer coisa do tipo ``/admin/*``) deve ser acessada somente por usuários
com o perfil ``ROLE_ADMIN``. Perfis são a base para a maioria das autorizações:
o usuário pode acessar ``/admin/foo`` somente se tiver o perfil ``ROLE_ADMIN``.

.. image:: /images/book/security_anonymous_user_denied_authorization.png
   :align: center

Como antes, o firewall não solicita credenciais de acesso. Assim que a camada
de controle de acesso nega o acesso (por que o usuário não tem o perfil ``ROLE_ADMIN``),
porém, o firewall inicia o processo de autenticação. Este processo depende
do mecanismo de autenticação que estiver utilizando. Por exemplo, se estiver utilizando
o método de formulário de autenticação (``form login``), o usuário será redirecionado
para a página de login. Se estiver utilizando o método básico de autenticação HTTP,
o navegador recebe uma resposta do tipo HTTP 401 para que ao usuário seja exibida
a janela de login/senha do navegador.

O usuário agora tem a oportunidade de digitar suas credenciais no aplicativo. Se as
credenciais forem válidas, a requisição original será solicitada novamente.

.. image:: /images/book/security_ryan_no_role_admin_access.png
   :align: center

No exemplo, o usuário ``ryan`` se autentica com sucesso pelo firewall. Como, porém,
``ryan`` não tem o perfil ``ROLE_ADMIN``, ele ainda terá seu acesso negado ao
recurso ``/admin/foo``. Infelizmente, isto significa que o usuário verá
uma mensagem indicando que o acesso foi negado.

.. tip::

    Quando o Symfony nega acesso a um usuário, o usuário vê uma tela de
    erro e o navegador recebe uma resposta com o HTTP status code 403 (``Forbidden``).
    É possível personalizar a tela de erro de acesso negado seguindo as
    instruções em :ref:`Error Pages<cookbook-error-pages-by-status-code>` do
    do texto do Symfony 2 - Passo-a-passo que ensina a personalizar a página
    de erro 403.

Finalmente, se o usuário ``admin`` requisitar ``/admin/foo``, um processo similar
entra em ação, mas neste caso, após a autenticação, a camada de controle de acesso
permitirá que a requisição seja completada:

.. image:: /images/book/security_admin_role_access.png
   :align: center

O fluxo de requisição quando um usuário solicita um recurso protegido é direto,
mas muito flexível. Como verá mais tarde, a autenticação pode acontecer de
diversas maneiras, incluindo formulário de login, certificado X.509, ou
autenticação pelo Twitter. Independente do método de autenticação, o fluxo
de requisiçao é sempre o mesmo:

#. Um usuário acessa um recurso protegido;
#. O aplicativo redireciona o usuário para o formulário de login;
#. O usuário envia suas credenciais (e.g. login/senha);
#. O firewall autentica o usuário;
#. O usuário autenticado é redirecionado para o recurso solicitado originalmente.

.. note::

    O processo *exato* na verdade depende um pouco do mecanismo de autenticação
    que estiver usando. Por exemplo, quando estiver utilizando formulário de login,
    o usuário envia suas credenciais para a URL que processa o formulário (por exemplo,
    ``/login_check``) e depois é redirecionado de volta para a URL solicitada
    originalmente (por exemplo, ``/admin/foo``). Se utilizar autenticação básica
    HTTP, porém, o usuário envia suas credenciais diretamente para a URL original
    (por exemplo, ``/admin/foo``) e depois a página é retornada para o usuário
    na mesma requisição (isto significa que não há redirecionamentos).

    Estes detalhes técnicos não devem ser relevantes no uso do sistema de
    segurança, mas é bom ter uma idéia a respeito.

.. tip::

    Você aprenderá mais tarde como *qualquer coisa* pode ser protegida no Symfony2,
    incluindo controladores específicos, objetos, ou até métodos PHP.

.. _book-security-form-login:

Usando um formulário de login em HTML
-------------------------------------

Até agora, você viu como cobrir seu aplicativo depois do firewall e assim
restringir o acesso de certas áreas a certos perfis. Utilizando a autenticação
básica HTTP, é possível, sem esforços, submeter login/senha através da
janela do navegador. O Symfony, porém, suporta de fábrica muitos outros
mecanismos de autenticação. Para detalhes sobre todos eles, consulte
:doc:`Referência Da Configuração De Segurança</reference/configuration/security>`.

Nesta seção, você aprimorará o processo permitindo que o usuário se autentique através
de um formulário de login tradicional em HTML.

Primeiro habilite o formulário no seu firewall:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    pattern:    ^/
                    anonymous: ~
                    form_login:
                        login_path:  /login
                        check_path:  /login_check

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="secured_area" pattern="^/">
                    <anonymous />
                    <form-login login_path="/login" check_path="/login_check" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern' => '^/',
                    'anonymous' => array(),
                    'form_login' => array(
                        'login_path' => '/login',
                        'check_path' => '/login_check',
                    ),
                ),
            ),
        ));

.. tip::

    Se não precisar de personlizar os valores de ``login_path`` ou ``check_path``
    (os valores utilizados acima são os valores padrão), você pode encurtar
    seu configuração:

    .. configuration-block::

        .. code-block:: yaml

            form_login: ~

        .. code-block:: xml

            <form-login />

        .. code-block:: php

            'form_login' => array(),

Agora, quando o sistema de segurança inicia o processo de autenticação,
ele redirecionará o usuário para o formulário de login (``/login`` por padrão).
É sua tarefa implementar o visual desse formulário. Primeiro, crie duas rotas:
uma para a exibição do formulário de login (no caso, ``/login``) e outra
para processar a submissão do formulário (no caso, ``/login_check``):

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        login:
            pattern:   /login
            defaults:  { _controller: AcmeSecurityBundle:Security:login }
        login_check:
            pattern:   /login_check

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="login" pattern="/login">
                <default key="_controller">AcmeSecurityBundle:Security:login</default>
            </route>
            <route id="login_check" pattern="/login_check" />

        </routes>

    ..  code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('login', new Route('/login', array(
            '_controller' => 'AcmeDemoBundle:Security:login',
        )));
        $collection->add('login_check', new Route('/login_check', array()));

        return $collection;

.. note::

    *Não* é preciso implementar o controller para a URL ``/login_check``
    pois o firewall interceptará e processará o que foi submitido para essa URL.
    É opcional, porém útil, criar uma rota para que você possa gerar o link
    de submissão na template do formulário de login.

.. versionadded:: 2.1
    Com o Symfony 2.1, você *deve* possuir rotas configuradas para suas URLs ``login_path``
    (ex. ``/login``), ``check_path`` (ex. ``/login_check``) e ``logout``
    (ex. ``/logout`` - veja `Logging Out`_).

Observe que o nome da rota ``login`` não é importante. O que importa é que a
URL da rota corresponda o que foi colocado na configuração  ``login_path``, pois
é para onde o sistema de segurança redirecionará os usuários que precisarem
se autenticar.

O próximo passo é criar o controller que exibirá o formulário de login:

.. code-block:: php

    // src/Acme/SecurityBundle/Controller/Main;
    namespace Acme\SecurityBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\Security\Core\SecurityContext;

    class SecurityController extends Controller
    {
        public function loginAction()
        {
            $request = $this->getRequest();
            $session = $request->getSession();

            // get the login error if there is one
            if ($request->attributes->has(SecurityContext::AUTHENTICATION_ERROR)) {
                $error = $request->attributes->get(SecurityContext::AUTHENTICATION_ERROR);
            } else {
                $error = $session->get(SecurityContext::AUTHENTICATION_ERROR);
            }

            return $this->render('AcmeSecurityBundle:Security:login.html.twig', array(
                // last username entered by the user
                'last_username' => $session->get(SecurityContext::LAST_USERNAME),
                'error'         => $error,
            ));
        }
    }

Não se confunda com esse controller. Como verá, quando o usuário submete o formulário,
o sistema de segurança automaticamente processar a submissão para você. Se o usuário
entrou com login e/ou senha inválidos, este controller pega o erro ocorrido do sistema
de segurança para poder exibir ao usuário.

Em outras palavras, seu trabalho é exibir o formulário de login e qualquer
erro ocorrido durante a tentativa de autenticação, mas o sistema de segurança
já toma conta de checar se as credenciais são válidas e de autenticar o usuário.

Finalmente crie a template correspondente:

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

            {#
                If you want to control the URL the user is redirected to on success (more details below)
                <input type="hidden" name="_target_path" value="/account" />
            #}

            <input type="submit" name="login" />
        </form>

    .. code-block:: html+php

        <?php // src/Acme/SecurityBundle/Resources/views/Security/login.html.php ?>
        <?php if ($error): ?>
            <div><?php echo $error->getMessage() ?></div>
        <?php endif; ?>

        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="<?php echo $last_username ?>" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <!--
                If you want to control the URL the user is redirected to on success (more details below)
                <input type="hidden" name="_target_path" value="/account" />
            -->

            <input type="submit" name="login" />
        </form>

.. tip::

    A variável ``error`` passada para a template é uma instância de
    :class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`.
    Esta pode conter mais informações - ou até informações sensíveis - sobre a
    falha na autenticação, por isso use-a com sabedoria!

O formulário tem que atender alguns requisitos. Primeiro, ao submeter o formulário
para ``/login_check`` (através da rota  ``login_check``), o sistema de segurança
interceptará a submissão do formulário e o processará. Segundo, o sistema de segurança
espera que os campos submetidos sejam chamados ``_username`` e ``_password``
(estes nomes podem ser :ref:`configured<reference-security-firewall-form-login>`).

E é isso! Quando submeter um formulário, o sistema de segurança irá automaticamente
checar as credenciais do usuário e autenticá-lo ou enviar o ele de volta ao
formulário de login para o erro ser exibido.

Vamos revisar o processo inteiro:

#. O usuário tenta acessar um recurso que está protegido;
#. O firewall inicia o processo de autenticação redirecionando o
   usuário para o formulário de login(``/login``);
#. A página ``/login`` produz o formulário de login através da rota
   e controlador criados neste exemplo;
#. O usuário submete o formulário de login para ``/login_check``;
#. O sistema de segurança intercepta a solicitação, verifica as credenciais
   submetidas pelo usuário, autentica o mesmo se tiverem corretas ou envia
   de volta para o formulário de login caso contrário;

Por padrão, se as credenciais estiverem corretas, o usuário será redirecionado
para a página que solicitou originalmente (e.g. ``/admin/foo``). Se o usuário
originalmente solicitar a página de login, ele será redirecionado para a página
principal. Isto pode ser modificado se necessário, o que permitiria você
redirecionar o usuário para um outra URL específica.

Para maiores detalhes sobre isso e como personalizar o processamento do
formulário de login acesse
:doc:`/cookbook/security/form_login`.

.. _book-security-common-pitfalls:

.. sidebar:: Evite os erros comuns

    Quando estiver configurando seu formulário de login, fique atendo
    aos seguintes erros comuns.

    **1. Crie as rotas corretas**

    Primeiro, tenha certeza que definiu as rotas ``/login`` e ``/login_check``
    corretamente e que elas correspondem aos calores das configurações
    ``login_path`` e ``check_path``. A configuração errada pode significar que
    você será redirecionado para a página de erro 404 ao invés da página
    de login ou a submissão do formulário de login não faça nada (você
    sempre vê o formulário sem sair dele).

    **2. Tenha certeza que a página de login não é protegida**

    Também tenha certeza que a página de login *não* precisa de qualquer
    perfil para ser vizualizada. Por exemplo, a seguinte configuração,
    que exige o perfil ``ROLE_ADMIN`` para todas as URLs (incluindo a
    URL ``/login``), causará um redirecionamento circular:

    .. configuration-block::

        .. code-block:: yaml

            access_control:
                - { path: ^/, roles: ROLE_ADMIN }

        .. code-block:: xml

            <access-control>
                <rule path="^/" role="ROLE_ADMIN" />
            </access-control>

        .. code-block:: php

            'access_control' => array(
                array('path' => '^/', 'role' => 'ROLE_ADMIN'),
            ),

    Removendo o controle de acesso para a URL ``/login`` resolve o problema:

    .. configuration-block::

        .. code-block:: yaml

            access_control:
                - { path: ^/login, roles: IS_AUTHENTICATED_ANONYMOUSLY }
                - { path: ^/, roles: ROLE_ADMIN }

        .. code-block:: xml

            <access-control>
                <rule path="^/login" role="IS_AUTHENTICATED_ANONYMOUSLY" />
                <rule path="^/" role="ROLE_ADMIN" />
            </access-control>

        .. code-block:: php

            'access_control' => array(
                array('path' => '^/login', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY'),
                array('path' => '^/', 'role' => 'ROLE_ADMIN'),
            ),

    Além disso, se o seu firewall *não* permite usuários anônimos, você precisará
    criar um firewall especial para permitir usuários anônimos para a página de login:

    .. configuration-block::

        .. code-block:: yaml

            firewalls:
                login_firewall:
                    pattern:    ^/login$
                    anonymous:  ~
                secured_area:
                    pattern:    ^/
                    form_login: ~

        .. code-block:: xml

            <firewall name="login_firewall" pattern="^/login$">
                <anonymous />
            </firewall>
            <firewall name="secured_area" pattern="^/">
                <form_login />
            </firewall>

        .. code-block:: php

            'firewalls' => array(
                'login_firewall' => array(
                    'pattern' => '^/login$',
                    'anonymous' => array(),
                ),
                'secured_area' => array(
                    'pattern' => '^/',
                    'form_login' => array(),
                ),
            ),

    **3. Tenha certeza que ``/login_check`` está protegida por um firewall**

    Certifique-se que a URL indicada em ``check_path`` (no caso, ``/login_check``)
    esteja protegida por um firewall que está utilizando seu formulário de login
    (neste exemplo, um único firewall filtra *todas* as URLs, incluindo ``/login_check``).
    Se ``/login_check`` não estiver atrás de nenhum firewall, uma exceção
    será gerada ``Unable to find the controller for path "/login_check"``.

    **4. Múltiplos firewalls não compartilham o mesmo contexto de segurança**

    Se estiver utilizando múltiplos firewalls e se autenticar em um firewall,
    você *não* estará autenticado nos outros firewalls automaticamente.
    Firewalls diferentes funcionam como sistemas de segurança diferente. Isto
    acontece por que para a maioria dos aplicativos ter somente um
    firewall é o suficiente.

Autorização
-------------

O primeiro passo na segurança é sempre a autenticação: o processo de verficar quem
o usuário é. No Symfony, a autenticação pode ser feita de várias maneiras -  via
formulário de login, autenticação básica HTTP ou até mesmo pelo Facebook.

Uma vez que o usuário está autenticado, a autorização começa. Autorização fornece
uma maneira padrão e poderosa de decidir se o usuário pode acessar algum recurso
(uma  URL, um objeto do modelo, um método...). Isto funciona com perfis atribuídos
para cada usuário e exigindo perfis diferentes para diferentes recursos.

O processo de autorização tem dois lados diferentes:

#. O usuário tem um conjunto de perfis específico;
#. Um recurso requer um perfil específico para ser acessado.

Nesta seção, o foco será em como tornar seguros diferentes recursos (por exemplo URLs,
chamadas a métodos, etc) com diferentes perfis. Mais tarde, você aprenderá mais
como perfis são criados e atribuídos aos usuários.

Protegendo padrões de URLs
~~~~~~~~~~~~~~~~~~~~~~~~~~

A maneira mais básica de proteger seu aplicativo é proteger um padrão de URL.
Você já viu no primeiro exemplo deste capítulo que qualquer requisição que
se encaixasse na expressão regular ``^/admin`` exigiria o perfil ``ROLE_ADMIN``.

Você pode definir quantos padrões precisar. Cada um é uma expressão regular.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            # ...
            access_control:
                - { path: ^/admin/users, roles: ROLE_SUPER_ADMIN }
                - { path: ^/admin, roles: ROLE_ADMIN }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <!-- ... -->
            <rule path="^/admin/users" role="ROLE_SUPER_ADMIN" />
            <rule path="^/admin" role="ROLE_ADMIN" />
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            // ...
            'access_control' => array(
                array('path' => '^/admin/users', 'role' => 'ROLE_SUPER_ADMIN'),
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

.. tip::

    Iniciando o padrão com ``^`` garante que somente URLs *começando* com o padrão
    terá uma comparação positiva. Por exemplo, o padrão simples ``/admin`` (sem
    o ``^``) resultaria em uma comparação positiva para ``/admin/foo``, mas
    também para URLs como ``/foo/admin``.

Para cada requisição que chega, o Symfony2 tenta encontrar uma regra de acesso
correspondente, com comparação positiva do padrão (a primeira que encontrar ganha).
Se o usuário não estiver autenticado ainda, a autenticação é iniciada (isto é,
o usuário tem a chance de fazer login). Se o usuário, porém, já *estiver* autenticado,
mas não tiver o perfil exigido, uma exceção é disparada
:class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException` ,
que você pode tratar e transformar em uma apresentável página de "Acesso Negado"
para o usuário. Veja :doc:`/cookbook/controller/error_pages` para mais informações.

Como o Symfony utiliza a primeira regra de acesso que der uma comparação positiva, uma
URL como ``/admin/users/new`` corresponderá a primeira regra e exigirá somente o perfil
``ROLE_SUPER_ADMIN``. Qualquer URL como ``/admin/blog`` corresponderá a segunda regra e
exigirá o perfil ``ROLE_ADMIN``.

.. _book-security-securing-ip:

Protegendo por IP
~~~~~~~~~~~~~~~~~

Algumas situações podem exigir que você restrinja o acesso de uma determinada rota com base no IP.
Isto é particularmente relevante no caso de :ref:`Edge Side Includes<edge-side-includes>` (ESI),
por exemplo, que utiliza a rota com nome "_internal". Quanto ESI é utilizado, a rota
_internal é requerida pelo gateway cache (gerente de cache) para possibilitar diferentes
opções de caching para subseções dentro de uma determinada página. Esta rota vem com o
prefixo ^/_internal por padrão na edição padrão (assumindo que você ativou estas linhas
do seu arquivo de configuração de rotas - routing.yml).

Aqui está um exemplo de como poderia proteger esta rota de acesso externo:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/_internal, roles: IS_AUTHENTICATED_ANONYMOUSLY, ip: 127.0.0.1 }

    .. code-block:: xml

            <access-control>
                <rule path="^/_internal" role="IS_AUTHENTICATED_ANONYMOUSLY" ip="127.0.0.1" />
            </access-control>

    .. code-block:: php

            'access_control' => array(
                array('path' => '^/_internal', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY', 'ip' => '127.0.0.1'),
            ),

.. _book-security-securing-channel:

Protegendo por canal
~~~~~~~~~~~~~~~~~~~~

Assim como a proteção por IP, exigir o uso de SSL é tão simples quanto adicionar uma
nova entrar em access_control:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            access_control:
                - { path: ^/cart/checkout, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }

    .. code-block:: xml

            <access-control>
                <rule path="^/cart/checkout" role="IS_AUTHENTICATED_ANONYMOUSLY" requires_channel="https" />
            </access-control>

    .. code-block:: php

            'access_control' => array(
                array('path' => '^/cart/checkout', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY', 'requires_channel' => 'https'),
            ),

.. _book-security-securing-controller:

Protegendo um Controller
~~~~~~~~~~~~~~~~~~~~~~~~

Proteger seu aplicativo baseado em padrões de URL é fácil, mas este método pode não ser
específico o bastante em certos casos. Quando necessário, você pode ainda facilmente
forçar autorização de dentro de um controller:

.. code-block:: php

    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    // ...

    public function helloAction($name)
    {
        if (false === $this->get('security.context')->isGranted('ROLE_ADMIN')) {
            throw new AccessDeniedException();
        }

        // ...
    }

.. _book-security-securing-controller-annotations:

Você pode ainda instalar e utilizar opcionalmente o ``JMSSecurityExtraBundle``,
que te permite proteger controllers através de anotações:

.. code-block:: php

    use JMS\SecurityExtraBundle\Annotation\Secure;

    /**
     * @Secure(roles="ROLE_ADMIN")
     */
    public function helloAction($name)
    {
        // ...
    }

Para mais informações, veja a documentação `JMSSecurityExtraBundle`_ . Se você
a distribuição Standard do Symfony, este bundle está habilitado por padrão.
Se não estiver, você pode facilmente baixar e instalá-lo.

Protegendo outros serviços
~~~~~~~~~~~~~~~~~~~~~~~~~~

De fato, qualquer coisa pode ser protegida em Symfony utilizando uma estratégia
similar a apresentada na seção anterior. Por exemplo, suponha que você tem
um serviço (uma classe PHP, por exemplo) que seu trabalho é enviar e-mails
de um usuário para outro. Você pode restringir o uso dessa classe - não
importa de onde está sendo utilizada - a usuários que tenham um perfil específico.

Para mais informações sobre como você pode utilizar o componente de segurança
para proteger diferentes serviços e métodos de seu aplicativo, consulte :doc:`/cookbook/security/securing_services`.

Listas De Controle De Acesso (ACLs): Protegendo Objetos Específicos Do Banco De Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Imagine que você está projetando um sistema de blog onde seus usuários podem
comentar seus posts. Agora, você quer que um usuário tenha a possibilidade de editar
seus próprios comentários, mas não aqueles de outros usuários. Além disso, como
administrador, você quer poder editar *todos* os comentários.

O componente de segurança possui um sistema de listas de controle de acesso (ACL)
que te permite controlar acesso a instâncias individuais de um objeto no seu
sistema. *Sem* ACL, você consegue proteger seu sistema para que
somente usuários específicos possam editar os comentários. *Com* ACL, porém, você
pode restringir ou permitir o acesso por comentário.

Para mais informação, veja o passo-a-passo: :doc:`/cookbook/security/acl`.

Usuários
--------

Nas seções anteriores, você aprendeu como proteger diferentes recursos exigindo
um conjunto de *perfis* para o acesso a um recurso. Nesta seção exploraremos
outro aspecto da autorização: os usuários.

De onde os usuários vêm? (*User Providers*)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Durante a autenticação, o usuário submete um conjunto de credenciais (normalmente
login e senha). O trabalho do sistema de autenticação é verificar essas credenciais
contra um conjunto de usuários. De onde essa lista de usuários vem então?

No Symfony2, usuários podem vir de qualquer lugar - um arquivo de configuração,
um banco de dados, um serviço web ou qualquer outra fonte que desejar. Qualquer
coisa que disponibiliza um ou mais usuários para o sistema de autenticação é conhecido
como "user provider". O Symfony2 vem por padrão com os dois mais comuns: um que
carrega os usuários do arquivo de configuração e outro que carrega os usuários do
banco de dados.

Especificando usuários no arquivo de configuração
.................................................

O jeito mais fácil de especificar usuários é diretamnete no arquivo de configuração.
De fato, você já viu isso em um exemplo neste capítulo.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            # ...
            providers:
                default_provider:
                    memory:
                        users:
                            ryan:  { password: ryanpass, roles: 'ROLE_USER' }
                            admin: { password: kitten, roles: 'ROLE_ADMIN' }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <!-- ... -->
            <provider name="default_provider">
                <memory>
                    <user name="ryan" password="ryanpass" roles="ROLE_USER" />
                    <user name="admin" password="kitten" roles="ROLE_ADMIN" />
                </memory>
            </provider>
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            // ...
            'providers' => array(
                'default_provider' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array('password' => 'ryanpass', 'roles' => 'ROLE_USER'),
                            'admin' => array('password' => 'kitten', 'roles' => 'ROLE_ADMIN'),
                        ),
                    ),
                ),
            ),
        ));

Este *user provider* é chamado de "in-memory" user provider, já que os
usuários não estão armazenados em nenhum banco de dados. O objeto usuário
é fornecido pelo Symfony (:class:`Symfony\\Component\\Security\\Core\\User\\User`).

.. tip::
    Qualquer user provider pode carregar usuários diretamente da configuração se
    especificar o parâmetro de configuração ``users`` e listar os usuários
    abaixo dele.

.. caution::

    Se seu login é todo numérico (``77``, por exemplo) ou contém hífen (``user-name``, por exemplo),
    você deveria utilizar a sintaxe alternativa quando especificar usuários em YAML:

    .. code-block:: yaml

        users:
            - { name: 77, password: pass, roles: 'ROLE_USER' }
            - { name: user-name, password: pass, roles: 'ROLE_USER' }

Para sites menores, este método é rápido e fácil de configurar. Para sistemas mais complexos,
você provavelmente desejará carregar os usuários do banco de dados.

.. _book-security-user-entity:

Carregando usuários do banco de dados
.....................................

Se você desejar carregar seus usuários através do Doctrine ORM, você pode
facilmente o fazer criando uma classe ``User`` e configurando o ``entity`` provider

.. tip:

    Um bundle open source de alta qualidade está disponível que permite armazenar
    seus usuários com Doctrine ORM ou ODM. Leia mais sobre o `FOSUserBundle`_ no GitHub.

Nessa abordagem, você primeiro precisa criar sua própria classe ``User``, que
será persistida no banco de dados.

.. code-block:: php

    // src/Acme/UserBundle/Entity/User.php
    namespace Acme\UserBundle\Entity;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity
     */
    class User implements UserInterface
    {
        /**
         * @ORM\Column(type="string", length="255")
         */
        protected $username;

        // ...
    }

Ao que diz respeito ao sistema de segurança, o único requisito para sua
classe ``User`` personalizada é que ela implemente a interface
:class:`Symfony\\Component\\Security\\Core\\User\\UserInterface` . Isto significa
que conceito de usuário pode ser qualquer um, desde que implemente essa interface.

.. versionadded:: 2.1
    No Symfony 2.1, o método ``equals`` foi removido do ``UserInterface``.
    Se você precisa sobrescrever a implementação default da lógica de comparação,
    implemente a nova interface :class:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface`
    .

.. note::

    O objeto User será serializado e salvo na sessão entre requisições, por isso
    é recomendado que você `implemente a interface \Serializable`_ em sua classe User.
    Isto é especialmente importante se sua classe ``User`` tem uma classe pai com
    propriedades private.

Em seguida, configure um user provider ``entity`` e aponte-o para sua classe ``User``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            providers:
                main:
                    entity: { class: Acme\UserBundle\Entity\User, property: username }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <provider name="main">
                <entity class="Acme\UserBundle\Entity\User" property="username" />
            </provider>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'main' => array(
                    'entity' => array('class' => 'Acme\UserBundle\Entity\User', 'property' => 'username'),
                ),
            ),
        ));

Com a introdução desse novo provider, o sistema de autenticação tentará
carregar o objeto ``User`` do banco de dados a partir do campo ``username`` da classe.

.. note::
    Este exemplo é somente para demonstrar a idéia básica por trás do provider ``entity``.
    Para um exemplo completo, consulte :doc:`/cookbook/security/entity_provider`.

Para mais informações sobre como criar seu próprio provider (se precisar carregar usuários
do seu serviço web por exemplo), consulte :doc:`/cookbook/security/custom_provider`.

Protegendo a senha do usuário
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Até agora, por simplicidade, todos os exemplos armazenavam as senhas dos usuários
em texto puro (sendo armazenados no arquivo de configuração ou no banco de dados).
Claro que em um aplicativo profissional você desejará proteger as senhas dos seus
usuários por questões de segurança. Isto é facilmente conseguido mapeando sua
classe User para algum "encoder" disponível. Por exemplo, para armazenar seus
usuário em memória, mas proteger a senha deles através da função de hash `sha1``,
faça o seguinte:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            # ...
            providers:
                in_memory:
                    memory:
                        users:
                            ryan:  { password: bb87a29949f3a1ee0559f8a57357487151281386, roles: 'ROLE_USER' }
                            admin: { password: 74913f5cd5f61ec0bcfdb775414c2fb3d161b620, roles: 'ROLE_ADMIN' }

            encoders:
                Symfony\Component\Security\Core\User\User:
                    algorithm:   sha1
                    iterations: 1
                    encode_as_base64: false

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <!-- ... -->
            <provider name="in_memory">
                <memory>
                    <user name="ryan" password="bb87a29949f3a1ee0559f8a57357487151281386" roles="ROLE_USER" />
                    <user name="admin" password="74913f5cd5f61ec0bcfdb775414c2fb3d161b620" roles="ROLE_ADMIN" />
                </memory>
            </provider>

            <encoder class="Symfony\Component\Security\Core\User\User" algorithm="sha1" iterations="1" encode_as_base64="false" />
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            // ...
            'providers' => array(
                'in_memory' => array(
                    'memory' => array(
                        'users' => array(
                            'ryan' => array('password' => 'bb87a29949f3a1ee0559f8a57357487151281386', 'roles' => 'ROLE_USER'),
                            'admin' => array('password' => '74913f5cd5f61ec0bcfdb775414c2fb3d161b620', 'roles' => 'ROLE_ADMIN'),
                        ),
                    ),
                ),
            ),
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => array(
                    'algorithm'         => 'sha1',
                    'iterations'        => 1,
                    'encode_as_base64'  => false,
                ),
            ),
        ));

Ao definir ``iterations`` como ``1`` e ``encode_as_base64`` como false, a senha codificada
é simplesmente obtida como o resultado de ``sha1`` após uma iteração apenas, sem codificação
extra. Você pode agora calcular a senha codificada por código PHP (e.g. ``hash('sha1', 'ryanpass')``)
ou através de alguma ferramenta online como `functions-online.com`_ .

Se você estiver criando seus usuário dinamicamente e os armazenando no banco de dados,
você pode usar algoritmos the hash ainda mais complexos e então delegar em um objeto
encoder para ajudar a codificar as senhas. Por exemplo, suponha que seu objeto User é
``Acme\UserBundle\Entity\User`` (como no exemplo acima). Primeiro, configure o encoder para
aquele usuário:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            # ...

            encoders:
                Acme\UserBundle\Entity\User: sha512

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <!-- ... -->

            <encoder class="Acme\UserBundle\Entity\User" algorithm="sha512" />
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            // ...

            'encoders' => array(
                'Acme\UserBundle\Entity\User' => 'sha512',
            ),
        ));

Neste caso, você está utilizando um algoritmo mais forte ``sha512``. Além disso,
desde que você especificou o algoritmo (``sha512``) como um texto, o sistema
irá, por padrão, utilizar a função de hash 5000 vezes em uma linha e então o codificar
como base64. Em outras palavras, a senha foi muito codificada de maneira que a senha
não pode ser decodificada (isto é, você não pode determinar qual a senha
a partir da senha codificada).

Se você tem alguma espécie de formulário de registro para os visitantes, você precisará
a senha codificada para poder armazenar. Não importa o algoritmo que configurar para
sua classe User, a senha codificada pode sempre ser determinada da seguinte maneira
a partir de um controller:

.. code-block:: php

    $factory = $this->get('security.encoder_factory');
    $user = new Acme\UserBundle\Entity\User();

    $encoder = $factory->getEncoder($user);
    $password = $encoder->encodePassword('ryanpass', $user->getSalt());
    $user->setPassword($password);

Obtendo o objeto User
~~~~~~~~~~~~~~~~~~~~~

Após a autenticação, o objeto ``User`` do usuário atual pode ser acessado através
do serviço ``security.context``. De dentro de um controller, faça o seguinte:

.. code-block:: php

    public function indexAction()
    {
        $user = $this->get('security.context')->getToken()->getUser();
    }

No controller, também existe o atalho:

.. code-block:: php

    public function indexAction()
    {
        $user = $this->getUser();
    }


.. note::

    Usuários anônimos são tecnicamente autenticados, significando que o médodo ``isAuthenticated()``
    de um objeto User autenticado anonimamente retornará verdadeiro. Para verificar se seu usuário está
    realmente autenticado, verifique se o perfil ``IS_AUTHENTICATED_FULLY`` está atribuído ao mesmo.

Utilizando múltiplos User Providers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cada mecanismo de autenticação (exemplos: Autenticação HTTP, formulário de login, etc)
usa exatamente um user provider, e utilizará, por padrão, o primeiro user provider configurado. O que acontece
se você quiser que alguns de seus usuários sejam autenticados por arquivo de configuração e o
resto por banco de dados? Isto é possível criando um novo user provider que ativa os dois juntos:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            providers:
                chain_provider:
                    chain:
                        providers: [in_memory, user_db]
                in_memory:
                    users:
                        foo: { password: test }
                user_db:
                    entity: { class: Acme\UserBundle\Entity\User, property: username }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <provider name="chain_provider">
                <chain>
                    <provider>in_memory</provider>
                    <provider>user_db</provider>
                </chain>
            </provider>
            <provider name="in_memory">
                <user name="foo" password="test" />
            </provider>
            <provider name="user_db">
                <entity class="Acme\UserBundle\Entity\User" property="username" />
            </provider>
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'chain_provider' => array(
                    'chain' => array(
                        'providers' => array('in_memory', 'user_db'),
                    ),
                ),
                'in_memory' => array(
                    'users' => array(
                        'foo' => array('password' => 'test'),
                    ),
                ),
                'user_db' => array(
                    'entity' => array('class' => 'Acme\UserBundle\Entity\User', 'property' => 'username'),
                ),
            ),
        ));

Agora, todos mecanismos de autenticação utilizarão o ``chain_provider``, já que é
o primeiro configurado. O ``chain_provider`` tentará carregar o usuário de ambos
providers ``in_memory`` e ``user_db``.

.. tip::

    Se você não tem razões para separar seus usuários ``in_memory`` dos seus
    usuários ``user_db``, você pode conseguir o mesmo resultado mais facilmente,
    combinando as duas origens em um único provider:

    .. configuration-block::

        .. code-block:: yaml

            # app/config/security.yml
            security:
                providers:
                    main_provider:
                        memory:
                            users:
                                foo: { password: test }
                        entity:
                            class: Acme\UserBundle\Entity\User,
                            property: username

        .. code-block:: xml

            <!-- app/config/config.xml -->
            <config>
                <provider name=="main_provider">
                    <memory>
                        <user name="foo" password="test" />
                    </memory>
                    <entity class="Acme\UserBundle\Entity\User" property="username" />
                </provider>
            </config>

        .. code-block:: php

            // app/config/config.php
            $container->loadFromExtension('security', array(
                'providers' => array(
                    'main_provider' => array(
                        'memory' => array(
                            'users' => array(
                                'foo' => array('password' => 'test'),
                            ),
                        ),
                        'entity' => array('class' => 'Acme\UserBundle\Entity\User', 'property' => 'username'),
                    ),
                ),
            ));

Você pode ainda configurar o firewall ou mecanismos de autenticação individuais para utilizar
um user provider específico. Novamente, a menos que um provider seja especificado explicitamente,
o primeiro será sempre utilizado:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            firewalls:
                secured_area:
                    # ...
                    provider: user_db
                    http_basic:
                        realm: "Secured Demo Area"
                        provider: in_memory
                    form_login: ~

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <firewall name="secured_area" pattern="^/" provider="user_db">
                <!-- ... -->
                <http-basic realm="Secured Demo Area" provider="in_memory" />
                <form-login />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'provider' => 'user_db',
                    'http_basic' => array(
                        // ...
                        'provider' => 'in_memory',
                    ),
                    'form_login' => array(),
                ),
            ),
        ));

Neste exemplo, se um usuário tentar se autenticar através de autenticação HTTP, o sistema
utilizará o user provider  ``in_memory``. Se o usuário tentar, porém, se autenticar através
do formulário de login, o provider ``user_db`` será usado (pois é o padrão para todo o firewall).

Para mais informações sobre a configuração do user provider e do firewall,
veja :doc:`/reference/configuration/security`.

Perfis (Roles)
--------------

A idéia de um "perfil" é chave no processo de autorização. Para cada usuário é atribuído
um conjunto de perfis e então cada recurso exige um ou mais perfis. Se um usuário
tem os perfis requeridos, o acesso é concedido. Caso contrário, o acesso é negado.

Perfis são muito simples e basicamente textos que você pode inventar e utilizar
de acordo com suas necessidades (embora perfis sejam objetos PHP internamente).
Por exemplo, se precisar limitar acesso a uma seção administrativa do blog
de seu website, você pode proteger a seção utilizando o perfil ``ROLE_BLOG_ADMIN``.
Este perfil não precisa de estar definido em lugar nenhum - você pode simplesmente
usar o mesmo.

.. note::

    Todos os perfis **devem** começar com o prefixo ``ROLE_`` para serem gerenciados
    pelo Symfony2. Se você definir seus próprios perfis com uma classe ``Role``
    dedicada (mais avançado), não utilize o prefixo ``ROLE_``.

Hierarquia de Perfis
~~~~~~~~~~~~~~~~~~~~

Ao invés de associar muitos perfis aos usuários, você pode defined regras
de herança ao criar uma hierarquia de perfis:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            role_hierarchy:
                ROLE_ADMIN:       ROLE_USER
                ROLE_SUPER_ADMIN: [ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH]

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <role id="ROLE_ADMIN">ROLE_USER</role>
            <role id="ROLE_SUPER_ADMIN">ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH</role>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'role_hierarchy' => array(
                'ROLE_ADMIN'       => 'ROLE_USER',
                'ROLE_SUPER_ADMIN' => array('ROLE_ADMIN', 'ROLE_ALLOWED_TO_SWITCH'),
            ),
        ));

Na configuração acima, usuários com o perfil ``ROLE_ADMIN`` terão também o perfil ``ROLE_USER``.
O perfil ``ROLE_SUPER_ADMIN`` tem os ``ROLE_ADMIN``, ``ROLE_ALLOWED_TO_SWITCH`` e ``ROLE_USER``
(herdado do ``ROLE_ADMIN``).

Saindo do sistema
-----------------

Normalmente, você também quer que seus usuários possam sair do sistema.
Felizmente, o firewall consegue lidar com isso automaticamente quando o parâmetro
de configuração ``logout`` está ativo:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        security:
            firewalls:
                secured_area:
                    # ...
                    logout:
                        path:   /logout
                        target: /
            # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <config>
            <firewall name="secured_area" pattern="^/">
                <!-- ... -->
                <logout path="/logout" target="/" />
            </firewall>
            <!-- ... -->
        </config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'logout' => array('path' => 'logout', 'target' => '/'),
                ),
            ),
            // ...
        ));

Uma vez que está configurado no seu firewall, redirecionando o usuário para ``/logout``
(ou qualquer outro caminho que configurar em ``path``), o usuário não estará
mais autenticado. O usuário será então redirecionado para a página principal
(o valor definido no parâmetro ``target``). Ambas configurações ``path`` e ``target``
tem valor padrão iguais ao especificado aqui. Em outras palavras, a menos que
precise personalizar, você pode simplesmente os omitir completamente e simplificar
sua configuração:

.. configuration-block::

    .. code-block:: yaml

        logout: ~

    .. code-block:: xml

        <logout />

    .. code-block:: php

        'logout' => array(),

Note que você *não* precisará implementar o controller para a URL ``/logout``
já que o firewall cuida disso. Você *deve*, entretante, precisar criar uma rota para que
possa usar para gerar a URL:

.. warning::

    Com o Symfony 2.1, você *deve* ter uma rota que corresponde ao seu caminho para 
    logout. Sem esta rota, o logout não irá funcionar.

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        logout:
            pattern:   /logout

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="logout" pattern="/logout" />

        </routes>

    ..  code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('logout', new Route('/logout', array()));

        return $collection;

Uma vez que o usuário não está mais autenticado, ele será redirecionado para o
que tiver definido no parâmetro ``target``. Para mais informações sobre
a configuração de logout, veja
:doc:`Security Configuration Reference</reference/configuration/security>`.

Controle de Acesso em Templates
-------------------------------

Se você quiser checar se o usuário atual tem um determinado perfil de dentro de
uma template, use a função:

.. configuration-block::

    .. code-block:: html+jinja

        {% if is_granted('ROLE_ADMIN') %}
            <a href="...">Delete</a>
        {% endif %}

    .. code-block:: html+php

        <?php if ($view['security']->isGranted('ROLE_ADMIN')): ?>
            <a href="...">Delete</a>
        <?php endif; ?>

.. note::

    Se você usar esta função e *não* estiver em uma URL que está atrás de um
    firewall ativo, uma exceção será gerada. Novamente, quase sempre é uma boa
    idéia ter um firewall principal que protege todas as URLs (como visto neste
    capítulo).

Controle de Acesso em Controllers
---------------------------------

Se você quer verificar se o usuário atual tem um perfil de dentro de um controller,
use o método ``isGranted`` do contexto de segurança:

.. code-block:: php

    public function indexAction()
    {
        // show different content to admin users
        if ($this->get('security.context')->isGranted('ADMIN')) {
            // Load admin content here
        }
        // load other regular content here
    }

.. note::

    Um firewall deve estar ativo ou uma exceção será gerada quanto o método ``isGranted``
    for chamado. Veja a nota acima sobre templates para mais detalhes.

Passando por outro usuário
--------------------------

As vezes, é útil poder trocar de um usuário para outro sem ter que sair e se
autenticar novamente (por exemplo quando você está depurando or tentando entender
uma falha que um usuário vê e você não consegue reproduzir). Isto pode ser feito
ativando o listener ``switch_user`` do firewall:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    # ...
                    switch_user: true

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <!-- ... -->
                <switch-user />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => true
                ),
            ),
        ));

Para mudar para outro usuário, basta adicionar o parâmetro de URL ``_switch_user``
indicando o usuário (username) na URL atual:

    http://example.com/somewhere?_switch_user=thomas

Para voltar ao usuário original, use como nome de usuário o texto ``_exit``:

    http://example.com/somewhere?_switch_user=_exit

Claro que esta funcionalidade precisar estar disponível para um grupo reduzido de usuários.
Por padrão, o acesso é restrito a usuários que tem o perfil ``ROLE_ALLOWED_TO_SWITCH``.
O nome deste perfil pode ser modificado através do parâmetro de configuração ``role``.
Para segurança extra, você pode ainda mudar o nome do parâmetro de URL através da
configuração ``parameter``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    // ...
                    switch_user: { role: ROLE_ADMIN, parameter: _want_to_be_this_user }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall>
                <!-- ... -->
                <switch-user role="ROLE_ADMIN" parameter="_want_to_be_this_user" />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => array('role' => 'ROLE_ADMIN', 'parameter' => '_want_to_be_this_user'),
                ),
            ),
        ));

Autenticação Sem Estado
-----------------------

Por padrão, o Symfony2 confia a um cookie (a Session) para persistir o contexto de
segurança de um usuário. Se você utiliza, porém, certificados ou autenticação HTTP, por exemplo,
persistência não é necessário já que as credenciais estão disponíveis em
cada requisição. Neste caso, e se não precisar de armazenar nada entre as requisições,
você pode ativar a autenticação sem estado (que significa que nenhum cookie será criado pelo
Symfony2):

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    http_basic: ~
                    stateless:  true

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <firewall stateless="true">
                <http-basic />
            </firewall>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array('http_basic' => array(), 'stateless' => true),
            ),
        ));

.. note::

    Se utiliza formulário de login, o Symfony2 criará um cookie mesmo se
    você definir ``stateless`` como ``true``.

Palavras Finais
---------------

Segurança pode ser um assunto profundo e complexo de se resolver em uma aplicação.
Felizmente, o componente de segurança do Symfony segue um bom modelo
baseado em *autenticação* e *autorização*. Autenticação, que sempre acontece antes,
é gerenciada pelo firewall cujo trabalho é determinar a identidade do usuário
através de diversos possíveis métodos (exemplo, autenticação HTTP, formulário de login, etc).
No passo-a-passo, você encontrará exemplos de como outros métodos de autenticação
podem ser utilizados, incluindo como implementar o funcionalidade de
"Lembrar de mim" baseada em cookie.

Uma vez que o usuário está autenticado, a camada de autorização pode determinar
se o usuário deve ou não deve ter acesso a um recurso específico. Comumente,
*perfis* são aplicados a URLs, classes ou métodos e se o usuário atual não
possuir o perfil, o acesso é negado. A camada de autorização, porém,
é muito mais extensa e segue o sistema de votação onde várias partes
podem determinar se o usuário atual deve ter acesso a determinado recurso.
Saiba mais sobre este e outros tópicos no passo-a-passo.

Aprenda mais do Passo-a-Passo
-----------------------------

* :doc:`Forçando HTTP/HTTPS </cookbook/security/force_https>`
* :doc:`Coloque usuários por IP na lista negra com um voter personalizado </cookbook/security/voters>`
* :doc:`Listas de Controle de Acesso (ACLs) </cookbook/security/acl>`
* :doc:`/cookbook/security/remember_me`

.. _`componente de segurança`: https://github.com/symfony/Security
.. _`JMSSecurityExtraBundle`: https://github.com/schmittjoh/JMSSecurityExtraBundle
.. _`FOSUserBundle`: https://github.com/FriendsOfSymfony/FOSUserBundle
.. _`implemente a interface \Serializable`: http://php.net/manual/en/class.serializable.php
.. _`functions-online.com`: http://www.functions-online.com/sha1.html
