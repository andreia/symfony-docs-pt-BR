.. index::
   single: Configuration; Semantic
   single: Bundle; Extension configuration

Como criar configuração amigável para um Bundle
=================================================

Se você abrir o arquivo de configuração da sua aplicação (geralmente ``app/config/config.yml``),
você verá seções de configuração diferentes, como ``framework``, ``twig`` and ``doctrine``.
Cada uma dessas seções configura um bundle específico, te permitindo definir opções em um alto nível 
e deixar o bundle fazer todas as mudanças complexas e de baixo nível baseado em suas configurações.

Por exemplo, a configuração a seguir diz ao FrameworkBundle que permita 
a integração de form, o que envolve a definição de alguns serviços bem como
integração de outros componentes relacionados:

.. configuration-block::

    .. code-block:: yaml

        framework:
            form: true

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:form />
            </framework:config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('framework', array(
            'form' => true,
        ));

.. sidebar:: Usando parâmetros para configurar seu Bundle
    
    Se você não tem planos de compartilhar seu bundle entre projetos,
    não faz sentido usar essa forma de configuração mais avançada. Uma vez que
    você use o bundle apenas em um projeto, você pode mudar a configuração de 
    serviço cada vez que for necessário.

    Se você quiser ser capaz de configurar algo de dentro de
    ``config.yml``, você sempre pode criar um parâmetro lá e usar este 
    parâmetro em outro lugar.

Usando o Bundle Extension
--------------------------

A ideia básica é que em vez de ter o usuário sobrescrevendo parâmetros
individuais, você permite o usuário configurar algumas opções especificamente criadas.
Como desenvolvedor do bundle, você analisa a configuração a carrega os serviços corretos
e parâmetros dentro de uma classe "Extension".

Como exemplo, imagine que você esta criando um bundle social, que provê integração
com Twitter. Para ser possível reusar seu bundle, você tem de fazer as variáveis 
``client_id`` e ``client_secret`` configuráveis. A configuração do seu bundle poderia 
parecer com:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        acme_social:
            twitter:
                client_id: 123
                client_secret: $ecret

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:acme-social="http://example.org/dic/schema/acme_social"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

           <acme-social:config>
               <twitter client-id="123" client-secret="$ecret" />
           </acme-social:config>

           <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('acme_social', array(
            'client_id'     => 123,
            'client_secret' => '$ecret',
        ));

.. seealso::

    Leia mais sobre a extensão em :doc:`/cookbook/bundles/extension`.

.. tip::

    Se um bundle provê uma classe Extension, geralmente você não deve sobrescrever
    algum parâmetro do contêiner de serviços deste bundle. A ideia é que se uma classe
    Extension está presente, toda configuração que deve ser configurável deve 
    estar presente na configuração disponibilizada por esta classe. Em outras 
    palavras, a classe de extensão define todas as definições de configurações
    cuja compatibilidade com versões anteriores será mantida.

.. seealso::

    Para manipulação de parâmetros num contêiner de injeção de dependência veja
    :doc:`/cookbook/configuration/using_parameters_in_dic`.


Processando o Array ``$configs``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Primeiramente você tem que criar um classe de extensão como explicado em
:doc:`extension`.

Sempre que um usuário inclui a chave ``acme_social`` (que é o pseudônimo do DI)
em um arquivo de configuração, a configuração sob ele é adicionada a um array de 
configurações e passada ao método ``load()`` da sua extensão (Symfony automaticamente
converte XML e YAML em array).

Para a configuração de exemplo na seção anterior, o array passado ao seu método
``load()`` vai se parecer com isso::

Note que esse é um *array de arrays*, não apenas um único array plano de valores
de configuração. Isto é intencional, uma vez que permite que o Symfony analise 
várias fontes de recursos. Por exemplo, se ``acme_social`` aparece em outro arquivo
de configuração - say ``config_dev.yml`` - com valores diferentes abaixo dele, 
o array de entrada deve parecer com isso::

    array(
        // values from config.yml
        array(
            'twitter' => array(
                'client_id' => 123,
                'client_secret' => '$secret',
            ),
        ),
        // values from config_dev.yml
        array(
            'twitter' => array(
                'client_id' => 456,
            ),
        ),
    )

A ordem dos dois arrays depende de qual é configurado primeiro.

