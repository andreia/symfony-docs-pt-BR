Symfony2 versus o PHP puro
==========================

**Por que usar o Symfony2 é melhor do que abrir um arquivo e sair escrevendo PHP puro?**

Se você nunca utilizou um framework PHP, não está familiarizado com a filosofia
MVC ou está apenas interessado em entender todo esse *hype* sobre o Symfony2, este
capítulo é para você. Em vez de *dizer* que o Symfony2 permite que você
desenvolva mais rápido e melhor do que com PHP puro, você vai ver por si mesmo.

Nesse capítulo, você irá escrever uma simples aplicação em PHP puro, e, então,
refatora-la para deixa-la mais organizada. Você vai viajar no tempo, vendo as
decisões sobre o porquê o desenvolvimento web evoluiu com o passar dos tempos
para onde ele está agora.

Ao final, você verá como o Symfony2 pode resgata-lo das tarefas simples e coloca-lo
de volta no controle do seu código.

Um simples Blog em PHP puro
---------------------------

Nesse capítulo, você vai construir uma aplicação para um blog utilizando apenas o
PHP puro. Para começar, crie uma única página que exibe as postagens armazenadas
no banco de dados. Escrever isso em PHP puro é rápido e simples:

.. code-block:: html+php

    <?php
    // index.php

    $link = mysql_connect('localhost', 'myuser', 'mypassword');
    mysql_select_db('blog_db', $link);

    $result = mysql_query('SELECT id, title FROM post', $link);
    ?>

    <html>
        <head>
            <title>List of Posts</title>
        </head>
        <body>
            <h1>List of Posts</h1>
            <ul>
                <?php while ($row = mysql_fetch_assoc($result)): ?>
                <li>
                    <a href="/show.php?id=<?php echo $row['id'] ?>">
                        <?php echo $row['title'] ?>
                    </a>
                </li>
                <?php endwhile; ?>
            </ul>
        </body>
    </html>

    <?php
    mysql_close($link);

Simples de escrever, rápido de executar e, conforme sua aplicação crescer, impossível
de manter. Existem diversos problemas que precisam ser tratados:

* **Sem verificações de erros**: E se a conexão com o banco de dados falhar?

* **Organização pobre**: Se a aplicação crescer, esse arquivo também irá crescer
  e ficará impossível de dar manutenção. Onde você deve colocar o código que cuida
  de tratar os envios de formulários? Como você valida os dados? Onde você deve
  colocar o código que envia emails?

* **Dificuldade para reutilizar código**: Uma vez que tudo está em um único arquivo,
  não há como reutilizar qualquer parte dele em outras "páginas" do blog.

.. note::
    Um outro problema não mencionado aqui é o fato do banco de dados estar amarrado
    ao MySQL. Apesar de não ser tratado aqui, o Symfony2 integra-se totalmente com
    o `Doctrine`_, uma biblioteca dedicada a abstração de banco de dados e mapeamento.

Vamos ao trabalho de resolver esses problemas e mais ainda.

Isolando a Apresentação
~~~~~~~~~~~~~~~~~~~~~~~

O código pode ter ganhos imediatos ao separar a "lógica" da aplicação do código
que prepara o HTML para "apresentação":

.. code-block:: html+php

    <?php
    // index.php

    $link = mysql_connect('localhost', 'myuser', 'mypassword');
    mysql_select_db('blog_db', $link);

    $result = mysql_query('SELECT id, title FROM post', $link);

    $posts = array();
    while ($row = mysql_fetch_assoc($result)) {
        $posts[] = $row;
    }

    mysql_close($link);

    // include the HTML presentation code
    require 'templates/list.php';

Agora o código HTML está armazenado em um arquivo separado (``templates/list.php``),
que é um arquivo HTML que utiliza um sintaxe PHP parecida com a de templates:

.. code-block:: html+php

    <html>
        <head>
            <title>List of Posts</title>
        </head>
        <body>
            <h1>List of Posts</h1>
            <ul>
                <?php foreach ($posts as $post): ?>
                <li>
                    <a href="/read?id=<?php echo $post['id'] ?>">
                        <?php echo $post['title'] ?>
                    </a>
                </li>
                <?php endforeach; ?>
            </ul>
        </body>
    </html>

Por convenção, o arquivo que contém toda a lógica da aplicação - ``index.php`` -
é conhecido como "controller". O termo :term:`controller` é uma palavra que você
vai escutar bastante, independente da linguagem ou framework você utilize. Ela
refere-se a área do *seu* código que processa as entradas do usuário e prepara uma
resposta.

