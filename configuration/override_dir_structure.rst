.. index::
    single: Sobrescrever Symfony

Como Sobrescrever a Estrutura de Diretório Padrão do Symfony
============================================================

As aplicações Symfony possuem a seguinte estrutura de diretórios padrão, mas você pode
sobrescrevê-la para criar sua própria estrutura:

.. code-block:: text

    your-project/
    ├─ assets/
    ├─ bin/
    │  └─ console
    ├─ config/
    ├─ public/
    │  └─ index.php
    ├─ src/
    │  └─ ...
    ├─ templates/
    ├─ tests/
    ├─ translations/
    ├─ var/
    │  ├─ cache/
    │  ├─ log/
    │  └─ ...
    └─ vendor/

.. _override-config-dir:

Sobrescrever o Diretório de Configuração
----------------------------------------

O diretório de configuração é o único que não pode ser sobrescrito em uma
aplicação Symfony. Sua localização está fixa no código para o diretório ``config/``
no diretório raiz do projeto.

.. _override-cache-dir:

Sobrescrever o Diretório de Cache
---------------------------------

Você pode alterar o diretório de cache padrão sobrescrevendo o método
``getCacheDir()`` na classe ``Kernel`` da sua aplicação::

    // src/Kernel.php

    // ...
    class Kernel extends BaseKernel
    {
        // ...

        public function getCacheDir()
        {
            return dirname(__DIR__).'/var/'.$this->environment.'/cache';
        }
    }

Nesse código, ``$this->environment`` é o ambiente atual (ou seja, ``dev``).
Nesse caso, o local do diretório de cache foi alterado para
``var/{environment}/cache/``.

.. caution::

    Você deve manter o diretório de cache diferente para cada ambiente,
    caso contrário, poderá ocorrer um comportamento inesperado. Cada ambiente gera
    seus próprios arquivos de configuração em cache e, portanto, cada um precisa de seu próprio
    diretório para armazenar esses arquivos de cache.

.. _override-logs-dir:

Sobrescrever o Diretório de Log
-------------------------------

Para sobrescrever o diretório ``var/log/`` utilizamos o mesmo procedimento usado para sobrescrever
o diretório ``var/cache/``. A única diferença é que você precisa sobrescrever o método
``getLogDir()``::

    // src/Kernel.php

    // ...
    class Kernel extends Kernel
    {
        // ...

        public function getLogDir()
        {
            return dirname(__DIR__).'/var/'.$this->environment.'/log';
        }
    }

No exemplo acima, a localização do diretório foi alterada para ``var/{environment}/log/``.

.. _override-templates-dir:

Sobrescrever o Diretório de Templates
-------------------------------------

Se seus templates não estiverem armazenados no diretório ``templates/`` padrão, use
a opção de configuração :ref:`twig.paths <config-twig-paths>` para definir seu
próprio diretório (ou diretórios) de templates:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            # ...
            paths: ["%kernel.project_dir%/resources/views"]

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:path>%kernel.project_dir%/resources/views</twig:path>
            </twig:config>

        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'paths' => [
                '%kernel.project_dir%/resources/views',
            ],
        ]);

Sobrescrever o Diretório de Traduções
-------------------------------------

Se os seus arquivos de tradução não estiverem armazenados no diretório padrão
``translations/``, use a opção de configuração :ref:`framework.translator.paths <reference-translator-paths>`
para definir seu próprio diretório (ou diretórios) de traduções:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/translation.yaml
        framework:
            translator:
                # ...
                paths: ["%kernel.project_dir%/i18n"]

    .. code-block:: xml

        <!-- config/packages/translation.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <framework:config>
                <framework:translator>
                    <framework:path>%kernel.project_dir%/i18n</framework:path>
                </framework:translator>
            </framework:config>

        </container>

    .. code-block:: php

        // config/packages/translation.php
        $container->loadFromExtension('framework', [
            'translator' => [
                'paths' => [
                    '%kernel.project_dir%/i18n',
                ],
            ],
        ]);

.. _override-web-dir:
.. _override-the-web-directory:

Sobrescrever o Diretório Público
--------------------------------

Se você precisar renomear ou mover o diretório ``public/``, a única coisa que
você precisa garantir é que o caminho para o diretório ``var/`` ainda esteja correto no
seu front controller ``index.php``. Se você renomeou o diretório, está ok.
Mas se você o moveu de alguma forma, pode ser necessário modificar esses caminhos dentro desses
arquivos::

    require_once __DIR__.'/../path/to/vendor/autoload.php';

Você também precisa alterar a opção ``extra.public-dir`` no arquivo
``composer.json``:

.. code-block:: json

    {
        "...": "...",
        "extra": {
            "...": "...",
            "public-dir": "my_new_public_dir"
        }
    }

.. tip::

    Alguns hosts compartilhados possuem um diretório web raiz ``public_html/``. Renomeando
    seu diretório web de ``public/`` para ``public_html/`` é uma maneira de fazer
    seu projeto Symfony funcionar em seu host compartilhado. Outra maneira é implantar
    sua aplicação para um diretório fora da raiz web, excluir seu
    diretório ``public_html/`` e substituí-lo por um link simbólico para
    o diretório ``public/`` no seu projeto.

Sobrescrever o Diretório Vendor
-------------------------------

Para sobrescrever o diretório ``vendor/``, você precisa definir a opção
``vendor-dir`` no seu arquivo ``composer.json`` dessa forma:

.. code-block:: json

    {
        "config": {
            "bin-dir": "bin",
            "vendor-dir": "/some/dir/vendor"
        },
    }

.. tip::

    Essa modificação pode ser interessante se você estiver trabalhando em um ambiente virtual
    e não pode usar NFS - por exemplo, se você estiver executando uma aplicação 
    Symfony usando o Vagrant/VirtualBox em um sistema operacional convidado (guest).
