Como Minificar JavaScripts e Folhas de Estilo com o YUI Compressor
==================================================================

A Yahoo! oferece um excelente utilitário, chamado `YUI Compressor`_, para minificar 
JavaScripts e folhas de estilo, assim, eles são carregados mais rapidamente. 
Graças ao Assetic, você pode tirar proveito desta ferramenta de forma muito fácil.

Baixe o JAR do YUI Compressor 
-----------------------------

O YUI Compressor é escrito em Java e distribuído como um JAR. `Faça o download do JAR`_
no site da Yahoo! e salve-o em ``app/Resources/java/yuicompressor.jar``.

Configure os Filtros do YUI
---------------------------

Agora você precisa configurar dois filtros Assetic em sua aplicação, um para
minificar os JavaScripts com o YUI Compressor e um para minificar as
folhas de estilo:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                yui_css:
                    jar: "%kernel.root_dir%/Resources/java/yuicompressor.jar"
                yui_js:
                    jar: "%kernel.root_dir%/Resources/java/yuicompressor.jar"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="yui_css"
                jar="%kernel.root_dir%/Resources/java/yuicompressor.jar" />
            <assetic:filter
                name="yui_js"
                jar="%kernel.root_dir%/Resources/java/yuicompressor.jar" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'yui_css' => array(
                    'jar' => '%kernel.root_dir%/Resources/java/yuicompressor.jar',
                ),
                'yui_js' => array(
                    'jar' => '%kernel.root_dir%/Resources/java/yuicompressor.jar',
                ),
            ),
        ));

Você agora tem acesso a dois novos filtros Assetic em sua aplicação:
``yui_css`` e ``yui_js``. Eles utilizarão o YUI Compressor para minificar
as folhas de estilo e JavaScripts, respectivamente.

Minifique os seus Assets
------------------------

Você agora tem o YUI Compressor configurado, mas nada vai acontecer até
aplicar um desses filtros para um asset. Uma vez que os seus assets fazem 
parte da camada de visão, este trabalho é feito em seus templates:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/*' filter='yui_js' %}
        <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/*'),
            array('yui_js')) as $url): ?>
        <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>

.. note::

    O exemplo acima assume que você tem um bundle chamado ``AcmeFooBundle``
    e os seus arquivos JavaScript estão no diretório``Resources/public/js`` sob
    o seu bundle. Entretante, isso não é importante - você pode incluir os seus arquivos 
    JavaScript, não importa onde eles estiverem.

Com a adição do filtro ``yui_js`` para as tags asset acima, você deve agora ver 
os JavaScripts minificados sendo carregados muito mais rápido. O mesmo processo
pode ser repetido para minificar as suas folhas de estilo.

.. configuration-block::

    .. code-block:: html+jinja

        {% stylesheets '@AcmeFooBundle/Resources/public/css/*' filter='yui_css' %}
        <link rel="stylesheet" type="text/css" media="screen" href="{{ asset_url }}" />
        {% endstylesheets %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->stylesheets(
            array('@AcmeFooBundle/Resources/public/css/*'),
            array('yui_css')) as $url): ?>
        <link rel="stylesheet" type="text/css" media="screen" href="<?php echo $view->escape($url) ?>" />
        <?php endforeach; ?>

Desative a minificação no modo de depuração
-------------------------------------------

Os JavaScripts e as folhas de estilo minificados são muito difíceis de ler, e muito menos
depurar. Devido a isso, o Assetic permite desabilitar um certo filtro quando a sua
aplicação está no modo de depuração. Você pode fazer isso prefixando o nome do filtro
em seu template com um ponto de interrogação: ``?``. Isto diz ao Assetic para apenas
aplicar esse filtro quando o modo de depuração está desligado.

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/*' filter='?yui_js' %}
        <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/*'),
            array('?yui_js')) as $url): ?>
        <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>

.. _`YUI Compressor`: http://developer.yahoo.com/yui/compressor/
.. _`Faça o download do JAR`: http://yuilibrary.com/downloads/#yuicompressor
