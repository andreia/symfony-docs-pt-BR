.. index::
   single: Templating

Criando e usando Templates
============================

Como você sabe o :doc:`controller </book/controller>` é responsável por
controlar cada requisição que venha de uma aplicação Symfony2. Na realidade,
o controller delega muito do trabalho pesado para outros lugares então aquele
código pode ser testado e reusado. Quando um controller precisa gerar HTML,
CSS e qualquer outro conteúdo, ele entrega o trabalho para o engine de template.
Nesse capítulo, você irá aprender como escrever templates poderosas que podem ser
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
Twig permite a você escrever templates concisos e legíveis que são mais amigáveis
para web designers e, de certa forma, mais poderosos que templates de PHP:

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

Twig define dois tipos de sintaxe especiais:

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
e nova funções podem ser facilmente adicionadas. Por exemplo, a seguinte função usa
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

Quando o modo ``debug`` é habilitado (comum no ambiente ``dev``), um template
Twig será automaticamente recompilado quando mudanças são feitas nele. Isso
significa que durante o desenvolvimento você pode alegremente fazer mudanças para um template Twig
e imediatamente ver as mudanças sem precisar se preocupar sobre limpar qualquer
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
o engine de template para primeiro avaliar o template base, que configura
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

Conteúdo Assíncrono com hinclude.js
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.1
    suporte ao hinclude.js foi adicionado no Symfony 2.1

Os controladores podem ser incorporados assíncronamente usando a biblioteca javascript hinclude.js_.
Como o conteúdo incorporado vêm de outra página (ou controlador, neste assunto), o Symfony2 usa o 
helper padrão ``render`` para configurar tags ``hinclude``:

.. configuration-block::

    .. code-block:: jinja

        {% render '...:news' with {}, {'standalone': 'js'} %}

    .. code-block:: php

        <?php echo $view['actions']->render('...:news', array(), array('standalone' => 'js')) ?>

.. note::

   hinclude.js_  nprecisa ser incluído em sua página para funcionar.

Conteúdo padrão (enquanto carregar ou se o javascript está desabilitado) pode ser definido globalmente
na configuração da sua aplicação:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            templating:
                hinclude_default_template: AcmeDemoBundle::hinclude.html.twig

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:templating hinclude-default-template="AcmeDemoBundle::hinclude.html.twig" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating'      => array(
                'hinclude_default_template' => array('AcmeDemoBundle::hinclude.html.twig'),
            ),
        ));

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

