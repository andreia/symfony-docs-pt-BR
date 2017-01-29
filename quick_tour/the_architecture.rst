A Arquitetura
=============

Você é meu herói! Quem imaginaria que você ainda estaria aqui após as
três primeiras partes? Seus esforços serão bem recompensados ​​em breve. As três 
primeiras partes não contemplaram profundamente a arquitetura do framework. 
Porque ela faz o Symfony destacar-se na multidão de frameworks, vamos mergulhar 
na arquitetura agora.

Compreendendo a estrutura de diretório
--------------------------------------

A estrutura de diretório de uma :term:`aplicação` do Symfony é bastante flexível,
mas a estrutura do diretório da distribuição *Standard Edition* reflete
a estrutura típica e recomendada de uma aplicação Symfony:

* ``app/``: A configuração da aplicação;
* ``src/``: O código PHP do projeto;
* ``vendor/``: As dependências de terceiros;
* ``web/``: O diretório raiz web.

O Diretório ``web/``
~~~~~~~~~~~~~~~~~~~~

O diretório raiz web é o local de todos os arquivos públicos e estáticos, como imagens,
folhas de estilo e arquivos JavaScript. É também o local onde cada :term:`front controller`
reside::

    // web/app.php
    require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('prod', false);
    $kernel->loadClassCache();
    $kernel->handle(Request::createFromGlobals())->send();

O kernel primeiro solicita o arquivo ``bootstrap.php.cache``, que inicializa
a estrutura e regista o autoloader (veja abaixo).

Como qualquer front controller, o ``app.php`` usa uma classe Kernel, ``AppKernel``, 
para a inicialização da aplicação.

.. _the-app-dir:

O Diretório ``app/``
~~~~~~~~~~~~~~~~~~~~

A classe ``AppKernel`` é o principal ponto de entrada da configuração da aplicação
e, como tal, ele é armazenado no diretório ``app/``.

Essa classe deve implementar dois métodos:

* ``registerBundles()`` que deve retornar um array de todos os bundles necessários para 
  executar a aplicação.

* ``registerContainerConfiguration()`` que carrega a configuração da aplicação
  (veremos mais sobre isso depois).

O autoloading do PHP pode ser configurado via ``app/autoload.php``::

    // app/autoload.php
    use Symfony\Component\ClassLoader\UniversalClassLoader;

    $loader = new UniversalClassLoader();
    $loader->registerNamespaces(array(
        'Symfony'          => array(__DIR__.'/../vendor/symfony/symfony/src', __DIR__.'/../vendor/bundles'),
        'Sensio'           => __DIR__.'/../vendor/bundles',
        'JMS'              => __DIR__.'/../vendor/jms/',
        'Doctrine\\Common' => __DIR__.'/../vendor/doctrine/common/lib',
        'Doctrine\\DBAL'   => __DIR__.'/../vendor/doctrine/dbal/lib',
        'Doctrine'         => __DIR__.'/../vendor/doctrine/orm/lib',
        'Monolog'          => __DIR__.'/../vendor/monolog/monolog/src',
        'Assetic'          => __DIR__.'/../vendor/kriswallsmith/assetic/src',
        'Metadata'         => __DIR__.'/../vendor/jms/metadata/src',
    ));
    $loader->registerPrefixes(array(
        'Twig_Extensions_' => __DIR__.'/../vendor/twig/extensions/lib',
        'Twig_'            => __DIR__.'/../vendor/twig/twig/lib',
    ));

    // ...

    $loader->registerNamespaceFallbacks(array(
        __DIR__.'/../src',
    ));
    $loader->register();

O :class:`Symfony\\Component\\ClassLoader\\UniversalClassLoader` é usado para
fazer o autoload dos arquivos que respeitam as `normas`_ técnicas de interoperabilidade
para namespaces do PHP 5.3 ou a `convenção`_ de nomenclatura para classes do PEAR. Como você
pode ver aqui, todas as dependências são armazenadas sob o diretório ``vendor/``, mas
isso é apenas uma convenção. Você pode armazená-las onde quiser, globalmente em
seu servidor ou localmente em seus projetos.

.. note::

    Se você quiser saber mais sobre a flexibilidade do autoloader 
    do Symfony, leia o capítulo ":doc:`/components/class_loader`".

Compreendendo o Sistema dos Bundles
-----------------------------------

Esta seção apresenta um dos maiores e mais poderosos recursos do Symfony, 
o sistema de :term:`bundle`.

