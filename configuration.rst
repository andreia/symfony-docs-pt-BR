.. index::
   single: Configuração

Configurando o Symfony (e Ambientes)
====================================

Toda aplicação Symfony consiste de uma coleção de bundles que adicionam ferramentas úteis
(:doc:`serviços </service_container>`) ao seu projeto. Cada bundle pode ser personalizado
através de arquivos de configuração que residem - por padrão - no diretório ``app/config``.

Configuração: config.yml
------------------------

O arquivo de configuração principal é chamado ``config.yml``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: parameters.yml }
            - { resource: security.yml }
            - { resource: services.yml }

        framework:
            secret:          '%secret%'
            router:          { resource: '%kernel.root_dir%/config/routing.yml' }
            # ...

        # Twig Configuration
        twig:
            debug:            '%kernel.debug%'
            strict_variables: '%kernel.debug%'

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <imports>
                <import resource="parameters.yml" />
                <import resource="security.yml" />
                <import resource="services.yml" />
            </imports>

            <framework:config secret="%secret%">
                <framework:router resource="%kernel.root_dir%/config/routing.xml" />
                <!-- ... -->
            </framework:config>

            <!-- Twig Configuration -->
            <twig:config debug="%kernel.debug%" strict-variables="%kernel.debug%" />

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $this->import('parameters.yml');
        $this->import('security.yml');
        $this->import('services.yml');

        $container->loadFromExtension('framework', array(
            'secret' => '%secret%',
            'router' => array(
                'resource' => '%kernel.root_dir%/config/routing.php',
            ),
            // ...
        ));

        // Twig Configuration
        $container->loadFromExtension('twig', array(
            'debug'            => '%kernel.debug%',
            'strict_variables' => '%kernel.debug%',
        ));

        // ...

A maioria das teclas de nível superior - como `` e `` framework`` twig`` - são de configuração para um
pacote específico (ou seja, ``FrameworkBundle`` e ``TwigBundle``).

.. sidebar:: Formatos de Configuração

    Ao longo dos capítulos, todos os exemplos de configuração serão mostrados em
    três formatos (YAML, XML e PHP). YAML é usado por padrão, mas você pode
    escolher o que mais gosta. Não há diferença de desempenho:

    * :doc:`/components/yaml/yaml_format`: Simples, limpo e legível;

    * *XML*: Mais poderoso do que YAML às vezes e suporta autocompletar das IDEs;

    * *PHP*: Muito poderoso, mas menos legível do que os formatos de configuração padrão.

Configuração de Referência e Dumping
------------------------------------

Há *duas* formas de saber *quais* chaves você pode configurar:

#. Use a :doc:`Seção de Referência </reference/index>`;

#. Use o comando ``config:dump-reference``.

Por exemplo, se você deseja configurar algo no Twig, você pode ver um exemplo
através do dump de todas as opções de configuração disponíveis, executando:

.. code-block:: terminal

    $ php bin/console config:dump-reference twig

.. index::
   single: Ambientes; Introdução

.. _page-creation-environments:
.. _page-creation-prod-cache-clear:

A chave imports: Carregar outros Arquivos de Configuração
---------------------------------------------------------

O arquivo principal de configuração do symfony é ``app/config/config.yml``. Mas, para organização,
ele *também* carrega outros arquivos de configuração através de sua chave ``imports``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: parameters.yml }
            - { resource: security.yml }
            - { resource: services.yml }
        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <imports>
                <import resource="parameters.yml" />
                <import resource="security.yml" />
                <import resource="services.yml" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $this->import('parameters.yml');
        $this->import('security.yml');
        $this->import('services.yml');

        // ...

A chave ``imports`` se parece muito com a função ``include()`` do PHP: os conteúdos de
``parameters.yml``, ``security.yml`` e ``services.yml`` são lidos e carregados. Também
é possível carregar arquivos XML ou PHP.

.. _config-parameter-intro:

A chave parameters: Parâmetros (Variáveis)
------------------------------------------

Outra chave especial é chamada `` parameters``: é usada para definir *variáveis* ​​que
podem ser referenciadas em *qualquer* outro arquivo de configuração. Por exemplo, em ``config.yml``,
um parâmetro ``locale`` é definido e, então, referenciado logo abaixo sob a chave
`` framework``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        # ...

        parameters:
            locale: en

        framework:
            # ...

            # any string surrounded by two % is replaced by that parameter value
            default_locale:  "%locale%"

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <!-- ... -->
            <parameters>
                <parameter key="locale">en</parameter>
            </parameters>

            <framework:config default-locale="%locale%">
                <!-- ... -->
            </framework:config>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        // ...

        $container->setParameter('locale', 'en');

        $container->loadFromExtension('framework', array(
            'default_locale' => '%locale%',
            // ...
        ));

        // ...

