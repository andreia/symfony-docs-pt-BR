.. index::
   single: Assetic; YUI Compressor

Como Minificar JavaScripts e Folhas de Estilo com o YUI Compressor
==================================================================

.. caution::

    O YUI Compressor `não é mais mantido pela Yahoo`_. É por isso que você é
    **fortemente aconselhado a evitar o uso dos utilitários YUI**, a menos que seja estritamente necessário.
    Leia :doc:`/frontend/assetic/uglifyjs` para uma alternativa moderna e atualizada.

.. include:: /assetic/_standard_edition_warning.rst.inc

A Yahoo! oferece um excelente utilitário para minificar JavaScripts e folhas de estilo
para que eles sejam carregados mais rápido, o `YUI Compressor`_. Graças ao Assetic,
você pode tirar proveito desta ferramenta de forma muito fácil.

Baixe o JAR do YUI Compressor 
-----------------------------

O YUI Compressor é escrito em Java e distribuído como um JAR. `Faça o download do JAR`_
no site da Yahoo! e salve-o em ``app/Resources/java/yuicompressor.jar``.

Configure os Filtros do YUI
---------------------------

Agora você precisa configurar dois filtros do Assetic em sua aplicação, um para
minificar os JavaScripts com o YUI Compressor e um para minificar as
folhas de estilo:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            # java: '/usr/bin/java'
            filters:
                yui_css:
                    jar: '%kernel.project_dir%/app/Resources/java/yuicompressor.jar'
                yui_js:
                    jar: '%kernel.project_dir%/app/Resources/java/yuicompressor.jar'

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
                    name="yui_css"
                    jar="%kernel.project_dir%/app/Resources/java/yuicompressor.jar" />
                <assetic:filter
                    name="yui_js"
                    jar="%kernel.project_dir%/app/Resources/java/yuicompressor.jar" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            // 'java' => '/usr/bin/java',
            'filters' => array(
                'yui_css' => array(
                    'jar' => '%kernel.project_dir%/app/Resources/java/yuicompressor.jar',
                ),
                'yui_js' => array(
                    'jar' => '%kernel.project_dir%/app/Resources/java/yuicompressor.jar',
                ),
            ),
        ));

.. note::

    Usuários do Windows precisam lembrar de atualizar a configuração para a localização correta do Java.
    No Windows7 x64 bit, por padrão, é ``C:\Program Files (x86)\Java\jre6\bin\java.exe``.

Você agora tem acesso a dois novos filtros do Assetic em sua aplicação:
``yui_css`` e ``yui_js``. Eles utilizarão o YUI Compressor para minificar
as folhas de estilo e os JavaScripts, respectivamente.

Minifique os seus Assets
------------------------

Você agora tem o YUI Compressor configurado, mas nada vai acontecer até
você aplicar um desses filtros em um asset. Uma vez que os seus assets fazem parte
da camada de visão, este trabalho é feito em seus templates:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' filter='yui_js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('yui_js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    O exemplo acima assume que você tem um bundle chamado AppBundle e os seus
    arquivos JavaScript estão no diretório ``Resources/public/js`` do seu
    bundle. Entretanto, isso não é importante - você pode incluir os seus arquivos
    JavaScript, não importa onde eles estão.

Com a adição do filtro ``yui_js`` nas tags de asset acima, você agora
deve ver os JavaScripts minificados sendo carregados muito mais rápido. O mesmo processo
pode ser repetido para minificar as suas folhas de estilo.

.. configuration-block::

    .. code-block:: html+twig

        {% stylesheets '@AppBundle/Resources/public/css/*' filter='yui_css' %}
            <link rel="stylesheet" type="text/css" media="screen" href="{{ asset_url }}" />
        {% endstylesheets %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->stylesheets(
            array('@AppBundle/Resources/public/css/*'),
            array('yui_css')
        ) as $url): ?>
            <link rel="stylesheet" type="text/css" media="screen" href="<?php echo $view->escape($url) ?>" />
        <?php endforeach ?>

Desativando a Minificação no Modo de Depuração
----------------------------------------------

Os JavaScripts e as folhas de estilo minificados são muito difíceis de ler, e mais ainda de
depurar. Por isso, o Assetic permite desabilitar um certo filtro quando a sua
aplicação está no modo de depuração. Você pode fazer isso prefixando o nome do filtro
em seu template com um ponto de interrogação: ``?``. Isto diz ao Assetic para aplicar
esse filtro apenas quando o modo de depuração estiver desligado.

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' filter='?yui_js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('?yui_js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. tip::

    Em vez de adicionar o filtro às tags de asset, você também pode habilitá-lo
    globalmente adicionando o atributo ``apply_to`` à configuração do filtro, por
    exemplo, no filtro ``yui_js`` adicione ``apply_to: "\.js$"``. Para aplicar o filtro
    apenas em produção, adicione isso ao arquivo ``config_prod``, em vez do
    arquivo de configuração comum. Para obter detalhes sobre a aplicação de filtros por extensão de arquivo,
    consulte :ref:`assetic-apply-to`.

.. _`YUI Compressor`: http://yui.github.io/yuicompressor/
.. _`Faça o download do JAR`: https://github.com/yui/yuicompressor/releases
.. _`não é mais mantido pela Yahoo`: http://yuiblog.com/blog/2013/01/24/yui-compressor-has-a-new-owner/
