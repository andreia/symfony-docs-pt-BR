.. index::
   single: Templating

Criando e usando Templates
============================

Como voc� sabe o :doc:`controller </book/controller>` � respons�vel por
controlar cada requisi��o que venha de uma aplica��o Symfony2. Na realidade,
o controller delega muito do trabalho pesado para outros lugares ent�o aquele
c�digo pode ser testado e reusado. Quando um controller precisa gerar HTML,
CSS e qualquer outro conte�do, ele entrega o trabalho para o engine de template.
Nesse cap�tulo, voc� ir� aprendder como escrever templates poderosas que podem ser
usada para retornar conte�do para o usu�rio, preencher corpo de e-mail, e mais. Voc� 
ir� aprender atalhos, maneiras espertas de extender templates e como reusar c�digo
de template.

.. index::
   single: Templating;O que � um template?

Templates
---------

Um template � simplesmente um arquivo de texto que pode gerar qualquer formato baseado em texto 
(HTML, XML, CSV, LaTeX ...). O tipo mais familiar de template � um 
template em *PHP* - um arquivo de texto analisado pelo PHP que cont�m uma mistura de texto e c�digo PHP::

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

.. index:: Twig; Introdu��o

Mas Symfony2 empacota at� mesmo uma linguagem muito poderosa de template chamada `Twig`_.
Twig permit a voc� escrever templates consisos e leg�veis que s�o mais amig�veis
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

* ``{{ ... }}``: "Diga algo": exibe uma vari�vel ou o resultado de uma
  express�o para o template;
  
* ``{% ... %}``: "Fa�a algo": uma **tag** que controla a l�gica do
  template; ela � usada para executar certas senten�as como for-loops por exemplo.