Nesse caso, nosso controller prepara os dados do banco de dados e então inclui um
template para apresenta-los. Com o controller isolado, você pode facilmente mudar
*apenas* o arquivo de template caso precise renderizar os posts de blog em algum
outro formato (por exemplo, ``list.json.php`` para o formato JSON).

Isolando a Lógica (Domínio) da Aplicacão
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por enquanto a aplicação tem apenas uma página. Mas e se uma segunda página
precisar utilizar a mesma conexão com o banco de dados, ou até o mesmo array de
posts do blog? Refatore o código de forma que o comportamento principal e as
funções de acesso aos dados da aplicação fiquem isolados em um novo arquivo chamado
``model.php``:

.. code-block:: html+php

    <?php
    // model.php

    function open_database_connection()
    {
        $link = mysql_connect('localhost', 'myuser', 'mypassword');
        mysql_select_db('blog_db', $link);

        return $link;
    }

    function close_database_connection($link)
    {
        mysql_close($link);
    }

    function get_all_posts()
    {
        $link = open_database_connection();

        $result = mysql_query('SELECT id, title FROM post', $link);
        $posts = array();
        while ($row = mysql_fetch_assoc($result)) {
            $posts[] = $row;
        }
        close_database_connection($link);

        return $posts;
    }

.. tip::

   O nome ``model.php`` foi utilizado porque a lógica e o acesso aos dados de
   uma aplicação são tradicionalmente conhecidos como a camada de "modelo". Em
   uma aplicação bem organizada, a maioria do código representando as suas
   "regras de negócio" devem estar apenas no model (em vez de estar em um controller). 
   Ao contrário desse exemplo, somente uma parte do model (ou nenhuma) está realmente
   relacionada ao banco de dados.

Agora o controller (``index.php``) ficou bem simples:

.. code-block:: html+php

    <?php
    require_once 'model.php';

    $posts = get_all_posts();

    require 'templates/list.php';

Agora, a única tarefa do controller é recuperar os dados da camada de modelo da
sua aplicação (o model) e chamar o template para renderiza-los. Esse é um exemplo
bem simples do padrão model-view-controller.

Isolando o Layout
~~~~~~~~~~~~~~~~~

Até esse ponto a aplicação foi refatorada em três partes distintas, oferecendo
várias vantagens e a oportunidade de reutilizar quase qualquer coisa em outras
páginas.

A única parte do código que *não pode* ser reutilizada é o layout da página.
Conserte isso criando um novo arquivo chamado ``layout.php``:

.. code-block:: html+php

    <!-- templates/layout.php -->
    <html>
        <head>
            <title><?php echo $title ?></title>
        </head>
        <body>
            <?php echo $content ?>
        </body>
    </html>

Assim o template (``templates/list.php``) pode ficar mais simples "extendendo" o
layout:

.. code-block:: html+php

    <?php $title = 'List of Posts' ?>

    <?php ob_start() ?>
        <h1>List of Posts</h1>
        <ul>
            <?php foreach ($posts as $post): ?>
            <li>
                <a href="/read?id=<?php echo $post['id'] ?>">
                    <?php echo $post['title'] ?>
                </a>
            </li>
            <?php endforeach; ?>
        </ul>
    <?php $content = ob_get_clean() ?>

    <?php include 'layout.php' ?>

Agora você foi apresentado a uma metodologia que permite a reutilização do layout.
Infelizmente, para fazer isso, você é forçado a utilizar no template algumas funções
feias do PHP (``ob_start()``, ``ob_get_clean()``). O Symfony2 utiliza o componente
``Templating`` que permite realizar isso de uma maneira limpa e fácil. Logo você
verá esse componente em ação.

Adicionando a página "show" ao Blog
-----------------------------------

A página "list" foi refatorada para que o código fique mais organizado e reutilizável.
Para provar isso, adicione ao blog uma página chamada "show", que exibe um único post
identificado pelo parâmetro ``id``.

Para começar, crie uma nova função no arquivo ``model.php`` que recupera o post
com base no id informado::

    // model.php
    function get_post_by_id($id)
    {
        $link = open_database_connection();

        $id = mysql_real_escape_string($id);
        $query = 'SELECT date, title, body FROM post WHERE id = '.$id;
        $result = mysql_query($query);
        $row = mysql_fetch_assoc($result);

        close_database_connection($link);

        return $row;
    }

Em seguida, crie um novo arquivo chamado ``show.php`` - o controller para essa nova
página:

