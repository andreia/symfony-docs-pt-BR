.. index::
   single: Assetic; UglifyJS

Como Minificar Arquivos CSS/JS (Usando UglifyJS e UglifyCSS)
============================================================

.. include:: /assetic/_standard_edition_warning.rst.inc

O `UglifyJS`_ é um kit de ferramentas de análise/compressão/embelezamento de JavaScript. Ele pode ser usado
para combinar e minificar assets JavaScript para que eles exijam menos requisições HTTP
e façam seu site carregar mais rápido. O `UglifyCSS`_ é um compressor/embelezador CSS
muito semelhante ao UglifyJS.

Neste artigo, a instalação, configuração e uso do UglifyJS são
mostrados em detalhes. O UglifyCSS funciona da mesma forma e é apenas
mencionado brevemente.

Instalando o UglifyJS
---------------------

O UglifyJS está disponível como um módulo do `Node.js`_. Primeiro, você precisa `instalar o Node.js`_
e então decidir o método de instalação: global ou local.

Instalação Global
~~~~~~~~~~~~~~~~~

O método de instalação global faz com que todos os seus projetos usem exatamente a mesma versão do
UglifyJS, o que simplifica sua manutenção. Abra seu console de comando e execute
o seguinte comando (você pode precisar executá-lo como um usuário root):

.. code-block:: terminal

    $ npm install -g uglify-js

Agora você pode executar o comando global ``uglifyjs`` em qualquer lugar do seu sistema:

.. code-block:: terminal

    $ uglifyjs --help

Instalação Local
~~~~~~~~~~~~~~~~

Também é possível instalar o UglifyJS apenas dentro do seu projeto, o que é útil
quando seu projeto requer uma versão específica do UglifyJS. Para fazer isso, instale-o
sem a opção ``-g`` e especifique o caminho onde instalará o módulo:

.. code-block:: terminal

    $ cd /caminho/para/seu/projeto/symfony
    $ npm install uglify-js --prefix app/Resources

É recomendado que você instale o UglifyJS na sua pasta ``app/Resources`` e
adicione a pasta ``node_modules`` ao controle de versão. Alternativamente, você pode criar
um arquivo `package.json`_ do npm e especificar suas dependências lá.

Agora você pode executar o comando ``uglifyjs`` disponível no diretório
``node_modules``:

.. code-block:: terminal

    $ "./app/Resources/node_modules/.bin/uglifyjs" --help

Configurando o Filtro ``uglifyjs2``
-----------------------------------

Agora precisamos configurar o Symfony para usar o filtro ``uglifyjs2`` ao processar
seus JavaScripts:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                uglifyjs2:
                    # o caminho para o executável uglifyjs
                    bin: /usr/local/bin/uglifyjs

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config>
                <!-- bin: o caminho para o executável uglifyjs -->
                <assetic:filter
                    name="uglifyjs2"
                    bin="/usr/local/bin/uglifyjs" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'uglifyjs2' => array(
                    // o caminho para o executável uglifyjs
                    'bin' => '/usr/local/bin/uglifyjs',
                ),
            ),
        ));

.. note::

    O caminho onde o UglifyJS está instalado pode variar dependendo do seu sistema.
    Para descobrir onde o npm mantém a pasta ``bin``, execute o seguinte comando:

    .. code-block:: terminal

        $ npm bin -g

    Ele deve exibir uma pasta em seu sistema, dentro da qual você deve encontrar
    o executável do UglifyJS.

    Se você instalou o UglifyJS localmente, você pode encontrar a pasta ``bin`` dentro
    da pasta ``node_modules``. Ela é chamada ``.bin`` neste caso.

Você agora tem acesso ao filtro ``uglifyjs2`` em sua aplicação.

Configurando o Binário ``node``
-------------------------------

