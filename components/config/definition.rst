.. index::
   single: Config; Definindo e processando valores de configuração

Definindo e processando valores de configuração
===============================================

Validando valores de configuração
---------------------------------

Após carregar valores de configuração a partir de diversos tipos de fontes, os
valores e suas estruturas podem ser validados utilizando a parte "Definition" do
Config Component. É comumente esperado que valores de configuração exibam algum
tipo de hierarquia. Além de que, valores devem ser de algum certo tipo, ser 
restrito em número, ou ser um item de um dado conjunto de valores. Por exemplo,
a configuração a seguir (em Yaml) demonstra uma hierarquia clara e algumas
regras de validação que devem ser aplicadas a ela (como: "o valor para
``auto_connect`` deve ser um valor booleano"):

.. code-block:: yaml

    auto_connect: true
    default_connection: mysql
    connections:
        mysql:
            host: localhost
            driver: mysql
            username: user
            password: pass
        sqlite:
            host: localhost
            driver: sqlite
            memory: true
            username: user
            password: pass

Quando se carrega multiplos arquivos de configuração, deve ser possível
fundir e sobrescrever alguns valores. Outros valores não devem ser fundidos
e permanecerem da mesma forma que estão quando encontrados pela primeria vez.
Além de que, alhumas chaves só estão disponíveis quando outra chave tem um
valor específico (na configuração de exmeplo abaixo: a chave ``memory`` só
faz sentido quando ``driver`` for ``sqlite``).

Definindo a hierarquia de valores de configuração utilizando o Treebuilder
--------------------------------------------------------------------------

Todas as regras referentes a configuração de valores podem ser definidas
utilizando o :class:`Symfony\\Component\\Config\\Definition\\Builder\\TreeBuilder`.

Uma instância :class:`Symfony\\Component\\Config\\Definition\\Builder\\TreeBuilder` 
deve ser retornada a partir de uma classe ``Configuration`` customizada, que 
implementa o :class:`Symfony\\Component\\Config\\Definition\\ConfigurationInterface`::

    namespace Acme\DatabaseConfiguration;

    use Symfony\Component\Config\Definition\ConfigurationInterface;
    use Symfony\Component\Config\Definition\Builder\TreeBuilder;

    class DatabaseConfiguration implements ConfigurationInterface
    {
        public function getConfigTreeBuilder()
        {
            $treeBuilder = new TreeBuilder();
            $rootNode = $treeBuilder->root('database');

            // ... adiciona definições de nó a raiz da ávore
            // ... add node definitions to the root of the tree

            return $treeBuilder;
        }
    }

Adicionando definições de nó a arvore
-------------------------------------

Nós de variável
~~~~~~~~~~~~~~~

Uma árvore contém definições de nós que podem ser estabelecidas de forma semântica.
Isso significa que utilizando identação e notação fluente, é possível refletir a
estrutura real dos valores de configuração::

    $rootNode
        ->children()
            ->booleanNode('auto_connect')
                ->defaultTrue()
            ->end()
            ->scalarNode('default_connection')
                ->defaultValue('default')
            ->end()
        ->end()
    ;

O nó raiz em sí é um nó array, e possui filhos, como o nó booleano ``auto_connect``
e o nó escalar ``default_connection``. Em geral: após definir um nó, uma chamada
para ``end()`` leva você um passo a cima na hierarquia.

Tipo de nó
~~~~~~~~~

É possível validar o tipo de um valor fornecido utilizando a definição apropriada
de nó. O tipo nó está disponível para:

* scalar
* boolean
* integer (new in 2.2)
* float (new in 2.2)
* enum (new in 2.1)
* array
* variable (no validation)

e são creiados com ``node($name, $type)`` ou com seu método atalho
``xxxxNode($name)``.

Limitadores de nós numéricos
~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.2
    Os nós numéricos (float and integer) são novos em 2.2

Os nós numéricos (float and integer) fornecem dois limitadores extras -
:method:`Symfony\\Component\\Config\\Definition\\Builder::min` e
:method:`Symfony\\Component\\Config\\Definition\\Builder::max` -
possibilitando validar o valor::

    $rootNode
        ->children()
            ->integerNode('positive_value')
                ->min(0)
            ->end()
            ->floatNode('big_value')
                ->max(5E45)
            ->end()
            ->integerNode('value_inside_a_range')
                ->min(-50)->max(50)
            ->end()
        ->end()
    ;

Nós enum
~~~~~~~~~~

.. versionadded:: 2.1
    O nó enum é novo no Symfony 2.1

Nós enum fornecem um limitador para comparar a entrada contra um conjunto de 
valores::

    $rootNode
        ->children()
            ->enumNode('sexo')
                ->values(array('masculino', 'feminino'))
            ->end()
        ->end()
    ;

Isso irá restringir a opção ``sexo`` a ser ``masculino`` ou ``feminino``.

Nós array
~~~~~~~~~~~

É possível adicionar um nó mais profundo à hierarquia, acrescentando um
nó array. O próprio nó array pode ter um conjunto de variáveis nó pré definidas::

    $rootNode
        ->children()
            ->arrayNode('connection')
                ->children()
                    ->scalarNode('driver')->end()
                    ->scalarNode('host')->end()
                    ->scalarNode('username')->end()
                    ->scalarNode('password')->end()
                ->end()
            ->end()
        ->end()
    ;

Ou você pode definir um protótipo para cada nó dentro de um nó array::

    $rootNode
        ->children()
            ->arrayNode('connections')
                ->prototype('array')
                ->children()
                    ->scalarNode('driver')->end()
                    ->scalarNode('host')->end()
                    ->scalarNode('username')->end()
                    ->scalarNode('password')->end()
                ->end()
            ->end()
        ->end()
    ;

Um protótipo pode ser utilizado para adicionar uma definição que pode ser repetida
várias vezes dentro do nó atual. De acordo com a definição do protótipo no exemplo 
acima, é possível ter multiplas arrays connection (contendo um ``driver``, ``host``
, etc.).

Opções do nó array
~~~~~~~~~~~~~~~~~~

Antes de definir o filho de um nó array, você pode fornecer opções, como:

``useAttributeAsKey()``
    Fornece o nome de um nó filho, do qual o valor deve ser utilizado como a chave no array 
    resultante.
``requiresAtLeastOneElement()``
    Deve haver pelo menos um elemento no array (funciona somente quando ``isRequired()``
    também é chamado).
``addDefaultsIfNotSet()``
    Se qualquer nós filho tiver valor padrão, use-os caso valores explicitos não forem fornecidos.

Um exemplo disso::

    $rootNode
        ->children()
            ->arrayNode('parameters')
                ->isRequired()
                ->requiresAtLeastOneElement()
                ->useAttributeAsKey('name')
                ->prototype('array')
                    ->children()
                        ->scalarNode('value')->isRequired()->end()
                    ->end()
                ->end()
            ->end()
        ->end()
    ;

Em YAML, a configuração poderia ser dessa forma:

.. code-block:: yaml

    database:
        parameters:
            param1: { value: param1val }

Em XML, cada nó ``parameters`` teria um atributo ``name`` (junto com
``value``), que seria removido e utilizado como a chave para aquele
elemento no array final. O ``useAttributeAsKey`` É útil para normalizar
a forma como arrays são especificados entre diferentes formatos como XML 
e YAML.

Valores padrão e valores obrigatórios
---------------------------

For all node types, it is possible to define default values and replacement
values in case a node
has a certain value:

``defaultValue()``
    Define o valor padrão
``isRequired()``
    Deve ser definido (mas pode ser vazio)
``cannotBeEmpty()``
    Não pode conter um valor vazio
``default*()``
    (``null``, ``true``, ``false``), atalho para ``defaultValue()``
``treat*Like()``
    (``null``, ``true``, ``false``), fornece um valor substituto no caso do valor ser ``*.``

.. code-block:: php

    $rootNode
        ->children()
            ->arrayNode('connection')
                ->children()
                    ->scalarNode('driver')
                        ->isRequired()
                        ->cannotBeEmpty()
                    ->end()
                    ->scalarNode('host')
                        ->defaultValue('localhost')
                    ->end()
                    ->scalarNode('username')->end()
                    ->scalarNode('password')->end()
                    ->booleanNode('memory')
                        ->defaultFalse()
                    ->end()
                ->end()
            ->end()
            ->arrayNode('settings')
                ->addDefaultsIfNotSet()
                ->children()
                    ->scalarNode('name')
                        ->isRequired()
                        ->cannotBeEmpty()
                        ->defaultValue('value')
                    ->end()
                ->end()
            ->end()
        ->end()
    ;

Seções Opcionais
-----------------

.. versionadded:: 2.2
    Os métodos ``canBeEnabled`` e ``canBeDisabled`` são novos no Symfony 2.2

Se você possui uma seção inteira que é opicional e pode ser enabled/disabled,
você pode se aproveitar da vantagem dos métodos atalho
:method:`Symfony\\Component\\Config\\Definition\\Builder\\ArrayNodeDefinition::canBeEnabled` e
:method:`Symfony\\Component\\Config\\Definition\\Builder\\ArrayNodeDefinition::canBeDisabled`::

    $arrayNode
        ->canBeEnabled()
    ;

    // é equivalente a

    $arrayNode
        ->treatFalseLike(array('enabled' => false))
        ->treatTrueLike(array('enabled' => true))
        ->treatNullLike(array('enabled' => true))
        ->children()
            ->booleanNode('enabled')
                ->defaultFalse()
    ;

O método ``canBeDisabled`` procura pela mesma exceção que a seção habilitaria 
por padrão.

Opções de mesclagem
---------------

Opções extras referentes ao processo de mesclagem podem ser fornecidas. Para arrays:

``performNoDeepMerging()``
    Quando o valor também é definido em uma segunda array configuração, não
    tenta mesclar um array, mas sobrescrevê-lo inteiramente

Para todos os nós:

``cannotBeOverwritten()``
    não permite que outros arrays configuração sobrescrevam um valor existente neste nó

Seções de acréscimo
------------------

Se você tiver uma configuração complexa para validar, então a árvore pode
crescer para se tornar extensa e você pode querer divi-la em seções. Você
pode fazer isso criando uma seção, um nó separado e então acrescenta-lo na
árvore principal com ``append()``::

    public function getConfigTreeBuilder()
    {
        $treeBuilder = new TreeBuilder();
        $rootNode = $treeBuilder->root('database');

        $rootNode
            ->children()
                ->arrayNode('connection')
                    ->children()
                        ->scalarNode('driver')
                            ->isRequired()
                            ->cannotBeEmpty()
                        ->end()
                        ->scalarNode('host')
                            ->defaultValue('localhost')
                        ->end()
                        ->scalarNode('username')->end()
                        ->scalarNode('password')->end()
                        ->booleanNode('memory')
                            ->defaultFalse()
                        ->end()
                    ->end()
                    ->append($this->addParametersNode())
                ->end()
            ->end()
        ;

        return $treeBuilder;
    }

    public function addParametersNode()
    {
        $builder = new TreeBuilder();
        $node = $builder->root('parameters');

        $node
            ->isRequired()
            ->requiresAtLeastOneElement()
            ->useAttributeAsKey('name')
            ->prototype('array')
                ->children()
                    ->scalarNode('value')->isRequired()->end()
                ->end()
            ->end()
        ;

        return $node;
    }

Isso também é útil para ajudar a previnir que você se repita se você tiver
seções do config que são repetidas em diferentes locais.

Nomralização
-------------

Quando os arquivos config são processados, eles são primeiramente normalizados,
em seguida mesclados e finalmente a árvore é utilizada para validar o resultado
do array. O processo de normalização é utilizado para remover algumas das
diferenças que resultam de diferentes formatos de configuração, principalmente
as diferenças entre Yaml e XML.

O separador utilizado nas chaves são tipicamente ``_`` em Yaml e ``-`` em XML. 
Por exemplo, ``auto_connect`` em Yaml e ``auto-connect``. A normalização
converteria ambos para ``auto_connect``.

.. caution::
    
    A chaves alvo não será alterada se ela estiver misturada, como
    ``foo-bar_moo`` ou se ela já existir.

Outra diferença entre Yaml e XML é na forma em que os arrays de valores
podem ser representados. Em Yaml você pode ter:

.. code-block:: yaml

    twig:
        extensions: ['twig.extension.foo', 'twig.extension.bar']

e em XML:

.. code-block:: xml

    <twig:config>
        <twig:extension>twig.extension.foo</twig:extension>
        <twig:extension>twig.extension.bar</twig:extension>
    </twig:config>

Esse diferença pode ser removida na normalização através da pluralização
da chave utilizada no XML. Você pode especificar que você quer que uma
chave seja pluralizada dessa forma com ``fixXmlConfig()``::

    $rootNode
        ->fixXmlConfig('extension')
        ->children()
            ->arrayNode('extensions')
                ->prototype('scalar')->end()
            ->end()
        ->end()
    ;

Se for uma pluralização irregular, você pode especificar o plural para
utilizar como um segundo argumento::

    $rootNode
        ->fixXmlConfig('child', 'children')
        ->children()
            ->arrayNode('children')
        ->end()
    ;


Assim como corrigindo isso, ``fixXmlConfig`` garante que elementos xml individuais
ainda estejam sendo convertidos em array. Então você deve ter:

.. code-block:: xml

    <connection>default</connection>
    <connection>extra</connection>

e às vezes somente:

.. code-block:: xml

    <connection>default</connection>

Por padrão ``connection`` seria um array no primeiro caso e uma string
no segundo tornado-a difícil de validar. Você pode assegurar que ele sempre
será um array com ``fixXmlConfig``.

Você pode controlar ainda mais o processo de normalização. Por exemplo,
Você pode querer permitir que uma string seja definida e utilizada como uma
chave particular ou várias chaves sejam definidas explicitamente. De modo que,
tudo menos ``name`` seja opcional nessa configuração:

.. code-block:: yaml

    connection:
        name: my_mysql_connection
        host: localhost
        driver: mysql
        username: user
        password: pass

você também pode permitir o seguinte:

.. code-block:: yaml

    connection: my_mysql_connection

Mudando de um valor string para uma array associativa utilizando ``name`` como chave::

    $rootNode
        ->children()
            ->arrayNode('connection')
                ->beforeNormalization()
                ->ifString()
                    ->then(function($v) { return array('name'=> $v); })
                ->end()
                ->children()
                    ->scalarNode('name')->isRequired()
                    // ...
                ->end()
            ->end()
        ->end()
    ;

Regras de validação
----------------

Validações mais avançadas podem ser forneceidas utilizado o
:class:`Symfony\\Component\\Config\\Definition\\Builder\\ExprBuilder`. Este
builder implementa uma interface fluente para estruturas de controle comuns.
O builder é utilizado para acrescentar regras de validação para definições de nó, como::

    $rootNode
        ->children()
            ->arrayNode('connection')
                ->children()
                    ->scalarNode('driver')
                        ->isRequired()
                        ->validate()
                        ->ifNotInArray(array('mysql', 'sqlite', 'mssql'))
                            ->thenInvalid('Invalid database driver "%s"')
                        ->end()
                    ->end()
                ->end()
            ->end()
        ->end()
    ;

Uma regra de validação sempre tem uma parte "if". Você pode especificar esta
parte das seguintes formas:

- ``ifTrue()``
- ``ifString()``
- ``ifNull()``
- ``ifArray()``
- ``ifInArray()``
- ``ifNotInArray()``
- ``always()``

Uma regra de validação também requer uma parte "then":

- ``then()``
- ``thenEmptyArray()``
- ``thenInvalid()``
- ``thenUnset()``

Normalmente, "then" é um closure. Seu valor de retorno será utilizado como
um novo valor para o nó, ao invés
do valor original do nó.

Processando valores de configuração
-----------------------------------

O :class:`Symfony\\Component\\Config\\Definition\\Processor` utiliza a
árvore como ela foi construida utilizando o :class:`Symfony\\Component\\Config\\Definition\\Builder\\TreeBuilder`
para processar multiplos arrays de valores de configuração que deveriam
ser misturados. Se algum valor não for do tipo esperado, for obrigatório
e ainda indefinido, ou não puder ser validado de alguma outra forma, uma
uma exceção será lançada. Caso contrário o resultado é um array de valores
de configuração limpo::

    use Symfony\Component\Yaml\Yaml;
    use Symfony\Component\Config\Definition\Processor;
    use Acme\DatabaseConfiguration;

    $config1 = Yaml::parse(__DIR__.'/src/Matthias/config/config.yml');
    $config2 = Yaml::parse(__DIR__.'/src/Matthias/config/config_extra.yml');

    $configs = array($config1, $config2);

    $processor = new Processor();
    $configuration = new DatabaseConfiguration;
    $processedConfiguration = $processor->processConfiguration(
        $configuration,
        $configs)
    ;
