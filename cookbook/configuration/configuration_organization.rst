.. index::
    single: Configuração

Como Organizar os Arquivos de Configuração
==========================================

A Edição Standard do Symfony define três
:doc:`ambientes de execução </cookbook/configuration/environments>` chamados
``dev``, ``prod`` e ``test``. Um ambiente representa simplesmente uma forma de
executar o mesmo código com diferentes configurações.

Para selecionar o arquivo de configuração que será carregado em cada ambiente,
o Symfony executa o método ``registerContainerConfiguration()`` da classe
``AppKernel``::

    // app/AppKernel.php
    use Symfony\Component\HttpKernel\Kernel;
    use Symfony\Component\Config\Loader\LoaderInterface;

    class AppKernel extends Kernel
    {
        // ...

        public function registerContainerConfiguration(LoaderInterface $loader)
        {
            $loader->load($this->getRootDir().'/config/config_'.$this->getEnvironment().'.yml');
        }
    }

Esse método carrega o arquivo ``app/config/config_dev.yml`` para o ambiente 
``dev`` e assim por diante. Por sua vez, esse arquivo carrega o arquivo de configuração
comum localizado em ``app/config/config.yml``. Portanto, os arquivos de configuração da
Edição Standard do Symfony seguem esta estrutura:

.. code-block:: text

    <your-project>/
    ├─ app/
    │  └─ config/
    │     ├─ config.yml
    │     ├─ config_dev.yml
    │     ├─ config_prod.yml
    │     ├─ config_test.yml
    │     ├─ parameters.yml
    │     ├─ parameters.yml.dist
    │     ├─ routing.yml
    │     ├─ routing_dev.yml
    │     └─ security.yml
    ├─ src/
    ├─ vendor/
    └─ web/

Essa estrutura padrão foi escolhida por sua simplicidade - um arquivo por ambiente.
Mas, como qualquer outra funcionalidade do Symfony, você pode personalizá-la para melhor atender às suas necessidades.
As seções a seguir explicam diferentes formas de organizar seus arquivos de
configuração. Para simplificar os exemplos, apenas os ambientes
``dev`` and ``prod`` são considerados.

Diretórios Diferentes por Ambiente
----------------------------------

Em vez de adicionar os sufixos ``_dev`` e ``_prod`` nos arquivos, esta técnica
agrupa todos os arquivos de configuração relacionados sob um diretório com o mesmo
nome do ambiente:

.. code-block:: text

    <your-project>/
    ├─ app/
    │  └─ config/
    │     ├─ common/
    │     │  ├─ config.yml
    │     │  ├─ parameters.yml
    │     │  ├─ routing.yml
    │     │  └─ security.yml
    │     ├─ dev/
    │     │  ├─ config.yml
    │     │  ├─ parameters.yml
    │     │  ├─ routing.yml
    │     │  └─ security.yml
    │     └─ prod/
    │        ├─ config.yml
    │        ├─ parameters.yml
    │        ├─ routing.yml
    │        └─ security.yml
    ├─ src/
    ├─ vendor/
    └─ web/

Para que isso funcione, altere o código do
método
:method:`Symfony\\Component\\HttpKernel\\KernelInterface::registerContainerConfiguration`::

    // app/AppKernel.php
    use Symfony\Component\HttpKernel\Kernel;
    use Symfony\Component\Config\Loader\LoaderInterface;

    class AppKernel extends Kernel
    {
        // ...

        public function registerContainerConfiguration(LoaderInterface $loader)
        {
            $loader->load($this->getRootDir().'/config/'.$this->getEnvironment().'/config.yml');
        }
    }

Então, certifique-se de que cada arquivo ``config.yml`` carrega o resto dos arquivos
de configuração, incluindo os arquivos comuns. Por exemplo, estas seriam as importações
necessárias para o arquivo ``app/config/dev/config.yml``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/dev/config.yml
        imports:
            - { resource: '../common/config.yml' }
            - { resource: 'parameters.yml' }
            - { resource: 'security.yml' }

        # ...

    .. code-block:: xml

        <!-- app/config/dev/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="../common/config.xml" />
                <import resource="parameters.xml" />
                <import resource="security.xml" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/dev/config.php
        $loader->import('../common/config.php');
        $loader->import('parameters.php');
        $loader->import('security.php');

        // ...

.. include:: /components/dependency_injection/_imports-parameters-note.rst.inc

Arquivos de Configuração Semântica
----------------------------------

Uma estratégia de organização diferente pode ser necessária para aplicações complexas com
arquivos de configuração grandes. Por exemplo, você pode criar um arquivo por bundle
e vários arquivos para definir todos os serviços da aplicação:

