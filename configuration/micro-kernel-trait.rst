Construindo seu próprio Framework com o MicroKernelTrait
========================================================

Uma :ref:`aplicação Symfony tradicional <installation-creating-the-app>` contém uma
estrutura de diretório coerente, vários arquivos de configuração e um ``AppKernel`` com vários
bundles já registrados. É uma aplicação com vários recursos e está pronta para uso.

Mas você sabia que é possível criar uma aplicação Symfony totalmente funcional em apenas
um arquivo? Isso é possível graças ao novo
:class:`Symfony\\Bundle\\FrameworkBundle\\Kernel\\MicroKernelTrait`. Ele permite que
você comece com uma pequena aplicação, e, então, adicione recursos e estrutura conforme
você precisa.

A Aplicação Symfony de Único Arquivo
------------------------------------

Comece com um diretório completamente vazio. Obtenha ``symfony/symfony`` como uma dependência
via Composer:

.. code-block:: bash

    $ composer require symfony/symfony

Em seguida, crie um arquivo ``index.php`` que adiciona uma classe kernel e executa ela::

    use Symfony\Bundle\FrameworkBundle\Kernel\MicroKernelTrait;
    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpFoundation\JsonResponse;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpKernel\Kernel;
    use Symfony\Component\Routing\RouteCollectionBuilder;

    // require Composer's autoloader
    require __DIR__.'/vendor/autoload.php';

    class AppKernel extends Kernel
    {
        use MicroKernelTrait;

        public function registerBundles()
        {
            return array(
                new Symfony\Bundle\FrameworkBundle\FrameworkBundle()
            );
        }

        protected function configureContainer(ContainerBuilder $c, LoaderInterface $loader)
        {
            // PHP equivalent of config.yml
            $c->loadFromExtension('framework', array(
                'secret' => 'S0ME_SECRET'
            ));
        }

        protected function configureRoutes(RouteCollectionBuilder $routes)
        {
            // kernel is a service that points to this class
            // optional 3rd argument is the route name
            $routes->add('/random/{limit}', 'kernel:randomAction');
        }

        public function randomAction($limit)
        {
            return new JsonResponse(array(
                'number' => rand(0, $limit)
            ));
        }
    }

    $kernel = new AppKernel('dev', true);
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);
    $response->send();
    $kernel->terminate($request, $response);

É isso! Para testar, você pode iniciar o servidor web embutido:

.. code-block:: bash

    $ php -S localhost:8000

Então veja a resposta JSON no seu navegador:

> http://localhost:8000/random/10

Os Métodos de um "Micro" Kernel
-------------------------------

Quando você usa o ``MicroKernelTrait``, seu kernel precisa ter exatamente três métodos
que definem seus bundles, seus serviços e suas rotas:

**registerBundles()**
    Este é o mesmo ``registerBundles()`` que você vê em um kernel normal.

**configureContainer(ContainerBuilder $c, LoaderInterface $loader)**
    Este método constrói e configura o container. Na prática, você irá usar
    ``loadFromExtension`` para configurar bundles diferentes (é o equivalente
    ao que você vê em um arquivo ``config.yml`` normal). Você também pode registrar serviços
    diretamente em PHP ou carregar arquivos de configuração externos (veja abaixo).

**configureRoutes(RouteCollectionBuilder $routes)**
    Seu trabalho neste método é adicionar rotas para a aplicação. O ``RouteCollectionBuilder``
    é novo no Symfony 2.8 e possui métodos que tornam a adição de rotas em PHP mais divertida.
    Você também pode carregar arquivos de roteamento externos (veja abaixo).


Exemplo Avançado: Twig, Anotações e a Barra de Ferramentas Web para Depuração
-----------------------------------------------------------------------------

O objetivo do `` MicroKernelTrait`` *não* é ter uma aplicação de um arquivo único.
Em vez disso, seu objetivo é fornecer o poder para você escolher seus bundles e sua estrutura.

Primeiro, você provavelmente desejará colocar suas classes PHP em um diretório ``src/``. Configure
seu arquivo ``composer.json`` para carregar a partir daí:

.. code-block:: json

    {
        "require": {
            "...": "..."
        },
        "autoload": {
            "psr-4": {
                "": "src/"
            }
        }
    }

Agora, suponha que você queira usar o Twig e carregar as rotas através de anotações. Para usar rotas via
anotação, você precisa do SensioFrameworkExtraBundle. Ele vem com um projeto Symfony normal.
Mas, nesse caso, você precisa fazer o download:

.. code-block:: bash

    $ composer require sensio/framework-extra-bundle

