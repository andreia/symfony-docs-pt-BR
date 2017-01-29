.. index::
   single: Configuração; Semântica
   single: Bundle; Configuração de Extensão

Como expor uma Configuração Semântica para um Bundle
====================================================

Se você abrir o arquivo de configuração da sua aplicação (geralmente ``app/config/config.yml``),
verá um número de diferentes "namespaces" de configurações, como ``framework``,
``twig`` e ``doctrine``. Cada um deles configura um bundle específico, permitindo configurar 
as coisas a um alto nível e deixar o bundle fazer todas as modificações complexas, 
de baixo nível resultantes.

Por exemplo, o código a seguir diz ao ``FrameworkBundle`` para habilitar a integração do 
formulário, a qual envolve a definição de alguns serviços, bem
como a integração de outros componentes relacionados:

.. configuration-block::

    .. code-block:: yaml

        framework:
            # ...
            form:            true

    .. code-block:: xml

        <framework:config>
            <framework:form />
        </framework:config>

    .. code-block:: php

        $container->loadFromExtension('framework', array(
            // ...
            'form'            => true,
            // ...
        ));

Quando você cria um bundle, você tem duas opções para lidar com a configuração:

1. **Configuração de Serviço Normal** (*fácil*):

    Você pode especificar seus serviços em um arquivo de configuração (por exemplo ``services.yml``)
    que reside em seu bundle e, então, importá-lo a partir da configuração principal da 
    sua aplicação. Isto é realmente fácil, rápido e totalmente eficaz. Se você
    fazer uso de :ref:`parâmetros<book-service-container-parameters>`, então, 
    você ainda tem a flexibilidade para personalizar seu bundle a partir da configuração 
    da sua aplicação. Veja ":ref:`service-container-imports-directive`" para mais
    detalhes.

2. **Expondo Configuração Semântica** (*avançado*):

    Esta é a maneira como a configuração é feita com os bundles do núcleo (como descrito
    acima). A idéia básica é que, em vez de o usuário sobrescrever parâmetros
    individuais, você deixa ele configurar apenas algumas opções criadas 
    . Como desenvolvedor do bundle, você então faz o parse desta configuração
    e carrega os serviços dentro de uma classe "Extension". Com este método, você não vai
    precisar importar quaisquer recursos de configuração a partir da configuração principal da 
    sua aplicação: a classe de Extensão (Extension) pode lidar com tudo isso.

A segunda opção - que você vai aprender neste artigo - é muito mais flexível, mas 
também requer mais tempo para configurar. Se você está se perguntando qual
método deve usar, provavelmente é uma boa idéia começar com o método 1,
e depois mudar para o 2 mais tarde, se você precisar.

O segundo método tem várias vantagens específicas:

* Muito mais poderoso do que simplesmente definir parâmetros: um valor de opção específico
  pode acionar a criação de muitas definições de serviço;

* Possibilidade de ter hierarquia de configuração

* Smart merging quando possuir vários arquivos de configuração (por exemplo ``config_dev.yml``
  e ``config.yml``) sobrescrevendo a configuração um do outro;

* Validação de configuração (se você usar uma :ref:`Classe de Configuração<cookbook-bundles-extension-config-class>`);

* Auto-completar da IDE quando você criar um XSD e os desenvolvedores usarem XML.

.. sidebar:: Sobresecrevendo parâmetros do bundle 

    Se um Bundle fornecer uma classe Extension, então, você geralmente *não* deve 
    sobrescrever quaisquer parâmetros do container de serviço daquele bundle. A idéia
    é que, se uma classe Extension estiver presente, cada definição que deve ser
    configurável deve estar presente na configuração disponibilizada por
    esta classe. Em outras palavras, a classe Extension as definições de configuração
    públicas suportadas para as quais a compatibilidade com versões anteriores 
    será mantida.

.. index::
   single: Bundle; Extensão
   single: Injeção de Dependência; Extensão

Criando uma Classe Extension
----------------------------

Se você optar por expor uma configuração semântica para seu bundle, você vai
precisar primeiro criar uma nova classe "Extension", que irá lidar com o processo.
Esta classe deve residir no diretório ``DependencyInjection`` de seu bundle
e o seu nome deve ser construído substituindo o sufixo ``Bundle`` do
nome da classe Bundle por ``Extension``. Por exemplo, a classe Extension
do ``AcmeHelloBundle`` seria chamada ``AcmeHelloExtension``::

    // Acme/HelloBundle/DependencyInjection/AcmeHelloExtension.php
    namespace Acme\HelloBundle\DependencyInjection;

    use Symfony\Component\HttpKernel\DependencyInjection\Extension;
    use Symfony\Component\DependencyInjection\ContainerBuilder;

    class AcmeHelloExtension extends Extension
    {
        public function load(array $configs, ContainerBuilder $container)
        {
            // ... where all of the heavy logic is done
        }

        public function getXsdValidationBasePath()
        {
            return __DIR__.'/../Resources/config/';
        }

        public function getNamespace()
        {
            return 'http://www.example.com/symfony/schema/';
        }
    }

