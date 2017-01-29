.. index::
   single: Sobrescrever Symfony

Como Substituir a Estrutura de Diretório Padrão do Symfony
===========================================================

O Symfony vem automaticamente com uma estrutura de diretórios padrão. Você pode
facilmente substituir essa estrutura de diretório criando a seu própria. A estrutura 
de diretório padrão é:

.. code-block:: text

    app/
        cache/
        config/
        logs/
        ...
    src/
        ...
    vendor/
        ...
    web/
        app.php
        ...

Substituir o diretório ``cache``
--------------------------------

Você pode substituir o diretório cache, sobrescrevendo o método ``getCacheDir``
na classe ``AppKernel`` de sua aplicação::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        // ...

        public function getCacheDir()
        {
            return $this->rootDir.'/'.$this->environment.'/cache/';
        }
    }

``$this->rootDir`` é o caminho absoluto para o diretório ``app`` e ``$this->environment``
é o ambiente atual (ou seja, ``dev``). Neste caso você alterou
a localização do diretório cache para ``app/{environment}/cache``.

.. caution::

    Você deve manter o diretório ``cache`` diferente para cada ambiente, caso contrário,
    algum comportamento inesperado pode acontecer. Cada ambiente gera seus próprios arquivos
    de configuração em cache, e assim, cada um precisa de seu próprio diretório para armazenar
    os arquivos de cache.

Substituir o diretório ``logs``
-------------------------------

O processo para substituir o diretório ``logs`` é o mesmo do diretório ``cache``,
a única diferença é que você precisa sobrescrever o método
``getLogDir``::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        // ...

        public function getLogDir()
        {
            return $this->rootDir.'/'.$this->environment.'/logs/';
        }
    }

Aqui você alterou o local do diretório para ``app/{environment}/logs``.

Substituir o diretório ``web``
------------------------------

Se você precisa renomear ou mover o seu diretório ``web``, a única coisa que você
precisa garantir é que o caminho para o diretório ``app`` ainda está correto
em seus front controllers ``app.php`` e ``app_dev.php``. Se você simplesmente
renomear o diretório, então está tudo ok. Mas se você moveu de alguma forma, 
pode precisar modificar os caminhos dentro desses arquivos::

    require_once __DIR__.'/../Symfony/app/bootstrap.php.cache';
    require_once __DIR__.'/../Symfony/app/AppKernel.php';

.. tip::

    Alguns hosts compartilhados tem um diretório raiz web ``public_html``. Renomeando
    seu diretório web de ``web`` para ``public_html`` é uma maneira de fazer
    funcionar o seu projeto Symfony em seu servidor compartilhado. Outra forma é implantar
    sua aplicação em um diretório fora do raiz web, excluir seu
    diretório ``public_html`` e, então, substituí-lo por um link simbólico para
    o ``web`` em seu projeto.

.. note::

    Se você utiliza o AsseticBundle precisará configurar o seguinte, para que ele possa usar
    o diretório ``web`` correto:

    .. code-block:: yaml

        # app/config/config.yml

        # ...
        assetic:
            # ...
            read_from: "%kernel.root_dir%/../../public_html"

    Agora você só precisa realizar o dump dos assets novamente e sua aplicação deve
    funcionar:

    .. code-block:: bash

        $ php app/console assetic:dump --env=prod --no-debug