O principal propósito da função ``asset`` é tornar sua aplicação mais portátil.
Se sua aplicação reside na raiz do seu host (ex: http://example.com),
então os atalhos interpretados deveriam ser ``/images/logo.png``. Mas se sua aplicação
reside em um sub-diretório (ex: http://example.com/my_app), cada caminho do asset 
deveria interpretar com o diretório (e.g. ``/my_app/images/logo.png``). A função
``asset`` toma conta disto ao determinar como sua aplicação está
sendo usada e gerando os atalhos de acordo com o caminho correto.

Adicionalmente, se você usar função ``asset``, Symfony pode automaticamente
anexar uma string de consulta para asset, em detrimento de garantir que assets
estáticos atualizados não serão armazenados quando distribuídos. Por exemplo, ``/images/logo.png`` poderia
parecer como ``/images/logo.png?v2``. Para mais informações, veja a opção de configuração 
:ref:`ref-framework-assets-version`.

.. index::
   single: Templating; Incluindo folhas de estilo e Javascripts
   single: Folhas de estilo; Incluindo folhas de estilo
   single: Javascripts; Incluindo Javascripts

Incluindo Folhas de Estilo e Javascript no Twig
---------------------------------------------

Nenhum site seria completo sem incluir arquivos Javascript e folhas de estilo.
Em Symfony, a inclusão desses assets é elegantemente manipulada ao tirar
vantagem das heranças de template do Symfony.

.. tip::
    
    Esta seção irá ensinar você a filosofia por trás disto, incluindo folha de estilo
    e asset Javascript em Symfony. Symfony também engloba outra biblioteca,
    chamada Assetic, que segue essa filosofia mas também permite você fazer coisas
    muito mais interessantes com esses assets. Para mais informações sobre
    usar Assetic veja :doc:`/cookbook/assetic/asset_management`.

Comece adicionando dois blocos a seu template base que irá abrigar seus assets:
uma chamada  ``stylesheets`` dentro da tag ``head`` e outra chamada ``javascripts``
justamente acima do fechamento da tag ``body``. Esses blocos irão conter todas as
folhas de estilo e Javascripts que você irá precisar através do seu site:

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

Isso é fácil o bastante ! Mas e se você precisar incluir uma folha de estilo ou
Javascript de um template filho ? Por exemplo, suponha que você tenha uma página
de contatos e você precise incluir uma folha de estilo ``contact.css`` *bem* naquela
página. Dentro do template da página de contatos, faça o seguinte:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Contact/contact.html.twig #}
    {# extends '::base.html.twig' #}

    {% block stylesheets %}
        {{ parent() }}
        
        <link href="{{ asset('/css/contact.css') }}" type="text/css" rel="stylesheet" />
    {% endblock %}
    
    {# ... #}

No template filho, você simplesmente sobrepõe o bloco ``stylesheets`` e
coloca sua nova tag de folha de estilo dentro daquele bloco. Claro, desde que você queira
adicionar ao conteúdo do bloco pai (e realmente não irá *substituí-lo), você
deveria usar a função ``parent()`` do Twig function para incluir tudo do bloco ``stylesheets``
do template base.

Você pode também incluir assets localizados em seus arquivos de pacotes ``Resources/public``.
Você precisará executar o comando``php app/console assets:install target [--symlink]`` ,
que move (ou symlinks) arquivos dentro da localização correta. 
(target é sempre por padrão "web).

.. code-block:: html+jinja

   <link href="{{ asset('bundles/acmedemo/css/contact.css') }}" type="text/css" rel="stylesheet" />

O resultado final é uma página que inclui ambas as folhas de estilo 
``main.css`` e ``contact.css``.

.. index::
   single: Templating; O Serviço de Templating

Configurando e usando o Serviço ``templating`` 
------------------------------------------------

O coração do sistema de template em Symfony2 é o template ``Engine``.
Este objeto especial é responsável por manipular templates e retornar
o conteúdo deles. Quando você manipula um template em um controller, por exemplo,
você está na verdade usando o serviço do template engine. Por exemplo:

.. code-block:: php

    return $this->render('AcmeArticleBundle:Article:index.html.twig');

é equivalente a:

.. code-block:: php

    $engine = $this->container->get('templating');
    $content = $engine->render('AcmeArticleBundle:Article:index.html.twig');

    return $response = new Response($content);

.. _template-configuration:

O engine de template (ou "serviço") é pré-configurada para trabalhar automaticamente
dentro de Symfony2. Ele pode, claro, ser configurado mais adiante no arquivo
de configuração da aplicação:

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

Várias opções de configuração estão disponíveis e estão cobertos em
:doc:`Configuration Appendix</reference/configuration/framework>`.

.. note::
   
   O engine ``twig`` é obrigatório para usar o webprofiler (bem como outros
   pacotes de terceiros).

.. index::
    single; Template; Sobrepondo templates

.. _overriding-bundle-templates:

Sobrepondo Templates de Pacote
---------------------------

A comunidade Symfony2 orgulha-se de si própria em criar e manter pacotes
de alta qualidade (veja `Symfony2Bundles.org`_) para um grande número de funcionalidades diferentes.
Uma vez que você use um pacote de terceiros, você irá certamente precisar sobrepor e personalizar
um ou mais de seus templates.

Suponha que você incluiu o imaginário open-source ``AcmeBlogBundle`` em seu
projeto (ex: no diretório ``src/Acme/BlogBundle``). E enquanto você estiver
realmente feliz com tudo, você quer sobrepor  a página de "lista" do blog para
personalizar a marcação especificamente para sua aplicação. Ao se aprofundar no
controller ``Blog`` eo ``AcmeBlogBundle``, você encontrará o seguinte::

    public function indexAction()
    {
        $blogs = // some logic to retrieve the blogs

        $this->render('AcmeBlogBundle:Blog:index.html.twig', array('blogs' => $blogs));
    }

Quando ``AcmeBlogBundle:Blog:index.html.twig`` é manipulado, Symfony2 realmente
observa duas diferentes localizações para o template:

#. ``app/Resources/AcmeBlogBundle/views/Blog/index.html.twig``
#. ``src/Acme/BlogBundle/Resources/views/Blog/index.html.twig``

Para sobrepor o template de pacote, só copie o template ``index.html.twig`` 
do pacote para  ``app/Resources/AcmeBlogBundle/views/Blog/index.html.twig``
(o diretório ``app/Resources/AcmeBlogBundle`` não existirão, então você precisará
criá-lo). Você está livre agora para personalizar o template.

Esta lógica também se aplica a templates de pacote base. Suponha também que cada
template em  ``AcmeBlogBundle`` herda de um template base chamado
``AcmeBlogBundle::layout.html.twig``. Justo como antes, Symfony2 irá observar os
seguintes dois lugares para o template:

#. ``app/Resources/AcmeBlogBundle/views/layout.html.twig``
#. ``src/Acme/BlogBundle/Resources/views/layout.html.twig``

Uma vez novamente, para sobrepor o template, apenas copie ele para
``app/Resources/AcmeBlogBundle/views/layout.html.twig``. Você agora está livre para
personalizar esta cópia como você quiser.

Se você voltar um passo atrás, verá que Symfony2 sempre começa a observar no 
diretório ``app/Resources/{BUNDLE_NAME}/views/`` por um template. Se o
template não existe aqui, ele continua checando dentro do diretório
``Resources/views`` do próprio pacote. Isso significa que todos os templates
do pacote podem ser sobrepostos ao colocá-los no sub-diretório correto 
``app/Resources``.

.. _templating-overriding-core-templates:

.. index::
    single; Template; Overriding exception templates

Sobrepondo Templates Centrais
~~~~~~~~~~~~~~~~~~~~~~~~~

Como o framework Symfony é um pacote por si só, templates centrais podem ser
sobrepostos da mesma forma. Por exemplo, o núcleo ``TwigBundle`` contém
um númeto de diferentes templates "exception" e "error" que podem ser sobrepostas
ao copiar cada uma do diretório ``Resources/views/Exception`` do ``TwigBundle`` 
para, você adivinhou, o diretório
``app/Resources/TwigBundle/views/Exception`` .

.. index::
   single: Templating; Three-level inheritance pattern

Herança de Três Níveis
-----------------------

Um modo comum de usar herança é usar uma aproximação em três níveis. Este
método trabalha perfeitamente com três tipos diferentes de templates que nós
certamente cobrimos:

* Criar um arquivo ``app/Resources/views/base.html.twig`` que contém o layout
  principal para sua aplicação (como nos exemplos anteriores). Internamente, este
  template é chamado ``::base.html.twig``;

* Cria um template para cada "seção" do seu site. Por exemplo, um ``AcmeBlogBundle``,
  teria um template chamado ``AcmeBlogBundle::layout.html.twig`` que contém somente
  elementos específicos para a sessão no blog:

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/layout.html.twig #}
        {% extends '::base.html.twig' %}

        {% block body %}
            <h1>Blog Application</h1>

            {% block content %}{% endblock %}
        {% endblock %}

* Criar templates individuais para cada página e fazer cada um estender a template
  de sessão apropriada. Por exemplo, a página "index" deveria ser chamada de algo
  próximo a ``AcmeBlogBundle:Blog:index.html.twig`` e listar os blogs de posts reais.

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Blog/index.html.twig #}
        {% extends 'AcmeBlogBundle::layout.html.twig' %}

        {% block content %}
            {% for entry in blog_entries %}
                <h2>{{ entry.title }}</h2>
                <p>{{ entry.body }}</p>
            {% endfor %}
        {% endblock %}

Perceba que este template estende a template de sessão - (``AcmeBlogBundle::layout.html.twig``)
que por sua vez estende o layout de aplicação base (``::base.html.twig``).
Isso é o modelo comum de herança de três níveis.

Quando construir sua aplicação, você pode escolher seguir esse método ou simplesmente
tornar cada template de página estender a template de aplicação base diretamente
(ex: ``{% extends '::base.html.twig' %}``). O modelo de três templates é
o método de melhor prática usado por vendor bundles então aquele template
base para um pacote pode ser facilmente sobreposto para propriamente estender seu
layout base de aplicação.

.. index::
   single: Templating; Saída para escape

Saída para escape
---------------

Quando gerar HTML de um template, sempre há um risco que uma variável de 
template pode gerar HTML involutário ou codigo do lado cliente perigoso. O resultado
é que o conteúdo dinâmico poderia quebrar o HTML de uma página de resultados ou
permitir um usuário maldoso realizar um ataque `Cross Site Scripting`_ (XSS). Considere
esse exemplo clássico:

.. configuration-block::

    .. code-block:: jinja

        Hello {{ name }}

    .. code-block:: php

        Hello <?php echo $name ?>

Imagine que o usuário entre o seguinte código como o nome dele/dela::

    <script>alert('hello!')</script>

Sem qualquer outra saída de escape, o resultado da template irá causar uma caixa de alerta
em JavaScript para saltar na tela::

    Hello <script>alert('hello!')</script>

E enquanto isso parece inofensivo, se um usuário pode chegar tão longe, o
mesmo usuário deveria também ser capaz de escrever Javascript que realiza
ações maliciosas dentro de uma área segura de um usuário legítimo e desconhecido.

A resposta para o problema é saída para escape. Sem a saída para escape ativa,
o mesmo template irá manipular inofensivamente, e literalmente imprimir a tag
``script`` na tela::

    Hello &lt;script&gt;alert(&#39;helloe&#39;)&lt;/script&gt;

Os sistemas de templating Twig e PHP aproximam-se do problema de formas diferentes.
Se você está usando Twig, saída para escape é ativado por padrão e você está protegido.
Em PHP, saída para escape não é automático, significando que você precisará manualmente
fazer o escape quando necessário.

Saída para escape em Twig
~~~~~~~~~~~~~~~~~~~~~~~~~

Se você está usando templates Twig, então saída para escape é ativado por padrão. Isto
significa que você está protegido externamente de consequencias acidentais por código
submetido por usuário. Por padrão, a saída para escape assume que o conteúdo está
sendo escapado pela saída HTML.

Em alguns casos, você precisará desabilitar saída para escape quando você está manipulando
uma variável que é confiável e contém marcação que não poderia ter escape.
Suponha que usuários administrativos são capazes de escrever artigos que contenham
código HTML. Por padrão, Twig irá escapar o corpo do artigo. Para fazê-lo normalamente,
adicione o filtro ``raw``: ``{{ article.body | raw }}``.

Você pode também desabilitar saída para escape dentro de uma área ``{% block %}`` ou
para um template inteiro. Para mais informações, veja `Output Escaping`_ na
documentação do Twig.

Saída para escape em PHP
~~~~~~~~~~~~~~~~~~~~~~~~

Saída para escape não é automática quando usamos templates PHP. Isso significa
que a menos que você escolha escapar uma variável explicitamente, você não está
protegido. Para usar saída para escape use o método de view ``escape()``::

    Hello <?php echo $view->escape($name) ?>

Por padrão, o método ``escape()`` assume que a variável está sendo manipulada
dentro de um contexto HTML (e assim a variável escapa e está segura para o HTML).
O segundo argumento deixa você mudar o contexto. Por exemplo, para gerar algo
em uma string Javascript, use o contexto ``js`` :

.. code-block:: js

    var myMsg = 'Hello <?php echo $view->escape($name, 'js') ?>';

.. index::
   single: Templating; Formats

.. _template-formats:

Debugging
---------

.. versionadded:: 2.0.9
    Esta funcionalidade está disponível no Twig ``1.5.x``, e foi adicionada primeiramente
    no Symfony 2.0.9.

Ao utilizar o PHP, você pode usar o ``var_dump()`` se precisa encontrar rapidamente 
o valor de uma variável passada. Isso é útil, por exemplo, dentro de seu controlador.
O mesmo pode ser conseguido ao usar o Twig com a extensão de depuração. Esta extensão
precisa ser ativada na configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            acme_hello.twig.extension.debug:
                class:        Twig_Extension_Debug
                tags:
                     - { name: 'twig.extension' }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <services>
            <service id="acme_hello.twig.extension.debug" class="Twig_Extension_Debug">
                <tag name="twig.extension" />
            </service>
        </services>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;

        $definition = new Definition('Twig_Extension_Debug');
        $definition->addTag('twig.extension');
        $container->setDefinition('acme_hello.twig.extension.debug', $definition);

O dump dos parâmetros do template pode ser feito usando a função ``dump``:

.. code-block:: html+jinja

    {# src/Acme/ArticleBundle/Resources/views/Article/recentList.html.twig #}
    {{ dump(articles) }}

    {% for article in articles %}
        <a href="/article/{{ article.slug }}">
            {{ article.title }}
        </a>
    {% endfor %}

O dump das variáveis somente será realizado se a definição ``debug`` do Twig (no ``config.yml``)
for ``true``. Por padrão, isto signifca que será feito o dump das variáveis no ambiente ``dev``
mas não no ``prod``.

Verificação de Sintaxe
----------------------

.. versionadded:: 2.1
    O comando ``twig:lint`` foi adicionado no Symfony 2.1

Você pode verificar erros de sintaxe nos templates do Twig usando o
comando de console ``twig:lint``:

.. code-block:: bash

    # You can check by filename:
    $ php app/console twig:lint src/Acme/ArticleBundle/Resources/views/Article/recentList.html.twig

    # or by directory:
    $ php app/console twig:lint src/Acme/ArticleBundle/Resources/views

    # or using the bundle name:
    $ php app/console twig:lint @AcmeArticleBundle

Formatos de Template
--------------------

Templates são uma forma genérica de modificar conteúdo em *qualquer* formato. E enquanto
em muitos casos você irá usar templates para modificar conteúdo HTML, um template pode
tão fácil como certo gerar JavaScript, CSS, XML ou qualquer outro formato que você possa sonhar.

Por exemplo, o mesmo "recurso" é sempre modificado em diversos formatos diferentes.
Para modificar uma página inicial de um artigo XML, simplesmente inclua o formato no
nome do template:

* *nome do template XML*: ``AcmeArticleBundle:Article:index.xml.twig``
* *nome do arquivo do template XML*: ``index.xml.twig``

Na realidade, isso é nada mais que uma convenção de nomeação e o template
não é realmente modificado de forma diferente ao baseado no formato dele.

Em muitos casos, você pode querer permitir um controller unitário para modificar
múltiplos formatos diferentes baseado no "formato de requisição". Por aquela razão,
um padrão comum é fazer o seguinte:

.. code-block:: php

    public function indexAction()
    {
        $format = $this->getRequest()->getRequestFormat();
    
        return $this->render('AcmeBlogBundle:Blog:index.'.$format.'.twig');
    }

O ``getRequestFormat`` no objeto ``Request`` padroniza para ``html``,
mas pode retornar qualquer outro formato baseado no formato solicitado pelo usuário.
O formato solicitado é frequentemente mais gerenciado pelo roteamento, onde uma rota
pode ser configurada para que ``/contact``  configure o formato requisitado ``html`` enquanto
``/contact.xml`` configure o formato para ``xml``. Para mais informações, veja
:ref:`Advanced Example in the Routing chapter <advanced-routing-example>`.

Para criar links que incluam o parâmetro de formato, inclua uma chave ``_format``
no detalhe do parâmetro:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path('article_show', {'id': 123, '_format': 'pdf'}) }}">
            PDF Version
        </a>

    .. code-block:: html+php

        <a href="<?php echo $view['router']->generate('article_show', array('id' => 123, '_format' => 'pdf')) ?>">
            PDF Version
        </a>

Considerações finais
--------------------

O engine de template em Symfony é uma ferramenta poderosa que pode ser usada cada momento
que você precisa para gerar conteúdo de apresentação em HTML, XML ou qualquer outro formato.
E apesar de tempaltes serem um jeito comum de gerar conteúdo em um controller,
o uso deles não são obrigatórios. O objeto ``Response`` object retornado por um controller
pode ser criado com ou sem o uso de um template:

.. code-block:: php

    // creates a Response object whose content is the rendered template
    $response = $this->render('AcmeArticleBundle:Article:index.html.twig');

    // creates a Response object whose content is simple text
    $response = new Response('response content');

Engine de template do Symfony é muito flexível e dois editores de template
diferentes estão disponíveis por padrão: os tradicionais templates do *PHP* e os
polidos e poderosos templates do *Twig* . Ambos suportam uma hierarquia de template e
vêm empacotados com um conjunto rico de funções helper capazes de realizar
as tarefas mais comuns.

No geral, o tópico de template poderia ser pensado como uma ferramenta poderosa
que está à sua disposição. Em alguns casos, você pode não precisar modificar um template,
e em Symfony2, isso é absolutamente legal.

Aprenda mais do Cookbook
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
