.. index::
   single: Criação de páginas

Criando Páginas no Symfony2
==========================

Criar uma nova página em Symfony2 é um processo simples em dois passos:

* *Criar uma rota*: Uma rota define a URL (ex: ``/about``) para sua página
  e especifica um controller (que é uma função PHP) que Symfony2 deveria
  executar quando a URL de uma requisição de entrada corresponder a um padrão da rota;
  
* *Criar um controller*: Um controller é uma função PHP que pega a requisição
  de entrada e transforma ela em um objeto ``Response`` do Symfony2 que é retornado
  ao usuário.
  
Essa simples abordagem é linda porque corresponde à forma que a Web trabalha.
Cada interação na Web é inicializada por uma requisição HTTP. A tarefa de sua
aplicação é simplesmente interpretar a requisição e retornar a requisição 
HTTP apropriada.

Symfony2 segue a filosofia e fornece você com ferramentas e convenções para
manter sua aplicação organizada conforme ela cresce em usuários e complexidade.

Parece simples o bastante ? Vamos vasculhar !

.. index::
   single: Criação de Página; Exemplo

A Página "Hello Symfony!" 
-------------------------

Vamos começar com uma versão derivada da clássica aplicação "Hello World!". Quando
você estiver pronto, o usuário será capaz de obter uma saudação pessoal (ex: "Hello Symfony")
ao acessar a seguinte URL:

.. code-block:: text

    http://localhost/app_dev.php/hello/Symfony

Na verdade, você será capaz de substituir ``Symfony`` com qualquer outro nome para
ser saudado. Para criar a página, siga o simples processo em dois passos.

.. note::
    
    O tutorial presume que você realmente fez o download do Symfony2 e configurou
    seu servidor web. A URL acima presume que   ``localhost`` aponta para o diretório
    ``web`` de seu novo projeto do Symfony2. Para informações detalhadas
    deste processo, veja :doc:`Installing Symfony2</book/installation>`.

Antes de você começar: Crie o Pacote
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Antes de você começar, você irá precisar criar um *pacote*. Em Symfony2, um :term:`bundle`
é como um plugin, exceto que todo o código em sua aplicação irá residir dentro de um pacote.

Um pacote é nada mais que um diretório que abriga tudo relacionado a uma funcionalidade
específica, incluindo classes PHP, configuração, e mesmo arquivos em estilo cascata e Javascript 
(veja :ref:`page-creation-bundles`).

Para criar um pacote chamado ``AcmeHelloBundle`` (um pacote de execução que você irá
construir nesse capítulo), corra o seguinte comando e siga as instruções na tela (use 
todas as opções padrão):

.. code-block:: bash

    php app/console generate:bundle --namespace=Acme/HelloBundle --format=yml

Atrás dos bastidores, um diretório é criado para o pacote em ``src/Acme/HelloBundle``.
Uma linha é também automaticamente adicionada ao arquivo ``app/AppKernel.php`` então
aquele pacote é registrado com o kernel::

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

Agora que você tem uma configuração de pacote, você pode começar a construir sua
aplicação dentro do pacote.

Step 1: Crie a Rota
~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, o arquivo de configuração de rota em uma aplicação Symfony2 é
localizado em ``app/config/routing.yml``. Como todas as configurações em Symfony2,
você também pode escolher usar XML ou PHP 
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

Essa entrada é bem básica: ela informa o Symfony a carregar a configuração de rota do
arquivo ``Resources/config/routing.yml`` que reside dentro de ``AcmeHelloBundle``.
Isto significa que você insere a configuração de rota diretamente em ``app/config/routing.yml``
ou organizar suas rotas através de sua aplicação, e importá-las de lá.

Agora que o arquivo ``routing.yml`` do pacote é também importado, adicione a 
nova rota que define a URL da página que você irá criar:

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