.. note::

    Os métodos ``getXsdValidationBasePath`` e ``getNamespace`` são necessários apenas
    se o bundle fornece XSDs opcionais para a configuração.

A presença da classe anterior significa que agora você pode definir um namespace de configuração
``acme_hello`` em qualquer arquivo de configuração. O namespace ``acme_hello``
é construído a partir do nome da classe de extensão, removendo a palavra ``Extension``
e, em seguida, deixando o resto do nome todo em letras minúsculas e com underscores. Em outras
palavras, ``AcmeHelloExtension`` torna-se ``acme_hello``.

Você pode começar a especificar a configuração sob este namespace imediatamente:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        acme_hello: ~

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_hello="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://www.example.com/symfony/schema/ http://www.example.com/symfony/schema/hello-1.0.xsd">

           <acme_hello:config />

           <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('acme_hello', array());

.. tip::

    Se você seguir as convenções de nomenclatura estabelecidas acima, então, o método
    ``load()`` de seu código de extensão é sempre chamado, uma vez que seu bundle
    está registrado no Kernel. Em outras palavras, mesmo se o usuário não
    fornecer qualquer configuração (ou seja, a entrada ``acme_hello`` nem mesmo
    aparecer), o método ``load()`` será chamado e passado um array ``$configs`` 
    vazio. Você ainda pode fornecer alguns padrões ​​para seu bundle se
    desejar.

Fazendo o parse do array ``$configs``
-------------------------------------

Sempre que um usuário inclui o namespace ``acme_hello`` em um arquivo de configuração,
a configuração abaixo dele é adicionada à um array de configurações e
passado para o método ``load()`` de sua extensão (o Symfony2 automaticamente
converte XML e YAML para um array).

Assuma a seguinte configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        acme_hello:
            foo: fooValue
            bar: barValue

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_hello="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://www.example.com/symfony/schema/ http://www.example.com/symfony/schema/hello-1.0.xsd">

            <acme_hello:config foo="fooValue">
                <acme_hello:bar>barValue</acme_hello:bar>
            </acme_hello:config>

        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('acme_hello', array(
            'foo' => 'fooValue',
            'bar' => 'barValue',
        ));

O array passado para seu método ``load()`` ficará parecido com o seguinte::

    array(
        array(
            'foo' => 'fooValue',
            'bar' => 'barValue',
        )
    )

Observe que este é um *array de arrays*, e não apenas um único array simples de
valores de configuração. Isso é intencional. Por exemplo, se ``acme_hello`` aparece em 
outro arquivo de configuração - digamos ``config_dev.yml`` - com valores diferentes
abaixo dele, então, o array de entrada poderia ser assim::

    array(
        array(
            'foo' => 'fooValue',
            'bar' => 'barValue',
        ),
        array(
            'foo' => 'fooDevValue',
            'baz' => 'newConfigEntry',
        ),
    )

A ordem dos dois arrays depende de qual é definido primeiro.

É o seu trabalho, então, decidir como deve ser o merge dessas configurações 
. Você pode, por exemplo, ter valores posteriores sobrescrevendo os valores anteriores
ou de alguma forma fazer o merge deles.

Mais tarde, na seção sobre a :ref:`Classe de Configuração<cookbook-bundles-extension-config-class>`
, você vai aprender uma forma verdadeiramente robusta para lidar com isso. Mas, por ora,
você pode apenas fazer o merge manualmente::

    public function load(array $configs, ContainerBuilder $container)
    {
        $config = array();
        foreach ($configs as $subConfig) {
            $config = array_merge($config, $subConfig);
        }

        // ... now use the flat $config array
    }

.. caution::

    Certifique-se que a técnica de merge acima faz sentido para o seu bundle. Este
    é apenas um exemplo, e você deve ter cuidado para não usá-lo cegamente.

Usando o Método ``load()``
--------------------------

Dentro do ``load()`` a variável ``$container`` refere-se a um container que apenas
sabe sobre essa configuração de namespace (ou seja, não contêm informação de serviço
carregada a partir de outros bundles). O objetivo do método ``load()``
é manipular o container, adicionando e configurando quaisquer métodos ou serviços
necessários ao seu bundle.