Em vez de colocar *tudo* no arquivo ``index.php``, crie um novo ``app/AppKernel.php``
para armazenar o kernel. Agora, ele parece com o seguinte::

    // app/AppKernel.php

    use Symfony\Bundle\FrameworkBundle\Kernel\MicroKernelTrait;
    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Kernel;
    use Symfony\Component\Routing\RouteCollectionBuilder;
    use Doctrine\Common\Annotations\AnnotationRegistry;

    // require Composer's autoloader
    $loader = require __DIR__.'/../vendor/autoload.php';
    // auto-load annotations
    AnnotationRegistry::registerLoader(array($loader, 'loadClass'));

    class AppKernel extends Kernel
    {
        use MicroKernelTrait;

        public function registerBundles()
        {
            $bundles = array(
                new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
                new Symfony\Bundle\TwigBundle\TwigBundle(),
                new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle()
            );

            if ($this->getEnvironment() == 'dev') {
                $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            }

            return $bundles;
        }

        protected function configureContainer(ContainerBuilder $c, LoaderInterface $loader)
        {
            $loader->load(__DIR__.'/config/config.yml');

            // configure WebProfilerBundle only if the bundle is enabled
            if (isset($this->bundles['WebProfilerBundle'])) {
                $c->loadFromExtension('web_profiler', array(
                    'toolbar' => true,
                    'intercept_redirects' => false,
                ));
            }
        }

        protected function configureRoutes(RouteCollectionBuilder $routes)
        {
            // import the WebProfilerRoutes, only if the bundle is enabled
            if (isset($this->bundles['WebProfilerBundle'])) {
                $routes->mount('/_wdt', $routes->import('@WebProfilerBundle/Resources/config/routing/wdt.xml'));
                $routes->mount('/_profiler', $routes->import('@WebProfilerBundle/Resources/config/routing/profiler.xml'));
            }

            // load the annotation routes
            $routes->mount(
                '/',
                $routes->import(__DIR__.'/../src/App/Controller/', 'annotation')
            );
        }
    }

Ao contrário do kernel anterior, esse carrega um arquivo ``app/config/config.yml`` externo,
porque a configuração começou a ficar maior:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            secret: S0ME_SECRET
            templating:
                engines: ['twig']
            profiler: { only_exceptions: false }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config secret="S0ME_SECRET">
                <framework:templating>
                    <framework:engine>twig</framework:engine>
                </framework:templating>
                <framework:profiler only-exceptions="false" />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'secret' => 'S0ME_SECRET',
            'templating' => array(
                'engines' => array('twig'),
            ),
            'profiler' => array(
                'only_exceptions' => false,
            ),
        ));

Ele também carrega as rotas de anotação de um diretório ``src/App/Controller/``, que
tem um arquivo nele::

    // src/App/Controller/MicroController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

    class MicroController extends Controller
    {
        /**
         * @Route("/random/{limit}")
         */
        public function randomAction($limit)
        {
            $number = rand(0, $limit);

            return $this->render('micro/random.html.twig', array(
                'number' => $number
            ));
        }
    }

Os arquivos de template devem ficar no diretório ``Resources/views`` de qualquer diretório
onde seu kernel *reside*. Uma vez que o ``AppKernel`` está em ``app/``, esse template localiza-se
em ``app/Resources/views/micro/random.html.twig``.

Finalmente, você precisa de um front controller para inicializar e executar a aplicação. Crie um
``web/index.php``:

    // web/index.php

    use Symfony\Component\HttpFoundation\Request;

    require __DIR__.'/../app/AppKernel.php';

    $kernel = new AppKernel('dev', true);
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);
    $response->send();
    $kernel->terminate($request, $response);

É isso! A URL ``/random/10`` vai funcionar, o Twig vai renderizar, e você ainda vai
ver a barra de ferramentas web para depuração aparecer na parte inferior. A estrutura final parecerá com
a seguinte:

.. code-block:: text

    your-project/
    ├─ app/
    |  ├─ AppKernel.php
    │  ├─ cache/
    │  ├─ config/
    │  ├─ logs/
    │  └─ Resources
    |     └─ views
    |        ├─ base.html.twig
    |        └─ micro
    |           └─ random.html.twig
    ├─ src/
    │  └─ App
    |     └─ Controller
    |        └─ MicroController.php
    ├─ vendor/
    │  └─ ...
    ├─ web/
    |  └─ index.php
    ├─ composer.json
    └─ composer.lock

Ei, isso parece muito com uma aplicação Symfony *tradicional*! Você está certo: o
``MicroKernelTrait`` ainda *é* Symfony: mas você pode controlar a estrutura e
recursos com muita facilidade.
