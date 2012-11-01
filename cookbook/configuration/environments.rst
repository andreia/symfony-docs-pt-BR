.. index::
   single: Ambientes

Como Dominar e Criar novos Ambientes
====================================

Cada aplicação é a combinação de código e um conjunto de configurações que
dita como o código deve funcionar. A configuração pode: definir o banco de 
dados a ser utilizado, se algo deve ou não ser armazenado em cache, ou como 
deve ser a verbosidade do log. No Symfony2, a idéia de "ambientes" é a idéia de que
a mesma base de código pode ser executada usando várias configurações diferentes. Por
exemplo, o ambiente ``dev`` deve usar uma configuração que faça com que o desenvolvimento
seja fácil e amigável, enquanto o ambiente ``prod`` deve usar um conjunto de configurações
otimizada para a velocidade.

.. index::
   single: Ambientes; Arquivos de configuração

Ambientes Diferentes, Diferentes Arquivos de Configuração
---------------------------------------------------------

Uma aplicação típica do Symfony2 começa com três ambientes: ``dev``,
``prod`` e ``test``. Como discutido, cada "ambiente" simplesmente representa
uma forma de executar o mesmo código com configurações diferentes. Não deve 
ser nenhuma surpresa, então, que cada ambiente carrega seu arquivo de configuração 
individual. Se você estiver usando o formato de configuração YAML, os seguintes arquivos
são usados:

* para o ambiente ``dev``: ``app/config/config_dev.yml``
* para o ambiente ``prod``: ``app/config/config_prod.yml``
* para o ambiente ``test``: ``app/config/config_test.yml``

Isso funciona por meio de uma convenção simples que é usada, por padrão, dentro da classe
``AppKernel``:

.. code-block:: php

    // app/AppKernel.php

    // ...
    
    class AppKernel extends Kernel
    {
        // ...

        public function registerContainerConfiguration(LoaderInterface $loader)
        {
            $loader->load(__DIR__.'/config/config_'.$this->getEnvironment().'.yml');
        }
    }

Como você pode ver, quando o Symfony2 é carregado, ele usa um dado ambiente para
determinar qual arquivo de configuração deve carregar. Isto alcança o objetivo de
vários ambientes de uma forma elegante, poderosa e transparente.

É claro que, na realidade, cada ambiente só difere um pouco dos outros.
Geralmente, todos os ambientes irão compartilhar uma grande base de configuração comum.
Abrindo o arquivo de configuração "dev", você pode ver como isso é feito
fácil e transparentemente:

.. configuration-block::

    .. code-block:: yaml

        imports:
            - { resource: config.yml }
        # ...

    .. code-block:: xml

        <imports>
            <import resource="config.xml" />
        </imports>
        <!-- ... -->

    .. code-block:: php

        $loader->import('config.php');
        // ...

Para compartilhar as configurações comuns, cada arquivo de configuração de ambiente
simplesmente primeiro importa de um arquivo de configuração central (``config.yml``).
O restante do arquivo pode então desviar-se da configuração padrão
sobrescrevendo os parâmetros individuais. Por exemplo, por padrão, a barra de ferramentas
``web_profiler`` está desativada. No entanto, no ambiente ``dev``, a barra de ferramentas é
ativada, modificando o valor padrão no arquivo de configuração ``dev``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        imports:
            - { resource: config.yml }

        web_profiler:
            toolbar: true
            # ...

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->
        <imports>
            <import resource="config.xml" />
        </imports>

        <webprofiler:config
            toolbar="true"
            ... />

    .. code-block:: php

        // app/config/config_dev.php
        $loader->import('config.php');

        $container->loadFromExtension('web_profiler', array(
            'toolbar' => true,
            ...,
        ));

.. index::
   single: Ambientes; Execução de diferentes ambientes

A execução de uma Aplicação em Ambientes Diferentes
---------------------------------------------------