A rota consiste de duas partes básicas: o ``pattern``, que é a URL
que essa rota irá corresponder, e um array ``defaults``, que especifica o 
controller que deveria ser executado. A sintaxe do espaço reservado no padrão
(``{name}``) é um coringa. Isso significa que ``/hello/Ryan``, ``/hello/Fabien``
ou qualquer outra URL similar irá combinar com essa rota. O parâmetro de espaço reservado 
``{name}`` irá também ser passado ao controller então você pode usar este valor
para pessoalmente cumprimentar o usuário.

.. note::
   
  O sistema de rota tem muito mais grandes funcionalidades para criar 
  estruturas URL flexíveis e poderosas em sua aplicação. Para mais detalhes, veja
  tudo sobre o capítulo :doc:`Routing </book/routing>`.

Step 2: Criando o Controller
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando uma URL como``/hello/Ryan`` é manipulada pela aplicação, a rota ``hello``
é combinada e o controller ``AcmeHelloBundle:Hello:index`` é executada
pelo framework. O segundo passo da página de criação é criar aquele 
controller.

O controller - ``AcmeHelloBundle:Hello:index`` é o nome *lógico* do
controller, e ele mapeia o método ``indexAction`` de uma classe PHP
chamada ``Acme\HelloBundle\Controller\Hello``. Comece a criar esse arquivo
dentro de ``AcmeHelloBundle``::

    // src/Acme/HelloBundle/Controller/HelloController.php
    namespace Acme\HelloBundle\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
    }

Na verdade, o controller é nada mais que um método PHP que você cria e 
o Symfony executa. Isso é onde seu código usa informação da requisição
para construir e preparar o recurso sendo requisitado. Exceto em alguns casos 
avançados, o produto final de um controller é sempre o mesmo: um objeto 
Symfony2 ``Response``.

Crie o método ``indexAction`` que o Symfony irá executar quando a rota ``hello``
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

O controller é simples: ele cria um novo objeto ``Response``, cujo primeiro
argumento é um conteúdo que deve ser usado em uma resposta (uma pequena
página HTML neste exemplo).

Parabéns! Após criar somente uma rota e um controller, você realmente
rem uma página totalmente funcional ! Se você configurou tudo corretamente, sua
aplicação deverá cumprimentar você:

.. code-block:: text

    http://localhost/app_dev.php/hello/Ryan

.. tip::
    
    Você também pode ver sua aplicação no "prod":ref:`environment<environments-summary>`
    ao visitar:

    .. code-block:: text

        http://localhost/app.php/hello/Ryan
    
    Se você tiver um erro, isto é comum porque você precisa limpar o cache
    ao executar:
    
    .. code-block:: bash

        php app/console cache:clear --env=prod --no-debug

Um terceiro passo opcional,mas comum, no processo é criar um template.

.. note::
    
   Controllers são o ponto de entrada principal para seu código e um ingrediente
   chave quando criar páginas. Muito mais informações podem ser encontradas em
   :doc:`Controller Chapter </book/controller>`.

