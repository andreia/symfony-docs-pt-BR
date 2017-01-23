.. index::
   single: Assetic; Aplicar filtros

Como Aplicar um filtro Assetic a uma extensão de arquivo específica
===================================================================

.. include:: /assetic/_standard_edition_warning.rst.inc

Os filtros ``Assetic`` podem ser aplicados em arquivos individuais, grupos de arquivos ou até mesmo,
como você verá aqui, em arquivos que possuem uma extensão específica. Para mostrar como
lidar com cada opção, vamos supor que você deseja usar o filtro ``CoffeeScript`` do ``Assetic``, 
que compila arquivos ``CoffeeScript`` em Javascript.

A configuração principal é apenas os caminhos para ``coffee``, ``node`` e ``node_modules``.
Uma configuração de exemplo será parecida com a seguinte:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                coffee:
                    bin:        /usr/bin/coffee
                    node:       /usr/bin/node
                    node_paths: [/usr/lib/node_modules/]

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
                    name="coffee"
                    bin="/usr/bin/coffee/"
                    node="/usr/bin/node/">
                    <assetic:node-path>/usr/lib/node_modules/</assetic:node-path>
                </assetic:filter>
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'coffee' => array(
                    'bin'  => '/usr/bin/coffee',
                    'node' => '/usr/bin/node',
                    'node_paths' => array('/usr/lib/node_modules/'),
                ),
            ),
        ));

Filtrando um único arquivo
--------------------------

Agora, você pode servir um único arquivo ``CoffeeScript`` como JavaScript a partir de seus
templates:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/example.coffee' filter='coffee' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/example.coffee'),
            array('coffee')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Isso é tudo o que é necessário para compilar este arquivo ``CoffeeScript`` e servir ele  
como JavaScript compilado.

Filtrando vários arquivos
-------------------------

Você também pode combinar vários arquivos ``CoffeeScript`` em um único arquivo de saída:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/example.coffee'
                       '@AppBundle/Resources/public/js/another.coffee'
            filter='coffee' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@AppBundle/Resources/public/js/example.coffee',
                '@AppBundle/Resources/public/js/another.coffee',
            ),
            array('coffee')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Ambos os arquivos agora serão servidos como um único arquivo compilado em
JavaScript regular.

.. _assetic-apply-to:

Filtrando com base em uma extensão de arquivo
---------------------------------------------

Uma das grandes vantagens de usar o ``Assetic`` é minimizar o número de arquivos
``asset`` para reduzir as solicitações HTTP. A fim de fazer seu pleno uso, seria
bom combinar *todos* os seus arquivos JavaScript e ``CoffeeScript`` juntos,
uma vez que, todos serão servidos como JavaScript. Infelizmente, apenas
adicionar os arquivos JavaScript aos arquivos a serem combinados como acima não funcionará,
pois, os arquivos JavaScript regulares não sobreviverão a compilação do ``CoffeeScript``.

Esse problema pode ser evitado usando a opção ``apply_to``, que permite especificar
que filtro deverá ser sempre aplicado à determinadas extensões de arquivo.
Nesse caso, você pode especificar que o filtro ``coffee`` será
aplicado em todos os arquivos ``.coffee``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                coffee:
                    bin:        /usr/bin/coffee
                    node:       /usr/bin/node
                    node_paths: [/usr/lib/node_modules/]
                    apply_to:   '\.coffee$'

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
                    name="coffee"
                    bin="/usr/bin/coffee"
                    node="/usr/bin/node"
                    apply_to="\.coffee$" />
                    <assetic:node-paths>/usr/lib/node_modules/</assetic:node-path>
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'coffee' => array(
                    'bin'      => '/usr/bin/coffee',
                    'node'     => '/usr/bin/node',
                    'node_paths' => array('/usr/lib/node_modules/'),
                    'apply_to' => '\.coffee$',
                ),
            ),
        ));

Com essa opção, você não precisa especificar o filtro ``coffee`` no template.
Você também pode listar os arquivos JavaScript regulares, todos os quais serão combinados
e renderizados como um único arquivo JavaScript (apenas com os arquivos ``.coffee`` 
sendo executados através do filtro ``CoffeeScript``):

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/example.coffee'
                       '@AppBundle/Resources/public/js/another.coffee'
                       '@AppBundle/Resources/public/js/regular.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@AppBundle/Resources/public/js/example.coffee',
                '@AppBundle/Resources/public/js/another.coffee',
                '@AppBundle/Resources/public/js/regular.js',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>