.. code-block:: html+php

    <?php
    require_once 'model.php';

    $post = get_post_by_id($_GET['id']);

    require 'templates/show.php';

Por fim, crie um novo arquivo de template - ``templates/show.php`` - para renderizar
individualmente o post do blog:

.. code-block:: html+php

    <?php $title = $post['title'] ?>

    <?php ob_start() ?>
        <h1><?php echo $post['title'] ?></h1>

        <div class="date"><?php echo $post['date'] ?></div>
        <div class="body">
            <?php echo $post['body'] ?>
        </div>
    <?php $content = ob_get_clean() ?>

    <?php include 'layout.php' ?>

Criar a segunda página foi bastante fácil e nenhum código foi duplicado. Ainda
assim, essa página criou mais alguns problemas persistentes que um framework pode
resolver para você. Por exemplo, se o parâmetro ``id`` não for informado, ou for
inválido, a página irá quebrar. Seria mais interessante exibir uma página de erro
404, mas isso ainda não pode ser feito de uma maneira fácil. Pior ainda, caso
você esqueça de tratar o ``id`` utilizando a função ``mysql_real_escape_string()``,
todo o seu banco de dados estará correndo o risco de sofrer ataques de SQL injection. 

Um problema ainda maior é que cada controller deve incluir o arquivo ``model.php``
individualmente. O que acontece se cada controller, de repente, precisar incluir
um arquivo adicional para executar alguma outra tarefa global (impor segurança,
por exemplo)? Da maneira como está agora, esse código teria que ser adicionado em
cada arquivo controller. Se você esquecer de incluir algo em algum arquivo
espero que não seja algo relacionado a segurança...

Um "Front Controller" para a salvação
-------------------------------------

A solução é utilizar um :term:`front controller`: um único arquivo PHP que irá
processar *todas* as requisições. Com um front controller, as URIs vão mudar um
pouco, mas começam a ficar mais flexíveis:

.. code-block:: text

    Without a front controller
    /index.php          => Blog post list page (index.php executed)
    /show.php           => Blog post show page (show.php executed)

    With index.php as the front controller
    /index.php          => Blog post list page (index.php executed)
    /index.php/show     => Blog post show page (index.php executed)

.. tip::
    O ``index.php`` pode ser removido da URI se você estiver utilizando regras
    de rewrite no Apache (ou algo equivalente). Nesse caso, a URI resultante para
    a página show será simplesmente ``/show``.

Ao utilizar um front controller, um único arquivo PHP (nesse caso o ``index.php``)
irá renderizar *todas* as requisições. Para a página show do blog, o endereço ``/index.php/show``
irá, na verdade, executar o arquivo ``index.php``, que agora é responsável por
redirecionar as requisições internamente baseado na URI completa. Como você pode ver, 
um front controller é uma ferramente bastante poderosa.

Criando o Front Controller
~~~~~~~~~~~~~~~~~~~~~~~~~~

Você está prestes a dar um **grande** passo com a sua aplicação. Com um arquivo
para gerenciar todas as suas requisições, você pode centralizar coisas como segurança,
configurações e roteamento. Nessa aplicação, o arquivo ``index.php`` deve ser esperto
o suficiente para renderizar a página com a lista de posts *ou* a página com um único
post baseado na URI da requisição:

.. code-block:: html+php

    <?php
    // index.php

    // load and initialize any global libraries
    require_once 'model.php';
    require_once 'controllers.php';

    // route the request internally
    $uri = $_SERVER['REQUEST_URI'];
    if ($uri == '/index.php') {
        list_action();
    } elseif ($uri == '/index.php/show' && isset($_GET['id'])) {
        show_action($_GET['id']);
    } else {
        header('Status: 404 Not Found');
        echo '<html><body><h1>Page Not Found</h1></body></html>';
    }

Por questão de organização, ambos os controllers (os antigos arquivos ``index.php``
e ``show.php``) agora são funções e cada uma foi movida para um arquivo separado,
chamado ``controllers.php``:

.. code-block:: php

    function list_action()
    {
        $posts = get_all_posts();
        require 'templates/list.php';
    }

    function show_action($id)
    {
        $post = get_post_by_id($id);
        require 'templates/show.php';
    }

Sendo um front controller, ``index.php`` agora tem um papel inteiramente novo, que
inclui carregar as bibliotecas principais e rotear a aplicação de forma que um dos
controllers (as funções ``list_action()`` e ``show_action()``) seja chamado. Na
verdade, o front controller está começando a ficar bastante parecido com o mecanismo
do Symfony2 utilizado para tratar e redirecionar as requisições:

.. tip::

   Uma outra vantagem do front controller é ter URLs flexíveis. Note que a URL para
   a página que exibe um post no blog pode mudar de ``/show`` para ``/read``
   alterando o código apenas em um único lugar. Antes, um arquivo teria que ser
   renomeado. No Symfony2 as URLs podem ser ainda mais flexíveis.

Até agora, a aplicação evoluiu de um único arquivo PHP para para uma estrutura
organizada que permite a reutilização de código. Você deve estar mais feliz, mas
longe de estar satisfeito. Por exemplo, o sistema de "roteamento" ainda não é
consistente e não reconhece que a página de listagem (``index.php``) também pode
ser acessada via ``/`` (se as regras de rewrite foram adicionadas no Apache). Além
disso, em vez de desenvolver o blog, boa parte do tempo foi gasto trabalhando na
"arquitetura" do código (por exemplo, roteamento, execução de controllers, templates
etc). Mais tempo ainda será necessário para tratar o envio de formulários, validação
das entradas, logs e segurança. Por que você tem que reinventar soluções para todos
esses problemas?

Adicione um toque de Symfony2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony2 para a salvação. Antes de realmente utilizar o Symfony2, você precisa
ter certeza que o PHP sabe onde encontrar as classes do framework. Isso pode ser
feito com o autoloader fornecido pelo Symfony. Um autoloader é uma ferramenta que
permite a utilização de classes PHP sem a necessidade de incluir os seus arquivos
explicitamente.

Primeiro, `faça o download do symfony`_ e o coloque no diretório ``vendor/symfony/symfony/``.
A seguir, crie um o arquivo ``app/bootstrap.php``. Utilize-o para dar ``require`` 
dos dois arquivos da aplicação e para configurar o autoloader:

.. code-block:: html+php

    <?php
    // bootstrap.php
    require_once 'model.php';
    require_once 'controllers.php';
    require_once 'vendor/symfony/symfony/src/Symfony/Component/ClassLoader/UniversalClassLoader.php';

    $loader = new Symfony\Component\ClassLoader\UniversalClassLoader();
    $loader->registerNamespaces(array(
        'Symfony' => __DIR__.'/../vendor/symfony/symfony/src',
    ));

    $loader->register();

Esse código diz ao autoloader onde estão as classes do ``Symfony``. Com isso, você
pode começar a utilizar as classes sem precisar de um ``require`` para os arquivos
que as contém.

Dentro da filosofia do Symfony está a idéia de que a principal tarefa de uma aplicação
é interpretar cada requisição e retornar uma resposta. Para essa finalidade, o
Symfony2 fornece as classes :class:`Symfony\\Component\\HttpFoundation\\Request` e
:class:`Symfony\\Component\\HttpFoundation\\Response`. Elas são representações
orientadas a objetos da requisição HTTP pura sendo processada e da resposta HTTP
sendo retornada. Utilize-as para melhorar o blog:

.. code-block:: html+php

    <?php
    // index.php
    require_once 'app/bootstrap.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    $request = Request::createFromGlobals();

    $uri = $request->getPathInfo();
    if ($uri == '/') {
        $response = list_action();
    } elseif ($uri == '/show' && $request->query->has('id')) {
        $response = show_action($request->query->get('id'));
    } else {
        $html = '<html><body><h1>Page Not Found</h1></body></html>';
        $response = new Response($html, 404);
    }

    // echo the headers and send the response
    $response->send();

Agora os controller são responsáveis por retornar um objeto ``Response``. Para 
tornar isso mais fácil, você pode adicionar uma nova função chamada ``render_template()``,
que, a propósito, funciona de forma um pouco parecida com o mecanismo de template
do Symfony2:

.. code-block:: php

    // controllers.php
    use Symfony\Component\HttpFoundation\Response;

    function list_action()
    {
        $posts = get_all_posts();
        $html = render_template('templates/list.php', array('posts' => $posts));

        return new Response($html);
    }

    function show_action($id)
    {
        $post = get_post_by_id($id);
        $html = render_template('templates/show.php', array('post' => $post));

        return new Response($html);
    }

    // helper function to render templates
    function render_template($path, array $args)
    {
        extract($args);
        ob_start();
        require $path;
        $html = ob_get_clean();

        return $html;
    }