Carregando Recursos de Configuração Externos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Algo comum de se fazer é carregar um arquivo de configuração externo que pode conter a 
maioria dos serviços necessários para o seu bundle. Por exemplo, suponha que você tem 
um arquivo ``services.xml`` que contém muitas das configurações de serviços do seu 
bundle::

    use Symfony\Component\DependencyInjection\Loader\XmlFileLoader;
    use Symfony\Component\Config\FileLocator;

    public function load(array $configs, ContainerBuilder $container)
    {
        // ... prepare your $config variable

        $loader = new XmlFileLoader($container, new FileLocator(__DIR__.'/../Resources/config'));
        $loader->load('services.xml');
    }

Você pode até fazer isso condicionalmente, com base em um dos valores de configuração.
Por exemplo, supondo que você quer carregar um conjunto de serviços somente se a opção 
``enabled`` for passada e definida com true::

    public function load(array $configs, ContainerBuilder $container)
    {
        // ... prepare your $config variable

        $loader = new XmlFileLoader($container, new FileLocator(__DIR__.'/../Resources/config'));

        if (isset($config['enabled']) && $config['enabled']) {
            $loader->load('services.xml');
        }
    }

Configurando Serviços e Definindo Parâmetros
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Uma vez que você já carregou alguma configuração de serviço, você pode precisar modificar a
configuração com base em alguns dos valores de entrada. Por exemplo, supondo que existe um 
serviço cujo primeiro argumento é alguma string "type" que ele irá usar internamente. 
Você gostaria que isto fosse facilmente configurado pelo usuário do bundle, 
então, em seu arquivo de configuração do serviço (ex. ``services.xml``), você define este
serviço e usa um parâmetro em branco - ``acme_hello.my_service_type`` - como
seu primeiro argumento:

.. code-block:: xml

    <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
    <container xmlns="http://symfony.com/schema/dic/services"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

        <parameters>
            <parameter key="acme_hello.my_service_type" />
        </parameters>

        <services>
            <service id="acme_hello.my_service" class="Acme\HelloBundle\MyService">
                <argument>%acme_hello.my_service_type%</argument>
            </service>
        </services>
    </container>

Mas por que definir um parâmetro vazio e então passá-lo ao seu serviço?
A resposta é que você vai definir este parâmetro em sua classe de extensão, com base
nos valores de configuração de entrada. Suponha, por exemplo, que você quer
permitir ao usuário definir esta opção *type* sob uma chave chamada ``my_type``.
Para fazer isso, adicione o seguinte ao método ``load()``::

    public function load(array $configs, ContainerBuilder $container)
    {
        // ... prepare your $config variable

        $loader = new XmlFileLoader($container, new FileLocator(__DIR__.'/../Resources/config'));
        $loader->load('services.xml');

        if (!isset($config['my_type'])) {
            throw new \InvalidArgumentException('The "my_type" option must be set');
        }

        $container->setParameter('acme_hello.my_service_type', $config['my_type']);
    }

Agora, o usuário pode efetivamente configurar o serviço especificando o valor
de configuração ``my_type``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        acme_hello:
            my_type: foo
            # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_hello="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://www.example.com/symfony/schema/ http://www.example.com/symfony/schema/hello-1.0.xsd">

            <acme_hello:config my_type="foo">
                <!-- ... -->
            </acme_hello:config>

        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('acme_hello', array(
            'my_type' => 'foo',
            // ...
        ));

Parâmetros Globais
~~~~~~~~~~~~~~~~~~

Quando estiver configurando o container, esteja ciente de que você tem os seguintes
parâmetros globais disponíveis para uso:

* ``kernel.name``
* ``kernel.environment``
* ``kernel.debug``
* ``kernel.root_dir``
* ``kernel.cache_dir``
* ``kernel.logs_dir``
* ``kernel.bundle_dirs``
* ``kernel.bundles``
* ``kernel.charset``

.. caution::

    Todos os nomes de parâmetros e serviços, começando com um ``_`` são reservados para o
    framework, e os novos não devem ser definidos por bundles.

.. _cookbook-bundles-extension-config-class:

Validação e Merging com uma Classe de Configuração
--------------------------------------------------

Até agora, você já fez o merge de seus arrays de configuração manualmente e
está verificando a presença de valores de configuração manualmente usando a função ``isset()`` 
do PHP. Um sistema opcional de *Configuração* está também disponível que
pode ajudar com merge, validação, valores padrão e normalização de formato.

.. note::

    Normalização de formato refere-se ao fato de que certos formatos - em grande parte XML -
    resultam em arrays de configuração ligeiramente diferentes e que estes arrays
    precisam ser "normalizados" para corresponder com todo o resto.