O Assetic tenta encontrar o binário node automaticamente. Se ele não puder ser encontrado, você
pode configurar sua localização usando a chave ``node``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            # o caminho para o executável node
            node: /usr/bin/nodejs
            filters:
                uglifyjs2:
                    # o caminho para o executável uglifyjs
                    bin: /usr/local/bin/uglifyjs

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config
                node="/usr/bin/nodejs" >
                <assetic:filter
                    name="uglifyjs2"
                    bin="/usr/local/bin/uglifyjs" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'node'      => '/usr/bin/nodejs',
            'uglifyjs2' => array(
                // o caminho para o executável uglifyjs
                'bin' => '/usr/local/bin/uglifyjs',
            ),
        ));

Minificando seus Assets
-----------------------

Para aplicar o UglifyJS em seus assets, adicione a opção ``filter`` nas
tags de asset dos seus templates para dizer ao Assetic para usar o filtro ``uglifyjs2``:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' filter='uglifyjs2' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('uglifyj2s')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    O exemplo acima assume que você tem um bundle chamado AppBundle e seus
    arquivos JavaScript estão no diretório ``Resources/public/js`` do seu
    bundle. Entretanto, você pode incluir seus arquivos JavaScript, não importa onde eles estejam.

Com a adição do filtro ``uglifyjs2`` às tags de asset acima, você
agora deve ver os JavaScripts minificados sendo carregados muito mais rápido.

Desativando a Minificação no Modo de Depuração
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Os JavaScripts minificados são muito difíceis de ler, e mais ainda de depurar. Por
isso, o Assetic permite desabilitar um certo filtro quando a sua aplicação está no
modo de depuração (por exemplo, ``app_dev.php``). Você pode fazer isso prefixando o nome do filtro
em seu template com um ponto de interrogação: ``?``. Isto diz ao Assetic para aplicar
esse filtro apenas quando o modo de depuração estiver desligado. (por exemplo, ``app.php``):

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' filter='?uglifyjs2' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('?uglifyjs2')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Para testar isso, mude para o seu ambiente ``prod`` (``app.php``). Mas antes
de mudar, não se esqueça de :ref:`limpar seu cache <page-creation-prod-cache-clear>`
e :ref:`fazer o dump dos assets do assetic <assetic-dump-prod>`.

.. tip::

    Em vez de adicionar o filtro às tags de asset, você também pode configurar quais
    filtros aplicar em cada arquivo no arquivo de configuração da sua aplicação.
    Consulte :ref:`assetic-apply-to` para mais detalhes.

Instalando, Configurando e Usando o UglifyCSS
---------------------------------------------

O uso do UglifyCSS funciona da mesma maneira que o UglifyJS. Primeiro, certifique-se de que
o pacote do node está instalado:

.. code-block:: terminal

    # instalação global
    $ npm install -g uglifycss

    # instalação local
    $ cd /caminho/para/seu/projeto/symfony
    $ npm install uglifycss --prefix app/Resources

Em seguida, adicione a configuração para este filtro:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                uglifycss:
                    bin: /usr/local/bin/uglifycss

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config>
                <assetic:filter
                    name="uglifycss"
                    bin="/usr/local/bin/uglifycss" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'uglifycss' => array(
                    'bin' => '/usr/local/bin/uglifycss',
                ),
            ),
        ));

Para usar o filtro em seus arquivos CSS, adicione o filtro ao helper ``stylesheets``
do Assetic:

.. configuration-block::

    .. code-block:: html+twig

        {% stylesheets 'bundles/App/css/*' filter='uglifycss' filter='cssrewrite' %}
             <link rel="stylesheet" href="{{ asset_url }}" />
        {% endstylesheets %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->stylesheets(
            array('bundles/App/css/*'),
            array('uglifycss'),
            array('cssrewrite')
        ) as $url): ?>
            <link rel="stylesheet" href="<?php echo $view->escape($url) ?>" />
        <?php endforeach ?>

Assim como com o filtro ``uglifyjs2``, se você prefixar o nome do filtro com
``?`` (isto é, ``?uglifycss``), a minificação irá ocorrer apenas quando você
não estiver em modo de depuração.

.. _`UglifyJS`: https://github.com/mishoo/UglifyJS
.. _`UglifyCSS`: https://github.com/fmarcia/UglifyCSS
.. _`Node.js`: https://nodejs.org/
.. _`instalar o Node.js`: https://nodejs.org/
.. _`package.json`: http://browsenpm.org/package.json