Mas não se preocupe! A configuração de componentes do Symfony vai ajudá-lo a mesclar esses valores,
prover padrões e oferecer erros de validação de usuário em casos de má configuração.
Segue como isso funciona. Crie uma classe ``Configuration`` no diretório ``DependencyInjection``
e construa uma árvore que defina a estrutura de configuração do seu pacote.

A classe ``Configuration`` que lida com o exemplo de configuração parece com o seguinte::

    // src/Acme/SocialBundle/DependencyInjection/Configuration.php
    namespace Acme\SocialBundle\DependencyInjection;

    use Symfony\Component\Config\Definition\Builder\TreeBuilder;
    use Symfony\Component\Config\Definition\ConfigurationInterface;

    class Configuration implements ConfigurationInterface
    {
        public function getConfigTreeBuilder()
        {
            $treeBuilder = new TreeBuilder();
            $rootNode = $treeBuilder->root('acme_social');

            $rootNode
                ->children()
                    ->arrayNode('twitter')
                        ->children()
                            ->integerNode('client_id')->end()
                            ->scalarNode('client_secret')->end()
                        ->end()
                    ->end() // twitter
                ->end()
            ;

            return $treeBuilder;
        }
    }

.. seealso::

    A classe ``Configuration`` pode ser muito mais complicada do que mostrado aqui,
    suportando nós "protótipo", validação avançada, normalização específica de XML
    e fusão avançada. Você pode ler mais sobre isso em
    :doc:`documentação do componente Config </components/config/definition>`. Você
    também pode ver isso em ação dando uma olhada em algumas classes de configuração
    do core, como `FrameworkBungle Configuration`_ or the `TwigBundle Configuration`_.

Agora esta classe pode ser usada em seu método ``load()`` para mesclar configurações
e forçar validação (e.g. se uma opção adicional for passada uma exceção vai ser jogada)::

    public function load(array $configs, ContainerBuilder $container)
    {
        $configuration = new Configuration();

        $config = $this->processConfiguration($configuration, $configs);
        // ...
    }

O método ``processConfiguration()`` usa a árvore de configuração que você definiu
na classe ``Configuration`` para validar, normalizar e mesclar todas or arrays
de configuração juntos.

.. tip::

    Em vez de chamar ``processConfiguration()`` na sua extensão cada vez que que você
    provêr opções de configuração, você deve querer usar a
    :class:`Symfony\\Component\\HttpKernel\\DependencyInjection\\ConfigurableExtension`
    para fazer isso automaticamente para você::

        // src/Acme/HelloBundle/DependencyInjection/AcmeHelloExtension.php
        namespace Acme\HelloBundle\DependencyInjection;

        use Symfony\Component\DependencyInjection\ContainerBuilder;
        use Symfony\Component\HttpKernel\DependencyInjection\ConfigurableExtension;

        class AcmeHelloExtension extends ConfigurableExtension
        {
            // note that this method is called loadInternal and not load
            protected function loadInternal(array $mergedConfig, ContainerBuilder $container)
            {
                // ...
            }
        }

    Essa classe usa o método ``getConfiguration()`` para instanciar a configuração.
    Você deve sobrescrevê-lo se sua classe de configuração não se chama ``Configuration``
    ou se não está localizada no mesmo namespace que a extensão.

.. sidebar:: Processndo a configuração você mesmo

    Usar o componente Config é totalmente opcional. O método ``load()`` pegar um
    array de valores de configuração. Você pode simplesmente interpretar este array você 
    mesmo (e.g. sobrecrevendo configuraçãos e usando :phpfunction:`isset` para checar 
    a existência de um valor). Esteja consciente que vai ser muito difícil suportar XML.

    .. code-block:: php

        public function load(array $configs, ContainerBuilder $container)
        {
            $config = array();
            // let resources override the previous set value
            foreach ($configs as $subConfig) {
                $config = array_merge($config, $subConfig);
            }

            // ... now use the flat $config array
        }

Modificando a configuração de outro Bundle
---------------------------------------------

Se você tem múltiplos pacotes que dependem uns dos outros, será útil
permitir que uma classe ``Extension`` modifique a configuração passada a 
outra classe ``Extension`` do bundle, como se o desenvolvedor-cliente tenha realmente
colocando aquela configuração no seu arquivo ``app/config/config.yml``. Isso pode ser 
alcançado usando uma extensão pré-fixada. Para mais detalhes, veja
:doc:`/cookbook/bundles/prepend_extension`.

Imprimir a configuração
----------------------

O comando ``config:dump-reference`` imprimi a configuração padrão de um
bundle no console usando o formato Yaml.

