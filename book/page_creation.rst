.. index::
   single: Cria��o de p�ginas

Criando P�ginas no Symfony2
==========================

Criar uma nova p�gina em Symfony2 � um processo simples em dois passos:

* *Criar uma rota*: Uma rota define a URL (ex: ``/about``) para sua p�gina
  e especifica um controller (que � uma fun��o PHP) que Symfony2 deveria
  executar quando a URL de uma requisi��o de entrada corresponder a um padr�o da rota;
  
* *Criar um controller*: Um controller � uma fun��o PHP que pega a requisi��o
  de entrada e transforma ela em um objeto ``Response`` do Symfony2 que � retornado
  ao usu�rio.
  
Essa simples abordagem � linda porque corresponde � forma que a Web trabalha.
Cada intera��o na Web � inicializada por uma requisi��o HTTP. A tarefa de sua
aplica��o � simplesmente interpretar a requisi��o e retornar a requisi��o 
HTTP apropriada.

Symfony2 segue a filosofia e fornece voc� com ferramentas e conven��es para
manter sua aplica��o organizada conforme ela cresce em usu�rios e complexidade.

Parece simples o bastante ? Vamos vasculhar !

.. index::
   single: Cria��o de P�gina; Exemplo

A P�gina "Hello Symfony!" 
-------------------------

Vamos come�ar com uma vers�o derivada da cl�ssica aplica��o "Hello World!". Quando
voc� estiver pronto, o usu�rio ser� capaz de obter uma sauda��o pessoal (ex: "Hello Symfony")
ao acessar a seguinte URL:

.. code-block:: text

    http://localhost/app_dev.php/hello/Symfony

Na verdade, voc� ser� capaz de substituir ``Symfony`` com qualquer outro nome para
ser saudado. Para criar a p�gina, siga o simples processo em dois passos.

.. note::
    
    O tutorial presume que voc� realmente fez o download do Symfony2 e configurou
    seu servidor web. A URL acima presume que   ``localhost`` aponta para o diret�rio
    ``web`` de seu novo projeto do Symfony2. Para informa��es detalhadas
    deste processo, veja :doc:`Installing Symfony2</book/installation>`.

Antes de voc� come�ar: Crie o Pacote
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Antes de voc� come�ar, voc� ir� precisar criar um *pacote*. Em Symfony2, um :term:`bundle`
� como um plugin, exceto que todo o c�digo em sua aplica��o ir� residir dentro de um pacote.

Um pacote � nada mais que um diret�rio que abriga tudo relacionado a uma funcionalidade
espec�fica, incluindo classes PHP, configura��o, e mesmo arquivos em estilo cascata e Javascript 
(veja :ref:`page-creation-bundles`).

Para criar um pacote chamado ``AcmeHelloBundle`` (um pacote de execu��o que voc� ir�
construir nesse cap�tulo), corra o seguinte comando e siga as instru��es na tela (use 
todas as op��es padr�o):

.. code-block:: bash

    php app/console generate:bundle --namespace=Acme/HelloBundle --format=yml

Atr�s dos bastidores, um diret�rio � criado para o pacote em ``src/Acme/HelloBundle``.
Uma linha � tamb�m automaticamente adicionada ao arquivo ``app/AppKernel.php`` ent�o
aquele pacote � registrado com o kernel::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            // ...
            new Acme\HelloBundle\AcmeHelloBundle(),
        );
        // ...

        return $bundles;
    }

Agora que voc� tem uma configura��o de pacote, voc� pode come�ar a construir sua
aplica��o dentro do pacote.

Step 1: Crie a Rota
~~~~~~~~~~~~~~~~~~~~~~~~

Por padr�o, o arquivo de configura��o de rota em uma aplica��o Symfony2 �
localizado em ``app/config/routing.yml``. Como todas as configura��es em Symfony2,
voc� tamb�m pode escolher usar XML ou PHP 
you can also choose to use XML or PHP out of the box to configure routes.

If you look at the main routing file, you'll see that Symfony already added
an entry when you generated the ``AcmeHelloBundle``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        AcmeHelloBundle:
            resource: "@AcmeHelloBundle/Resources/config/routing.yml"
            prefix:   /

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@AcmeHelloBundle/Resources/config/routing.xml" prefix="/" />
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->addCollection(
            $loader->import('@AcmeHelloBundle/Resources/config/routing.php'),
            '/',
        );

        return $collection;

Essa entrada � bem b�sica: ela informa o Symfony a carregar a configura��o de rota do
arquivo ``Resources/config/routing.yml`` que reside dentro de ``AcmeHelloBundle``.
Isto significa que voc� insere a configura��o de rota diretamente em ``app/config/routing.yml``
ou organizar suas rotas atrav�s de sua aplica��o, e import�-las de l�.

