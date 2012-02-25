.. index::
   single: Roteamento

Roteamento
=======

URLs bonitas são uma obrigação absoluta para qualquer aplicação web séria. Isto
significa deixar para trás URLs feias como ``index.php?article_id=57`` em favor
de algo como ``/read/intro-to-symfony``.

Ter flexibilidade é ainda mais importante. E se você precisasse mudar a
URL de uma página de ``/blog`` para ``/news``? Quantos links você precisaria para
investigá-los e atualizar para fazer a mudança ? Se você está usando o roteador do Symfony,
a mudança é simples.

O roteador do Symfony2 deixa você definir URLs criativas que você mapeia para diferentes
áreas de sua aplicação. No final deste capítulo, você será capaz de :
* Criar rotas complexas que mapeiam para os controllers
* Gerar URLs dentro de templates e controllers
* Carregar recursos de roteamento  de pacotes (ou algum lugar a mais)
* Depurar suas rotas

.. index::
   single: Roteamento; Bases

Roteamento em Ação
-----------------

Um *rota* é um mapa de um padrão URL para um controller. Por exemplo, suponha
que você queira ajustar qualquer URL como ``/blog/my-post`` ou ``/blog/all-about-symfony``
e enviá-la ao controller que pode olhar e mudar aquela entrada do blog.
A rota é simples:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        blog_show:
            pattern:   /blog/{slug}
            defaults:  { _controller: AcmeBlogBundle:Blog:show }

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_show" pattern="/blog/{slug}">
                <default key="_controller">AcmeBlogBundle:Blog:show</default>
            </route>
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog_show', new Route('/blog/{slug}', array(
            '_controller' => 'AcmeBlogBundle:Blog:show',
        )));

        return $collection;