Para executar a aplicação em cada ambiente, carregue a aplicação usando
o front controller ``app.php`` (para o ambiente ``prod``) ou o ``app_dev.php``
(para o ambiente ``dev``):

.. code-block:: text

    http://localhost/app.php      -> *prod* environment
    http://localhost/app_dev.php  -> *dev* environment

.. note::

   As URLs fornecidas assumem que o seu servidor web está configurado para usar o diretório
   ``web/`` da aplicação como sua raiz. Leia mais em
   :doc:`Installing Symfony2</book/installation>`.

Se você abrir um desses arquivos, verá rapidamente que o ambiente
utilizado por cada um é definido explicitamente:

.. code-block:: php
   :linenos:

    <?php

    require_once __DIR__.'/../app/bootstrap_cache.php';
    require_once __DIR__.'/../app/AppCache.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppCache(new AppKernel('prod', false));
    $kernel->handle(Request::createFromGlobals())->send();

Como você pode ver, a chave ``prod`` especifica que esse ambiente será executado
no ambiente ``prod``. Uma aplicação Symfony2 pode ser executada em qualquer
ambiente usando este código e alterando a string de ambiente.

.. note::

   O ambiente ``test`` é utilizado quando escrevemos testes funcionais e não é
   acessível diretamente no navegador através de um front controller. Em outras
   palavras, ao contrário dos outros ambientes, não há um arquivo
   front controller ``app_test.php``.

.. index::
   single: Configuração; Modo de Depuração

.. sidebar:: Modo *Depuração*

    Importante, mas sem relação com o tema de *ambientes* é a chave ``false``
    na linha 8 do front controller acima. Esta especifica se a aplicação
    deve ou não ser executada em "modo de depuração". Independentemente do ambiente,
    uma aplicação Symfony2 pode ser executada com o modo de depuração definido como ``true``
    ou ``false``. Isso afeta muitas coisas na aplicação, como se os erros devem ou não ser 
    apresentados ou se os arquivos de cache são dinamicamente reconstruídos em cada pedido. 
    Apesar de não ser uma exigência, o modo de depuração é geralmente definido
    para ``true`` nos ambientes ``dev`` e ``test`` e ``false`` para
    o ambiente ``prod``.

    Internamente, o valor do modo de depuração torna-se o parâmetro
    ``kernel.debug`` utilizado dentro do :doc:`service container </book/service_container>`.
    Se você olhar dentro do arquivo de configuração da aplicação, verá o
    parâmetro utilizado, por exemplo, para ligar ou desligar o log ao usar o
    Doctrine DBAL:

    .. configuration-block::

        .. code-block:: yaml

            doctrine:
               dbal:
                   logging:  "%kernel.debug%"
                   # ...

        .. code-block:: xml

            <doctrine:dbal logging="%kernel.debug%" ... />

        .. code-block:: php

            $container->loadFromExtension('doctrine', array(
                'dbal' => array(
                    'logging'  => '%kernel.debug%',
                    ...,
                ),
                ...
            ));

.. index::
   single: Ambientes; Criando um novo ambiente

Criação de um Novo Ambiente
---------------------------

Por padrão, uma aplicação Symfony2 tem que lidar com três ambientes na maioria
dos casos. Claro, desde que um ambiente nada mais é do que uma string que
corresponde a um conjunto de configuração, a criação de um novo ambiente é bastante
fácil.

Suponha, por exemplo, que antes da implantação, você precisa realizar um benchmark da sua
aplicação. Uma forma de realizar o benchmark da aplicação é usar configurações próximas da 
produção, mas com o ``web_profiler`` do Symfony2 habilitado. Isto permite ao Symfony2
registrar as informações sobre sua aplicação enquanto o realiza o benchmark.