Agora que o arquivo ``routing.yml`` do pacote � tamb�m importado, adicione a 
nova rota que define a URL da p�gina que voc� ir� criar:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/routing.yml
        hello:
            pattern:  /hello/{name}
            defaults: { _controller: AcmeHelloBundle:Hello:index }

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" pattern="/hello/{name}">
                <default key="_controller">AcmeHelloBundle:Hello:index</default>
            </route>
        </routes>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('hello', new Route('/hello/{name}', array(
            '_controller' => 'AcmeHelloBundle:Hello:index',
        )));

        return $collection;

A rota consiste de duas partes b�sicas: o ``pattern``, que � a URL
que essa rota ir� corresponder, e um array ``defaults``, que especifica o 
controller que deveria ser executado. A sintaxe do espa�o reservado no padr�o
(``{name}``) � um coringa. Isso significa que ``/hello/Ryan``, ``/hello/Fabien``
ou qualquer outra URL similar ir� combinar com essa rota. O par�metro de espa�o reservado 
``{name}`` ir� tamb�m ser passado ao controller ent�o voc� pode usar este valor
para pessoalmente cumprimentar o usu�rio.

.. note::
   
  O sistema de rota tem muito mais grandes funcionalidades para criar 
  estruturas URL flex�veis e poderosas em sua aplica��o. Para mais detalhes, veja
  tudo sobre o cap�tulo :doc:`Routing </book/routing>`.

Step 2: Criando o Controller
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando uma URL como``/hello/Ryan`` � manipulada pela aplica��o, a rota ``hello``
� combinada e o controller ``AcmeHelloBundle:Hello:index`` � executada
pelo framework. O segundo passo da p�gina de cria��o � criar aquele 
controller.

O controller - ``AcmeHelloBundle:Hello:index`` � o nome *l�gico* do
controller, e ele mapeia o m�todo ``indexAction`` de uma classe PHP
chamada ``Acme\HelloBundle\Controller\Hello``. Comece a criar esse arquivo
dentro de ``AcmeHelloBundle``::

    // src/Acme/HelloBundle/Controller/HelloController.php
    namespace Acme\HelloBundle\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
    }

Na verdade, o controller � nada mais que um m�todo PHP que voc� cria e 
o Symfony executa. Isso � onde seu c�digo usa informa��o da requisi��o
para construir e preparar o recurso sendo requisitado. Exceto em alguns casos 
avan�ados, o produto final de um controller � sempre o mesmo: um objeto 
Symfony2 ``Response``.

Crie o m�todo ``indexAction`` que o Symfony ir� executar quando a rota ``hello``
for correspondida::

    // src/Acme/HelloBundle/Controller/HelloController.php

    // ...
    class HelloController
    {
        public function indexAction($name)
        {
            return new Response('<html><body>Hello '.$name.'!</body></html>');
        }
    }

O controller � simples: ele cria um novo objeto ``Response``, cujo primeiro
argumento � um conte�do que deve ser usado em uma resposta (uma pequena
p�gina HTML neste exemplo).

Parab�ns! Ap�s criar somente uma rota e um controller, voc� realmente
rem uma p�gina totalmente funcional ! Se voc� configurou tudo corretamente, sua
aplica��o dever� cumprimentar voc�:

.. code-block:: text

    http://localhost/app_dev.php/hello/Ryan

.. tip::
    
    Voc� tamb�m pode ver sua aplica��o no "prod":ref:`environment<environments-summary>`
    ao visitar:

    .. code-block:: text

        http://localhost/app.php/hello/Ryan
    
    Se voc� tiver um erro, isto � comum porque voc� precisa limpar o cache
    ao executar:
    
    .. code-block:: bash

        php app/console cache:clear --env=prod --no-debug

Um terceiro passo opcional,mas comum, no processo � criar um template.

.. note::
    
   Controllers s�o o ponto de entrada principal para seu c�digo e um ingrediente
   chave quando criar p�ginas. Muito mais informa��es podem ser encontradas em
   :doc:`Controller Chapter </book/controller>`.

Passo Opcional 3: Criar o Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Templates permitem a voc� mover todas as apresenta��es (ex: c�digo HTML) em
um arquivo separado e reusar por��es diferentes do layout da p�gina. Ao inv�s
de escrever o HTML dentro do controller, gere um template:

.. code-block:: php
    :linenos:

    // src/Acme/HelloBundle/Controller/HelloController.php
    namespace Acme\HelloBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class HelloController extends Controller
    {
        public function indexAction($name)
        {
            return $this->render('AcmeHelloBundle:Hello:index.html.twig', array('name' => $name));

            // render a PHP template instead
            // return $this->render('AcmeHelloBundle:Hello:index.html.php', array('name' => $name));
        }
    }

.. note::
    Em ordem de usar o m�todo ``render()``, seu controller deve estender a classe
   ``Symfony\Bundle\FrameworkBundle\Controller\Controller`` (API
   docs: :class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`),
   que adiciona atalhos para tarefas que s�o comum dentro de controllers. Isso
   � feito no exemplo acima ao adicionar a senten�a ``use`` na linha 4
   e ent�o estender a ``Controller`` na linha 6.

O m�todo ``render()`` cria um objeto ``Response`` coberto com o conte�do de
um template dado, gerado. Como qualquer outro controller, voc� ir� ultimamente
retornar o objeto ``Response``.

Perceba que existem dois exemplos diferentes de gerar a template.
Por padr�o, Symfony2 suporta duas linguagens diferentes de template: templates
PHP cl�ssicas e as sucintas mas poderosas templates `Twig`_. N�o fique
alarmado - voc� � livre para escolher uma ou mesmo ambas no mesmo projeto.

O controller manipula o template ``AcmeHelloBundle:Hello:index.html.twig``,
o qual usa a seguinte conven��o de nome:

    **BundleName**:**ControllerName**:**TemplateName**

Esse � o nome *l�gico* do template, o qual � mapeado para uma localiza��o
f�sica usando a seguinte conven��o.

    **/path/to/BundleName**/Resources/views/**ControllerName**/**TemplateName**

Neste caso, ``AcmeHelloBundle`` � o nome do pacote, ``Hello`` � o
controller, e ``index.html.twig`` o template:

.. configuration-block::

    .. code-block:: jinja
       :linenos:

        {# src/Acme/HelloBundle/Resources/views/Hello/index.html.twig #}
        {% extends '::base.html.twig' %}

        {% block body %}
            Hello {{ name }}!
        {% endblock %}

    .. code-block:: php

        <!-- src/Acme/HelloBundle/Resources/views/Hello/index.html.php -->
        <?php $view->extend('::base.html.php') ?>

        Hello <?php echo $view->escape($name) ?>!

Vamos detalhar atrav�s do template Twig linha-a-linha:

* *line 2*: O token ``extends`` define um template pai. O template
  explicitamente define um arquivo de layout dentro do qual ir� ser colocado.
  
* *line 4*: O token ``block`` diz que tudo dentro deveria ser colocado dentro de um
  bloco chamado ``body``. Como voc� ver�, isto � responsabilidade do
  template pai (``base.html.twig``) para ultimamente transformar o bloco
  chamado ``body``.

No template pai, ``::base.html.twig``, est� faltando tanto as por��es do nome de **BundleName**
como **ControllerName** (por isso o duplo dois pontos (``::``)
no come�o). Isso significa que o template reside fora dos pacotes e dentro do 
diret�rio ``app``:

.. configuration-block::

    .. code-block:: html+jinja

        {# app/Resources/views/base.html.twig #}
        <!DOCTYPE html>
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
                <title>{% block title %}Welcome!{% endblock %}</title>
                {% block stylesheets %}{% endblock %}
                <link rel="shortcut icon" href="{{ asset('favicon.ico') }}" />
            </head>
            <body>
                {% block body %}{% endblock %}
                {% block javascripts %}{% endblock %}
            </body>
        </html>

    .. code-block:: php

        <!-- app/Resources/views/base.html.php -->
        <!DOCTYPE html>
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
                <title><?php $view['slots']->output('title', 'Welcome!') ?></title>
                <?php $view['slots']->output('stylesheets') ?>
                <link rel="shortcut icon" href="<?php echo $view['assets']->getUrl('favicon.ico') ?>" />
            </head>
            <body>
                <?php $view['slots']->output('_content') ?>
                <?php $view['slots']->output('stylesheets') ?>
            </body>
        </html>

O arquivo de template base define o layout de HTML e gera o bloco ``body`` que
voc� definiu no template ``index.html.twig``. Ele tamb�m gera um bloco ``title``
, no qual voc� poderia escolher definir no template ``index.html.twig``.
Desde que voc� n�o defina o bloco ``title`` no template filho, o padr�o ser� "Welcome!".

Templates s�o um modo poderoso de gerar e organizar o conte�do para sua 
p�gina. Um template pode gerar tudo, de marca��o HTML, para c�digo CSS, ou algo mais
que o controller precise retornar.

No ciclo de vida do controle de uma requisi��o, a engine de template � simplesmente
uma ferramenta opcional. Rechamar o objetido de cada controller � retornar um objeto
``Response``. Templates s�o ferramentas poderosas, mas opcionais , para criar o 
conte�do para o objeto ``Response``.

.. index::
   single: Directory Structure

A Estrutura de Diret�rio
-----------------------

Ap�s umas poucas se��es, voc� realmente entende a filosofia por tr�s da cria��o
e transforma��o de p�ginas no Symfony2. Voc� tamb�m come�ou a perceber como os projetos
do Symfony2 s�o estruturados e organizados. No final desta se��o,
voc� saber� onde achar e colocar diferentes tipos de arquivos e o porqu�.

Apesar de inteiramente flex�vel, por padr�o, cada Symfony :term:`application` tem
a mesma estrutura b�sica e recomendada de estrutura de diret�rios:

* ``app/``: Este diret�rio cont�m a configura��o da aplica��o;

* ``src/``: Todos os c�digos do projeto PHP s�o armazenados neste diret�rio;

* ``vendor/``: Qualquer bibliotecas vendor s�o colocadas aqui por conven��o;

* ``web/``: Esse � o diret�rio web raiz e cont�m qualquer arquivo acess�vel publicamente;

O Diret�rio Web
~~~~~~~~~~~~~~~~~

O diret�rio web raiz � a origem de todos os arquivos p�blicos e est�ticos incluindo
imagens, folhas de estilo, e os arquivos Javascript. � l� tamb�m onde cada
:term:`front controller` fica::

    // web/app.php
    require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('prod', false);
    $kernel->loadClassCache();
    $kernel->handle(Request::createFromGlobals())->send();

O arquivo de front controller (``app.php`` neste examplo) � o verdadeiro arquivo PHP
que � executado quando usa a aplica��o Symfony2 application e � o trabalho dele usar
uma classe Kernel, ``AppKernel``, para iniciar a aplica��o.

.. tip::

    Ter um front controller significa URLs diferentes e mais flex�veis que as 
    usadas em uma aplica��o PHP pura. Ao usar um front controller,
    URLs s�o formatadas da seguinte forma:

    .. code-block:: text

        http://localhost/app.php/hello/Ryan

    O front controller, ``app.php``, � executado e a "internal:" URL
    ``/hello/Ryan`` � roteada internamente usando a configura��o de rota.
    Ao usar as regras ``mod_rewrite`` do Apache , voc� pode for�ar o arquivo ``app.php`` 
    para ser executado sem a necessidade de especific�-la na URL:

    .. code-block:: text

        http://localhost/hello/Ryan

Apesar de front controllers serem essenciais em manipular cada requisi��o, voc� ir�
raramente precisar modificar ou mesmo pensar neles. N�s iremos mencion�-los novamente
em breve na se��o `Environments`_.

O Diret�rio de Aplica��o (``app``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Como voc� viu no front controller, a classe ``AppKernel`` no ponto de entrada
principal da aplica��o e � respons�vel por todas as configura��es. Por padr�o,
� armazenado no diret�rio ``app/``.

Essa classe deve implementar dois m�todos que definem tudo que Symfony
precisa saber sobre sua aplica��o. Voc� nem precisa mesmo se preocupar sobre
esses m�todos ao iniciar - Symfony preenche eles para voc� com padr�es coerentes.

* ``registerBundles()``: Retorna um vetor de todos os pacotes necess�rios para rodar a
  aplica��o (see :ref:`page-creation-bundles`);

* ``registerContainerConfiguration()``: Carrega o arquivo de recurso da configura��o 
   da aplica��o principal (veja a se��o `Application Configuration`_).

No desenvolvimento do dia-a-dia, voc� ir� usar bastante o diret�rio ``app/`` para 
modificar arquivos de configura��o e rotas no diret�rio ``app/config/`` (veja
`Application Configuration`_). Ele tamb�m cont�m o diret�rio de cache da aplica��o
(``app/cache``), um diret�rio de log (``app/logs``) e um diret�rio
para arquivos de recursos de n�vel de aplica��o, como os templates (``app/Resources``).
Voc� ir� aprender mais sobre cada um desses diret�rios nos �ltimos cap�tulos.

.. _autoloading-introduction-sidebar:

.. sidebar:: Autocarregamento

    Quando Symfony est� carregando, um arquivo especial - ``app/autoload.php`` - � inclu�do.
    Esse arquivo � respons�vel por configurar o autoloader, que ir� auto-carregar
    seus arquivos de aplica��o do diret�rio ``src/`` e bibliotecas de terceiros do
    diret�rio ``vendor/``.

    Por causa do autoloader, voc� nunca ir� precisar se preocupar sobre usar cl�usulas ``include``
    ou ``require``. Ao inv�s disso, Symfony2 usa o namespace de uma classe para
    determinar sua localiza��o e automaticamente incluir o arquivo no momento que voc� precisar da classe
    , para sua seguran�a.
    
    O autoloader � realmente configurado para verificar no diret�rio ``src/`` para
    qualquer uma de suas classes PHP. Para o autocarregamento funcionar, o nome da classe 
    e o atalho para o arquivo tem que seguir omesmo padr�o:
    
    .. code-block:: text

        Class Name:
            Acme\HelloBundle\Controller\HelloController
        Path:
            src/Acme/HelloBundle/Controller/HelloController.php
    
    Tipicamente, o �nico momento que voc� precisar� se preocupar sobre o 
    arquivo ``app/autoload.php`` � quando voc� estar� incluindo uma nova biblioteca
    de terceiros no diret�rio ``vendor/``. Para mais informa��es no autocarregamento, veja
    :doc:`How to autoload Classes</cookbook/tools/autoloader>`.

O Diret�rio de Recursos (``src``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Coloque simplesmente, o diret�rio ``src/`` cont�m todo o c�digo de verdade (c�digo PHP,
templates, arquivos de configura��o, folhas de estilo, etc) que comanda *sua* aplica��o.
Enquanto estiver desenvolvendo, a vasta maioria de seu trabalho ser� feita dentro de um ou
mais pacotes que voc� cria nesse diret�rio.

Mas o que � exatamente um :term:`pacote`?

.. _page-creation-bundles:

O Sistema de Pacotes
-----------------

Um pacote � similar a um plugin em outro software, mas at� mesmo melhor. A diferen�a-chave
� que *tudo* � um pacote no Symfony2, incluindo ambas as funcionalidades centrais do
framework e o c�digo escrito para sua aplica��o.
Pacotes s�o cidad�os de primeira classe no Symfony2. Isso d� a voc� a flexibilidade para
usar funcionalidades pr�-montadas englobadas em `third-party bundles`_ ou para distribuir
seus pr�prios pacotes. Ele torna f�cil para selecionar e escolher qual funcionalidade habilitar
em sua aplica��o e para otimiz�-los da forma que voc� quer.

.. note::

   Quando voc� for aprender as bases aqui, uma entrada inteira do livro de receitas � dedicado para
   a organiza��o e as melhores pr�ticas de:doc:`pacotes</cookbook/bundles/best_practices>`.

Um pacote � simplesmente um conjunto estruturado de arquivos dentro de um diret�rio que implementa
uma �nica funcionalidade. Voc� poderia criar um ``BlogBundle``, um ``ForumBundle`` ou
um pacote para gerenciamento de usu�rio (muitos desses realmente existem como pacote de c�digo
aberto).
Cada diret�rio cont�m tudo relacionado �quela funcionalidade, incluindo arquivos PHP, templates,
folhas de estilo, Javascripts, testes e qualquer coisa mais.
Cada aspecto de uma funcionalidade existe em um pacote e cada funcionalidade fica dentro de 
um pacote.

Uma aplica��o � feita por pacotes definidos no m�todo ``registerBundles()``
da classe ``AppKernel`` ::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            new Symfony\Bundle\MonologBundle\MonologBundle(),
            new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            new Symfony\Bundle\DoctrineBundle\DoctrineBundle(),
            new Symfony\Bundle\AsseticBundle\AsseticBundle(),
            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
            new JMS\SecurityExtraBundle\JMSSecurityExtraBundle(),
        );

        if (in_array($this->getEnvironment(), array('dev', 'test'))) {
            $bundles[] = new Acme\DemoBundle\AcmeDemoBundle();
            $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            $bundles[] = new Sensio\Bundle\DistributionBundle\SensioDistributionBundle();
            $bundles[] = new Sensio\Bundle\GeneratorBundle\SensioGeneratorBundle();
        }

        return $bundles;
    }

Com o m�todo ``registerBundles()`` , voc� tem controle total sobre quais pacotes s�o 
usados por sua aplica��o (incluindo os pacotes centrais do Symfony).

.. tip::
    
    Um pacote pode residir *em qualquer lugar* desde que possa ser autocarregado (pelo
   autoloader configurado em ``app/autoload.php``).

Criando um Pacote
~~~~~~~~~~~~~~~~~

O Symfony Standard Edition v�m com uma tarefa conveniente que cria um pacote inteiramente funcional
para voc�. � claro, criar um pacote na m�o � muito f�cil tamb�m.

Para mostrar a voc� o qu�o simple o sistema de pacotes �, crie um novo pacote
chamado ``AcmeTestBundle`` e habilite ele.

.. tip::

    A por��o ``Acme`` � s� um nome bobo que poderia ser substitu�do por algum nome 
    de "vendedor" que represente voc� ou sua organiza��o (ex: ``ABCTestBundle``
    para alguma empresa chamada ``ABC``).

Comece criando um diret�rio ``src/Acme/TestBundle/`` e adicionando um novo arquivo
chamado ``AcmeTestBundle.php``::

    // src/Acme/TestBundle/AcmeTestBundle.php
    namespace Acme\TestBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class AcmeTestBundle extends Bundle
    {
    }

.. tip::

   O nome``AcmeTestBundle`` segue o padr�o :ref:`Conven��o para nomes de pacotes<bundles-naming-conventions>`.
   Voc� poderia tamb�m escolher encurtar o nome do pacote para simplesmente ``TestBundle``
   ao nomear essa classe``TestBundle`` (e nomeando o arquivo ``TestBundle.php``).

Essa classe vazia � a �nica pe�a que voc� precisa para criar o novo pacote. Apesar de normalmente
vazia, essa classe � poderosa e pode ser usada para personalizar o comportamento do pacote.

Agora que voc� criou o pacote, abilite-o via classe ``AppKernel`` ::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            // ...

            // register your bundles
            new Acme\TestBundle\AcmeTestBundle(),
        );
        // ...

        return $bundles;
    }

E enquanto n�o fizer algo ainda,  ``AcmeTestBundle`` est� pronto para
ser usado.

E t�o simples quanto �, Symfony tamb�m providencia uma interface por linha de comando para
gerar um esqueleto de um pacote b�sico:

.. code-block:: bash

    php app/console generate:bundle --namespace=Acme/TestBundle

O esqueleto do pacote gera um controller b�sico, template e recurso para rota que
pode ser personalizado. Voc� ir� aprender mais sobre ferramentas de linha de comando do
Symfony2 mais tarde.

.. tip::

   Quando sempre criar um novo pacote ou usar um pacote de terceiros, always make
   sure the bundle has been enabled in ``registerBundles()``. When using
   the ``generate:bundle`` command, this is done for you.

Estrutura de Diret�rio de Pacotes
~~~~~~~~~~~~~~~~~~~~~~~~~~

A estrutura de diret�rio de um pacote � simples e flex�vel. Por padr�o, o sistema de pacote
segue um conjunto de conven��es que ajudam a manter o c�digo consistente entre todos
os pacotes do Symfony2. D� uma olhada em ``AcmeHelloBundle``, pois cont�m 
alguns dos elementos mais comuns de um pacote:

* ``Controller/`` cont�m todos os controllers do pacote (ex: ``HelloController.php``);

* ``Resources/config/`` abriga configura��es, incluindo configura��es de rota
  (e.g. ``routing.yml``);

* ``Resources/views/`` cont�m templates organizados pelo nome do controller (ex:
  ``Hello/index.html.twig``);

* ``Resources/public/`` cont�m assets web (imagens, folhas de estilo, etc) e �
  copiada ou simbolicamente vinculada dentro do projeto ``web/`` directory via
  comando de console ``assets:install`` ;

* ``Tests/`` cont�m todos os testes para o pacote.

Um pacote pode ser pequeno ou grande conforme as funcionalidades que implementa. Cont�m
s� os arquivos que voc� precisa e nada mais.

Conforme avan�amos pelo livro, voc� aprender� como persistir objetos para um banco de dados,
criar e validar formul�rios, criar tradu��es para sua aplica��o, escrever testes e muito mais.
Cada um desses tem seu pr�prio local e papel dentro do pacote.

Configura��es de aplica��o
-------------------------

Uma aplica��o consiste de uma cole��o de pacotes representando todas as funcionalidades
e capacidades de sua aplica��o. Cada pacote pode ser personalizado via arquivos de 
configura��o escritos em  YAML, XML ou PHP. Por padr�o, o arquivo de configura��o 
padr�o fica no diret�rio ``app/config/`` e � chamado ou ``config.yml``, ``config.xml`` 
ou ``config.php`` dependendo do formato que voc� preferir:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: parameters.yml }
            - { resource: security.yml }
        
        framework:
            secret:          %secret%
            charset:         UTF-8
            router:          { resource: "%kernel.root_dir%/config/routing.yml" }
            form:            true
            csrf_protection: true
            validation:      { enable_annotations: true }
            templating:      { engines: ['twig'] } #assets_version: SomeVersionScheme
            session:
                default_locale: %locale%
                auto_start:     true

        # Twig Configuration
        twig:
            debug:            %kernel.debug%
            strict_variables: %kernel.debug%

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <imports>
            <import resource="parameters.yml" />
            <import resource="security.yml" />
        </imports>
        
        <framework:config charset="UTF-8" secret="%secret%">
            <framework:router resource="%kernel.root_dir%/config/routing.xml" />
            <framework:form />
            <framework:csrf-protection />
            <framework:validation annotations="true" />
            <framework:templating assets-version="SomeVersionScheme">
                <framework:engine id="twig" />
            </framework:templating>
            <framework:session default-locale="%locale%" auto-start="true" />
        </framework:config>

        <!-- Twig Configuration -->
        <twig:config debug="%kernel.debug%" strict-variables="%kernel.debug%" />

        <!-- ... -->

    .. code-block:: php

        $this->import('parameters.yml');
        $this->import('security.yml');

        $container->loadFromExtension('framework', array(
            'secret'          => '%secret%',
            'charset'         => 'UTF-8',
            'router'          => array('resource' => '%kernel.root_dir%/config/routing.php'),
            'form'            => array(),
            'csrf-protection' => array(),
            'validation'      => array('annotations' => true),
            'templating'      => array(
                'engines' => array('twig'),
                #'assets_version' => "SomeVersionScheme",
            ),
            'session' => array(
                'default_locale' => "%locale%",
                'auto_start'     => true,
            ),
        ));

        // Twig Configuration
        $container->loadFromExtension('twig', array(
            'debug'            => '%kernel.debug%',
            'strict_variables' => '%kernel.debug%',
        ));

        // ...

.. note::

   Voc� ir� aprender exatamento como carregar cada arquivo/formato na pr�xima se��o
   `Environments`_.

Cada entrada de n�vel superior como ``framework`` ou ``twig`` define a configura��o para
um pacote particular. Por exemplo,  a chave ``framework`` define a configura��o para
o n�cleo Symfony  ``FrameworkBundle`` e inclui configura��o para as rotas, templates
e outros sistemas n�cleo.

Neste momento, n�o se preocupe sobre uma op��o de configura��o espec�fica em cada sess�o.
O arquivo de configura��o � encaminhado com padr�es sensatos. Conforme voc� ler mais e
explorar cada parte de Symfony2, voc� ir� aprender sobre a configura��o espec�fica
de cada funcionalidade.

.. sidebar:: Configuration Formats

	Atrav�s dos cap�tulos, todas as configura��es de exemplo ser�o exibidas
	em todos os tr�s formatos (YAML, XML e PHP). Cada um tem suas vantagens
	e desvantagens. A escolha de qual utilizar � sua:
	
    * *YAML*: Simples, claro e leg�vel;

    * *XML*: Mais poderoso que YAML algumas vezes e suporta autocompletar da IDE;

    * *PHP*: Mais poderoso mas menos leg�vel que os formatos de configura��o padr�o.

.. index::
   single: Environments; Introduction

.. _environments-summary:

Ambientes
------------

Uma aplica��o pode executar em v�rios ambientes. Os ambientes diferentes
dividem o mesmo c�digo PHP (independente do front controller), mas usam
configur��o diferente. Por inst�ncia, um ambiente ``dev`` ir� exibir alertas
e erros, enquanto um ambiente  ``prod`` ir� somente logar erros. Alguns arquivos s�o
reconstru�dos em cada solicita��o no ambiente``dev`` (para conveni�ncia do desenvolvedor),
mas armazenados no ambiente ``prod`` . Todos os ambientes convivem juntos na mesma
m�quina e executam a mesma aplica��o.

Um projeto Symfony2 geralmente come�a com tr�s ambientes (``dev``, ``test``
e ``prod``), apesar que criar novos ambientes � f�cil. Voc� pode visualizar sua
aplica��o em diferentes ambientes simplesmente mudando o front controller
no seu navegador. Para ver a aplica��o no ambiente ``dev`` , accesse a 
aplica��o com o front controller de desenvolvimento:

.. code-block:: text

    http://localhost/app_dev.php/hello/Ryan

Se voc� gostaria de visualizar como sua aplica��o ir� se comportar no ambiente de produ��o,
ao inv�s disto ,chame o front controller ``prod``:

.. code-block:: text

    http://localhost/app.php/hello/Ryan

Como o ambiente ``prod`` � otimizado para velocidade; a configura��o, rota
e templates Twig s�o compilados em classes de PHP puro e armazenados.
Quando visualizar mudan�as no ambiente ``prod``, voc� ir� precisar limpar esses
arquivos armazenados e permitir reconstruir eles:

    php app/console cache:clear --env=prod --no-debug

.. note::

	Se voc� abrir  o arquivo ``web/app.php`` , voc� ir� perceber que est� explicitamente configurado 
   para utilizar o ambiente ``prod``::

       $kernel = new AppKernel('prod', false);
	
	Voc� pode criar um novo front controller para um novo ambiente ao copiar
	esse arquivo e mudando ``prod`` para algum outro valor.

.. note::
	
	O ambiente ``test`` � usado para executar testes automatizados e n�o pode ser acessado
	diretamente atrav�s do navegador. Veja :doc:`testing chapter</book/testing>`
    para mais detalhes.

.. index::
   single: Environments; Configuration

Configura��es de ambiente
~~~~~~~~~~~~~~~~~~~~~~~~~

A classe ``AppKernel`` � respons�vel por carregar de verdade o arquivo de configura��o
de sua escolha::

    // app/AppKernel.php
    public function registerContainerConfiguration(LoaderInterface $loader)
    {
        $loader->load(__DIR__.'/config/config_'.$this->getEnvironment().'.yml');
    }

Voc� realmente sabe que a extens�o ``.yml`` pode ser mudada para ``.xml`` ou
``.php`` se voc� preferir usar ou XML ou PHP para escrever sua configura��o.
Perceba tamb�m que cada ambiente carrega seu pr�prio arquivo de configura��o. Considere
o arquivo de configura��o para o ambiente ``dev``.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        imports:
            - { resource: config.yml }

        framework:
            router:   { resource: "%kernel.root_dir%/config/routing_dev.yml" }
            profiler: { only_exceptions: false }

        # ...

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->
        <imports>
            <import resource="config.xml" />
        </imports>

        <framework:config>
            <framework:router resource="%kernel.root_dir%/config/routing_dev.xml" />
            <framework:profiler only-exceptions="false" />
        </framework:config>

        <!-- ... -->

    .. code-block:: php

        // app/config/config_dev.php
        $loader->import('config.php');

        $container->loadFromExtension('framework', array(
            'router'   => array('resource' => '%kernel.root_dir%/config/routing_dev.php'),
            'profiler' => array('only-exceptions' => false),
        ));

        // ...

A chave ``imports`` � similar � senten�a ``include`` do PHP e garante
que o arquivo de configura��o principal (``config.yml``) seja carregado primeiro. O resto
do arquivo modifica a configura��o padr�o para registro de log melhorado e outras
configura��es condizentes para um ambiente de produ��o.

Ambos os ambientes ``prod`` e ``test`` seguem o mesmo modelo: cada ambiente
importa o arquivo de configura��o base e ent�o modifica os valores de configura��o
para ajustar as necessidades de um ambiente espec�fico. Isso � apenas uma conven��o,
mas uma que permite voc� a reutilizar a maior parte de suas configura��es e personalizar
apenas peda�os do que est� entre os ambientes.

Sum�rio
-------

Parab�ns ! Voc� agora viu cada aspecto fundamental de Symfony2 e esperan�osamente
descobriu o qu�o f�cil e flexivel pode ser. E enquanto existem *um monte* de funcionalidades
por vir, esteja certo de manter os seguintes pontos em mente:

* criar uma p�gina � um processo de tr�s passos envolvendo uma **rota**, um **controller**
  e (opcionalmente) um **template**.

* cada projeto cont�m apenas alguns poucos diret�rios: ``web/`` (web assets e
  o front controllers), ``app/`` (configura��o), ``src/`` (seus pacotes),
  e ``vendor/`` (c�digo de terceiros) (existe tamb�m um diret�rio ``bin/`` que �
  utilizado para ajudar com bibliotecas vendor atualizadas);

* Cada funcionalidade em Symfony2 (incluindo um n�cleo do framework Symfony2) � organizado 
  em um *pacote*, que � um conjunto de arquivos estruturados para aquela funcionalidade;

* A **configura��o** para cada pacote reside no diret�rio ``app/config`` e
  pode ser especificado em YAML, XML ou PHP;

* cada **ambiente** � acess�vel por meio de um front controller diferente (ex:
  ``app.php`` e ``app_dev.php``) e carrega uma configura��o de arquivo diferente.

A partir daqui, cada cap�tulo ir� introduzir a voc� ferramentas mais e mais poderosas
e conceitos avan�ados. Quanto mais souber sobre  Symfony2, mais ir� apreciar a
flexibilidade da arquitetura dela e o poder que fornecer� a voc�
para desenvolber aplica��es rapidamente.

.. _`Twig`: http://twig.sensiolabs.org
.. _`third-party bundles`: http://symfony2bundles.org/
.. _`Symfony Standard Edition`: http://symfony.com/download