Ao adicionar uma pequena parte do Symfony2, a aplicação ficou mais flexível e
confiável. A classe ``Request`` fornece uma maneira segura para acessar informações
sobre a requisição HTTP. Especificamente, o método ``getPathInfo()`` retorna a URI
limpa (sempre retornando ``/show`` e nunca ``/index.php/show``). Assim, mesmo que
o usuário utilize ``/index.php/show``, a aplicação é inteligente o suficiente para
direcionar a requisição para ``show_action()``.

O objeto ``Response`` dá flexibilidade ao construir a resposta HTTP, permitindo a
adição de cabeçalhos HTTP e conteúdo através de um interface orientada a objetos.
Apesar das respostas nessa aplicação ainda serem simples, essa flexibilidade será
útil conforme a aplicação crescer.

A aplicação de exemplo no Symfony2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O blog já passou por um *longo* caminho, mas ele ainda tem muito código para uma aplicação
tão simples. Por esse caminho, nós também inventamos um simples sistema de roteamento
e um método utilizando ``ob_start()`` e ``ob_get_clean()`` para renderiar templates.
Se, por alguma razão, você precisasse continuar a construir esse "framework" do zero,
você poderia pelo menos utilizar isoladamente os components `Routing`_ e `Templating`_
do Symfony, que já resolveriam esses problemas.

Em vez de re-resolver problemas comuns, você pode deixar que o Symfony2 cuide deles
pra você. Aqui está um exemplo da mesma aplicação, agora feito com o Symfony2:

.. code-block:: html+php

    <?php
    // src/Acme/BlogBundle/Controller/BlogController.php
    namespace Acme\BlogBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class BlogController extends Controller
    {
        public function listAction()
        {
            $posts = $this->get('doctrine')->getEntityManager()
                ->createQuery('SELECT p FROM AcmeBlogBundle:Post p')
                ->execute();

            return $this->render('AcmeBlogBundle:Blog:list.html.php', array('posts' => $posts));
        }

        public function showAction($id)
        {
            $post = $this->get('doctrine')
                ->getEntityManager()
                ->getRepository('AcmeBlogBundle:Post')
                ->find($id);
            
            if (!$post) {
                // cause the 404 page not found to be displayed
                throw $this->createNotFoundException();
            }

            return $this->render('AcmeBlogBundle:Blog:show.html.php', array('post' => $post));
        }
    }

Os dois controller ainda estão bastante leves. Cada um utiliza a biblioteca de ORM
Doctrine para recuperar objetos do banco de dados e o componente ``Templating``
para renderizar e retornar um objeto `Response`. O template list ficou um pouco
mais simples:

.. code-block:: html+php

    <!-- src/Acme/BlogBundle/Resources/views/Blog/list.html.php --> 
    <?php $view->extend('::layout.html.php') ?>

    <?php $view['slots']->set('title', 'List of Posts') ?>

    <h1>List of Posts</h1>
    <ul>
        <?php foreach ($posts as $post): ?>
        <li>
            <a href="<?php echo $view['router']->generate('blog_show', array('id' => $post->getId())) ?>">
                <?php echo $post->getTitle() ?>
            </a>
        </li>
        <?php endforeach; ?>
    </ul>

O layout está praticamente idêntico:

.. code-block:: html+php

    <!-- app/Resources/views/layout.html.php -->
    <html>
        <head>
            <title><?php echo $view['slots']->output('title', 'Default title') ?></title>
        </head>
        <body>
            <?php echo $view['slots']->output('_content') ?>
        </body>
    </html>

.. note::

    Vamos deixar o template da página show como um exercício para você, uma vez
    que é trivial cria-lo com base no template da página list

Quando o mecanismo do Symfony2 (chamado de ``Kernel``) é iniciado, ele precisa de
um mapa que indique quais controllers devem ser executados de acordo com a requisição.
A configuração de roteamento contém essa informação em um formato legível:

.. code-block:: yaml

    # app/config/routing.yml
    blog_list:
        pattern:  /blog
        defaults: { _controller: AcmeBlogBundle:Blog:list }

    blog_show:
        pattern:  /blog/show/{id}
        defaults: { _controller: AcmeBlogBundle:Blog:show }

Agora que o Symfony2 está cuidando dessas tarefas simples, o front controller
ficou extremamente simples. Uma vez que ele faz tão pouco, você nunca mais terá
que mexer nele depois de criado (e se você estiver utilizando uma distribuição do
Symfony2, você nem mesmo precisará cria-lo!):

