O Controller
============

Ainda está com a gente depois das primeiras duas partes? Então você já está se
tornando um viciado no Symfony2! Sem mais delongas, vamos descobrir o que os
controllers podem fazer por você.

Usando Formatos
---------------

Atualmente, uma aplicação web deve ser capaz de entregar mais do que apenas 
páginas HTML. Desde XML para feeds RSS ou Web Services, até JSON para
requisições Ajax, existem muitos formatos diferentes para escolher. Dar suporte
para esses formatos no Symfony2 é simples. É só ajustar a rota, como aqui que
acrescentando um valor padrão ``xml`` para a variável ``_format``::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    /**
     * @Route("/hello/{name}", defaults={"_format"="xml"}, name="_demo_hello")
     * @Template()
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

Usando o formato de requisição (como definido pelo valor ``_format``), o
Symfony2 automaticamente seleciona o template correto, nesse caso o
``hello.xml.twig``:

.. code-block:: xml+php

    <!-- src/Acme/DemoBundle/Resources/views/Demo/hello.xml.twig -->
    <hello>
        <name>{{ name }}</name>
    </hello>

Isso é tudo. Para os formatos padrão, o Symfony2 também irá escolher
automaticamente o melhor cabeçalho ``Content-Type`` para a resposta. Se você
quiser dar suporte para diferentes formatos numa única action, em vez disso use
o marcador ``{_format}`` no padrão da rota::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    /**
     * @Route("/hello/{name}.{_format}", defaults={"_format"="html"}, requirements={"_format"="html|xml|json"}, name="_demo_hello")
     * @Template()
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

O controller agora será chamado por URLs parecidas com
``/demo/hello/Fabien.xml`` ou ``/demo/hello/Fabien.json``.

A entrada ``requirements`` define expressões regulares que os marcadores
precisam casar. Nesse exemplo, se você tentar requisitar
``/demo/hello/Fabien.js`` irá receber um erro HTTP 404 pois a requisição
não casa com o requisito ``_format``.

Redirecionamento e Encaminhamento
---------------------------------

Se você quiser redirecionar o usuário para outra página, use o método
``redirect()``::

    return $this->redirect($this->generateUrl('_demo_hello', array('name' => 'Lucas')));

O método ``generateUrl()`` é o mesmo que a função ``path()`` que usamos nos
templates. Ele pega o nome da rota e um array de parâmetros como argumentos e
retorna a URL amigável associada.

Você também pode facilmente encaminhar a action para uma outra com o método
``forward()``. Internamente, o Symfony faz uma "sub-requisição", e retorna
o objeto ``Response`` daquela sub-requisição::

    $response = $this->forward('AcmeDemoBundle:Hello:fancy', array('name' => $name, 'color' => 'green'));

    // faça algo com a resposta ou a retorne diretamente

Pegando informação da Requisição
--------------------------------

Além dos valores dos marcadores de rota, o controller tem acesso ao objeto
``Request``::

    $request = $this->getRequest();

    $request->isXmlHttpRequest(); // essa é uma requisição Ajax?

    $request->getPreferredLanguage(array('en', 'fr'));

    $request->query->get('page'); // pega um parâmetro $_GET

    $request->request->get('page'); // pega um parâmetro $_POST

Em um template, você também pode acessar o objeto ``Request`` via a variável
``app.request``:

.. code-block:: html+jinja

    {{ app.request.query.get('page') }}

    {{ app.request.parameter('page') }}

Persistindo os Dados na Sessão
------------------------------

Mesmo o protocolo HTTP sendo stateless (não tendo monitoração de estado), o
Symfony fornece um objeto interessante que representa o cliente (seja ele uma
pessoa real utilizando um navegador, um bot ou um web service). Entre duas
requisições, o Symfony2 guarda os atributos num cookie usando sessões nativas
do PHP.

É fácil guardar e recuperar a informação da sessão a partir de qualquer
controller::

    $session = $this->getRequest()->getSession();

    // guarda um atributo para reutilização na próxima requisição do usuário
    $session->set('foo', 'bar');

    // em outro controller para outra requisição
    $foo = $session->get('foo');

    // define a localidade do usuário
    $session->setLocale('fr');

Você pode guardar pequenas mensagens que ficarão disponíveis apenas para a
próxima requisição::

    // guarda uma mensagem para a próxima requisição somente (em um controller)
    $session->setFlash('notice', 'Congratulations, your action succeeded!');

    // mostra a mensagem na próxima requisição (em um template)
    {{ app.session.flash('notice') }}

Isso é útil quando você precisa definir uma mensagem de sucesso antes de
redirecionar o usuário para outra página (que então mostrará a mensagem).

Protegendo Recursos
-------------------

A versão Standard Edition do Symfony vem com uma configuração de segurança simples que
atende as necessidades mais comuns:

.. code-block:: yaml

    # app/config/security.yml
    security:
        encoders:
            Symfony\Component\Security\Core\User\User: plaintext

        role_hierarchy:
            ROLE_ADMIN:       ROLE_USER
            ROLE_SUPER_ADMIN: [ROLE_USER, ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH]

        providers:
            in_memory:
                users:
                    user:  { password: userpass, roles: [ 'ROLE_USER' ] }
                    admin: { password: adminpass, roles: [ 'ROLE_ADMIN' ] }

        firewalls:
            dev:
                pattern:  ^/(_(profiler|wdt)|css|images|js)/
                security: false

            login:
                pattern:  ^/demo/secured/login$
                security: false

            secured_area:
                pattern:    ^/demo/secured/
                form_login:
                    check_path: /demo/secured/login_check
                    login_path: /demo/secured/login
                logout:
                    path:   /demo/secured/logout
                    target: /demo/

Essa configuração requer que os usuários se autentiquem para acessar qualquer
URL começada por ``/demo/secured/`` e define dois usuários válidos: ``user`` e
``admin``.  Além disso o usuário ``admin`` tem uma permissão ``ROLE_ADMIN``,
que também inclui a permissão ``ROLE_USER`` (veja a configuração
``role_hierarchy``).

.. tip::

	Para melhorar a legibilidade, nessa nossa configuração simplificada as
	senhas são guardadas em texto puro, mas você pode usar algum algoritmo
	de hash ajustando a seção ``encoders``.
	
Indo para a URL	``http://localhost/Symfony/web/app_dev.php/demo/secured/hello``
você será automaticamente redirecionado para o formulário de login pois o
recurso é protegido por um ``firewall``.

Você também pode forçar a action para requisitar uma permissão especial usando
a annotation ``@Secure`` no controller::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;
    use JMS\SecurityExtraBundle\Annotation\Secure;

    /**
     * @Route("/hello/admin/{name}", name="_demo_secured_hello_admin")
     * @Secure(roles="ROLE_ADMIN")
     * @Template()
     */
    public function helloAdminAction($name)
    {
        return array('name' => $name);
    }