A melhor forma de realizar isto é através de um novo ambiente chamado, por exemplo,
``benchmark``. Comece por criar um novo arquivo de configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_benchmark.yml
        imports:
            - { resource: config_prod.yml }

        framework:
            profiler: { only_exceptions: false }

    .. code-block:: xml

        <!-- app/config/config_benchmark.xml -->
        <imports>
            <import resource="config_prod.xml" />
        </imports>

        <framework:config>
            <framework:profiler only-exceptions="false" />
        </framework:config>

    .. code-block:: php

        // app/config/config_benchmark.php
        $loader->import('config_prod.php')

        $container->loadFromExtension('framework', array(
            'profiler' => array('only-exceptions' => false),
        ));

E com esta simples adição, a aplicação agora suporta um novo ambiente
chamado ``benchmark``.

Este novo arquivo de configuração importa a configuração do ambiente ``prod``
e modifica ela. Isso garante que o novo ambiente é idêntico ao
ao ambiente ``prod``, exceto por quaisquer alterações explicitamente feitas aqui.

Já que você deseja que este ambiente seja acessível através de um navegador, você
também deve criar um front controller para ele. Copie o arquivo ``web/app.php``
para ``web/app_benchmark.php`` e edite o ambiente para ``benchmark``:

.. code-block:: php

    <?php

    require_once __DIR__.'/../app/bootstrap.php';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('benchmark', false);
    $kernel->handle(Request::createFromGlobals())->send();

O novo ambiente está agora acessível através::

    http://localhost/app_benchmark.php

.. note::

   Alguns ambientes, como o ambiente ``dev``, nunca são destinados ao
   acesso em qualquer servidor implantado para o público em geral. Isto é porque
   certos ambientes, para fins de depuração, podem fornecer muita informação
   sobre a aplicação ou a infra-estrutura subjacente. Para ter certeza que estes ambientes
   não são acessíveis, o front controller é normalmente protegido de endereços IP externos
   através do seguinte código no topo do controlador:
   
    .. code-block:: php

        if (!in_array(@$_SERVER['REMOTE_ADDR'], array('127.0.0.1', '::1'))) {
            die('You are not allowed to access this file. Check '.basename(__FILE__).' for more information.');
        }

.. index::
   single: Ambientes; Diretório de cache

Ambientes e o Diretório de Cache
--------------------------------

O Symfony2 aproveita o cache de muitas maneiras: para a configuração da aplicação,
configuração de roteamento, templates Twig e mais é realizado o cache para objetos PHP
armazenados em arquivos no sistema de arquivos.

Por padrão, esses arquivos em cache são armazenados, em grande parte, no diretório ``app/cache``.
No entanto, cada ambiente armazena seu próprio conjunto de arquivos:

.. code-block:: text

    app/cache/dev   - diretório cache para o ambiente *dev*
    app/cache/prod  - diretório cache para o ambiente *prod*

Às vezes, quando depurando, pode ser útil inspecionar um arquivo em cache para
compreender como algo está funcionando. Ao fazê-lo, lembre-se de olhar no
diretório do ambiente que você está usando (mais comumente ``dev`` enquanto estiver
desenvolvendo e depurando). Embora possa variar, o diretório ``app/cache/dev``
inclui o seguinte:

* ``appDevDebugProjectContainer.php`` - o "container de serviço" em cache que
  representa a configuração da aplicação em cache;

* ``appdevUrlGenerator.php`` - a classe PHP gerada a partir da configuração 
  de roteamento e usada ao gerar URLs;

* ``appdevUrlMatcher.php`` - a classe PHP usada para correspondência de rota - olhe
  aqui para ver a lógica de expressões regulares compiladas usadas para correspondência das URLs de 
  entrada para diferentes rotas;

* ``twig/`` - Este diretório contém todos templates Twig em cache.

.. note::

    Você pode facilmente mudar a localização do diretório e o seu nome. Para mais informações
    leia o artigo :doc:`/cookbook/configuration/override_dir_structure`.


Investigando mais Detalhadamente
--------------------------------

Leia o artigo em :doc:`/cookbook/configuration/external_parameters`.
