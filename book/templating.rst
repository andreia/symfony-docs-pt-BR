.. index::
   single: Templating

Criando e usando Templates
============================

Como você sabe o :doc:`controller </book/controller>` é responsável por
controlar cada requisição que venha de uma aplicação Symfony2. Na realidade,
o controller delega muito do trabalho pesado para outros lugares então aquele
código pode ser testado e reusado. Quando um controller precisa gerar HTML,
CSS e qualquer outro conteúdo, ele entrega o trabalho para a engine de template.
Nesse capítulo, você irá aprendder como escrever templates poderosas que podem ser
usada para retornar conteúdo para o usuário, preencher corpo de e-mail, e mais. Você 
irá aprender atalhos, maneiras espertas de extender templates e como reusar código
de template.

.. index::
   single: Templating;O que é um template?

Templates
---------

Um template é simplesmente um arquivo de texto que pode gerar qualquer formato baseado em texto 
(HTML, XML, CSV, LaTeX ...). O tipo mais familiar de template é um 
template em *PHP* - um arquivo de texto analisado pelo PHP que contém uma mistura de texto e código PHP::

    <!DOCTYPE html>
    <html>
        <head>
            <title>Welcome to Symfony!</title>
        </head>
        <body>
            <h1><?php echo $page_title ?></h1>

            <ul id="navigation">
                <?php foreach ($navigation as $item): ?>
                    <li>
                        <a href="<?php echo $item->getHref() ?>">
                            <?php echo $item->getCaption() ?>
                        </a>
                    </li>
                <?php endforeach; ?>
            </ul>
        </body>
    </html>

.. index:: Twig; Introdução

Mas Symfony2 empacota até mesmo uma linguagem muito poderosa de template chamada `Twig`_.
Twig permit a você escrever templates consisos e legíveis que são mais amigáveis
para web designers e, e de uma certa forma, mais poderosos que templates de PHP:

.. code-block:: html+jinja

    <!DOCTYPE html>
    <html>
        <head>
            <title>Welcome to Symfony!</title>
        </head>
        <body>
            <h1>{{ page_title }}</h1>

            <ul id="navigation">
                {% for item in navigation %}
                    <li><a href="{{ item.href }}">{{ item.caption }}</a></li>
                {% endfor %}
            </ul>
        </body>
    </html>

Twig degine dois tipos de sintaxe especiais:

* ``{{ ... }}``: "Diga algo": exibe uma variável ou o resultado de uma
  expressão para o template;
  
* ``{% ... %}``: "Faça algo": uma **tag** que controla a lógica do
  template; ela é usada para executar certas sentenças como for-loops por exemplo.