Agora, se autentique como ``user`` (que não tem a permissão ``ROLE_ADMIN``)
e, a partir da página protegida hello, clique no link "Hello resource secured".
O Symfony2 deve retornar um erro HTTP 403, indicando que o usuário está
"proibido" de acessar o recurso.

.. note::

    A camada de segurança do Symfony2 é bem flexível e vem com muitos serviços
    de usuários (como no Doctrine ORM) e autenticação (como HTTP básico, HTTP
    digest ou certificados X509). Leia o capítulo ":doc:`/book/security`" do
    livro para mais informação de como usá-los e configurá-los.


Fazendo Cache dos Recursos
--------------------------

A medida que seu site começa a ter mais tráfego, você vai querer evitar fazer a
geração dos mesmos recursos várias e várias vezes. O Symfony2 usa cabeçalhos de
cache HTTP para gerenciar o cache dos recursos. Para estratégias simples de
cache, use a annotation conveniente ``@Cache()``::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Cache;

    /**
     * @Route("/hello/{name}", name="_demo_hello")
     * @Template()
     * @Cache(maxage="86400")
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

Nesse exemplo, o recurso ficará em cache por um dia. Mas você também pode
usar validações em vez de expiração, ou uma combinação de ambos, se isso se
encaixar melhor nas suas necessidades.

O cache de recursos é gerenciado pelo proxy reverso embutido no Symfony2. Mas
como o cache é gerenciado usando cabeçalhos de cache HTTP normais, você pode
substituir o proxy reverso com o Varnish ou o Squid e estender a sua aplicação
de forma fácil.

.. note::

	Mas como se virar se você não puder fazer cache de páginas inteiras? O
	Symfony2 continua tendo a solução, via Edge Side Includes (ESI), que
	são suportados nativamente. Aprenda mais sobre isso lendo o capítulo
	":doc:`/book/http_cache`" do livro.

Considerações Finais
--------------------

Isso foi tudo, e acho que não gastamos nem 10 minutos. Fizemos uma breve
introdução aos bundles na primeira parte e todas as funcionalidades sobre as
quais aprendemos até agora são parte do bundle núcleo do framework.
Graças aos bundles, tudo no Symfony2 pode ser estendido ou substituído.
Esse é o tema da :doc:`próxima parte do tutorial<the_architecture>`.