Passo Opcional 3: Criar o Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Templates permitem a você mover todas as apresentações (ex: código HTML) em
um arquivo separado e reusar porções diferentes do layout da página. Ao invés
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
    Em ordem de usar o método ``render()``, seu controller deve estender a classe
   ``Symfony\Bundle\FrameworkBundle\Controller\Controller`` (API
   docs: :class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`),
   que adiciona atalhos para tarefas que são comum dentro de controllers. Isso
   é feito no exemplo acima ao adicionar a sentença ``use`` na linha 4
   e então estender a ``Controller`` na linha 6.

O método ``render()`` cria um objeto ``Response`` coberto com o conteúdo de
um template dado, gerado. Como qualquer outro controller, você irá ultimamente
retornar o objeto ``Response``.

Perceba que existem dois exemplos diferentes de gerar a template.
Por padrão, Symfony2 suporta duas linguagens diferentes de template: templates
PHP clássicas e as sucintas mas poderosas templates `Twig`_. Não fique
alarmado - você é livre para escolher uma ou mesmo ambas no mesmo projeto.

O controller manipula o template ``AcmeHelloBundle:Hello:index.html.twig``,
o qual usa a seguinte convenção de nome:

    **BundleName**:**ControllerName**:**TemplateName**

Esse é o nome *lógico* do template, o qual é mapeado para uma localização
física usando a seguinte convenção.

    **/path/to/BundleName**/Resources/views/**ControllerName**/**TemplateName**

Neste caso, ``AcmeHelloBundle`` é o nome do pacote, ``Hello`` é o
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

Vamos detalhar através do template Twig linha-a-linha:

* *line 2*: O token ``extends`` define um template pai. O template
  explicitamente define um arquivo de layout dentro do qual irá ser colocado.
  
* *line 4*: O token ``block`` diz que tudo dentro deveria ser colocado dentro de um
  bloco chamado ``body``. Como você verá, isto é responsabilidade do
  template pai (``base.html.twig``) para ultimamente transformar o bloco
  chamado ``body``.

No template pai, ``::base.html.twig``, está faltando tanto as porções do nome de **BundleName**
como **ControllerName** (por isso o duplo dois pontos (``::``)
no começo). Isso significa que o template reside fora dos pacotes e dentro do 
diretório ``app``:

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
você definiu no template ``index.html.twig``. Ele também gera um bloco ``title``
, no qual você poderia escolher definir no template ``index.html.twig``.
Desde que você não defina o bloco ``title`` no template filho, o padrão será "Welcome!".

Templates são um modo poderoso de gerar e organizar o conteúdo para sua 
página. Um template pode gerar tudo, de marcação HTML, para código CSS, ou algo mais
que o controller precise retornar.

No ciclo de vida do controle de uma requisição, a engine de template é simplesmente
uma ferramenta opcional. Rechamar o objetido de cada controller é retornar um objeto
``Response``. Templates são ferramentas poderosas, mas opcionais , para criar o 
conteúdo para o objeto ``Response``.

.. index::
   single: Directory Structure

A Estrutura de Diretório
-----------------------

Após umas poucas seções, você realmente entende a filosofia por trás da criação
e transformação de páginas no Symfony2. Você também começou a perceber como os projetos
do Symfony2 são estruturados e organizados. No final desta seção,
você saberá onde achar e colocar diferentes tipos de arquivos e o porquê.

Apesar de inteiramente flexível, por padrão, cada Symfony :term:`application` tem
a mesma estrutura básica e recomendada de estrutura de diretórios:

* ``app/``: Este diretório contém a configuração da aplicação;

* ``src/``: Todos os códigos do projeto PHP são armazenados neste diretório;

* ``vendor/``: Qualquer bibliotecas vendor são colocadas aqui por convenção;

* ``web/``: Esse é o diretório web raiz e contém qualquer arquivo acessível publicamente;

O Diretório Web
~~~~~~~~~~~~~~~~~

O diretório web raiz é a origem de todos os arquivos públicos e estáticos incluindo
imagens, folhas de estilo, e os arquivos Javascript. É lá também onde cada
:term:`front controller` fica::

    // web/app.php
    require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('prod', false);
    $kernel->loadClassCache();
    $kernel->handle(Request::createFromGlobals())->send();

O arquivo de front controller (``app.php`` neste examplo) é o verdadeiro arquivo PHP
que é executado quando usa a aplicação Symfony2 application e é o trabalho dele usar
uma classe Kernel, ``AppKernel``, para iniciar a aplicação.

.. tip::

    Ter um front controller significa URLs diferentes e mais flexíveis que as 
    usadas em uma aplicação PHP pura. Ao usar um front controller,
    URLs são formatadas da seguinte forma:

    .. code-block:: text

        http://localhost/app.php/hello/Ryan

    O front controller, ``app.php``, é executado e a "internal:" URL
    ``/hello/Ryan`` é roteada internamente usando a configuração de rota.
    Ao usar as regras ``mod_rewrite`` do Apache , você pode forçar o arquivo ``app.php`` 
    para ser executado sem a necessidade de especificá-la na URL:

    .. code-block:: text

        http://localhost/hello/Ryan

Apesar de front controllers serem essenciais em manipular cada requisição, você irá
raramente precisar modificar ou mesmo pensar neles. Nós iremos mencioná-los novamente
em breve na seção `Environments`_.

O Diretório de Aplicação (``app``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Como você viu no front controller, a classe ``AppKernel`` no ponto de entrada
principal da aplicação e é responsável por todas as configurações. Por padrão,
é armazenado no diretório ``app/``.

Essa classe deve implementar dois métodos que definem tudo que Symfony
precisa saber sobre sua aplicação. Você nem precisa mesmo se preocupar sobre
esses métodos ao iniciar - Symfony preenche eles para você com padrões coerentes.

* ``registerBundles()``: Retorna um vetor de todos os pacotes necessários para rodar a
  aplicação (see :ref:`page-creation-bundles`);

* ``registerContainerConfiguration()``: Carrega o arquivo de recurso da configuração 
   da aplicação principal (veja a seção `Application Configuration`_).

No desenvolvimento do dia-a-dia, você irá usar bastante o diretório ``app/`` para 
modificar arquivos de configuração e rotas no diretório ``app/config/`` (veja
`Application Configuration`_). Ele também contém o diretório de cache da aplicação
(``app/cache``), um diretório de log (``app/logs``) e um diretório
para arquivos de recursos de nível de aplicação, como os templates (``app/Resources``).
Você irá aprender mais sobre cada um desses diretórios nos últimos capítulos.

.. _autoloading-introduction-sidebar:

.. sidebar:: Autocarregamento

    Quando Symfony está carregando, um arquivo especial - ``app/autoload.php`` - é incluído.
    Esse arquivo é responsável por configurar o autoloader, que irá auto-carregar
    seus arquivos de aplicação do diretório ``src/`` e bibliotecas de terceiros do
    diretório ``vendor/``.

    Por causa do autoloader, você nunca irá precisar se preocupar sobre usar cláusulas ``include``
    ou ``require``. Ao invés disso, Symfony2 usa o namespace de uma classe para
    determinar sua localização e automaticamente incluir o arquivo no momento que você precisar da classe
    , para sua segurança.
    
    O autoloader é realmente configurado para verificar no diretório ``src/`` para
    qualquer uma de suas classes PHP. Para o autocarregamento funcionar, o nome da classe 
    e o atalho para o arquivo tem que seguir omesmo padrão:
    
    .. code-block:: text

        Class Name:
            Acme\HelloBundle\Controller\HelloController
        Path:
            src/Acme/HelloBundle/Controller/HelloController.php
    
    Tipicamente, o único momento que você precisará se preocupar sobre o 
    arquivo ``app/autoload.php`` é quando você estará incluindo uma nova biblioteca
    de terceiros no diretório ``vendor/``. Para mais informações no autocarregamento, veja
    :doc:`How to autoload Classes</cookbook/tools/autoloader>`.

O Diretório de Recursos (``src``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Coloque simplesmente, o diretório ``src/`` contém todo o código de verdade (código PHP,
templates, arquivos de configuração, folhas de estilo, etc) que comanda *sua* aplicação.
Enquanto estiver desenvolvendo, a vasta maioria de seu trabalho será feita dentro de um ou
mais pacotes que você cria nesse diretório.

Mas o que é exatamente um :term:`pacote`?

.. _page-creation-bundles:

O Sistema de Pacotes
-----------------

Um pacote é similar a um plugin em outro software, mas até mesmo melhor. A diferença-chave
é que *tudo* é um pacote no Symfony2, incluindo ambas as funcionalidades centrais do
framework e o código escrito para sua aplicação.
Pacotes são cidadãos de primeira classe no Symfony2. Isso dá a você a flexibilidade para
usar funcionalidades pré-montadas englobadas em `third-party bundles`_ ou para distribuir
seus próprios pacotes. Ele torna fácil para selecionar e escolher qual funcionalidade habilitar
em sua aplicação e para otimizá-los da forma que você quer.

.. note::

   Quando você for aprender as bases aqui, uma entrada inteira do livro de receitas é dedicado para
   a organização e as melhores práticas de:doc:`pacotes</cookbook/bundles/best_practices>`.

Um pacote é simplesmente um conjunto estruturado de arquivos dentro de um diretório que implementa
uma única funcionalidade. Você poderia criar um ``BlogBundle``, um ``ForumBundle`` ou
um pacote para gerenciamento de usuário (muitos desses realmente existem como pacote de código
aberto).
Cada diretório contém tudo relacionado àquela funcionalidade, incluindo arquivos PHP, templates,
folhas de estilo, Javascripts, testes e qualquer coisa mais.
Cada aspecto de uma funcionalidade existe em um pacote e cada funcionalidade fica dentro de 
um pacote.

Uma aplicação é feita por pacotes definidos no método ``registerBundles()``
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

Com o método ``registerBundles()`` , você tem controle total sobre quais pacotes são 
usados por sua aplicação (incluindo os pacotes centrais do Symfony).

.. tip::
    
    Um pacote pode residir *em qualquer lugar* desde que possa ser autocarregado (pelo
   autoloader configurado em ``app/autoload.php``).

Criando um Pacote
~~~~~~~~~~~~~~~~~

O Symfony Standard Edition vêm com uma tarefa conveniente que cria um pacote inteiramente funcional
para você. É claro, criar um pacote na mão é muito fácil também.

Para mostrar a você o quão simple o sistema de pacotes é, crie um novo pacote
chamado ``AcmeTestBundle`` e habilite ele.

.. tip::

    A porção ``Acme`` é só um nome bobo que poderia ser substituído por algum nome 
    de "vendedor" que represente você ou sua organização (ex: ``ABCTestBundle``
    para alguma empresa chamada ``ABC``).

Comece criando um diretório ``src/Acme/TestBundle/`` e adicionando um novo arquivo
chamado ``AcmeTestBundle.php``::

    // src/Acme/TestBundle/AcmeTestBundle.php
    namespace Acme\TestBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class AcmeTestBundle extends Bundle
    {
    }

.. tip::

   O nome``AcmeTestBundle`` segue o padrão :ref:`Convenção para nomes de pacotes<bundles-naming-conventions>`.
   Você poderia também escolher encurtar o nome do pacote para simplesmente ``TestBundle``
   ao nomear essa classe``TestBundle`` (e nomeando o arquivo ``TestBundle.php``).

Essa classe vazia é a única peça que você precisa para criar o novo pacote. Apesar de normalmente
vazia, essa classe é poderosa e pode ser usada para personalizar o comportamento do pacote.

Agora que você criou o pacote, abilite-o via classe ``AppKernel`` ::

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

E enquanto não fizer algo ainda,  ``AcmeTestBundle`` está pronto para
ser usado.

E tão simples quanto é, Symfony também providencia uma interface por linha de comando para
gerar um esqueleto de um pacote básico:

.. code-block:: bash

    php app/console generate:bundle --namespace=Acme/TestBundle

O esqueleto do pacote gera um controller básico, template e recurso para rota que
pode ser personalizado. Você irá aprender mais sobre ferramentas de linha de comando do
Symfony2 mais tarde.

.. tip::

   Quando sempre criar um novo pacote ou usar um pacote de terceiros, always make
   sure the bundle has been enabled in ``registerBundles()``. When using
   the ``generate:bundle`` command, this is done for you.

Estrutura de Diretório de Pacotes
~~~~~~~~~~~~~~~~~~~~~~~~~~

A estrutura de diretório de um pacote é simples e flexível. Por padrão, o sistema de pacote
segue um conjunto de convenções que ajudam a manter o código consistente entre todos
os pacotes do Symfony2. Dê uma olhada em ``AcmeHelloBundle``, pois contém 
alguns dos elementos mais comuns de um pacote:

* ``Controller/`` contém todos os controllers do pacote (ex: ``HelloController.php``);

* ``Resources/config/`` abriga configurações, incluindo configurações de rota
  (e.g. ``routing.yml``);

* ``Resources/views/`` contém templates organizados pelo nome do controller (ex:
  ``Hello/index.html.twig``);

* ``Resources/public/`` contém assets web (imagens, folhas de estilo, etc) e é
  copiada ou simbolicamente vinculada dentro do projeto ``web/`` directory via
  comando de console ``assets:install`` ;

* ``Tests/`` contém todos os testes para o pacote.

Um pacote pode ser pequeno ou grande conforme as funcionalidades que implementa. Contém
só os arquivos que você precisa e nada mais.

Conforme avançamos pelo livro, você aprenderá como persistir objetos para um banco de dados,
criar e validar formulários, criar traduções para sua aplicação, escrever testes e muito mais.
Cada um desses tem seu próprio local e papel dentro do pacote.

Configurações de aplicação
-------------------------

Uma aplicação consiste de uma coleção de pacotes representando todas as funcionalidades
e capacidades de sua aplicação. Cada pacote pode ser personalizado via arquivos de 
configuração escritos em  YAML, XML ou PHP. Por padrão, o arquivo de configuração 
padrão fica no diretório ``app/config/`` e é chamado ou ``config.yml``, ``config.xml`` 
ou ``config.php`` dependendo do formato que você preferir:

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

   Você irá aprender exatamento como carregar cada arquivo/formato na próxima seção
   `Environments`_.

Cada entrada de nível superior como ``framework`` ou ``twig`` define a configuração para
um pacote particular. Por exemplo,  a chave ``framework`` define a configuração para
o núcleo Symfony  ``FrameworkBundle`` e inclui configuração para as rotas, templates
e outros sistemas núcleo.

Neste momento, não se preocupe sobre uma opção de configuração específica em cada sessão.
O arquivo de configuração é encaminhado com padrões sensatos. Conforme você ler mais e
explorar cada parte de Symfony2, você irá aprender sobre a configuração específica
de cada funcionalidade.

.. sidebar:: Configuration Formats

	Através dos capítulos, todas as configurações de exemplo serão exibidas
	em todos os três formatos (YAML, XML e PHP). Cada um tem suas vantagens
	e desvantagens. A escolha de qual utilizar é sua:
	
    * *YAML*: Simples, claro e legível;

    * *XML*: Mais poderoso que YAML algumas vezes e suporta autocompletar da IDE;

    * *PHP*: Mais poderoso mas menos legível que os formatos de configuração padrão.

.. index::
   single: Environments; Introduction

.. _environments-summary:

Ambientes
------------

Uma aplicação pode executar em vários ambientes. Os ambientes diferentes
dividem o mesmo código PHP (independente do front controller), mas usam
configurção diferente. Por instância, um ambiente ``dev`` irá exibir alertas
e erros, enquanto um ambiente  ``prod`` irá somente logar erros. Alguns arquivos são
reconstruídos em cada solicitação no ambiente``dev`` (para conveniência do desenvolvedor),
mas armazenados no ambiente ``prod`` . Todos os ambientes convivem juntos na mesma
máquina e executam a mesma aplicação.

Um projeto Symfony2 geralmente começa com três ambientes (``dev``, ``test``
e ``prod``), apesar que criar novos ambientes é fácil. Você pode visualizar sua
aplicação em diferentes ambientes simplesmente mudando o front controller
no seu navegador. Para ver a aplicação no ambiente ``dev`` , accesse a 
aplicação com o front controller de desenvolvimento:

.. code-block:: text

    http://localhost/app_dev.php/hello/Ryan

Se você gostaria de visualizar como sua aplicação irá se comportar no ambiente de produção,
ao invés disto ,chame o front controller ``prod``:

.. code-block:: text

    http://localhost/app.php/hello/Ryan

Como o ambiente ``prod`` é otimizado para velocidade; a configuração, rota
e templates Twig são compilados em classes de PHP puro e armazenados.
Quando visualizar mudanças no ambiente ``prod``, você irá precisar limpar esses
arquivos armazenados e permitir reconstruir eles:

    php app/console cache:clear --env=prod --no-debug

.. note::

	Se você abrir  o arquivo ``web/app.php`` , você irá perceber que está explicitamente configurado 
   para utilizar o ambiente ``prod``::

       $kernel = new AppKernel('prod', false);
	
	Você pode criar um novo front controller para um novo ambiente ao copiar
	esse arquivo e mudando ``prod`` para algum outro valor.

.. note::
	
	O ambiente ``test`` é usado para executar testes automatizados e não pode ser acessado
	diretamente através do navegador. Veja :doc:`testing chapter</book/testing>`
    para mais detalhes.

.. index::
   single: Environments; Configuration

Configurações de ambiente
~~~~~~~~~~~~~~~~~~~~~~~~~

A classe ``AppKernel`` é responsável por carregar de verdade o arquivo de configuração
de sua escolha::

    // app/AppKernel.php
    public function registerContainerConfiguration(LoaderInterface $loader)
    {
        $loader->load(__DIR__.'/config/config_'.$this->getEnvironment().'.yml');
    }

Você realmente sabe que a extensão ``.yml`` pode ser mudada para ``.xml`` ou
``.php`` se você preferir usar ou XML ou PHP para escrever sua configuração.
Perceba também que cada ambiente carrega seu próprio arquivo de configuração. Considere
o arquivo de configuração para o ambiente ``dev``.

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

A chave ``imports`` é similar à sentença ``include`` do PHP e garante
que o arquivo de configuração principal (``config.yml``) seja carregado primeiro. O resto
do arquivo modifica a configuração padrão para registro de log melhorado e outras
configurações condizentes para um ambiente de produção.

Ambos os ambientes ``prod`` e ``test`` seguem o mesmo modelo: cada ambiente
importa o arquivo de configuração base e então modifica os valores de configuração
para ajustar as necessidades de um ambiente específico. Isso é apenas uma convenção,
mas uma que permite você a reutilizar a maior parte de suas configurações e personalizar
apenas pedaços do que está entre os ambientes.

Sumário
-------

Parabéns ! Você agora viu cada aspecto fundamental de Symfony2 e esperançosamente
descobriu o quão fácil e flexivel pode ser. E enquanto existem *um monte* de funcionalidades
por vir, esteja certo de manter os seguintes pontos em mente:

* criar uma página é um processo de três passos envolvendo uma **rota**, um **controller**
  e (opcionalmente) um **template**.

* cada projeto contém apenas alguns poucos diretórios: ``web/`` (web assets e
  o front controllers), ``app/`` (configuração), ``src/`` (seus pacotes),
  e ``vendor/`` (código de terceiros) (existe também um diretório ``bin/`` que é
  utilizado para ajudar com bibliotecas vendor atualizadas);

* Cada funcionalidade em Symfony2 (incluindo um núcleo do framework Symfony2) é organizado 
  em um *pacote*, que é um conjunto de arquivos estruturados para aquela funcionalidade;

* A **configuração** para cada pacote reside no diretório ``app/config`` e
  pode ser especificado em YAML, XML ou PHP;

* cada **ambiente** é acessível por meio de um front controller diferente (ex:
  ``app.php`` e ``app_dev.php``) e carrega uma configuração de arquivo diferente.

A partir daqui, cada capítulo irá introduzir a você ferramentas mais e mais poderosas
e conceitos avançados. Quanto mais souber sobre  Symfony2, mais irá apreciar a
flexibilidade da arquitetura dela e o poder que fornecerá a você
para desenvolber aplicações rapidamente.

.. _`Twig`: http://twig.sensiolabs.org
.. _`third-party bundles`: http://symfony2bundles.org/
.. _`Symfony Standard Edition`: http://symfony.com/download