Um bundle é como um plugin em outro software. Então por que ele é chamado de
*bundle* de não de *plugin*? Porque *tudo* é um bundle no Symfony, desde as 
funcionalidades do núcleo do framework até o código que você escreve para a sua
aplicação. Os bundles são cidadãos de primeira classe no Symfony. Isso lhe fornece
a flexibilidade de usar funcionalidades pré-construídas que vêm em bundles de terceiros
ou distribuir os seus próprios bundles. Isso torna mais fácil a tarefa de escolher quais
recursos que serão habilitados na sua aplicação e otimizá-los da maneira que desejar.
E, no final do dia, o código da sua aplicação é tão *importante* quanto
o próprio framework.

Registrando um Bundle
~~~~~~~~~~~~~~~~~~~~~

Uma aplicação é composta de bundles, que foram definidos no método ``registerBundles()``
da classe ``AppKernel``. Cada bundle é um diretório que contém uma única classe 
``Bundle`` que descreve ele::

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

Além do ``AcmeDemoBundle`` que nós já falamos, observe que o kernel 
também habilita outros bundles, como o ``FrameworkBundle``, 
``DoctrineBundle``, ``SwiftmailerBundle`` e o ``AsseticBundle``.
Todos eles fazem parte do framework.

Configurando um Bundle
~~~~~~~~~~~~~~~~~~~~~~

Cada bundle pode ser personalizado através dos arquivos de configuração escritos em YAML, XML 
ou PHP. Esta é a configuração padrão:

.. code-block:: yaml

    # app/config/config.yml
    imports:
        - { resource: parameters.yml }
        - { resource: security.yml }

    framework:
        #esi:             ~
        #translator:      { fallback: "%locale%" }
        secret:          "%secret%"
        router:          { resource: "%kernel.root_dir%/config/routing.yml" }
        form:            true
        csrf_protection: true
        validation:      { enable_annotations: true }
        templating:      { engines: ['twig'] } #assets_version: SomeVersionScheme
        default_locale:  "%locale%"
        session:
            auto_start:     true

    # Twig Configuration
    twig:
        debug:            "%kernel.debug%"
        strict_variables: "%kernel.debug%"

    # Assetic Configuration
    assetic:
        debug:          "%kernel.debug%"
        use_controller: false
        bundles:        [ ]
        # java: /usr/bin/java
        filters:
            cssrewrite: ~
            # closure:
            #     jar: "%kernel.root_dir%/java/compiler.jar"
            # yui_css:
            #     jar: "%kernel.root_dir%/java/yuicompressor-2.4.2.jar"

    # Doctrine Configuration
    doctrine:
        dbal:
            driver:   "%database_driver%"
            host:     "%database_host%"
            port:     "%database_port%"
            dbname:   "%database_name%"
            user:     "%database_user%"
            password: "%database_password%"
            charset:  UTF8

        orm:
            auto_generate_proxy_classes: "%kernel.debug%"
            auto_mapping: true

    # Swiftmailer Configuration
    swiftmailer:
        transport: "%mailer_transport%"
        host:      "%mailer_host%"
        username:  "%mailer_user%"
        password:  "%mailer_password%"

    jms_security_extra:
        secure_controllers:  true
        secure_all_services: false

Cada entrada como ``framework`` define a configuração para um bundle específico.
Por exemplo, ``framework`` configura o ``FrameworkBundle`` enquanto ``swiftmailer``
configura o ``SwiftmailerBundle``.

Cada :term:`ambiente` pode substituir a configuração padrão, ao fornecer um
arquivo de configuração específico. Por exemplo, o ambiente ``dev`` carrega o
arquivo ``config_dev.yml``, que carrega a configuração principal (ou seja, ``config.yml``)
e, então, modifica ela para adicionar algumas ferramentas de depuração:

.. code-block:: yaml

    # app/config/config_dev.yml
    imports:
        - { resource: config.yml }

    framework:
        router:   { resource: "%kernel.root_dir%/config/routing_dev.yml" }
        profiler: { only_exceptions: false }

    web_profiler:
        toolbar: true
        intercept_redirects: false

    monolog:
        handlers:
            main:
                type:  stream
                path:  "%kernel.logs_dir%/%kernel.environment%.log"
                level: debug
            firephp:
                type:  firephp
                level: info

    assetic:
        use_controller: true

Estendendo um Bundle
~~~~~~~~~~~~~~~~~~~~

Além de ser uma boa forma de organizar e configurar seu código, um bundle pode estender 
um outro bundle. A herança do bundle permite substituir qualquer bundle existente
a fim de personalizar seus controladores, templates ou qualquer um de seus arquivos.
Aqui é o onde os nomes lógicos (por exemplo, ``@AcmeDemoBundle/Controller/SecuredController.php``)
são úteis: eles abstraem onde o recurso é realmente armazenado.

Nomes Lógicos de Arquivos
.........................