Você pode definir quaisquer nomes de parâmetros que desejar sob a chave ``parameters`` de
qualquer arquivo de configuração. Para fazer referência a um parâmetro, envolva seu nome com dois
sinais de porcentagem - por exemplo, ``%locale%``.

.. seealso::

    Você também pode definir os parâmetros de forma dinâmica, como a partir de variáveis ​​de ambiente.
    Veja :doc:`/configuration/external_parameters`.

Para mais informações sobre parâmetros - incluindo como referênciá-los dentro
de um controlador - veja :ref:`service-container-parameters`.

.. _config-parameters-yml:

O Arquivo Especial parameters.yml
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Superficialmente, ``parameters.yml`` é como qualquer outro arquivo de configuração: ele
é importado por ``config.yml`` e define vários parâmetros:

.. code-block:: yaml

    parameters:
        # ...
        database_user:      root
        database_password:  ~

Não surpreendentemente, esses são referenciados dentro de ``config.yml`` e ajudam a
configurar o DoctrineBundle e outras partes do Symfony:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        doctrine:
            dbal:
                driver:   pdo_mysql
                # ...
                user:     '%database_user%'
                password: '%database_password%'

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal
                    driver="pdo_mysql"

                    user="%database_user%"
                    password="%database_password%" />
            </doctrine:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $configuration->loadFromExtension('doctrine', array(
            'dbal' => array(
                'driver'   => 'pdo_mysql',
                // ...

                'user'     => '%database_user%',
                'password' => '%database_password%',
            ),
        ));

Mas o arquivo ``parameters.yml`` *é* especial: ele define os valores que normalmente são
alterados em cada servidor. Por exemplo, as credenciais de banco de dados em sua máquina de
desenvolvimento local pode ser diferente das de seus colegas de trabalho. É por isso que esse arquivo
não está comprometido com o repositório compartilhado e só é armazenado em sua máquina.

Por causa disso, ** parameters.yml não está comprometido com o controle de versão **. De fato,
o `` .gitignore`` arquivo que vem com Symfony impede que seja cometido.

No entanto, um arquivo `` parameters.yml.dist`` * é * comprometidos (com valores fictícios). Este ficheiro
não é lido por Symfony: é apenas uma referência para que Symfony sabe quais parâmetros
precisa ser definido no arquivo `` parameters.yml``. Se você adicionar ou remover chaves para
`` Parameters.yml``, adicionar ou removê-los de `` parameters.yml.dist`` também, então ambos
arquivos estão sempre em sincronia.

.. sidebar:: O Handler de Parâmetro Interativo

    Quando você :ref:`instala um projeto symfony existente <install-existing-app>`, você
    precisa criar o arquivo ``parameters.yml`` usando o arquivo ``parameters.yml.dist``
    como referência. Para ajudar com isso, após executar ``compositor install``, um
    script Symfony criará automaticamente esse arquivo interativamente pedindo-lhe
    para fornecer o valor para cada parâmetro definido em ``parameters.yml.dist``. Para
    mais detalhes - ou para remover ou controlar esse comportamento - veja a
    documentação do `Incenteev Parameter Handler`_.

Ambientes e os outros arquivos de configuração
----------------------------------------------

Você tem apenas *uma* aplicação, porém, caso tenha percebido ou não, ela precisa comportar-se
*diferentemente* em determinados momentos:

* Enquanto estiver **desenvolvendo**, você quer que sua aplicação realize log de tudo e exiba ferramentas
  agradáveis para depuração;

* Depois de implantar em **produção**, você quer que a *mesma* aplicação seja otimizada para
  velocidade e grave em log apenas erros.

Como você pode fazer *uma* aplicação comportar-se de duas maneiras diferentes? Com *ambientes*.

Você provavelmente já está utilizando o ambiente ``dev``, mesmo sem conhecê-lo.
Depois de implantar, você vai usar o ambiente ``prod``.

Para saber mais sobre *como* executar e controlar cada ambiente, consulte
:doc:`/configuration/environments`.

Continue!
---------

Parabéns! Você abordou o básico no Symfony. Em seguida, aprenda sobre *cada*
parte do Symfony individualmente, seguindo os guias. Verifique:

* :doc:`/forms`
* :doc:`/doctrine`
* :doc:`/service_container`
* :doc:`/security`
* :doc:`/email`
* :doc:`/logging`

E muitos outros tópicos.

Aprenda mais
------------

.. toctree::
    :maxdepth: 1
    :glob:

    configuration/*

.. _`Incenteev Parameter Handler`: https://github.com/Incenteev/ParameterHandler
