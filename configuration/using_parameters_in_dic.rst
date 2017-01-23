.. index::
    single: Usando parâmetros dentro de uma Classe de Injeção de Dependência

Usando Parâmetros dentro de uma Classe de Injeção de Dependência
----------------------------------------------------------------

Você já viu como usar os parâmetros de configuração nos
:ref:`contêineres de serviço do Symfony <book-service-container-parameters>`.
Há casos especiais, como quando você quer, por exemplo, usar o
parâmetro ``%kernel.debug%`` para fazer os serviços em seu bundle entrarem
no modo de depuração. Para esse caso, há mais trabalho a ser feito, a fim
de fazer com que o sistema compreenda o valor do parâmetro. Por padrão,
seu parâmetro ``%kernel.debug%`` será tratado como uma
string simples. Veja este exemplo com o AcmeDemoBundle::

    // Inside Configuration class
    $rootNode
        ->children()
            ->booleanNode('logging')->defaultValue('%kernel.debug%')->end()
            // ...
        ->end()
    ;

    // Inside the Extension class
    $config = $this->processConfiguration($configuration, $configs);
    var_dump($config['logging']);

Agora, examine os resultados para ver isso de perto:

.. configuration-block::

    .. code-block:: yaml

        my_bundle:
            logging: true
            # true, as expected

        my_bundle:
            logging: "%kernel.debug%"
            # true/false (depends on 2nd parameter of AppKernel),
            # as expected, because %kernel.debug% inside configuration
            # gets evaluated before being passed to the extension

        my_bundle: ~
        # passes the string "%kernel.debug%".
        # Which is always considered as true.
        # The Configurator does not know anything about
        # "%kernel.debug%" being a parameter.

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:my-bundle="http://example.org/schema/dic/my_bundle">

            <my-bundle:config logging="true" />
            <!-- true, as expected -->

             <my-bundle:config logging="%kernel.debug%" />
             <!-- true/false (depends on 2nd parameter of AppKernel),
                  as expected, because %kernel.debug% inside configuration
                  gets evaluated before being passed to the extension -->

            <my-bundle:config />
            <!-- passes the string "%kernel.debug%".
                 Which is always considered as true.
                 The Configurator does not know anything about
                 "%kernel.debug%" being a parameter. -->
        </container>

    .. code-block:: php

        $container->loadFromExtension('my_bundle', array(
                'logging' => true,
                // true, as expected
            )
        );

        $container->loadFromExtension('my_bundle', array(
                'logging' => "%kernel.debug%",
                // true/false (depends on 2nd parameter of AppKernel),
                // as expected, because %kernel.debug% inside configuration
                // gets evaluated before being passed to the extension
            )
        );

        $container->loadFromExtension('my_bundle');
        // passes the string "%kernel.debug%".
        // Which is always considered as true.
        // The Configurator does not know anything about
        // "%kernel.debug%" being a parameter.

A fim de suportar esse caso de uso, a classe ``Configuration`` tem que
ser injetada com esse parâmetro, através de extensão, da seguinte forma::

    namespace Acme\DemoBundle\DependencyInjection;

    use Symfony\Component\Config\Definition\Builder\ArrayNodeDefinition;
    use Symfony\Component\Config\Definition\Builder\TreeBuilder;
    use Symfony\Component\Config\Definition\ConfigurationInterface;

    class Configuration implements ConfigurationInterface
    {
        private $debug;

        public function  __construct($debug)
        {
            $this->debug = (Boolean) $debug;
        }

        public function getConfigTreeBuilder()
        {
            $treeBuilder = new TreeBuilder();
            $rootNode = $treeBuilder->root('acme_demo');

            $rootNode
                ->children()
                    // ...
                    ->booleanNode('logging')->defaultValue($this->debug)->end()
                    // ...
                ->end()
            ;

            return $treeBuilder;
        }
    }

E definir ele no construtor de ``Configuration`` através da classe ``Extension``::

    namespace Acme\DemoBundle\DependencyInjection;

    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\DependencyInjection\Loader\XmlFileLoader;
    use Symfony\Component\HttpKernel\DependencyInjection\Extension;
    use Symfony\Component\Config\FileLocator;

    class AcmeDemoExtension extends Extension
    {
        // ...

        public function getConfiguration(array $config, ContainerBuilder $container)
        {
            return new Configuration($container->getParameter('kernel.debug'));
        }
    }

.. sidebar:: Definir o Padrão na Extensão

    Existem algumas instâncias do ``%kernel.debug%`` dentro de uma classe ``Configurator``
    no TwigBundle e AsseticBundle, no entanto isso é porque o valor padrão do parâmetro 
    é definido pela classe Extension. Por exemplo, no AsseticBundle,
    você pode encontrar::

        $container->setParameter('assetic.debug', $config['debug']);

    A string ``%kernel.debug%``, passada aqui como um argumento, lida com o
    trabalho de interpretação para o contêiner, que, por sua vez, faz a avaliação.
    Ambas as formas atingem objetivos semelhantes. AsseticBundle não vai usar
    ``%kernel.debug%`` mas sim o novo parâmetro ``%assetic.debug%`.