.. note::

   H� uma terceira sintaxe usada para criar coment�rios ``{# this is a comment #}``.
   Essa sintaxe pode ser usada atrav�s de m�ltiplas linhas, parecidas com a sintaxe
   equivalente em PHP ``/* comment */``.

Twig tamb�m cont�m **filtros**, que modificam conte�do antes de serem interpretados.
O seguinte filtro transforma a vari�vel ``title`` toda em letra mai�scula antes de interpret�-la:

.. code-block:: jinja

    {{ title | upper }}

Twig vem com uma longa lista de `tags`_ e `filtros`_ que est�o dispon�veis
por padr�o. Voc� pode at� mesmo `adicionar suas pr�prias extens�es`_ para o Twig quando precisar.

.. tip::

    Registrar uma extens�o Twig � t�o f�cil quanto criar um novo servi�o e atribuir tag
    nele com ``twig.extension`` :ref:`tag<reference-dic-tags-twig-extension>`.

Como voc� ver� atrav�s da documenta��o, Twig tamb�m suporta fun��es
e nova fun��es podem ser facilmente adicionadas. Por exemplo. a seguinte fun��o usa
uma tag padr�o ``for``  e a fun��o ``cycle`` para ent�o imprimir dez tags div, alternando
entre classes ``odd`` e ``even``:

.. code-block:: html+jinja

    {% for i in 0..10 %}
      <div class="{{ cycle(['odd', 'even'], i) }}">
        <!-- some HTML here -->
      </div>
    {% endfor %}

Durante este cap�tulo, exemplos de template ser�o mostrados tanto em Twig como PHP.

.. sidebar:: Por que Twig?
    
    Templates Twig s�o feitas para serem simples e n�o ir�o processar tags PHP. Isto
    � pelo design: o sistema de template do Twig � feito para expressar apresenta��o,
    n�o l�gica de programa. Quanto mais voc� usa Twig, mais voc� ir� apreciar e beneficiar
    desta distin��o. E claro, voc� ser� amado por web designers de todos os lugares.
    
    Twig pode tamb�m fazer coisas que PHP n�o pode, como por exemplo heran�a verdadeira de template
    (Templates do Twig compilam classes PHP que herdam uma da outra),
    controle de espa�o em branco, caixa de areia, e a inclus�o de fun��es personalizadas
    e filtros que somente afetam templates. Twig cont�m pequenos recursos
    que fazem escrita de templates mais f�cil e mais concisa. Considere o seguinte 
    exemplo, que combina um loop com uma senten�a l�gia ``if``:
    
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

Twig � r�pido. Cada template Twig � compilado para uma classe nativa PHP
que � processada na execu��o. As classes compiladas s�o localizadas no
diret�rio ``app/cache/{environment}/twig`` (onde ``{environment}`` � o
ambiente, como ``dev`` ou ``prod``), e em alguns casos pode ser �til durante 
a depura��o. Veja :ref:`environments-summary` para mais informa��es de
ambientes.

Quando o modo ``debug`` � abilitado (comum no ambiente ``dev``), um template
Twig ser� automaticamente recompilado quando mudan�as s�o feitas nele. Isso
signitica que durante o desenvolvimento voc� pode alegremente fazer mudan�as para um template Twig
e inst�ntaneamente ver as mudan�as sem precisar se preocupar sobre limpar qualquer
cache.

Quando o modo ``debug`` � desabilitado (comum no ambiente ``prod``), entretanto,
voc� deve limpar o cache do diret�rio Twig para que ent�o os templates Twig
se regenerem. Lembre de fazer isso quando distribuir sua aplica��o.

.. index::
   single: Templating; Inheritance

Heran�a e Layouts de Template
--------------------------------

Mais frequentemente que n�o, templates compartilham elementos comuns em um projeto, como o
header, footer, sidebar ou outros. Em Symfony2,  n�s gostamos de pensar sobre esse
problema de forma diferente: um template pode ser decorado por outro. Isso funciona
exatemente da mesma forma como classes PHP: heran�a de template permite voc� construir
um "layout" de template base que contenha todos os elementos comuns de seu site
definidos como **blocos** (pense em "classe PHP com m�todos base"). Um template filho
pode extender o layout base e sobrepor os blocos (pense "subclasse PHP 
que sobreponha certos m�todos de sua classe pai").

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
    
    Apesar da discuss�o sobre heran�a de template ser em termos do Twig,
    a filosofia � a mesma entre templates Twig e PHP.

Este template define o esqueleto do documento base HTML de um p�gina simples
de duas colunas. Neste exemplo, tr�s �reas ``{% block %}`` s�o definidas (``title``,
``sidebar`` e ``body``). Cada bloco pode ser sobreposto por um template filho
ou largado com sua implementa��o padr�o. Esse template poderia tamb�m ser processado
diretamente. Neste caso os blocos ``title``, ``sidebar`` e ``body`` blocks deveriam
simplesmente reter os valores padr�o neste template.

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
    
   O template pai � idenficado por uma sintaxe especial de string
   (``::base.html.twig``) que indica que o template reside no diret�rio
   ``app/Resources/views`` do projeto. Essa conven��o de nomeamento �
   explicada inteiramente em :ref:`template-naming-locations`.

A chave para heran�a template � a tag  ``{% extends %}``. Ela avisa
o engine de template para primeiro avaliar o template base, que configura
o layout e define v�rios blocos. O template filho � ent�o processado,
ao ponto que os blocos  ``title`` e ``body`` do template pai sejam substitu�dos
por aqueles do filho. Dependendo do valor de ``blog_entries``, a
sa�da poderia parecer com isso::

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

Perceba que como o template filho n�o definiu um bloco ``sidebar``, o
valor do template pai � usado no lugar. Conte�do dentro de uma tag  ``{% block %}``
em um template pai � sempre usado por padr�o.

Voc� pode usar muitos n�veis de heran�a quanto quiser. Na pr�xima sess�o,
um modelo comum de heran�a de tr�s n�veis ser� explicado assim como os templates
s�o organizados dentro de um projeto Symfony2.

Quando trabalhar com heran�a de template, aqui est�o algumas dicas para guardar na cabe�a:

* Se voc� usa ``{% extends %}`` em um template, ele deve ser a primeira tag 
  naquele template.
  
* Quanto mais tags ``{% block %}`` voc� tiver no template base, melhor.
  Lembre, templates filhos n�o precisam definir todos os blocos do pai, ent�o
  criar tantos blocos em seus templates base quanto voc� quiser e dar a cada um
  padr�o sensato. Quanto mais blocos seus templates base tiverem, mais flex�vel
  seu layout ser�.

* Se voc� achar voc� mesmo duplicando conte�do em um determinado n�mero de templates, isto provavelmente
  significa que voc� deveria mover aquele conte�do para um ``{% block %}`` no template pai.
  Em alguns casos, uma solu��o melhor pode ser mover o conte�do para um novo template
  e ``incluir`` ele (veja :ref:`including-templates`).

* Se voc� precisa obter o conte�do de um bloco do template pai, voc�
  pode usar a fun��o ``{{ parent() }}``. Isso � �til se voc� quiser adicionar
  ao conte�do de um bloco pai ao inv�s de sobrepor ele::
  
    .. code-block:: html+jinja

        {% block sidebar %}
            <h3>Table of Contents</h3>
            ...
            {{ parent() }}
        {% endblock %}

.. index::
   single: Templating; Conven��es de Nomea��o
   single: Templating; Localiza��o de Arquivos

.. _template-naming-locations:

Nomea��o de Template e Localiza��es
-----------------------------

Por padr�o, templates podem residir em duas localiza��es diferentes:

* ``app/Resources/views/``: O diret�rio de aplica��o de ``views`` pode abrigar
  templates bases para toda a aplica��o(ex: os layout de sua aplica��o) assim como
  os tempates que sobrep�em templates de pacote (veja :ref:`overriding-bundle-templates`);
  
  application-wide base templates (i.e. your application's layouts) as well as
  templates that override bundle templates (see
  :ref:`overriding-bundle-templates`);
  

* ``path/to/bundle/Resources/views/``: Cada pacote abriga as templates dele no diret�rio 
  ``Resources/views`` (e sub-diret�rios). A maioria dos templates ir� residir dentro de
  um pacote.
  
Symfony2 usa a sintaxe de string **bundle**:**controller**:**template** para
templates. Isso permite v�rios tipos diferente de template, cada um residindo
em uma localiza��o especifica:

* ``AcmeBlogBundle:Blog:index.html.twig``: Esta sintaxe � usada para especificar um
  template para uma p�gina espec�fica. As tr�s partes do string, cada uma separada
  por dois pontos, (``:``), signitca o seguinte:

    * ``AcmeBlogBundle``: (*bundle*) o template reside entro de
      ``AcmeBlogBundle`` (e.g. ``src/Acme/BlogBundle``);

    * ``Blog``: (*controller*) indica que o template reside dentro do
       sub-diret�rio ``Blog`` de ``Resources/views``;

    * ``index.html.twig``: (*template*) o verdadeiro nome do arquivo �
      ``index.html.twig``.

  Assumindo que o ``AcmeBlogBundle`` reside em ``src/Acme/BlogBundle``, o
  atalho final para o layout seria ``src/Acme/BlogBundle/Resources/views/Blog/index.html.twig``.

* ``AcmeBlogBundle::layout.html.twig``: Essa sintaxe refere ao template base que
  � espec�fica para ``AcmeBlogBundle``. Since the middle, "controller",
  portion is missing (e.g. ``Blog``), the template lives at
  ``Resources/views/layout.html.twig`` inside ``AcmeBlogBundle``.

* ``::base.html.twig``: Esta sintaxe refere a uma template base para toda a aplica��o ou
  layout. Perceba que a string come�a com dois sinais de dois pontos (``::``), significando
  que ambas as partes *bundle*  *controller* est�o faltando. Isto significa
  que o template n�o � localizado em qualquer pacote, mas sim na raiz do 
  diret�rio ``app/Resources/views/``.

Na se��o :ref:`overriding-bundle-templates`, voc� ir� descobrir como cada
template reside dentro do ``AcmeBlogBundle``, por exemplo, pode ser sobreposto
ao colocar um template de mesmo nome no diret�rio 
``app/Resources/AcmeBlogBundle/views/``. Isso d� o poder de sobrepor templates de qualquer pacote pago.

.. tip::
    
    Esperan�osamente, a sintaxe de nomea��o de template parece familiar - � a mesma conven��o
    para nomea��o usada para referir para :ref:`controller-string-syntax`.

Sufixo de Template 
~~~~~~~~~~~~~~~

O formato **bundle**:**controller**:**template** de cada template especifica
*onde* o arquivo de template est� localizado. Cada nome de template tamb�m tem duas express�es
que especificam o *formato* e *engine* para aquela template.

* **AcmeBlogBundle:Blog:index.html.twig** - formato HTML, engine Twig

* **AcmeBlogBundle:Blog:index.html.php** - formato HTML, engine PHP

* **AcmeBlogBundle:Blog:index.css.twig** - formato CSS, engine Twig

Por padr�o, qualquer template Symfony2 ou pode ser escrito em Twig ou em PHP, e
a �ltima  parte da extens�o (ex: ``.twig`` ou ``.php``) especifica qual
dessas duas *engines* deveria ser usada. A primeira parte da extens�o,
(ex: ``.html``, ``.css``, etc) � o formato final que o template ir�
gerar. Ao contr�rio de engine, que determina como Symfony2 analisa o template,
isso � simplesmente uma t�tica organizacional em caso do mesmo recurso precisar
ser transformado como HTML(``index.html.twig``), XML (``index.xml.twig``),
ou qualquer outro formato. Para mais informa��ess, leia a se��o :ref:`template-formats`.

.. note::
    
   As "engines" dispon�veis podem ser configurados e at� mesmo ter novas engines adicionadas.
   Veja :ref:`Configura��o de Template<template-configuration>` para mais detalhes.

.. index::
   single: Templating; Tags e Helpers
   single: Templating; Helpers

Tags e Helpers
----------------

Voc� j� entende as bases do templates, como eles s�o chamados e como usar 
heran�a de template. As partes mais dif�ceis est�o realmente atr�s de voc�. Nesta
se��o, voc� ir� aprender sobre um grande grande grupo de ferramentas dispon�veis para ajudar
a desempenhar as tarefas de template mais comuns como incluir outras templates,
vincular p�ginas e incluir imagens.


Symfony2 vem acompanhado com v�rias tags Twig especializadas e fun��es que
facilitam o trabalho do designer de template. Em PHP, o sistema de template providencia
um sistema extenso de *helper* que providencia funcionalidades �teis no contexto
de template.

N�s realmente vimos umas poucas tags Twig constru�das (``{% block %}`` e ``{% extends %}``)
como exemplo de um helper PHP (``$view['slots']``). Vamos aprender um
pouco mais.

.. index::
   single: Templating; Incluir outras templates

.. _including-templates:

Incluir outras Templates
~~~~~~~~~~~~~~~~~~~~~~~~~

Voc� ir� frequntemente querer incluir a mesma template ou fragmento de c�digo em v�rias
p�ginas diferentes. Por exemplo, em uma aplica��o com "artigos de not�cias", a exibi��o
do artigo no c�digo do template poderia ser usada numa p�gina de detalhes do artigo,
num a p�gina exibindo os artigos mais populares, ou em uma lista dos �ltimos
artigos.

Quando voc� precisa reutilizar um peda�o de um c�digo PHP, voc� tipicamente move o c�digo
para uma nova classe ou fun��o PHP. O mesmo � verdade para template. Ao mover o
c�digo do template reutilizado em um template pr�prio, ele pode ser inclu�do em qualquer outro
template. Primeiro, crie o template que voc� precisar� reutilizar.

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

Incluir este template de qualquer outro template � f�cil:

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

O template � inclu�do usando a tag ``{% include %}``. Perceba que o
nome do template segue a mesma conven��o t�pica. O template ``articleDetails.html.twig``
usa uma vari�vel ``article``. Isso � passado por um template ``list.html.twig``
usando o comando ``with``.

.. tip::

    A sintaxe ``{'article': article}`` � a sintaxe Twig padr�o para hash
    maps (ex:  um array com chaves nomeadas). Se n�s precisarmos pass�-lo em elementos
    m�ltiplos, ele poderia ser algo como: ``{'foo': foo, 'bar': bar}``.

.. index::
   single: Templating; Incorpora��o de a��es
   
.. _templating-embedding-controller:

Incorpora��o de Controllers
~~~~~~~~~~~~~~~~~~~~~

Em alguns casos, voc� precisa fazer mais do que incluir um template simples. Suponha
que voc� tenha uma barra lateral no seu layout que contenha os tr�s artigos mais recentes.
Recuperar os tr�s artigos podem incluir consultar a base de dados ou desempenhar
outra l�gica pesada que n�o pode ser a partir de um template.

A solu��o � simplesmnte incorporar o resultado de um controller inteiro para
seu template. Primeiro, crie o controller que retorne um certo n�mero de artigos
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

A template ``recentList`` � perfeitamente straightforward:

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
    Perceba que n�s fizemos uma gambiarra e fizemos um hardcode no artigo URL desse exemplo
    (ex: ``/article/*slug*``). Isso � uma pr�tica ruim. Na pr�xima se��o,
    voc� ir� aprender como fazer isso corretamente.
    
Para incluir um controller, voc� ir� precisar referir a ela usando a sintaxe de string
padr�o para controllers (isto �, **bundle**:**controller**:**action**):

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

Sempre quando voc� pensar que voc� precisa de uma vari�vel ou uma pe�a de informa��o que
voc� n�o tenha acesso em um template, considere transformar o controller.
Controllers s�o r�pidos para executar e promovem uma boa organiza��o e utiliza��o do c�digo.

.. index::
   single: Templating; Vincula��o �s p�ginas

Vincula��o �s P�ginas
~~~~~~~~~~~~~~~~

Criar links para outras p�gina em sua aplica��o � uma das tarefas mais 
comuns para um template. Ao inv�s de fazer um hardcode nas URLs nos templates, use a
fun��o do Twig ``path`` (ou o helper ``router`` no PHP) para gerar URLs baseadas na
configura��o de roteamento. Mais tarde, se voc� quiser modificar a URL de uma p�gina
particular, tudo que voc� precisar� fazer � mudar as configura��es de roteamento; os
templates ir�o automatricamente gerar a nova URL.

Primeiro, vincule a p�gina "_welcome" , que � acess�vel pela seguinte configura��o de
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

Para vincular � p�gina, apenas use a fun��o Twig ``path`` e refira para a rota:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path('_welcome') }}">Home</a>

    .. code-block:: php

        <a href="<?php echo $view['router']->generate('_welcome') ?>">Home</a>

Como esperado, isso ir� gerar a URL ``/``. Vamos ver como isso ir� funcionar com
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

Neste caso, voc� precisa especificar tanto o nome da rota (``article_show``) como
um valor para o par�metro ``{slug}``. Usando esta rota, vamos revisitar o 
template ``recentList`` da sess�o anterior e vincular aos artigos
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
    
    Voc� tamb�m pode gerar uma URL absoluta ao usar a fun��o ``url`` do Twig:

    .. code-block:: html+jinja

        <a href="{{ url('_welcome') }}">Home</a>
    
    O mesmo pode ser feito em templates PHP ao passar um terceiro argumento ao
    m�todo ``generate()``:

    .. code-block:: php

        <a href="<?php echo $view['router']->generate('_welcome', array(), true) ?>">Home</a>

.. index::
   single: Templating; Vinculando os assets

Vinculando os Assets
~~~~~~~~~~~~~~~~~

Templates podem frequentemente referir a imagens, Javascript, folhas de estilo e outros
recursos. Claro voc� poderia fazer um hardcode do atalho desses assets (ex: ``/images/logo.png``),
mas Symfony2 providencia uma op��o mais din�mica via fun��o ``assets`` do Twig:

.. configuration-block::

    .. code-block:: html+jinja

        <img src="{{ asset('images/logo.png') }}" alt="Symfony!" />

        <link href="{{ asset('css/blog.css') }}" rel="stylesheet" type="text/css" />

    .. code-block:: php

        <img src="<?php echo $view['assets']->getUrl('images/logo.png') ?>" alt="Symfony!" />

        <link href="<?php echo $view['assets']->getUrl('css/blog.css') ?>" rel="stylesheet" type="text/css" />

O principal prop�sito da fun��o ``asset`` � tornar sua aplica��o mais port�til.
Se sua aplica��o reside na raiz do seu host (ex: http://example.com),
ent�o os atalhos interpretados deveriam ser ``/images/logo.png``. Mas se sua aplica��o
reside em um sub-diret�rio (ex: http://example.com/my_app), cada caminho do asset 
deveria interpretar com o diret�rio (e.g. ``/my_app/images/logo.png``). A fun��o
``asset`` toma conta disto ao determinar como sua aplica��o est�
sendo usada e gerando os atalhos de acordo com o correto.

Adicionalmente, se voc� usar fun��o asset, Symfony pode automaticamente
anexar uma string de consulta para asset, em detrimento de garantir que assets
est�ticos atualizados n�o ser�o armazenados quando distribu�dos. Por exemplo, ``/images/logo.png`` poderia
parecer como ``/images/logo.png?v2``. Para mais informa��es, veja a op��o de configura��o 
:ref:`ref-framework-assets-version`.

.. index::
   single: Templating; Incluindo folhas de estilo e Javascripts
   single: Folhas de estilo; Incluindo folhas de estilo
   single: Javascripts; Incluindo Javascripts

Incluindo Folhas de Estilo e Javascript no Twig
---------------------------------------------

Nenhum site seria completo sem incluir arquivos Javascript e folhas de estilo.
Em Symfony, a inclus�o desses assets � elegantemente manipulada ao tirar
vantagem das heran�as de template do Symfony.

.. tip::
    
    Esta se��o ir� ensinar voc� a filosofia por tr�s disto, incluindo folha de estilo
    e asset Javascript em Symfony. Symfony tamb�m engloba outra biblioteca,
    chamada Assetic, que segue essa filosofia mas tamb�m permite voc� fazer mais coisas
    muito interessantes com esses assets. Para mais informa��es sobre
    usar Assetic veja :doc:`/cookbook/assetic/asset_management`.

Comece adicionando dois blocos a seu template base que ir� abrigar seus assets:
uma chamada  ``stylesheets`` dentro da tag ``head`` e outra chamada ``javascripts``
justamente acima do fechamento da tag ``body``. Esses blocos ir�o conter todas as
folhas de estilo e Javascripts que voc� ir� precisar atrav�s do seu site:

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

Isso � f�cil o bastante ! Mas e se voc� precisar incluir uma folha de estilo ou
Javascript de um template filho ? Por exemplo, suponha que voc� tenha uma p�gina
de contatos e voc� precise incluir uma folha de estilo ``contact.css`` *bem* naquela
p�gina. Dentro do template da p�gina de contatos, fa�a o seguinte:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Contact/contact.html.twig #}
    {# extends '::base.html.twig' #}

    {% block stylesheets %}
        {{ parent() }}
        
        <link href="{{ asset('/css/contact.css') }}" type="text/css" rel="stylesheet" />
    {% endblock %}
    
    {# ... #}

No template filho, voc� simplesmente sobrep�e o bloco ``stylesheets`` e
coloca sua nova tag de folha de estilo dentro daquele bloco. Claro, desde que voc� queira
adicionar ao conte�do do bloco pai (e realmente n�o ir� *substitu�-lo), voc�
deveria usar a fun��o ``parent()`` do Twig function para incluir tudo do bloco ``stylesheets``
do template base.

Voc� pode tamb�m incluir assets localizados em seus arquivos de pacotes ``Resources/public``.
Voc� precisar� executar o comando``php app/console assets:install target [--symlink]`` ,
que move (ou symlinks) arquivos dentro da localiza��o correta. 
(target � sempre por padr�o "web).

.. code-block:: html+jinja

   <link href="{{ asset('bundles/acmedemo/css/contact.css') }}" type="text/css" rel="stylesheet" />

O resultado final � uma p�gina que inclui ambas as folhas de estilo 
``main.css`` e ``contact.css``.

.. index::
   single: Templating; O Servi�o de Templating

Configurando e usando o Servi�o ``templating`` 
------------------------------------------------

O cora��o do sistema de template em Symfony2 � o template ``Engine``.
Este objeto especial � respons�vel por manipular templates e retornar
o conte�do deles. Quando voc� manipula um template em um controller, por exemplo,
voc� est� na verdade usando o servi�o do template engine. Por exemplo:

.. code-block:: php

    return $this->render('AcmeArticleBundle:Article:index.html.twig');

� equivalente a:

.. code-block:: php

    $engine = $this->container->get('templating');
    $content = $engine->render('AcmeArticleBundle:Article:index.html.twig');

    return $response = new Response($content);

.. _template-configuration:

O engine de template (ou "servi�o") � pr�-configurada para trabalhar automaticamente
dentro de Symfony2. Ele pode, claro, ser configurado mais adiante no arquivo
de configura��o da aplica��o:

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

V�rias op��es de configura��o est�o dispon�veis e est�o cobertos em
:doc:`Configuration Appendix</reference/configuration/framework>`.

.. note::
   
   O engine ``twig`` � obrigat�rio para usar o webprofiler (bem como outros
   pacotes de terceiros).

.. index::
    single; Template; Sobrepondo templates

.. _overriding-bundle-templates:

Sobrepondo Templates de Pacote
---------------------------

A comunidade Symfony2 orgulha-se de si pr�pria em criar e manter pacotes
de alta qualidade (veja `Symfony2Bundles.org`_) para um grande n�mero de funcionalidades diferentes.
Uma vez que voc� use um pacote de terceiros, voc� ir� certamente precisar sobrepor e personalizar
um ou mais de seus templates.

Suponha que voc� incluiu o imagin�rio open-source ``AcmeBlogBundle`` em seu
projeto (ex: no diret�rio ``src/Acme/BlogBundle``). E enquanto voc� estiver
realmente feliz com tudo, voc� quer sobrepor  a p�gina de "lista" do blog para
personalizar a marca��o especificamente para sua aplica��o. Ao se aprofundar no
controller ``Blog`` eo ``AcmeBlogBundle``, voc� encontrar� o seguinte::

    public function indexAction()
    {
        $blogs = // some logic to retrieve the blogs

        $this->render('AcmeBlogBundle:Blog:index.html.twig', array('blogs' => $blogs));
    }

Quando ``AcmeBlogBundle:Blog:index.html.twig`` � manipulado, Symfony2 realmente
observa duas diferentes localiza��es para o template:

#. ``app/Resources/AcmeBlogBundle/views/Blog/index.html.twig``
#. ``src/Acme/BlogBundle/Resources/views/Blog/index.html.twig``

Para sobrepor o template de pacote, s� copie o template ``index.html.twig`` 
do pacote para  ``app/Resources/AcmeBlogBundle/views/Blog/index.html.twig``
(o diret�rio ``app/Resources/AcmeBlogBundle`` n�o existir�o, ent�o voc� precisar�
cri�-lo). Voc� est� livre agora para personalizar o template.

Esta l�gica tamb�m se aplica a templates de pacote base. Suponha tamb�m que cada
template em  ``AcmeBlogBundle`` herda de um template base chamado
``AcmeBlogBundle::layout.html.twig``. Justo como antes, Symfony2 ir� observar os
seguintes dois lugares para o template:

#. ``app/Resources/AcmeBlogBundle/views/layout.html.twig``
#. ``src/Acme/BlogBundle/Resources/views/layout.html.twig``

Uma vez novamente, para sobrepor o template, apenas copie ele para
``app/Resources/AcmeBlogBundle/views/layout.html.twig``. Voc� agora est� livre para
personalizar esta c�pia como voc� quiser.

Se voc� voltar um passo atr�s, ver� que Symfony2 sempre come�a a observar no 
diret�rio ``app/Resources/{BUNDLE_NAME}/views/`` por um template. Se o
template n�o existe aqui, ele continua checando dentro do diret�rio
``Resources/views`` do pr�prio pacote. Isso significa que todos os templates
do pacote podem ser sobrepostos ao coloc�-los no sub-diret�rio correto 
``app/Resources``.

.. _templating-overriding-core-templates:

.. index::
    single; Template; Overriding exception templates

Sobrepondo Templates Centrais
~~~~~~~~~~~~~~~~~~~~~~~~~

Como o framework Symfony � um pacote por si s�, templates centrais podem ser
sobrepostos da mesma forma. Por exemplo, o n�cleo ``TwigBundle`` cont�m
um n�meto de diferentes templates "exception" e "error" que podem ser sobrepostas
ao copiar cada uma do diret�rio ``Resources/views/Exception`` do ``TwigBundle`` 
para, voc� adivinhou, o diret�rio
``app/Resources/TwigBundle/views/Exception`` .

.. index::
   single: Templating; Three-level inheritance pattern

Heran�a de Tr�s N�veis
-----------------------

Um modo comum de usar heran�a � usar uma aproxima��o em tr�s n�veis. Este
m�todo trabalha perfeitamente com tr�s tipos diferentes de templates que n�s
certamente cobrimos:

* Criar um arquivo ``app/Resources/views/base.html.twig`` que cont�m o layout
  principal para sua aplica��o (como nos exemplos anteriores). Internamente, este
  template � chamado ``::base.html.twig``;

* Cria um template para cada "se��o" do seu site. Por exemplo, um ``AcmeBlogBundle``,
  teria um template chamado ``AcmeBlogBundle::layout.html.twig`` que cont�m somente
  elementos espec�ficos para a sess�o no blog:

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/layout.html.twig #}
        {% extends '::base.html.twig' %}

        {% block body %}
            <h1>Blog Application</h1>

            {% block content %}{% endblock %}
        {% endblock %}

* Criar templates individuais para cada p�gina e fazer cada um estender a template
  de sess�o apropriada. Por exemplo, a p�gina "index" deveria ser chamada de algo
  pr�ximo a ``AcmeBlogBundle:Blog:index.html.twig`` e listar os blogs de posts reais.

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Blog/index.html.twig #}
        {% extends 'AcmeBlogBundle::layout.html.twig' %}

        {% block content %}
            {% for entry in blog_entries %}
                <h2>{{ entry.title }}</h2>
                <p>{{ entry.body }}</p>
            {% endfor %}
        {% endblock %}

Perceba que este template estende a template de sess�o - (``AcmeBlogBundle::layout.html.twig``)
que por sua vez estende o layout de aplica��o base (``::base.html.twig``).
Isso � o modelo comum de heran�a de tr�s n�veis.

Quando construir sua aplica��o, voc� pode escolher seguir esse m�todo ou simplesmente
tornar cada template de p�gina estender a template de aplica��o base diretamente
(ex: ``{% extends '::base.html.twig' %}``). O modelo de tr�s templates �
o m�todo de melhor pr�tica usado por vendor bundles ent�o aquele template
base para um pacote pode ser facilmente sobreposto para propriamente estender seu
layout base de aplica��o.

.. index::
   single: Templating; Sa�da para escape

Sa�da para escape
---------------

Quando gerar HTML de um template, sempre h� um risco que uma vari�vel de 
template pode gerar HTML involut�rio ou codigo do lado cliente perigoso. O resultado
� que o conte�do din�mico poderia quebrar o HTML de uma p�gina de resultados ou
permitir um usu�rio maldoso realizar um ataque `Cross Site Scripting`_ (XSS). Considere
esse exemplo cl�ssico:

.. configuration-block::

    .. code-block:: jinja

        Hello {{ name }}

    .. code-block:: php

        Hello <?php echo $name ?>

Imagine que o usu�rio entre o seguinte c�digo como o nome dele/dela::

    <script>alert('hello!')</script>

Sem qualquer outra sa�da de escape, o resultado da template ir� causar uma caixa de alerta
em JavaScript para saltar na tela::

    Hello <script>alert('hello!')</script>

E enquanto isso parece inofensivo, se um usu�rio pode chegar t�o longe, o
mesmo usu�rio deveria tamb�m ser capaz de escrever Javascript que realiza
a��es maliciosas dentro de uma �rea segura de um usu�rio leg�timo e desconhecido.

A resposta para o problema � sa�da para escape. Sem a sa�da para escape ativa,
o mesmo template ir� manipular inofensivamente, e literalmente imprimir a tag
``script`` na tela::

    Hello &lt;script&gt;alert(&#39;helloe&#39;)&lt;/script&gt;

Os sistemas de templating Twig e PHP aproximam-se do problema de formas diferentes.
Se voc� est� usando Twig, sa�da para escape � ativado por padr�o e voc� est� protegido.
Em PHP, sa�da para escape n�o � autom�tico, significando que voc� precisar� manualmente
fazer o escape quando necess�rio.

Sa�da para escape em Twig
~~~~~~~~~~~~~~~~~~~~~~~

Se voc� est� usando templates Twig, ent�o sa�da para escape � ativado por padr�o. Isto
significa que voc� est� protegido externamente de consequencias acidentais por c�digo
submetido por usu�rio. Por padr�o, a sa�da para escape assume que o conte�do est�
sendo escapado pela sa�da HTML.

Em alguns casos, voc� precisar� desabilitar sa�da para escape quando voc� est� manipulando
uma vari�vel que � confi�vel e cont�m marca��o que n�o poderia ter escape.
Suponha que usu�rios administrativos s�o capazes de escrever artigos que contenham
c�digo HTML. Por padr�o, Twig ir� escapar o corpo do artigo. Para faz�-lo normalamente,
adicione o filtro ``raw``: ``{{ article.body | raw }}``.

Voc� pode tamb�m desabilitar sa�da para escape dentro de uma �rea ``{% block %}`` ou
para um template inteiro. Para mais informa��es, veja `Output Escaping`_ na
documenta��o do Twig.

Sa�da para escape em PHP
~~~~~~~~~~~~~~~~~~~~~~

Sa�da para escape n�o � autom�tica quando usamos templates PHP. Isso significa
que a menos que voc� escolha escapar uma vari�vel explicitamente, voc� n�o est�
protegido. Para usar sa�da para escape use o m�todo de view ``escape()``::

    Hello <?php echo $view->escape($name) ?>

Por padr�o, o m�todo ``escape()`` assume que a vari�vel est� sendo manipulada
dentro de um contexto HTML (e assim a vari�vel escapa e est� segura para o HTML).
O segundo argumento deixa voc� mudar o contexto. Por exemplo, para gerar algo
em uma string Javascript, use o contexto ``js`` :

.. code-block:: js

    var myMsg = 'Hello <?php echo $view->escape($name, 'js') ?>';

.. index::
   single: Templating; Formats

.. _template-formats:

Formatos de Template
----------------

Templates s�o uma forma gen�rica de modificar conte�do em *qualquer* formato. E enquanto
em muitos casos voc� ir� usar templates para modificar conte�do HTML, um template pode
t�o f�cil como certo gerar JavaScript, CSS, XML ou qualquer outro formato que voc� possa sonhar.

Por exemplo, o mesmo "recurso" � sempre modificado em diversos formatos diferentes.
Para modificar uma p�gina inicial de um artigo XML, simplesmente inclua o formato no
nome do template:

* *nome do template XML*: ``AcmeArticleBundle:Article:index.xml.twig``
* *nome do arquivo do template XML*: ``index.xml.twig``

Na realidade, isso � nada mais que uma conven��o de nomea��o e o template
n�o � realmente modificado de forma diferente ao baseado no formato dele.

Em muitos casos, voc� pode querer permitir um controller unit�rio para modificar
m�ltiplos formatos diferentes baseado no "formato de requisi��o". Por aquela raz�o,
um padr�o comum � fazer o seguinte:

.. code-block:: php

    public function indexAction()
    {
        $format = $this->getRequest()->getRequestFormat();
    
        return $this->render('AcmeBlogBundle:Blog:index.'.$format.'.twig');
    }

O ``getRequestFormat`` no objeto ``Request`` padroniza para ``html``,
mas pode retornar qualquer outro formato baseado no formato solicitado pelo usu�rio.
O formato solicitado � frequentemente mais gerenciado pelo roteamento, onde uma rota
pode ser configurada para que ``/contact``  configure o formato requisitado ``html`` enquanto
``/contact.xml`` configure o formato para ``xml``. Para mais informa��es, veja
:ref:`Advanced Example in the Routing chapter <advanced-routing-example>`.

Para criar links que incluam o par�metro de formato, inclua uma chave ``_format``
no detalhe do par�metro:

.. configuration-block::

    .. code-block:: html+jinja

        <a href="{{ path('article_show', {'id': 123, '_format': 'pdf'}) }}">
            PDF Version
        </a>

    .. code-block:: html+php

        <a href="<?php echo $view['router']->generate('article_show', array('id' => 123, '_format' => 'pdf')) ?>">
            PDF Version
        </a>

Considera��es finais
--------------

O engine de template em Symfony � uma ferramenta poderosa que pode ser usada cada momento
que voc� precisa para gerar conte�do de apresenta��o em HTML, XML ou qualquer outro formato.
E apesar de tempaltes serem um jeito comum de gerar conte�do em um controller,
o uso deles n�o s�o obrigat�rios. O objeto ``Response`` object retornado por um controller
pode ser criado com ou sem o uso de um template:

.. code-block:: php

    // creates a Response object whose content is the rendered template
    $response = $this->render('AcmeArticleBundle:Article:index.html.twig');

    // creates a Response object whose content is simple text
    $response = new Response('response content');

Engine de template do Symfony � muito flex�vel e dois editores de template
diferentes est�o dispon�veis por padr�o: os tradicionais templates do *PHP* e os
polidos e poderosos templates do *Twig* . Ambos suportam uma hierarquia de template e
v�m empacotados com um conjunto rico de fun��es helper capazes de realizar
as tarefas mais comuns.

No geral, o t�pico de template poderia ser pensado como uma ferramenta poderosa
que est� � sua disposi��o. Em alguns casos, voc� pode n�o precisar modificar um template,
e em Symfony2, isso � absolutamente legal.

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