.. code-block:: text

    <your-project>/
    ├─ app/
    │  └─ config/
    │     ├─ bundles/
    │     │  ├─ bundle1.yml
    │     │  ├─ bundle2.yml
    │     │  ├─ ...
    │     │  └─ bundleN.yml
    │     ├─ environments/
    │     │  ├─ common.yml
    │     │  ├─ dev.yml
    │     │  └─ prod.yml
    │     ├─ routing/
    │     │  ├─ common.yml
    │     │  ├─ dev.yml
    │     │  └─ prod.yml
    │     └─ services/
    │        ├─ frontend.yml
    │        ├─ backend.yml
    │        ├─ ...
    │        └─ security.yml
    ├─ src/
    ├─ vendor/
    └─ web/

Mais uma vez, altere o código do método ``registerContainerConfiguration()`` para
fazer o Symfony reconhecer a nova organização dos arquivos::

    // app/AppKernel.php
    use Symfony\Component\HttpKernel\Kernel;
    use Symfony\Component\Config\Loader\LoaderInterface;

    class AppKernel extends Kernel
    {
        // ...

        public function registerContainerConfiguration(LoaderInterface $loader)
        {
            $loader->load($this->getRootDir().'/config/environments/'.$this->getEnvironment().'.yml');
        }
    }

Seguindo a mesma técnica explicada na seção anterior, certifique-se de
importar os arquivos de configuração adequados a partir de cada arquivo principal (``common.yml``,
``dev.yml`` e ``prod.yml``).

Técnicas Avançadas
------------------

O Symfony carrega os arquivos de configuração usando o
:doc:`componente Config </components/config/introduction>`, que fornece algumas
funcionalidades avançadas.

Misture e Combine Formatos de Configuração
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Os arquivos de configuração podem importar arquivos definidos com qualquer um dos formato de configuração
embutidos (``.yml``, ``.xml``, ``.php``, ``.ini``):

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: 'parameters.yml' }
            - { resource: 'services.xml' }
            - { resource: 'security.yml' }
            - { resource: 'legacy.php' }

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="parameters.yml" />
                <import resource="services.xml" />
                <import resource="security.yml" />
                <import resource="legacy.php" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $loader->import('parameters.yml');
        $loader->import('services.xml');
        $loader->import('security.yml');
        $loader->import('legacy.php');

        // ...

.. caution::

    O ``IniFileLoader`` faz o parse do conteúdo do arquivo usando a
    função :phpfunction:`parse_ini_file`. Portanto, você só pode definir
    parâmetros para valores string. Use um dos outros carregadores se quiser
    usar outros tipos de dados (por exemplo, boolean, integer, etc.).

Se você usar qualquer outro formato de configuração, é necessário definir sua própria classe loader
estendendo-a de :class:`Symfony\\Component\\DependencyInjection\\Loader\\FileLoader`.
Quando os valores de configuração são dinâmicos, você pode usar o arquivo de configuração
PHP para executar sua própria lógica. Além disso, você pode definir seus próprios serviços
para carregar as configurações do bancos de dados ou de web services.

Arquivos de Configuração Global
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alguns administradores de sistema podem preferir armazenar parâmetros sensíveis em arquivos
fora do diretório do projeto. Imagine que as credenciais do banco de dados para seu
site são armazenadas no arquivo ``/etc/sites/mysite.com/parameters.yml``. Carregar
esse arquivo é tão simples quanto indicar o caminho completo do arquivo ao importá-lo a partir de
qualquer outro arquivo de configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: 'parameters.yml' }
            - { resource: '/etc/sites/mysite.com/parameters.yml' }

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="parameters.yml" />
                <import resource="/etc/sites/mysite.com/parameters.yml" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $loader->import('parameters.yml');
        $loader->import('/etc/sites/mysite.com/parameters.yml');

        // ...

Na maioria das vezes, desenvolvedores locais não terão os mesmos arquivos existentes no
servidores de produção. Por essa razão, o componente de configuração fornece a
opção ``ignore_errors`` para silenciosamente descartar erros quando o arquivo carregado
não existir:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: 'parameters.yml' }
            - { resource: '/etc/sites/mysite.com/parameters.yml', ignore_errors: true }

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="parameters.yml" />
                <import resource="/etc/sites/mysite.com/parameters.yml" ignore-errors="true" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $loader->import('parameters.yml');
        $loader->import('/etc/sites/mysite.com/parameters.yml', null, true);

        // ...

Como você viu, há muitas maneiras de organizar seus arquivos de configuração. Você
pode escolher uma delas ou até mesmo criar sua própria forma customizada de organizar os
arquivos. Não se sinta limitado pela Edição Standard que vem com o Symfony. Para ainda
mais personalização, consulte ":doc:`/cookbook/configuration/override_dir_structure`".