O padrão definido pela rota ``blog_show`` age como ``/blog/*`` onde
o coringa é dado pelo nome ``slug``. Para a URL ``/blog/my-blog-post``,
a variável ``slug`` obtém um valor de ``my-blog-post``, que está disponível
para você usar em seu controller (continue lendo).

O parâmetro ``_controller`` é uma chave especial que avisa o Symfony qual controller
deveria ser executado quando uma URL corresponde a essa rota. A string ``_controller``
é chamada :ref:`logical name<controller-string-syntax>`. Ela segue um 
padrão que aponta para uma classe e método PHP específico:

.. code-block:: php

    // src/Acme/BlogBundle/Controller/BlogController.php
    
    namespace Acme\BlogBundle\Controller;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class BlogController extends Controller
    {
        public function showAction($slug)
        {
            $blog = // use the $slug varible to query the database
            
            return $this->render('AcmeBlogBundle:Blog:show.html.twig', array(
                'blog' => $blog,
            ));
        }
    }

Parabéns ! Você agora criou sua primeira rota conectou ela a
um controller. Agora, quando você visitar ``/blog/my-post``, o controller ``showAction``
será executado e a variável ``$slug`` será igual a ``my-post``.

Esse é o objetivo do roteador do Symfony2: mapear a URL de uma requisição para um
controller. Ao longo do caminho, você aprenderá todos os tipos de truques que tornam o mapeamento fácil,
mesmo das URLS mais complexas.

.. index::
   single: Roteamento; Por debaixo do capuz
   
Roteamento: Por debaixo do capuz
-----------------------

Quando uma requisição é feita para sua aplicação, ela contém um endereço para
o "recurso" exato que o cliente está requisitando.Esse endereço é chamado de
URL, (ou URI), e poderia ser ``/contact``, ``/blog/read-me``, ou qualquer coisa
a mais. Considere a seguinte requisição de exemplo :

.. code-block:: text

    GET /blog/my-blog-post

O objetido do sistema de roteamento do Symfony2 é analisar esta URL e determinar
qual controller deveria ser executado. O processo interior parece isso:

#. A requisação é controlada pelo front controller do Symfony2 front controller (ex: ``app.php``);

#. O núcleo do Symfony2 (ex: Kernel) pergunta ao roteador para inspecionar a requisição;

#. O roteador ajusta a URL recebida para uma rota específica e retorna informação
   sobre a rota, incluindo o controller que deveria ser executado;

#. O kernel do Symfony2 executa o controller, que retorna por último
   um objeto ``Response``.

.. figure:: /images/request-flow.png
   :align: center
   :alt: Symfony2 request flow

   A camada de roteamento é  uma ferramenta que traduza a URL recebida em um controller
   específico para executar.

.. index::
   single: Roteamento; Criando rotas

Criando rotas
---------------

Symfony carrega todas as rotas para sua aplicação de um arquivo de configuração 
de roteamento. O arquivo é geralmente ``app/config/routing.yml``, mas pode ser configurado
para ser qualquer coisa (incluindo um arquivo XML ou PHP) via arquivo de configuração
de aplicação:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            router:        { resource: "%kernel.root_dir%/config/routing.yml" }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config ...>
            <!-- ... -->
            <framework:router resource="%kernel.root_dir%/config/routing.xml" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'router'        => array('resource' => '%kernel.root_dir%/config/routing.php'),
        ));

.. tip::

    Mesmo que toda as rotas sejam carregadas de um arquivo só, é uma prática comum
    incluir recursos de roteamento adicionais de dentro do arquivo. 
    Veja a seção:ref:`routing-include-external-resources` para mais informação.

Configuração de rota básica
~~~~~~~~~~~~~~~~~~~~~~~~~

Definir uma rota é fácil, e uma aplicação típica terá um monte de rotas.
A basic route consists of just two parts: the ``pattern`` to match and a
``defaults`` array:

.. configuration-block::

    .. code-block:: yaml

        _welcome:
            pattern:   /
            defaults:  { _controller: AcmeDemoBundle:Main:homepage }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="_welcome" pattern="/">
                <default key="_controller">AcmeDemoBundle:Main:homepage</default>
            </route>

        </routes>

    ..  code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('_welcome', new Route('/', array(
            '_controller' => 'AcmeDemoBundle:Main:homepage',
        )));

        return $collection;

A rota combina a homepage (``/``) e mapeia ele para o controller 
``AcmeDemoBundle:Main:homepage``. A string ``_controller`` é traduzida pelo Symfony2 em uma
função verdadeira do PHP e exectudada. Aquele processo irá ser explicado brevemente
na seção :ref:`controller-string-syntax`.

.. index::
   single: Roteamento; Espaços reservados

Roteando com Espaços reservados
~~~~~~~~~~~~~~~~~~~~~~~~~

Claro que o sistema de roteamento suporta rotas muito mais interessantes. Muitas
rotas irão conter uma ou mais chamadas de espaços reservados "coringa":

.. configuration-block::

    .. code-block:: yaml

        blog_show:
            pattern:   /blog/{slug}
            defaults:  { _controller: AcmeBlogBundle:Blog:show }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_show" pattern="/blog/{slug}">
                <default key="_controller">AcmeBlogBundle:Blog:show</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog_show', new Route('/blog/{slug}', array(
            '_controller' => 'AcmeBlogBundle:Blog:show',
        )));

        return $collection;

O padrão irá corresponder a qualquer coisa que pareça ``/blog/*``. Melhor ainda,
o valor correspondendo ao espaço reservado ``{slug}`` estará disponível no seu controller. 
Em outras palavras, se a URL é ``/blog/hello-world``, uma variável 
``$slug``, com o valor de ``hello-world``, estará disponível no controller.
Isto pode ser usado, por exemplo, para carregar o post do blog correspondendo àquela string.

Este padrão *não* irá, entretanto, simplesmente ajustar ``/blog``. Isso é porque,
por padrão, todos os espaços reservados são requeridos. Isto pode ser mudado ao adicionar um valor
de espaço reservado ao array ``defaults``.

Espaços reservados Requeridos e Opcionais
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Para tornar as coisas mais excitantes, adicione uma nova rota que mostre uma lista de todos
os posts do blog para essa aplicação de blog imaginária:

.. configuration-block::

    .. code-block:: yaml

        blog:
            pattern:   /blog
            defaults:  { _controller: AcmeBlogBundle:Blog:index }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" pattern="/blog">
                <default key="_controller">AcmeBlogBundle:Blog:index</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog', array(
            '_controller' => 'AcmeBlogBundle:Blog:index',
        )));

        return $collection;

Até agora, essa rota é tão simples quanto possível - contém nenhum espaço reservado
e só irá corresponder à URL exata ``/blog``. Mas e se você precisar dessa rota
para suportar paginação, onde ``/blog/2`` mostre a segunda página do entradas do
blog ? Atualize a rota para ter uma nova ``{page}``  de espaço reservado:

.. configuration-block::

    .. code-block:: yaml

        blog:
            pattern:   /blog/{page}
            defaults:  { _controller: AcmeBlogBundle:Blog:index }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" pattern="/blog/{page}">
                <default key="_controller">AcmeBlogBundle:Blog:index</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AcmeBlogBundle:Blog:index',
        )));

        return $collection;

Como o espaço reservado ``{slug}`` anterior, o valor correspondendo a ``{page}``
estará disponível dentro do seu controller. Este valor pode ser usado para determinar qual
conjunto de posts do blog mostrar para determinada página.

Mas espere ! Como espaços reservados são requeridos por padrão, essa rota não irá
mais corresponder simplesmente a  ``/blog``. Ao invés disso, para ver a página 1 do blog,
você precisaria usar a URL ``/blog/1``! Como não há meios para uma aplicação web ricase 
comportar, modifique a rota para fazer o parâmetro ``{page}`` opcional.
Isto é feito ao incluir na coleção ``defaults``:

.. configuration-block::

    .. code-block:: yaml

        blog:
            pattern:   /blog/{page}
            defaults:  { _controller: AcmeBlogBundle:Blog:index, page: 1 }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" pattern="/blog/{page}">
                <default key="_controller">AcmeBlogBundle:Blog:index</default>
                <default key="page">1</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AcmeBlogBundle:Blog:index',
            'page' => 1,
        )));

        return $collection;

Ao adicionar ``page`` para a chave ``defaults``, o espaço reservado ``{page}`` não é mais
requerido. A URL ``/blog`` irá corresponder a essa rota e o valor do 
parâmetro ``page`` será fixado para ``1``. A URL ``/blog/2`` irá também
corresponder, atribuindo ao parâmetro ``page`` o valor ``2``. Perfeito.

+---------+------------+
| /blog   | {page} = 1 |
+---------+------------+
| /blog/1 | {page} = 1 |
+---------+------------+
| /blog/2 | {page} = 2 |
+---------+------------+

.. index::
   single: Roteamento; Requisitos

Adicionando Requisitos
~~~~~~~~~~~~~~~~~~~
Dê uma rápida olhada nos roteamentos que foram criados até agora:

.. configuration-block::

    .. code-block:: yaml

        blog:
            pattern:   /blog/{page}
            defaults:  { _controller: AcmeBlogBundle:Blog:index, page: 1 }

        blog_show:
            pattern:   /blog/{slug}
            defaults:  { _controller: AcmeBlogBundle:Blog:show }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" pattern="/blog/{page}">
                <default key="_controller">AcmeBlogBundle:Blog:index</default>
                <default key="page">1</default>
            </route>

            <route id="blog_show" pattern="/blog/{slug}">
                <default key="_controller">AcmeBlogBundle:Blog:show</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AcmeBlogBundle:Blog:index',
            'page' => 1,
        )));

        $collection->add('blog_show', new Route('/blog/{show}', array(
            '_controller' => 'AcmeBlogBundle:Blog:show',
        )));

        return $collection;

Você pode apontar o problema ? Perceba que ambas as rotas tem padrão que combinam
URL's que pareçam ``/blog/*``. O roteador do Symfony irá sempre escolher a
**primeira** rota correspondente que ele encontra. Em outras palavras, a rota ``blog_show``
*nunca* será correspondida. Ao invés disso, uma URL como ``/blog/my-blog-post`` irá corresponder
à primeira rota (``blog``) e retorna um valor sem sentido de ``my-blog-post``
ao parâmetro ``{page}``.

+--------------------+-------+-----------------------+
| URL                | route | parameters            |
+====================+=======+=======================+
| /blog/2            | blog  | {page} = 2            |
+--------------------+-------+-----------------------+
| /blog/my-blog-post | blog  | {page} = my-blog-post |
+--------------------+-------+-----------------------+

A resposta para o problema é adicionar mais *requisitos* de rota. As rotas neste
exemplo funcionariam perfeitamente se o padrão ``/blog/{page}`` *somente* correspondesse
a URLs onde a porção ``{page}`` fosse um integer. Felizmente, requisitos de expressões
regulares podem facilmente ser adicionados para cada parâmetro. Por exemplo:

.. configuration-block::

    .. code-block:: yaml

        blog:
            pattern:   /blog/{page}
            defaults:  { _controller: AcmeBlogBundle:Blog:index, page: 1 }
            requirements:
                page:  \d+

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" pattern="/blog/{page}">
                <default key="_controller">AcmeBlogBundle:Blog:index</default>
                <default key="page">1</default>
                <requirement key="page">\d+</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AcmeBlogBundle:Blog:index',
            'page' => 1,
        ), array(
            'page' => '\d+',
        )));

        return $collection;

O requisito ``\d+`` é uma expressão regular que diz o valor do 
parâmetro ``{page}`` deve ser um dígito (em outras palavras, um número). A rota``blog`` 
ainda irá correponder a uma URL como ``/blog/2`` (porque 2 é um número), mas não irá mair
corresponder a URL como ``/blog/my-blog-post`` (porque ``my-blog-post``
*não* é um número).

Como resultado, uma URL como ``/blog/my-blog-post`` não irá corresponder apropriadamente
à rota ``blog_show``.

+--------------------+-----------+-----------------------+
| URL                | rota      | parâmetros            |
+====================+===========+=======================+
| /blog/2            | blog      | {page} = 2            |
+--------------------+-----------+-----------------------+
| /blog/my-blog-post | blog_show | {slug} = my-blog-post |
+--------------------+-----------+-----------------------+

.. sidebar:: Rotas prematuras sempre Vencem

    Isso tudo significa que a ordem das rotas é muito importante.
    Se a rota ``blog_show`` foi colocada acima da rota ``blog``, a
    URL ``/blog/2``corresponderia a ``blog_show`` ao invés de ``blog`` já que
    o parâmetro``{slug}`` de ``blog_show`` não tem requisitos. By using proper
    ordering and clever requirements, you can accomplish just about anything.

Como os requisitos de parâmetros são expressões regulares, a complexidade 
e flexibilidade de cada requisito é inteiramente de sua responsabilidade. Suponha que a página inicial
de sua aplicação está disponível em dois idiomas diferentes, baseada na 
URL:

.. configuration-block::

    .. code-block:: yaml

        homepage:
            pattern:   /{culture}
            defaults:  { _controller: AcmeDemoBundle:Main:homepage, culture: en }
            requirements:
                culture:  en|fr

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="homepage" pattern="/{culture}">
                <default key="_controller">AcmeDemoBundle:Main:homepage</default>
                <default key="culture">en</default>
                <requirement key="culture">en|fr</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('homepage', new Route('/{culture}', array(
            '_controller' => 'AcmeDemoBundle:Main:homepage',
            'culture' => 'en',
        ), array(
            'culture' => 'en|fr',
        )));

        return $collection;

Para requisições recebidas, a parte ``{culture}`` da URL é comparada
com a expressão regular ``(en|fr)``.

+-----+--------------------------+
| /   | {culture} = en           |
+-----+--------------------------+
| /en | {culture} = en           |
+-----+--------------------------+
| /fr | {culture} = fr           |
+-----+--------------------------+
| /es | *won't match this route* |
+-----+--------------------------+

.. index::
   single: Roteamento; Requisição de método

Adicionando Requisição de Método HTTP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Em adição à URL, você também pode ajustar o "método" da requisição recebida
(em outras palavras, GET, HEAD, POST, PUT, DELETE).Suponha que você tenha um formulário de contato
com dois controllers - um para exibir o formulário (em uma requisição GET) e uma
para processar o formulário quando ele é enviado (em uma requisição POST). Isto pode 
ser acompanhando com a seguinte configuração de rota:

.. configuration-block::

    .. code-block:: yaml

        contact:
            pattern:  /contact
            defaults: { _controller: AcmeDemoBundle:Main:contact }
            requirements:
                _method:  GET

        contact_process:
            pattern:  /contact
            defaults: { _controller: AcmeDemoBundle:Main:contactProcess }
            requirements:
                _method:  POST

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" pattern="/contact">
                <default key="_controller">AcmeDemoBundle:Main:contact</default>
                <requirement key="_method">GET</requirement>
            </route>

            <route id="contact_process" pattern="/contact">
                <default key="_controller">AcmeDemoBundle:Main:contactProcess</default>
                <requirement key="_method">POST</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('contact', new Route('/contact', array(
            '_controller' => 'AcmeDemoBundle:Main:contact',
        ), array(
            '_method' => 'GET',
        )));

        $collection->add('contact_process', new Route('/contact', array(
            '_controller' => 'AcmeDemoBundle:Main:contactProcess',
        ), array(
            '_method' => 'POST',
        )));

        return $collection;

Apesar do fato que estas duas rotas tem padrões idênticos (``/contact``),
a primeira rota irá aceitar somente requisições GET e a segunda rota irá somente
aceitar requisiçõs POST.Isso significa que você pode exibir o formulário e enviar o
formulário pela mesma URL, enquanto usa controllers distintos para as duas ações.

.. note::
    If no ``_method`` requirement is specified, the route will match on
    *all* methods.

Como os outros requisitos, o requisito ``_method`` é analisado como uma expressão
regular. Para aceitar requisições ``GET`` *ou* ``POST``, você pode usar ``GET|POST``.

.. index::
   single: Roteamento; Exemplo avançado
   single: Roteamento; parâmetro _format 
   
.. _advanced-routing-example:


Exemplo avançado de roteamento
~~~~~~~~~~~~~~~~~~~~~~~~

Até esse ponto, você tem tudo que você precisa para criar uma poderosa estrutura
de roteamento em Symfony. O exemplo seguinte mostra quão flexível o
sistema de roteamento pode ser:

.. configuration-block::

    .. code-block:: yaml

        article_show:
          pattern:  /articles/{culture}/{year}/{title}.{_format}
          defaults: { _controller: AcmeDemoBundle:Article:show, _format: html }
          requirements:
              culture:  en|fr
              _format:  html|rss
              year:     \d+

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="article_show" pattern="/articles/{culture}/{year}/{title}.{_format}">
                <default key="_controller">AcmeDemoBundle:Article:show</default>
                <default key="_format">html</default>
                <requirement key="culture">en|fr</requirement>
                <requirement key="_format">html|rss</requirement>
                <requirement key="year">\d+</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('homepage', new Route('/articles/{culture}/{year}/{title}.{_format}', array(
            '_controller' => 'AcmeDemoBundle:Article:show',
            '_format' => 'html',
        ), array(
            'culture' => 'en|fr',
            '_format' => 'html|rss',
            'year' => '\d+',
        )));

        return $collection;

Como você viu, essa rota só irá funcionar se a parte ``{culture}`` da 
URL ou é ``en`` ou ``fr`` e se ``{year}`` é um número. Esta rota também
mostra como você pode usar um período entre espaços reservados ao invés de
uma barra. URLs que correspondam a esta rota poderia parecer como:

 * ``/articles/en/2010/my-post``
 * ``/articles/fr/2010/my-post.rss``

.. _book-routing-format-param:

.. sidebar:: O Parâmetro de Roteamento Especial ``_format`` 

    Esse exemplo também resslta o parâmetro de roteamento especial ``_format``.
    Quando usa esse parâmetro, o valor correspondido se torna o "formato requisitado"
    do ojeto ``Request``. Ultimamente, o formato requisitado é usado para certas
    coisas como as configurar o ``Content-Type`` da resposta (ex: um formato de requisição 
    ``json`` traduz em um ``Content-Type`` de ``application/json``).
    Ele também pode ser usado no controller para alterar um template diferente para
    cada valor de ``_format``. O parâmetro ``_format`` é um modo muito poderoso 
    para alterar o mesmo conteúdo em formatos diferentes.


Parâmetros de Roteamento Especiais
~~~~~~~~~~~~~~~~~~~~~~~~~~

Como você viu, cada parâmetro de roteamento ou valor padrão está eventualmente disponível
como um argumento no método do controller. Adicionalmente, existem três parâmetros
que são especiais: cada um adiciona uma parte única de funcionalidade dentro da sua aplicação:

* ``_controller``: Como você viu, este parâmetro é usado para determinar qual 
  controller é executado quando a rota é correspondida;
  
* ``_format``: Usado para fixar o formato de requisição (:ref:`read more<book-routing-format-param>`);

* ``_locale``: Usado para fixar a localidade na sessão (:ref:`read more<book-translation-locale-url>`);

.. index::
   single: Roteamento; Controllers
   single: Controller; Formato de Nomeamento de strings

.. _controller-string-syntax:

Padrão de Nomeamento do Controller
-------------------------

Cada rota deve ter um parâmetro ``_controller``, que ordena qual 
controller deveria ser executado quando uma rota é correspondida. Esse parâmetro
usa um padrão de string simples chamado *logical controller name*, que
o Symfony mapeia para uma classe e método PHP específico. O padrão tem três partes,
cada uma separada por dois pontos:

    **bundle**:**controller**:**action**

Por exemplo, um valor ``_controller`` de ``AcmeBlogBundle:Blog:show`` significa:

+----------------+---------------------+---------------+
|Pacote          |Classe do Controller |Nome do Método |
+================+=====================+===============+
| AcmeBlogBundle | BlogController      | showAction    |
+----------------+---------------------+---------------+

O controller poderia parecer assim:

.. code-block:: php

    // src/Acme/BlogBundle/Controller/BlogController.php
    
    namespace Acme\BlogBundle\Controller;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    
    class BlogController extends Controller
    {
        public function showAction($slug)
        {
            // ...
        }
    }

Perceba que Symfony adiciona a string ``Controller`` para o nome da classe (``Blog``
=> ``BlogController``) e ``Action`` para o nome do método (``show`` => ``showAction``).

Você também poderia referir a esse controler usando os nomes totalmente qualificados de
classe e método:``Acme\BlogBundle\Controller\BlogController::showAction``.
Mas se você seguir alguma convenções simples, o nome lógico é mais conciso
e permite mais flexibilidade.

.. note::

   Em complemento ao utilizar o nome lógico ou o nome de classe totalmente qualificado,
   Symfony suporta um terceiro modo de referir a um controller. Esse método
   usa somente um separador de dois pontos (ex: ``service_name:indexAction``) e
   referir um controller como um serviço (veja :doc:`/cookbook/controller/service`).

Parâmetros de Rota e Argumentos de Controller
-----------------------------------------

Os parâmetros de rota (ex: ``{slug}``) são especialmente importantes porque
cada um é disponibilizado como um argumento para o método do controller:

.. code-block:: php

    public function showAction($slug)
    {
      // ...
    }

Na realidade, a coleção inteira ``defaults`` é mesclada com um valor deparâmetro
para formar um único array. Cada chave daquele array está disponível como um
argumento no controller.

Em outras palavras, para cada argumento do método do seu controller, Symfony procura
por um parâmetro de rota daquele nome e atribui o valor para aquele argumento.
No exemplo avançado acima, qualquer combinação (em qualquer ordem) das seguintes
variáveis poderia ser usada como argumentos para o método ``showAction()``:

* ``$culture``
* ``$year``
* ``$title``
* ``$_format``
* ``$_controller``

Como os espaços resercados e a coleção ``defaults`` são mesclados juntos, mesmo
a variável ``$_controller`` está disponível. Para uma discussão mais detalhada,
veja :ref:`route-parameters-controller-arguments`.

.. tip::

    Você também pode usar uma variável especial ``$_route``, que é fixada para 
    o nome da rota que foi correspondida.

.. index::
   single: Roteamento; Importando recursos de roteamento

.. _routing-include-external-resources:

Incluindo Recursos Externos de Roteamento
------------------------------------

Todas as rotas são carregadas por um arquivo de configuração individual - geralmente ``app/config/routing.yml``
(veja `Criando Rotas`_ acima). É comum, entretanto, que você queria carregar recursos
de outros lugares, como um arquivo de roteamento que resida dentro de um pacote. Isso
pode ser feito mediante "importação" daquele arquivo:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        acme_hello:
            resource: "@AcmeHelloBundle/Resources/config/routing.yml"

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@AcmeHelloBundle/Resources/config/routing.xml" />
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->addCollection($loader->import("@AcmeHelloBundle/Resources/config/routing.php"));

        return $collection;

.. note::

   Quando importar recursos do YAML, a chave (ex: ``acme_hello``) é insignificante.
   Somente esteja certo que é única, então nenhuma outra linha a sobrescreverá.

A chave ``resource`` carrega o recurso de determinado roteamento. Neste exemplo 
o recurso é um atalho inteiro para o arquivo, onde a sintaxe do atalho ``@AcmeHelloBundle`` 
resolve o atalho daquele pacote. O arquivo importado poderia parecer algo como isso:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/routing.yml
       acme_hello:
            pattern:  /hello/{name}
            defaults: { _controller: AcmeHelloBundle:Hello:index }

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="acme_hello" pattern="/hello/{name}">
                <default key="_controller">AcmeHelloBundle:Hello:index</default>
            </route>
        </routes>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('acme_hello', new Route('/hello/{name}', array(
            '_controller' => 'AcmeHelloBundle:Hello:index',
        )));

        return $collection;

As rotas daquele arquivo são analisadas e carregadas da mesma forma que o arquivo
principal de roteamento.

Prefixando Rotas Importadas
~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode escolher providenciar um "prefixo" para as rotas importadas. Por exemplo
suponha que você queira que a rota ``acme_hello`` tnha um padrão final de ``/admin/hello/{name}``
ao invés de simplesmente ``/hello/{name}``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        acme_hello:
            resource: "@AcmeHelloBundle/Resources/config/routing.yml"
            prefix:   /admin

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@AcmeHelloBundle/Resources/config/routing.xml" prefix="/admin" />
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->addCollection($loader->import("@AcmeHelloBundle/Resources/config/routing.php"), '/admin');

        return $collection;

A string ``/admin`` irá agora ser prefixada ao padrão de cada rota
carregada do novo recurso de roteamento.

.. index::
   single: Roteamento; Depuração
   
Visualizando e Depurando Rotas
------------------------------

Enquanto adiciona e personalizar rotas, é útil ser capaz de visualizar
e obter informação detalhada sobre suas rotas. Um grande modo para ver cada rota
em sua aplicação é pelo comando de console ``router:debug``. Execute
o seguinte comando a partir da raiz de seu projeto.

.. code-block:: bash

    php app/console router:debug

O comando irá imprimir uma lista útil de *todas* as rotas configuradas em
sua aplicação:

.. code-block:: text

    homepage              ANY       /
    contact               GET       /contact
    contact_process       POST      /contact
    article_show          ANY       /articles/{culture}/{year}/{title}.{_format}
    blog                  ANY       /blog/{page}
    blog_show             ANY       /blog/{slug}

Você também pode obter informação muito específica em uma rota individual ao incluir
o nome da rota após o comando:

.. code-block:: bash

    php app/console router:debug article_show

.. index::
   single: Roteamento; Gerando URLs

Gerando URLs
---------------

O sistema de roteamento deveria também ser usado para gerar URLs. Na realidade, roteamento
é um sistema bi-direcional: mapeando a URL para um controller+parâmetros e
parâmetros+rota de voltar para a URL. Os métodos
:method:`Symfony\\Component\\Routing\\Router::match` e
:method:`Symfony\\Component\\Routing\\Router::generate` formam esse sistema
bi-directional. Considere a rota ``blog_show`` de um exemplo anterior::

    $params = $router->match('/blog/my-blog-post');
    // array('slug' => 'my-blog-post', '_controller' => 'AcmeBlogBundle:Blog:show')

    $uri = $router->generate('blog_show', array('slug' => 'my-blog-post'));
    // /blog/my-blog-post

Para gerar a URL, você precisa especificar o nome da rota (ex: ``blog_show``)
e quaisquer coringas(ex: ``slug = my-blog-post``) usado no padrão para 
aquela rota. Com essa informação, qualquer URL pode ser facilmente gerada:

.. code-block:: php

    class MainController extends Controller
    {
        public function showAction($slug)
        {
          // ...

          $url = $this->get('router')->generate('blog_show', array('slug' => 'my-blog-post'));
        }
    }

Em uma sessão futura, você irá aprender como gerar URLs a partir de templates.

.. tip::
    Se o frontend de sua aplicação usa requisições AJAX, você poderia querer
    ser capaz de ferar URLs em JavaScript baseados na sua configuração de roteamento.
    Ao usar `FOSJsRoutingBundle`_, você poderia fazer exatamente isso:
    
    .. code-block:: javascript
    
        var url = Routing.generate('blog_show', { "slug": 'my-blog-post});

    Para mais informações, veja a documentação para aquele pacote.

.. index::
   single: Roteamento; URLs Absolutas

Gerando URLs Absolutas
~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, o roteador irá gerar URLs relativas (ex: ``/blog``). Para gerar
uma URL absoluta, simplesmente passe ``true`` ao terceiro argumento do 
método ``generate()``:

.. code-block:: php

    $router->generate('blog_show', array('slug' => 'my-blog-post'), true);
    // http://www.example.com/blog/my-blog-post

.. note::

    O host que é usado quando gera uma URL absoluta é o host
    do objeto ``Request`` atual. Isso é detectado automaticamente baseado
    na informação do servidor abastecida pelo PHP. Quando gerar URLs absolutas para
    rodar scripts a partir da linha de comando, você precisará fixar manualmente o
    host no objeto ``Request``:
    
    .. code-block:: php
    
        $request->headers->set('HOST', 'www.example.com');

.. index::
   single: Roteamento; Gerando URLs num template
   
Gerando URLs com Strings de Consulta
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O método ``generate`` pega um array de valores coringa para gerar a URI.
Mas se você passar valores extras, eles serão adicionados ao URI como uma string de consulta::

    $router->generate('blog', array('page' => 2, 'category' => 'Symfony'));
    // /blog/2?category=Symfony

Gerando URLs de um template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O lugar mais comum para gerar uma URL é pelo template, ao fazer vinculação
entre páginas na sua aplicação.Isso é feito da mesma forma que antes, mas
usando uma função helper de template:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path('blog_show', { 'slug': 'my-blog-post' }) }}">
          Read this blog post.
        </a>

    .. code-block:: php

        <a href="<?php echo $view['router']->generate('blog_show', array('slug' => 'my-blog-post')) ?>">
            Read this blog post.
        </a>

URLs absolutas também podem ser geradas.

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ url('blog_show', { 'slug': 'my-blog-post' }) }}">
          Read this blog post.
        </a>

    .. code-block:: php

        <a href="<?php echo $view['router']->generate('blog_show', array('slug' => 'my-blog-post'), true) ?>">
            Read this blog post.
        </a>

Sumário
-------

Roteamento é um sistema para mapear a URL de requisiçõs recebidas para a função
do controller que deveria ser chamada para processar a requisição. Em ambas permite
a você especificar URLs bonitas e manter a funcionalidade de sua aplicação
desacoplada daquelas URLs. Roteamento é um mecanismo de duas vias, significando que 
também deveria ser usada para gerar URLs.

Aprenda mais do Cookbook
----------------------------

* :doc:`/cookbook/routing/scheme`

.. _`FOSJsRoutingBundle`: https://github.com/FriendsOfSymfony/FOSJsRoutingBundle