Para tirar vantagem deste sistema, você vai criar uma classe ``Configuration``
e construir uma árvore que define a sua configuração nesta classe::

    // src/Acme/HelloBundle/DependencyInjection/Configuration.php
    namespace Acme\HelloBundle\DependencyInjection;

    use Symfony\Component\Config\Definition\Builder\TreeBuilder;
    use Symfony\Component\Config\Definition\ConfigurationInterface;

    class Configuration implements ConfigurationInterface
    {
        public function getConfigTreeBuilder()
        {
            $treeBuilder = new TreeBuilder();
            $rootNode = $treeBuilder->root('acme_hello');

            $rootNode
                ->children()
                ->scalarNode('my_type')->defaultValue('bar')->end()
                ->end();

            return $treeBuilder;
        }

Este é um exemplo *muito* simples, mas agora você pode usar essa classe em seu método
``load()`` para o merge da sua configuração e forçar a validação. Se outras opções
que não sejam ``my_type`` forem passadas, o usuário será notificado com uma exceção
de que uma opção não suportada foi passada::

    public function load(array $configs, ContainerBuilder $container)
    {
        $configuration = new Configuration();

        $config = $this->processConfiguration($configuration, $configs);

        // ...
    }

O método ``processConfiguration()`` usa a árvore de configuração que você definiu
na classe ``Configuration`` para validar, normalizar e fazer o merge de todos os
arrays de configuração em conjunto.

A classe ``Configuration` pode ser muito mais complicada do que mostramos aqui,
suportando array nodes, "prototype" nodes, validação avançada, normalização específica de XML
e merge avançado. Você pode ler mais sobre isso na :doc:`documentação do Componente de Configuração</components/config/definition>`.
Você também pode vê-lo em ação verificando algumas das classes de Configuração do núcleo,
tais como a `Configuração do FrameworkBundle`_ ou a `Configuração do TwigBundle`_.

Dump de Configuração Padrão
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.1
    O comando ``config:dump-reference`` foi adicionado no Symfony 2.1 

O comando ``config:dump-reference`` permite que a configuração padrão de um bundle
seja impressa no console em YAML.

Enquanto a configuração do bundle está localizada no local padrão
(``YourBundle\DependencyInjection\Configuration``) e não tem um
``__constructor()`` ele vai funcionar automaticamente.  Se você tem algo
diferente a sua classe ``Extension`` terá que sobrescrever o
método ``Extension::getConfiguration()``.  Fazendo ele retornar uma instância de sua
``Configuration``.

Comentários e exemplos podem ser adicionados aos nós de configuração utilizando os
métodos ``->info()`` e ``->example()``::

    // src/Acme/HelloBundle/DependencyExtension/Configuration.php
    namespace Acme\HelloBundle\DependencyInjection;

    use Symfony\Component\Config\Definition\Builder\TreeBuilder;
    use Symfony\Component\Config\Definition\ConfigurationInterface;

    class Configuration implements ConfigurationInterface
    {
        public function getConfigTreeBuilder()
        {
            $treeBuilder = new TreeBuilder();
            $rootNode = $treeBuilder->root('acme_hello');

            $rootNode
                ->children()
                    ->scalarNode('my_type')
                        ->defaultValue('bar')
                        ->info('what my_type configures')
                        ->example('example setting')
                    ->end()
                ->end()
            ;

            return $treeBuilder;
        }

Este texto aparece como comentários yaml na saída do comando 
``config:dump-reference``.

.. index::
   pair: Convenção; Configuração

Convenções de Extensão
----------------------

Ao criar uma extensão, siga estas convenções simples:

* A extensão deve ser armazenada no sub-namespace ``DependencyInjection``;

* A extensão deve ser nomeada após o nome do bundle e com o sufixo
  ``Extension`` (``AcmeHelloExtension`` para ``AcmeHelloBundle``);

* A extensão deve fornecer um esquema XSD.

Se você seguir estas convenções simples, suas extensões serão registradas
automaticamente pelo Symfony2. Se não, sobrescreva o método Bundle 
:method:`Symfony\\Component\\HttpKernel\\Bundle\\Bundle::build` em
seu bundle::

    // ...
    use Acme\HelloBundle\DependencyInjection\UnconventionalExtensionClass;

    class AcmeHelloBundle extends Bundle
    {
        public function build(ContainerBuilder $container)
        {
            parent::build($container);

            // register extensions that do not follow the conventions manually
            $container->registerExtension(new UnconventionalExtensionClass());
        }
    }

Neste caso, a classe de extensão também deve implementar um método ``getAlias()``
e retornar um alias exclusivo nomeado após o bundle (por exemplo, ``acme_hello``). Isto
é necessário porque o nome da classe não segue os padrões terminando
em ``Extension``.

Além disso, o método ``load()`` de sua extensão será *apenas* chamado
se o usuário especificar o alias ``acme_hello`` em pelo menos um arquivo de configuração
. Mais uma vez, isso é porque a classe de extensão não segue os
padrões acima referidos, de modo que, nada acontece automaticamente.

.. _`Configuração do FrameworkBundle`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/DependencyInjection/Configuration.php
.. _`Configuração do TwigBundle`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/TwigBundle/DependencyInjection/Configuration.php