Quando você quer fazer referência à um arquivo de um bundle, use esta notação:
``@BUNDLE_NAME/path/to/file``; o Symfony irá resolver ``@BUNDLE_NAME``
para o caminho real do bundle. Por exemplo, o caminho lógico
``@AcmeDemoBundle/Controller/DemoController.php`` seria convertido para
``src/Acme/DemoBundle/Controller/DemoController.php``, pois o Symfony conhece
a localização do ``AcmeDemoBundle``.

Nomes Lógicos de Controladores
..............................

Para os controladores, você precisa referenciar os nomes de métodos usando o formato
``BUNDLE_NAME:CONTROLLER_NAME:ACTION_NAME``. Por exemplo,
``AcmeDemoBundle:Welcome:index`` mapeia para o método ``indexAction`` da classe 
``Acme\DemoBundle\Controller\WelcomeController``.

Nomes Lógicos de Templates
..........................

Para os templates, o nome lógico ``AcmeDemoBundle:Welcome:index.html.twig`` é convertido 
para o caminho de arquivo ``src/Acme/DemoBundle/Resources/views/Welcome/index.html.twig``.
Os templates tornam-se ainda mais interessantes quando você percebe que eles não precisam ser
armazenados no sistema de arquivos. Você pode facilmente armazená-los em uma tabela do banco de 
dados, por exemplo.

Estendendo Bundles
..................

Se você seguir estas convenções, então você pode usar :doc:`bundle inheritance</cookbook/bundles/inheritance>`
para "sobrescrever" os arquivos, controladores ou templates. Por exemplo, você pode criar
um bundle - ``AcmeNewBundle`` - e especificar que ele sobrescreve o ``AcmeDemoBundle``.
Quando o Symfony carregar o controlador ``AcmeDemoBundle:Welcome:index``, ele irá 
primeiro verificar a classe ``WelcomeController`` em ``AcmeNewBundle`` e, se 
ela não existir, então irá verificar o ``AcmeDemoBundle``. Isto significa que um bundle 
pode sobrescrever quase qualquer parte de outro bundle!

Você entende agora porque o Symfony é tão flexível? Compartilhe os seus bundles entre
aplicações, armazene-os localmente ou globalmente, a escolha é sua.

.. _using-vendors:

Usando os Vendors
-----------------

São grandes as probabilidades de que a sua aplicação dependerá de bibliotecas de terceiros. 
Estas devem ser armazenadas no diretório ``vendor/``. Este diretório já contém
as bibliotecas do Symfony, a biblioteca do SwiftMailer, o ORM Doctrine, o sistema de 
template Twig e algumas outras bibliotecas e bundles de terceiros.

Entendendo o Cache e Logs
-------------------------

O Symfony é provavelmente um dos mais rápidos frameworks full-stack atualmente. Mas como
pode ser tão rápido se ele analisa e interpreta dezenas de arquivos YAML e XML para
cada pedido? A velocidade é, em parte, devido ao seu sistema de cache. A configuração
da aplicação é analisada somente no primeiro pedido e depois compilada em código PHP 
comum, que é armazenado no diretório ``app/cache/``. No ambiente de desenvolvimento, 
o Symfony é inteligente o suficiente para liberar o cache quando você altera um
arquivo. Mas, no ambiente de produção, é sua a responsabilidade de limpar
o cache quando você atualizar o seu código ou alterar sua configuração.

Ao desenvolver uma aplicação web, as coisas podem dar errado em muitos aspectos. 
Os arquivos de log no diretório ``app/logs/`` dizem tudo sobre os pedidos
e ajudam a resolver os problemas rapidamente.

Utilizando a Interface da Linha de Comando
------------------------------------------

Cada aplicação vem com uma ferramenta de interface de linha de comando (``app/console``)
que ajuda na manutenção da sua aplicação. Ela fornece comandos que aumentam a sua
produtividade ao automatizar tarefas tediosas e repetitivas.

Execute-a sem argumentos para saber mais sobre suas capacidades:

.. code-block:: bash

    $ php app/console

A opção ``--help`` ajuda a descobrir o uso de um comando:

.. code-block:: bash

    $ php app/console router:debug --help

Considerações finais
--------------------

Me chame de louco, mas, depois de ler esta parte, você deve estar confortável em
mover as coisas e fazer o Symfony trabalhar para você. Tudo no Symfony é projetado 
para sair do seu caminho. Portanto, sinta-se livre para renomear e mover os diretórios
como você desejar.

E isso é tudo para o início rápido. Desde testes até o envio de e-mails, você ainda
precisa aprender muito para se tornar um mestre no Symfony. Pronto para aprofundar nestes
tópicos agora? Não procure mais - vá para o :doc:`/book/index` oficial e escolha 
qualquer tema que você desejar.

.. _normas: http://symfony.com/PSR0
.. _convenção: http://pear.php.net/