.. note::

   Há uma terceira sintaxe usada para criar comentários ``{# this is a comment #}``.
   Essa sintaxe pode ser usada através de múltiplas linhas, parecidas com a sintaxe
   equivalente em PHP ``/* comment */``.

Twig também contém **filtros**, que modificam conteúdo antes de serem interpretados.
O seguinte filtro transforma a variável ``title`` toda em letra maiúscula antes de interpretá-la:

.. code-block:: jinja

    {{ title | upper }}

Twig vem com uma longa lista de `tags`_ e `filtros`_ que estão disponíveis
por padrão. Você pode até mesmo `adicionar suas próprias extensões`_ para o Twig quando precisar.

.. tip::

    Registrar uma extensão Twig é tão fácil quanto criar um novo serviço e atribuir tag
    nele com ``twig.extension`` :ref:`tag<reference-dic-tags-twig-extension>`.

Como você verá através da documentação, Twig também suporta funções
e nova funções podem ser facilmente adicionadas. Por exemplo. a seguinte função usa
uma tag padrão ``for``  e a função ``cycle`` para então imprimir dez tags div, alternando
entre classes ``odd`` e ``even``:

.. code-block:: html+jinja

    {% for i in 0..10 %}
      <div class="{{ cycle(['odd', 'even'], i) }}">
        <!-- some HTML here -->
      </div>
    {% endfor %}

Durante este capítulo, exemplos de template serão mostrados tanto em Twig como PHP.

.. sidebar:: Por que Twig?
    
    Templates Twig são feitas para serem simples e não irão processar tags PHP. Isto
    é pelo design: o sistema de template do Twig é feito para expressar apresentação,
    não lógica de programa. Quanto mais você usa Twig, mais você irá apreciar e beneficiar
    desta distinção. E claro, você será amado por web designers de todos os lugares.
    
    Twig pode também fazer coisas que PHP não pode, como por exemplo herança verdadeira de template
    (Templates do Twig compilam classes PHP que herdam uma da outra),
    controle de espaço em branco, caixa de areia, e a inclusão de funções personalizadas
    e filtros que somente afetam templates. Twig contém pequenos recursos
    que fazem escrita de templates mais fácil e mais concisa. Considere o seguinte 
    exemplo, que combina um loop com uma sentença lógia ``if``:
    
    .. code-block:: html+jinja
    
        <ul>
            {% for user in users %}
                <li>{{ user.username }}</li>
            {% else %}
                <li>No users found</li>
            {% endfor %}
        </ul>

.. index::
   pair: Twig; Cache

Cache do Template Twig
~~~~~~~~~~~~~~~~~~~~~

Twig é rápido. Cada template Twig é compilado para uma classe nativa PHP
que é processada na execução. As classes compiladas são localizadas no
diretório ``app/cache/{environment}/twig`` (onde ``{environment}`` é o
ambiente, como ``dev`` ou ``prod``), e em alguns casos pode ser útil durante 
a depuração. Veja :ref:`environments-summary` para mais informações de
ambientes.

Quando o modo ``debug`` é abilitado (comum no ambiente ``dev``), um template
Twig será automaticamente recompilado quando mudanças são feitas nele. Isso
signitica que durante o desenvolvimento você pode alegremente fazer mudanças para um template Twig
e instântaneamente ver as mudanças sem precisar se preocupar sobre limpar qualquer
cache.

Quando o modo ``debug`` é desabilitado (comum no ambiente ``prod``), entretanto,
você deve limpar o cache do diretório Twig para que então os templates Twig
se regenerem. Lembre de fazer isso quando distribuir sua aplicação.

.. index::
   single: Templating; Inheritance

Herança e Layouts de Template
--------------------------------

Mais frequentemente que não, templates compartilham elementos comuns em um projeto, como o
header, footer, sidebar ou outros. Em Symfony2,  nós gostamos de pensar sobre esse
problema de forma diferente: um template pode ser decorado por outro. Isso funciona
exatemente da mesma forma como classes PHP: herança de template permite você construir
um "layout" de template base que contenha todos os elementos comuns de seu site
definidos como **blocos** (pense em "classe PHP com métodos base"). Um template filho
pode extender o layout base e sobrepor os blocos (pense "subclasse PHP 
que sobreponha certos métodos de sua classe pai").

Primeiro, construa um arquivo de layout de base:

.. configuration-block::

    .. code-block:: html+jinja

        {# app/Resources/views/base.html.twig #}
        <!DOCTYPE html>
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
                <title>{% block title %}Test Application{% endblock %}</title>
            </head>
            <body>
                <div id="sidebar">
                    {% block sidebar %}
                    <ul>
                        <li><a href="/">Home</a></li>
                        <li><a href="/blog">Blog</a></li>
                    </ul>
                    {% endblock %}
                </div>

                <div id="content">
                    {% block body %}{% endblock %}
                </div>
            </body>
        </html>

    .. code-block:: php

        <!-- app/Resources/views/base.html.php -->
        <!DOCTYPE html>
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
                <title><?php $view['slots']->output('title', 'Test Application') ?></title>
            </head>
            <body>
                <div id="sidebar">
                    <?php if ($view['slots']->has('sidebar'): ?>
                        <?php $view['slots']->output('sidebar') ?>
                    <?php else: ?>
                        <ul>
                            <li><a href="/">Home</a></li>
                            <li><a href="/blog">Blog</a></li>
                        </ul>
                    <?php endif; ?>
                </div>

                <div id="content">
                    <?php $view['slots']->output('body') ?>
                </div>
            </body>
        </html>

.. note::
    
    Apesar da discussão sobre herança de template ser em termos do Twig,
    a filosofia é a mesma entre templates Twig e PHP.

Este template define o esqueleto do documento base HTML de um página simples
de duas colunas. Neste exemplo, três áreas ``{% block %}`` são definidas (``title``,
``sidebar`` e ``body``). Cada bloco pode ser sobreposto por um template filho
ou largado com sua implementação padrão. Esse template poderia também ser processado
diretamente. Neste caso os blocos ``title``, ``sidebar`` e ``body`` blocks deveriam
simplesmente reter os valores padrão neste template.

Um template filho poderia ser como este:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Blog/index.html.twig #}
        {% extends '::base.html.twig' %}

        {% block title %}My cool blog posts{% endblock %}

        {% block body %}
            {% for entry in blog_entries %}
                <h2>{{ entry.title }}</h2>
                <p>{{ entry.body }}</p>
            {% endfor %}
        {% endblock %}

    .. code-block:: php

        <!-- src/Acme/BlogBundle/Resources/views/Blog/index.html.php -->
        <?php $view->extend('::base.html.php') ?>

        <?php $view['slots']->set('title', 'My cool blog posts') ?>

        <?php $view['slots']->start('body') ?>
            <?php foreach ($blog_entries as $entry): ?>
                <h2><?php echo $entry->getTitle() ?></h2>
                <p><?php echo $entry->getBody() ?></p>
            <?php endforeach; ?>
        <?php $view['slots']->stop() ?>

.. note::
    
   O template pai é idenficado por uma sintaxe especial de string
   (``::base.html.twig``) que indica que o template reside no diretório
   ``app/Resources/views`` do projeto. Essa convenção de nomeamento é
   explicada inteiramente em :ref:`template-naming-locations`.

A chave para herança template é a tag  ``{% extends %}``. Ela avisa
a engine de template para primeiro avaliar o template base, que configura
o layout e define vários blocos. O template filho é então processado,
ao ponto que os blocos  ``title`` e ``body`` do template pai sejam substituídos
por aqueles do filho. Dependendo do valor de ``blog_entries``, a
saída poderia parecer com isso::

    <!DOCTYPE html>
    <html>
        <head>
            <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
            <title>My cool blog posts</title>
        </head>
        <body>
            <div id="sidebar">
                <ul>
                    <li><a href="/">Home</a></li>
                    <li><a href="/blog">Blog</a></li>
                </ul>
            </div>

            <div id="content">
                <h2>My first post</h2>
                <p>The body of the first post.</p>

                <h2>Another post</h2>
                <p>The body of the second post.</p>
            </div>
        </body>
    </html>

Perceba que como o template filho não definiu um bloco ``sidebar``, o
valor do template pai é usado no lugar. Conteúdo dentro de uma tag  ``{% block %}``
em um template pai é sempre usado por padrão.

Você pode usar muitos níveis de herança quanto quiser. Na próxima sessão,
um modelo comum de herança de três níveis será explicado assim como os templates
são organizados dentro de um projeto Symfony2.

Quando trabalhar com herança de template, aqui estão algumas dicas para guardar na cabeça:

* Se você usa ``{% extends %}`` em um template, ele deve ser a primeira tag 
  naquele template.
  
* Quanto mais tags ``{% block %}`` você tiver no template base, melhor.
  Lembre, templates filhos não precisam definir todos os blocos do pai, então
  criar tantos blocos em seus templates base quanto você quiser e dar a cada um
  padrão sensato. Quanto mais blocos seus templates base tiverem, mais flexível
  seu layout será.

* Se você achar você mesmo duplicando conteúdo em um determinado número de templates, isto provavelmente
  significa que você deveria mover aquele conteúdo para um ``{% block %}`` no template pai.
  Em alguns casos, uma solução melhor pode ser mover o conteúdo para um novo template
  e ``incluir`` ele (veja :ref:`including-templates`).

* Se você precisa obter o conteúdo de um bloco do template pai, você
  pode usar a função ``{{ parent() }}``. Isso é útil se você quiser adicionar
  ao conteúdo de um bloco pai ao invés de sobrepor ele::
  
    .. code-block:: html+jinja

        {% block sidebar %}
            <h3>Table of Contents</h3>
            ...
            {{ parent() }}
        {% endblock %}

.. index::
   single: Templating; Convenções de Nomeação
   single: Templating; Localização de Arquivos

.. _template-naming-locations:

Nomeação de Template e Localizações
-----------------------------

Por padrão, templates podem residir em duas localizações diferentes:

* ``app/Resources/views/``: O diretório de aplicação de ``views`` pode abrigar
  templates bases para toda a aplicação(ex: os layout de sua aplicação) assim como
  os tempates que sobrepõem templates de pacote (veja :ref:`overriding-bundle-templates`);
  
  application-wide base templates (i.e. your application's layouts) as well as
  templates that override bundle templates (see
  :ref:`overriding-bundle-templates`);
  

* ``path/to/bundle/Resources/views/``: Cada pacote abriga as templates dele no diretório 
  ``Resources/views`` (e sub-diretórios). A maioria dos templates irá residir dentro de
  um pacote.
  
Symfony2 usa a sintaxe de string **bundle**:**controller**:**template** para
templates. Isso permite vários tipos diferente de template, cada um residindo
em uma localização especifica:

* ``AcmeBlogBundle:Blog:index.html.twig``: Esta sintaxe é usada para especificar um
  template para uma página específica. As três partes do string, cada uma separada
  por dois pontos, (``:``), signitca o seguinte:

    * ``AcmeBlogBundle``: (*bundle*) o template reside entro de
      ``AcmeBlogBundle`` (e.g. ``src/Acme/BlogBundle``);

    * ``Blog``: (*controller*) indica que o template reside dentro do
       sub-diretório ``Blog`` de ``Resources/views``;

    * ``index.html.twig``: (*template*) o verdadeiro nome do arquivo é
      ``index.html.twig``.

  Assumindo que o ``AcmeBlogBundle`` reside em ``src/Acme/BlogBundle``, o
  atalho final para o layout seria ``src/Acme/BlogBundle/Resources/views/Blog/index.html.twig``.

* ``AcmeBlogBundle::layout.html.twig``: Essa sintaxe refere ao template base que
  é específica para ``AcmeBlogBundle``. Since the middle, "controller",
  portion is missing (e.g. ``Blog``), the template lives at
  ``Resources/views/layout.html.twig`` inside ``AcmeBlogBundle``.

* ``::base.html.twig``: Esta sintaxe refere a uma template base para toda a aplicação ou
  layout. Perceba que a string começa com dois sinais de dois pontos (``::``), significando
  que ambas as partes *bundle*  *controller* estão faltando. Isto significa
  que o template não é localizado em qualquer pacote, mas sim na raiz do 
  diretório ``app/Resources/views/``.

Na seção :ref:`overriding-bundle-templates`, você irá descobrir como cada
template reside dentro do ``AcmeBlogBundle``, por exemplo, pode ser sobreposto
ao colocar um template de mesmo nome no diretório 
``app/Resources/AcmeBlogBundle/views/``. Isso dá o poder de sobrepor templates de qualquer pacote pago.

.. tip::
    
    Esperançosamente, a sintaxe de nomeação de template parece familiar - é a mesma convenção
    para nomeação usada para referir para :ref:`controller-string-syntax`.

Sufixo de Template 
~~~~~~~~~~~~~~~

O formato **bundle**:**controller**:**template** de cada template especifica
*onde* o arquivo de template está localizado. Cada nome de template também tem duas expressões
que especificam o *formato* e *engine* para aquela template.

* **AcmeBlogBundle:Blog:index.html.twig** - formato HTML, engine Twig

* **AcmeBlogBundle:Blog:index.html.php** - formato HTML, engine PHP

* **AcmeBlogBundle:Blog:index.css.twig** - formato CSS, engine Twig

Por padrão, qualquer template Symfony2 ou pode ser escrito em Twig ou em PHP, e
a última  parte da extensão (ex: ``.twig`` ou ``.php``) especifica qual
dessas duas *engines* deveria ser usada. A primeira parte da extensão,
(ex: ``.html``, ``.css``, etc) é o formato final que o template irá
gerar. Ao contrário de engine, que determina como Symfony2 analisa o template,
isso é simplesmente uma tática organizacional em caso do mesmo recurso precisar
ser transformado como HTML(``index.html.twig``), XML (``index.xml.twig``),
ou qualquer outro formato. Para mais informaçõess, leia a seção :ref:`template-formats`.

.. note::
    
   As "engines" disponíveis podem ser configurados e até mesmo ter novas engines adicionadas.
   Veja :ref:`Configuração de Template<template-configuration>` para mais detalhes.

.. index::
   single: Templating; Tags e Helpers
   single: Templating; Helpers

Tags e Helpers
----------------

Você já entende as bases do templates, como eles são chamados e como usar 
herança de template. As partes mais difíceis estão realmente atrás de você. Nesta
seção, você irá aprender sobre um grande grande grupo de ferramentas disponíveis para ajudar
a desempenhar as tarefas de template mais comuns como incluir outras templates,
vincular páginas e incluir imagens.


Symfony2 vem acompanhado com várias tags Twig especializadas e funções que
facilitam o trabalho do designer de template. Em PHP, o sistema de template providencia
um sistema extenso de *helper* que providencia funcionalidades úteis no contexto
de template.

Nós realmente vimos umas poucas tags Twig construídas (``{% block %}`` e ``{% extends %}``)
como exemplo de um helper PHP (``$view['slots']``). Vamos aprender um
pouco mais.

.. index::
   single: Templating; Incluir outras templates

.. _including-templates:

Incluir outras Templates
~~~~~~~~~~~~~~~~~~~~~~~~~

Você irá frequntemente querer incluir a mesma template ou fragmento de código em várias
páginas diferentes. Por exemplo, em uma aplicação com "artigos de notícias", a exibição
do artigo no código do template poderia ser usada numa página de detalhes do artigo,
num a página exibindo os artigos mais populares, ou em uma lista dos últimos
artigos.

Quando você precisa reutilizar um pedaço de um código PHP, você tipicamente move o código
para uma nova classe ou função PHP. O mesmo é verdade para template. Ao mover o
código do template reutilizado em um template próprio, ele pode ser incluído em qualquer outro
template. Primeiro, crie o template que você precisará reutilizar.

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/ArticleBundle/Resources/views/Article/articleDetails.html.twig #}
        <h2>{{ article.title }}</h2>
        <h3 class="byline">by {{ article.authorName }}</h3>

        <p>
          {{ article.body }}
        </p>

    .. code-block:: php

        <!-- src/Acme/ArticleBundle/Resources/views/Article/articleDetails.html.php -->
        <h2><?php echo $article->getTitle() ?></h2>
        <h3 class="byline">by <?php echo $article->getAuthorName() ?></h3>

        <p>
          <?php echo $article->getBody() ?>
        </p>

Incluir este template de qualquer outro template é fácil:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/ArticleBundle/Resources/Article/list.html.twig #}
        {% extends 'AcmeArticleBundle::layout.html.twig' %}

        {% block body %}
            <h1>Recent Articles<h1>

            {% for article in articles %}
                {% include 'AcmeArticleBundle:Article:articleDetails.html.twig' with {'article': article} %}
            {% endfor %}
        {% endblock %}

    .. code-block:: php

        <!-- src/Acme/ArticleBundle/Resources/Article/list.html.php -->
        <?php $view->extend('AcmeArticleBundle::layout.html.php') ?>

        <?php $view['slots']->start('body') ?>
            <h1>Recent Articles</h1>

            <?php foreach ($articles as $article): ?>
                <?php echo $view->render('AcmeArticleBundle:Article:articleDetails.html.php', array('article' => $article)) ?>
            <?php endforeach; ?>
        <?php $view['slots']->stop() ?>

O template é incluído usando a tag ``{% include %}``. Perceba que o
nome do template segue a mesma convenção típica. O template ``articleDetails.html.twig``
usa uma variável ``article``. Isso é passado por um template ``list.html.twig``
usando o comando ``with``.

.. tip::

    A sintaxe ``{'article': article}`` é a sintaxe Twig padrão para hash
    maps (ex:  um array com chaves nomeadas). Se nós precisarmos passá-lo em elementos
    múltiplos, ele poderia ser algo como: ``{'foo': foo, 'bar': bar}``.

.. index::
   single: Templating; Incorporação de ações
   
.. _templating-embedding-controller:

Incorporação de Controllers
~~~~~~~~~~~~~~~~~~~~~

Em alguns casos, você precisa fazer mais do que incluir um template simples. Suponha
que você tenha uma barra lateral no seu layout que contenha os três artigos mais recentes.
Recuperar os três artigos podem incluir consultar a base de dados ou desempenhar
outra lógica pesada que não pode ser a partir de um template.

A solução é simplesmnte incorporar o resultado de um controller inteiro para
seu template. Primeiro, crie o controller que retorne um certo número de artigos
recentes :

.. code-block:: php

    // src/Acme/ArticleBundle/Controller/ArticleController.php

    class ArticleController extends Controller
    {
        public function recentArticlesAction($max = 3)
        {
            // make a database call or other logic to get the "$max" most recent articles
            $articles = ...;

            return $this->render('AcmeArticleBundle:Article:recentList.html.twig', array('articles' => $articles));
        }
    }

A template ``recentList`` é perfeitamente straightforward:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/ArticleBundle/Resources/views/Article/recentList.html.twig #}
        {% for article in articles %}
          <a href="/article/{{ article.slug }}">
              {{ article.title }}
          </a>
        {% endfor %}

    .. code-block:: php

        <!-- src/Acme/ArticleBundle/Resources/views/Article/recentList.html.php -->
        <?php foreach ($articles as $article): ?>
            <a href="/article/<?php echo $article->getSlug() ?>">
                <?php echo $article->getTitle() ?>
            </a>
        <?php endforeach; ?>

.. note::
    Perceba que nós fizemos uma gambiarra e fizemos um hardcode no artigo URL desse exemplo
    (ex: ``/article/*slug*``). Isso é uma prática ruim. Na próxima seção,
    você irá aprender como fazer isso corretamente.
    
Para incluir um controller, você irá precisar referir a ela usando a sintaxe de string
padrão para controllers (isto é, **bundle**:**controller**:**action**):

.. configuration-block::

    .. code-block:: html+jinja

        {# app/Resources/views/base.html.twig #}
        ...

        <div id="sidebar">
            {% render "AcmeArticleBundle:Article:recentArticles" with {'max': 3} %}
        </div>

    .. code-block:: php

        <!-- app/Resources/views/base.html.php -->
        ...

        <div id="sidebar">
            <?php echo $view['actions']->render('AcmeArticleBundle:Article:recentArticles', array('max' => 3)) ?>
        </div>

Sempre quando você pensar que você precisa de uma variável ou uma peça de informação que
você não tenha acesso em um template, considere transformar o controller.
Controllers são rápidos para executar e promovem uma boa organização e utilização do código.

.. index::
   single: Templating; Vinculação às páginas

Vinculação às Páginas
~~~~~~~~~~~~~~~~

Criar links para outras página em sua aplicação é uma das tarefas mais 
comuns para um template. Ao invés de fazer um hardcode nas URLs nos templates, use a
função do Twig ``path`` (ou o helper ``router`` no PHP) para gerar URLs baseadas na
configuração de roteamento. Mais tarde, se você quiser modificar a URL de uma página
particular, tudo que você precisará fazer é mudar as configurações de roteamento; os
templates irão automatricamente gerar a nova URL.

Primeiro, vincule a página "_welcome" , que é acessível pela seguinte configuração de
roteamento:

.. configuration-block::

    .. code-block:: yaml

        _welcome:
            pattern:  /
            defaults: { _controller: AcmeDemoBundle:Welcome:index }

    .. code-block:: xml

        <route id="_welcome" pattern="/">
            <default key="_controller">AcmeDemoBundle:Welcome:index</default>
        </route>

    .. code-block:: php

        $collection = new RouteCollection();
        $collection->add('_welcome', new Route('/', array(
            '_controller' => 'AcmeDemoBundle:Welcome:index',
        )));

        return $collection;

Para vincular à página, apenas use a função Twig ``path`` e refira para a rota:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path('_welcome') }}">Home</a>

    .. code-block:: php

        <a href="<?php echo $view['router']->generate('_welcome') ?>">Home</a>

Como esperado, isso irá gerar a URL ``/``. Vamos ver como isso irá funcionar com
uma rota mais complicada:

.. configuration-block::

    .. code-block:: yaml

        article_show:
            pattern:  /article/{slug}
            defaults: { _controller: AcmeArticleBundle:Article:show }

    .. code-block:: xml

        <route id="article_show" pattern="/article/{slug}">
            <default key="_controller">AcmeArticleBundle:Article:show</default>
        </route>

    .. code-block:: php

        $collection = new RouteCollection();
        $collection->add('article_show', new Route('/article/{slug}', array(
            '_controller' => 'AcmeArticleBundle:Article:show',
        )));

        return $collection;

Neste caso, você precisa especificar tanto o nome da rota (``article_show``) como
um valor para o parâmetro ``{slug}``. Usando esta rota, vamos revisitar o 
template ``recentList`` da sessão anterior e vincular aos artigos
corretamente:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/ArticleBundle/Resources/views/Article/recentList.html.twig #}
        {% for article in articles %}
          <a href="{{ path('article_show', { 'slug': article.slug }) }}">
              {{ article.title }}
          </a>
        {% endfor %}

    .. code-block:: php

        <!-- src/Acme/ArticleBundle/Resources/views/Article/recentList.html.php -->
        <?php foreach ($articles in $article): ?>
            <a href="<?php echo $view['router']->generate('article_show', array('slug' => $article->getSlug()) ?>">
                <?php echo $article->getTitle() ?>
            </a>
        <?php endforeach; ?>

.. tip::
    
    Você também pode gerar uma URL absoluta ao usar a função ``url`` do Twig:

    .. code-block:: html+jinja

        <a href="{{ url('_welcome') }}">Home</a>
    
    O mesmo pode ser feito em templates PHP ao passar um terceiro argumento ao
    método ``generate()``:

    .. code-block:: php

        <a href="<?php echo $view['router']->generate('_welcome', array(), true) ?>">Home</a>

.. index::
   single: Templating; Vinculando os assets

Vinculando os Assets
~~~~~~~~~~~~~~~~~

Templates podem frequentemente referir a imagens, Javascript, folhas de estilo e outros
recursos. Claro você poderia fazer um hardcode do atalho desses assets (ex: ``/images/logo.png``),
mas Symfony2 providencia uma opção mais dinâmica via função ``assets`` do Twig:

.. configuration-block::

    .. code-block:: html+jinja

        <img src="{{ asset('images/logo.png') }}" alt="Symfony!" />

        <link href="{{ asset('css/blog.css') }}" rel="stylesheet" type="text/css" />

    .. code-block:: php

        <img src="<?php echo $view['assets']->getUrl('images/logo.png') ?>" alt="Symfony!" />

        <link href="<?php echo $view['assets']->getUrl('css/blog.css') ?>" rel="stylesheet" type="text/css" />

The ``asset`` function's main purpose is to make your application more portable.
If your application lives at the root of your host (e.g. http://example.com),
then the rendered paths should be ``/images/logo.png``. But if your application
lives in a subdirectory (e.g. http://example.com/my_app), each asset path
should render with the subdirectory (e.g. ``/my_app/images/logo.png``). The
``asset`` function takes care of this by determining how your application is
being used and generating the correct paths accordingly.

Additionally, if you use the ``asset`` function, Symfony can automatically
append a query string to your asset, in order to guarantee that updated static
assets won't be cached when deployed. For example, ``/images/logo.png`` might
look like ``/images/logo.png?v2``. For more information, see the :ref:`ref-framework-assets-version`
configuration option.

.. index::
   single: Templating; Including stylesheets and Javascripts
   single: Stylesheets; Including stylesheets
   single: Javascripts; Including Javascripts

Including Stylesheets and Javascripts in Twig
---------------------------------------------

No site would be complete without including Javascript files and stylesheets.
In Symfony, the inclusion of these assets is handled elegantly by taking
advantage of Symfony's template inheritance.

.. tip::

    This section will teach you the philosophy behind including stylesheet
    and Javascript assets in Symfony. Symfony also packages another library,
    called Assetic, which follows this philosophy but allows you to do much
    more interesting things with those assets. For more information on 
    using Assetic see :doc:`/cookbook/assetic/asset_management`.


Start by adding two blocks to your base template that will hold your assets:
one called ``stylesheets`` inside the ``head`` tag and another called ``javascripts``
just above the closing ``body`` tag. These blocks will contain all of the
stylesheets and Javascripts that you'll need throughout your site:

.. code-block:: html+jinja

    {# 'app/Resources/views/base.html.twig' #}
    <html>
        <head>
            {# ... #}

            {% block stylesheets %}
                <link href="{{ asset('/css/main.css') }}" type="text/css" rel="stylesheet" />
            {% endblock %}
        </head>
        <body>
            {# ... #}

            {% block javascripts %}
                <script src="{{ asset('/js/main.js') }}" type="text/javascript"></script>
            {% endblock %}
        </body>
    </html>

That's easy enough! But what if you need to include an extra stylesheet or
Javascript from a child template? For example, suppose you have a contact
page and you need to include a ``contact.css`` stylesheet *just* on that
page. From inside that contact page's template, do the following:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Contact/contact.html.twig #}
    {# extends '::base.html.twig' #}

    {% block stylesheets %}
        {{ parent() }}
        
        <link href="{{ asset('/css/contact.css') }}" type="text/css" rel="stylesheet" />
    {% endblock %}
    
    {# ... #}

In the child template, you simply override the ``stylesheets`` block and 
put your new stylesheet tag inside of that block. Of course, since you want
to add to the parent block's content (and not actually *replace* it), you
should use the ``parent()`` Twig function to include everything from the ``stylesheets``
block of the base template.

You can also include assets located in your bundles' ``Resources/public`` folder.
You will need to run the ``php app/console assets:install target [--symlink]``
command, which moves (or symlinks) files into the correct location. (target
is by default "web").

.. code-block:: html+jinja

   <link href="{{ asset('bundles/acmedemo/css/contact.css') }}" type="text/css" rel="stylesheet" />

The end result is a page that includes both the ``main.css`` and ``contact.css``
stylesheets.

.. index::
   single: Templating; The templating service

Configuring and using the ``templating`` Service
------------------------------------------------

The heart of the template system in Symfony2 is the templating ``Engine``.
This special object is responsible for rendering templates and returning
their content. When you render a template in a controller, for example,
you're actually using the templating engine service. For example:

.. code-block:: php

    return $this->render('AcmeArticleBundle:Article:index.html.twig');

is equivalent to

.. code-block:: php

    $engine = $this->container->get('templating');
    $content = $engine->render('AcmeArticleBundle:Article:index.html.twig');

    return $response = new Response($content);

.. _template-configuration:

The templating engine (or "service") is preconfigured to work automatically
inside Symfony2. It can, of course, be configured further in the application
configuration file:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            templating: { engines: ['twig'] }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:templating>
            <framework:engine id="twig" />
        </framework:templating>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating'      => array(
                'engines' => array('twig'),
            ),
        ));

Several configuration options are available and are covered in the
:doc:`Configuration Appendix</reference/configuration/framework>`.

.. note::

   The ``twig`` engine is mandatory to use the webprofiler (as well as many
   third-party bundles).

.. index::
    single; Template; Overriding templates

.. _overriding-bundle-templates:

Overriding Bundle Templates
---------------------------

The Symfony2 community prides itself on creating and maintaining high quality
bundles (see `Symfony2Bundles.org`_) for a large number of different features.
Once you use a third-party bundle, you'll likely need to override and customize
one or more of its templates.

Suppose you've included the imaginary open-source ``AcmeBlogBundle`` in your
project (e.g. in the ``src/Acme/BlogBundle`` directory). And while you're
really happy with everything, you want to override the blog "list" page to
customize the markup specifically for your application. By digging into the
``Blog`` controller of the ``AcmeBlogBundle``, you find the following::

    public function indexAction()
    {
        $blogs = // some logic to retrieve the blogs

        $this->render('AcmeBlogBundle:Blog:index.html.twig', array('blogs' => $blogs));
    }

When the ``AcmeBlogBundle:Blog:index.html.twig`` is rendered, Symfony2 actually
looks in two different locations for the template:

#. ``app/Resources/AcmeBlogBundle/views/Blog/index.html.twig``
#. ``src/Acme/BlogBundle/Resources/views/Blog/index.html.twig``

To override the bundle template, just copy the ``index.html.twig`` template
from the bundle to ``app/Resources/AcmeBlogBundle/views/Blog/index.html.twig``
(the ``app/Resources/AcmeBlogBundle`` directory won't exist, so you'll need
to create it). You're now free to customize the template.

This logic also applies to base bundle templates. Suppose also that each
template in ``AcmeBlogBundle`` inherits from a base template called
``AcmeBlogBundle::layout.html.twig``. Just as before, Symfony2 will look in
the following two places for the template:

#. ``app/Resources/AcmeBlogBundle/views/layout.html.twig``
#. ``src/Acme/BlogBundle/Resources/views/layout.html.twig``

Once again, to override the template, just copy it from the bundle to
``app/Resources/AcmeBlogBundle/views/layout.html.twig``. You're now free to
customize this copy as you see fit.

If you take a step back, you'll see that Symfony2 always starts by looking in
the ``app/Resources/{BUNDLE_NAME}/views/`` directory for a template. If the
template doesn't exist there, it continues by checking inside the
``Resources/views`` directory of the bundle itself. This means that all bundle
templates can be overridden by placing them in the correct ``app/Resources``
subdirectory.

.. _templating-overriding-core-templates:

.. index::
    single; Template; Overriding exception templates

Overriding Core Templates
~~~~~~~~~~~~~~~~~~~~~~~~~

Since the Symfony2 framework itself is just a bundle, core templates can be
overridden in the same way. For example, the core ``TwigBundle`` contains
a number of different "exception" and "error" templates that can be overridden
by copying each from the ``Resources/views/Exception`` directory of the
``TwigBundle`` to, you guessed it, the
``app/Resources/TwigBundle/views/Exception`` directory.

.. index::
   single: Templating; Three-level inheritance pattern

Three-level Inheritance
-----------------------

One common way to use inheritance is to use a three-level approach. This
method works perfectly with the three different types of templates we've just
covered:

* Create a ``app/Resources/views/base.html.twig`` file that contains the main
  layout for your application (like in the previous example). Internally, this
  template is called ``::base.html.twig``;

* Create a template for each "section" of your site. For example, an ``AcmeBlogBundle``,
  would have a template called ``AcmeBlogBundle::layout.html.twig`` that contains
  only blog section-specific elements;

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/layout.html.twig #}
        {% extends '::base.html.twig' %}

        {% block body %}
            <h1>Blog Application</h1>

            {% block content %}{% endblock %}
        {% endblock %}

* Create individual templates for each page and make each extend the appropriate
  section template. For example, the "index" page would be called something
  close to ``AcmeBlogBundle:Blog:index.html.twig`` and list the actual blog posts.

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Blog/index.html.twig #}
        {% extends 'AcmeBlogBundle::layout.html.twig' %}

        {% block content %}
            {% for entry in blog_entries %}
                <h2>{{ entry.title }}</h2>
                <p>{{ entry.body }}</p>
            {% endfor %}
        {% endblock %}

Notice that this template extends the section template -(``AcmeBlogBundle::layout.html.twig``)
which in-turn extends the base application layout (``::base.html.twig``).
This is the common three-level inheritance model.

When building your application, you may choose to follow this method or simply
make each page template extend the base application template directly
(e.g. ``{% extends '::base.html.twig' %}``). The three-template model is
a best-practice method used by vendor bundles so that the base template for
a bundle can be easily overridden to properly extend your application's base
layout.

.. index::
   single: Templating; Output escaping

Output Escaping
---------------

When generating HTML from a template, there is always a risk that a template
variable may output unintended HTML or dangerous client-side code. The result
is that dynamic content could break the HTML of the resulting page or allow
a malicious user to perform a `Cross Site Scripting`_ (XSS) attack. Consider
this classic example:

.. configuration-block::

    .. code-block:: jinja

        Hello {{ name }}

    .. code-block:: php

        Hello <?php echo $name ?>

Imagine that the user enters the following code as his/her name::

    <script>alert('hello!')</script>

Without any output escaping, the resulting template will cause a JavaScript
alert box to pop up::

    Hello <script>alert('hello!')</script>

And while this seems harmless, if a user can get this far, that same user
should also be able to write JavaScript that performs malicious actions
inside the secure area of an unknowing, legitimate user.

The answer to the problem is output escaping. With output escaping on, the
same template will render harmlessly, and literally print the ``script``
tag to the screen::

    Hello &lt;script&gt;alert(&#39;helloe&#39;)&lt;/script&gt;

The Twig and PHP templating systems approach the problem in different ways.
If you're using Twig, output escaping is on by default and you're protected.
In PHP, output escaping is not automatic, meaning you'll need to manually
escape where necessary.

Output Escaping in Twig
~~~~~~~~~~~~~~~~~~~~~~~

If you're using Twig templates, then output escaping is on by default. This
means that you're protected out-of-the-box from the unintentional consequences
of user-submitted code. By default, the output escaping assumes that content
is being escaped for HTML output.

In some cases, you'll need to disable output escaping when you're rendering
a variable that is trusted and contains markup that should not be escaped.
Suppose that administrative users are able to write articles that contain
HTML code. By default, Twig will escape the article body. To render it normally,
add the ``raw`` filter: ``{{ article.body | raw }}``.

You can also disable output escaping inside a ``{% block %}`` area or
for an entire template. For more information, see `Output Escaping`_ in
the Twig documentation.

Output Escaping in PHP
~~~~~~~~~~~~~~~~~~~~~~

Output escaping is not automatic when using PHP templates. This means that
unless you explicitly choose to escape a variable, you're not protected. To
use output escaping, use the special ``escape()`` view method::

    Hello <?php echo $view->escape($name) ?>

By default, the ``escape()`` method assumes that the variable is being rendered
within an HTML context (and thus the variable is escaped to be safe for HTML).
The second argument lets you change the context. For example, to output something
in a JavaScript string, use the ``js`` context:

.. code-block:: js

    var myMsg = 'Hello <?php echo $view->escape($name, 'js') ?>';

.. index::
   single: Templating; Formats

.. _template-formats:

Template Formats
----------------

Templates are a generic way to render content in *any* format. And while in
most cases you'll use templates to render HTML content, a template can just
as easily generate JavaScript, CSS, XML or any other format you can dream of.

For example, the same "resource" is often rendered in several different formats.
To render an article index page in XML, simply include the format in the
template name:

* *XML template name*: ``AcmeArticleBundle:Article:index.xml.twig``
* *XML template filename*: ``index.xml.twig``

In reality, this is nothing more than a naming convention and the template
isn't actually rendered differently based on its format.

In many cases, you may want to allow a single controller to render multiple
different formats based on the "request format". For that reason, a common
pattern is to do the following:

.. code-block:: php

    public function indexAction()
    {
        $format = $this->getRequest()->getRequestFormat();
    
        return $this->render('AcmeBlogBundle:Blog:index.'.$format.'.twig');
    }

The ``getRequestFormat`` on the ``Request`` object defaults to ``html``,
but can return any other format based on the format requested by the user.
The request format is most often managed by the routing, where a route can
be configured so that ``/contact`` sets the request format to ``html`` while
``/contact.xml`` sets the format to ``xml``. For more information, see the
:ref:`Advanced Example in the Routing chapter <advanced-routing-example>`.

To create links that include the format parameter, include a ``_format``
key in the parameter hash:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path('article_show', {'id': 123, '_format': 'pdf'}) }}">
            PDF Version
        </a>

    .. code-block:: html+php

        <a href="<?php echo $view['router']->generate('article_show', array('id' => 123, '_format' => 'pdf')) ?>">
            PDF Version
        </a>

Final Thoughts
--------------

The templating engine in Symfony is a powerful tool that can be used each time
you need to generate presentational content in HTML, XML or any other format.
And though templates are a common way to generate content in a controller,
their use is not mandatory. The ``Response`` object returned by a controller
can be created with our without the use of a template:

.. code-block:: php

    // creates a Response object whose content is the rendered template
    $response = $this->render('AcmeArticleBundle:Article:index.html.twig');

    // creates a Response object whose content is simple text
    $response = new Response('response content');

Symfony's templating engine is very flexible and two different template
renderers are available by default: the traditional *PHP* templates and the
sleek and powerful *Twig* templates. Both support a template hierarchy and
come packaged with a rich set of helper functions capable of performing
the most common tasks.

Overall, the topic of templating should be thought of as a powerful tool
that's at your disposal. In some cases, you may not need to render a template,
and in Symfony2, that's absolutely fine.

Learn more from the Cookbook
----------------------------

* :doc:`/cookbook/templating/PHP`
* :doc:`/cookbook/controller/error_pages`

.. _`Twig`: http://twig.sensiolabs.org
.. _`Symfony2Bundles.org`: http://symfony2bundles.org
.. _`Cross Site Scripting`: http://en.wikipedia.org/wiki/Cross-site_scripting
.. _`Output Escaping`: http://twig.sensiolabs.org
.. _`tags`: http://twig.sensiolabs.org/doc/tags/index.html
.. _`filters`: http://twig.sensiolabs.org/doc/templates.html#filters
.. _`add your own extensions`: http://twig.sensiolabs.org/doc/advanced.html