Contanto que a configuração do seu bundle esteja localizada na localização
padrão (``YourBundle\DependencyInjection\Configuration``) e não necessite de
receber argumentos no construtor isso funcionará automaticamente. Se você
têm algo diferente, sua classe ``Extension`` deve sobrescrever o método
:method:`Extension::getConfiguration() <Symfony\\Component\\HttpKernel\\DependencyInjection\\Extension::getConfiguration>`
e retornar uma instância da sua ``Configuration``.

Suportando XML
--------------

O Symfony permite às pessoas provês a configuração em três formatos diferentes:
Yaml, XML and PHP. Ambos Yaml e PHP usam a mesma sintaxe e são suportados por padrão
quando usam o componente Config. Suportar XML exige que você mais algumas coisas.
Porém quando compartilhar seu pacote com outros, é recomendado seguir estes passos.

Torne sua árvore Config pronta para XML
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O componente Config provê alguns métodos por padrão para permitir processar
configuração em XML corretamente. Veja ":ref:component-config-normalization" 
da documentação do componente. Entretando, você pode fazer algumas coisas opcionais,
isso vai melhorar a experiência de uso de configuração em XML.

Escolhendo o Namespace do XML
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No XML, o `XML namespace`_ é usado para determinar quais elementos pertencem
à configuração de um bundle específico. O namespace é retornado do método
:method:`Extension::getNamespace() <Symofny\\Component\\DependencyInjection\\Extension\\Extension::getNamespace>`
Por convenção, o namespace é uma URL (não tem de ser uma URL válida nem necessita existir).
Por padrão, o namespace de um bundle é ``http://example.org/dic/schema/DI_ALIAS``,
 onde ``DI_ALIAS`` é o apelido do DI da extensão. Você deve querer alterar isso para uma URL mais profissional::

    // src/Acme/HelloBundle/DependencyInjection/AcmeHelloExtension.php

    // ...
    class AcmeHelloExtension extends Extension
    {
        // ...

        public function getNamespace()
        {
            return 'http://acme_company.com/schema/dic/hello';
        }
    }

Provendo um Schema XML
~~~~~~~~~~~~~~~~~~~~~~

O XML tém uma funcionalidade muito útil chamada `XML schema`_. Ela permite que você
descreva todos os possíveis elementos e atributos e seus valores em um 
XML Schema Definition (um arquivo xsd). Esse arquivo XSD é usado por IDEs para 
auto compleção e é usado pelo componente Config para validar os elementos.

A fim de usar o schema, o arquivo de configuração XML deve prover um
atributo ``xsi:schemaLocation`` apontando o arquivo XSD para um determinado 
namespace XML. Esta localização sempre inicia com o namespace XML. Esse namespace XML
é então substituído pelo caminho base de validação XSD retornado do método
:method:`Extension::getXsdValidationBasePath() <Symfony\\Component\\DependencyInjection\\ExtensionInterface::getXsdValidationBasePath>` 

Por convenção, o arquivo XSD fica em ``Resources/config/schema``, porém você 
poe colocá-lo onde você preferir. Você deve retornar este caminho como o caminho base::

    // src/Acme/HelloBundle/DependencyInjection/AcmeHelloExtension.php

    // ...
    class AcmeHelloExtension extends Extension
    {
        // ...

        public function getXsdValidationBasePath()
        {
            return __DIR__.'/../Resources/config/schema';
        }
    }

Presumindo que o arquivo XSD se chame `hello-1.0.xsd`` a localização do schema
vai ser ``http://acme_company.com/schema/dic/hello-1.0.xsd``:

.. code-block:: xml

    <!-- app/config/config.xml -->
    <?xml version="1.0" ?>

    <container xmlns="http://symfony.com/schema/dic/services"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xmlns:acme-hello="http://acme_company.com/schema/dic/hello"
        xsi:schemaLocation="http://acme_company.com/schema/dic/hello
            http://acme_company.com/schema/dic/hello/hello-1.0.xsd">

        <acme-hello:config>
            <!-- ... -->
        </acme-hello:config>

        <!-- ... -->
    </container>

.. _`FrameworkBundle Configuration`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/DependencyInjection/Configuration.php
.. _`TwigBundle Configuration`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/TwigBundle/DependencyInjection/Configuration.php
.. _`XML namespace`: http://en.wikipedia.org/wiki/XML_namespace
.. _`XML schema`: http://en.wikipedia.org/wiki/XML_schema