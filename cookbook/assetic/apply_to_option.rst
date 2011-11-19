Como Aplicar um filtro Assetic a uma extensão de arquivo específica
===================================================================

Os filtros ``Assetic`` podem ser aplicados à arquivos individuais, grupos de arquivos ou até mesmo,
como você verá aqui, aos arquivos que possuem uma extensão específica. Para mostrar como
lidar com cada opção, vamos supor que você deseja usar o filtro ``CoffeeScript`` do ``Assetic``, 
que compila arquivos ``CoffeeScript`` em Javascript.

A configuração principal é apenas os caminhos para ``coffee`` e ``node``. Elas apontam, por padrão, 
para ``/usr/bin/coffee`` e ``/usr/bin/node``, respectivamente:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                coffee:
                    bin: /usr/bin/coffee
                    node: /usr/bin/node

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="coffee"
                bin="/usr/bin/coffee"
                node="/usr/bin/node" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'coffee' => array(
                    'bin' => '/usr/bin/coffee',
                    'node' => '/usr/bin/node',
                ),
            ),
        ));

Filtrando um único arquivo
--------------------------

Agora, você pode servir um único arquivo ``CoffeeScript`` como JavaScript a partir de seus
templates:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/example.coffee'
            filter='coffee'
        %}
        <script src="{{ asset_url }} type="text/javascript"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/example.coffee'),
            array('coffee')) as $url): ?>
        <script src="<?php echo $view->escape($url) ?>" type="text/javascript"></script>
        <?php endforeach; ?>

Isso é tudo o que é necessário para compilar este arquivo ``CoffeeScript`` e servir ele  
como JavaScript compilado.

Filtrando vários arquivos
-------------------------

Você também pode combinar vários arquivos ``CoffeeScript`` em um único arquivo de saída:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/example.coffee'
                       '@AcmeFooBundle/Resources/public/js/another.coffee'
            filter='coffee'
        %}
        <script src="{{ asset_url }} type="text/javascript"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/example.coffee',
                  '@AcmeFooBundle/Resources/public/js/another.coffee'),
            array('coffee')) as $url): ?>
        <script src="<?php echo $view->escape($url) ?>" type="text/javascript"></script>
        <?php endforeach; ?>

Ambos os arquivos agora serão servidos como um único arquivo compilado em 
JavaScript regular.

Filtrando com base em uma extensão de arquivo
---------------------------------------------

Uma das grandes vantagens de usar o ``Assetic`` é minimizar o número de arquivos
``asset`` para reduzir as solicitações HTTP. A fim de fazer seu pleno uso, seria
bom combinar *todos* os seus arquivos JavaScript e ``CoffeeScript`` juntos,
uma vez que, todos serão servidos como JavaScript. Infelizmente, apenas
adicionar os arquivos JavaScript aos arquivos combinados como acima não funcionará,
pois, os arquivos JavaScript regulares não sobreviverão a compilação do ``CoffeeScript``.

Este problema pode ser evitado usando a opção ``apply_to`` na configuração,
que permite especificar que filtro deverá ser sempre aplicado à determinadas
extensões de arquivo. Neste caso, você pode especificar que o filtro ``Coffee`` será
aplicado à todos os arquivos ``.coffee``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                coffee:
                    bin: /usr/bin/coffee
                    node: /usr/bin/node
                    apply_to: "\.coffee$"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="coffee"
                bin="/usr/bin/coffee"
                node="/usr/bin/node"
                apply_to="\.coffee$" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'coffee' => array(
                    'bin' => '/usr/bin/coffee',
                    'node' => '/usr/bin/node',
                    'apply_to' => '\.coffee$',
                ),
            ),
        ));

Com isso, você não precisa especificar o filtro ``coffee`` no template.
Você também pode listar os arquivos JavaScript regulares, todos os quais serão combinados
e renderizados como um único arquivo JavaScript (apenas com os arquivos ``.coffee`` 
sendo executados através do filtro ``CoffeeScript``):

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/example.coffee'
                       '@AcmeFooBundle/Resources/public/js/another.coffee'
                       '@AcmeFooBundle/Resources/public/js/regular.js'
        %}
        <script src="{{ asset_url }} type="text/javascript"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/example.coffee',
                  '@AcmeFooBundle/Resources/public/js/another.coffee',
                  '@AcmeFooBundle/Resources/public/js/regular.js'),
            as $url): ?>
        <script src="<?php echo $view->escape($url) ?>" type="text/javascript"></script>
        <?php endforeach; ?>