.. code-block:: html+php

    <?php
    // web/app.php
    require_once __DIR__.'/../app/bootstrap.php';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('prod', false);
    $kernel->handle(Request::createFromGlobals())->send();

A única tarefa do front controller é iniciar o mecanismo (``Kernel``) do Symfony2
e passar para ele o objeto ``Request`` que deve ser manuseado. Então o Symfony
utiliza o mapa de rotas para determinar qual controller chamar. Assim como antes,
o método controller é responsável por retornar o objeto ``Response``. Não há muito
mais que ele precise fazer. 

Para uma representação visual de como o Symfony2 trata cada requisição, veja o
:ref:`diagrama de fluxo da requisição<request-flow-figure>`.

Onde é vantagem utilizar o Symfony2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Nos próximos capítulos você irá aprender mais sobre cada parte do Symfony funciona
e a organização recomendada para um projeto. Por enquanto, vamos ver como a
migração do PHP puro para o Symfony2 facilitou a sua vida:

* A sua aplicação agora tem um **código limpo e organizado de forma consistente**
  apesar do Symfony não te forçar a isso). Isso aumenta a **usabilidade** e permite
  que novos desenvolvedores sejam produtivos no seu projeto de uma maneira mais rápida.

* 100% do código que você escreveu é para a *sua* aplicação. Você **não precisa
  desenvolver ou manter ferramentas de baixo nível** como :ref:`autoloading<autoloading-introduction-sidebar>`,
  :doc:`roteamento</book/routing>`, ou renderização nos :doc:`controllers</book/controller>`.

* O Symfony2 te dá **acesso a ferramentas open source** como Doctrine e os 
  componentes Templating, Security, Form, Validation e Translation (só para citar
  alguns).

* A aplicação agora faz uso de **URLs totalmente flexíveis** graças ao componente
  ``Routing``.

* A arquitetura do Symfony2 centrada no HTTP te dá acesso a poderosas ferramentas 
  como **HTTP caching** feito pelo **cache interno de HTTP do Symfony2** ou por 
  ferramentas ainda mais poderosas como o ``Varnish`_. Esse assunto será tratado
  em um próximo capítulo sobre :doc:`caching</book/http_cache>`.

E talvez o melhor de tudo, ao utilizar o Symfony2, você tem acesso a todo um conjunto
de **ferramentas open source de alta qualidade desenvolvidas pela comunidade do 
Symfony2**! Para mais informações, visite o site `Symfony2Bundles.org`_

Melhores templates
------------------

Se você optar por utiliza-lo, o Symfony2 vem com um sistema de template padrão
chamado `Twig`_ que torna mais fácil a tarefa de escrever templates e os deixa
mais fácil de ler. Isso significa que a aplicação de exemplo pode ter ainda menos
código! Pegue como exemplo o template list escrito com o Twig:

.. code-block:: html+jinja

    {# src/Acme/BlogBundle/Resources/views/Blog/list.html.twig #}

    {% extends "::layout.html.twig" %}
    {% block title %}List of Posts{% endblock %}

    {% block body %}
        <h1>List of Posts</h1>
        <ul>
            {% for post in posts %}
            <li>
                <a href="{{ path('blog_show', { 'id': post.id }) }}">
                    {{ post.title }}
                </a>
            </li>
            {% endfor %}
        </ul>
    {% endblock %}

O template ``layout.html.twig`` correspondente também fica mais fácil de escrever:

.. code-block:: html+jinja

    {# app/Resources/views/layout.html.twig #}

    <html>
        <head>
            <title>{% block title %}Default title{% endblock %}</title>
        </head>
        <body>
            {% block body %}{% endblock %}
        </body>
    </html>

O Twig é bem suportado no Symfony2. E, mesmo que os templates em PHP sempre serão
suportados pelo framework, continuaremos a discutir sobre as muitas vantagens do
Twig. Para mais informações, veja o :doc:`capítulo sobre templates</book/templating>`.

Aprenda mais no Cookbook
------------------------

* :doc:`/cookbook/templating/PHP`
* :doc:`/cookbook/controller/service`

.. _`Doctrine`: http://www.doctrine-project.org
.. _`faça o download do symfony`: http://symfony.com/download
.. _`Routing`: https://github.com/symfony/Routing
.. _`Templating`: https://github.com/symfony/Templating
.. _`Symfony2Bundles.org`: http://symfony2bundles.org
.. _`Twig`: http://twig.sensiolabs.org
.. _`Varnish`: http://www.varnish-cache.org
.. _`PHPUnit`: http://www.phpunit.de
