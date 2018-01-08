.. index::
    single: Front-end; Assetic, Bootstrap

Combinando, Compilando e Minificando Assets Web com Bibliotecas PHP
===================================================================

.. include:: /assetic/_standard_edition_warning.rst.inc

As Melhores Práticas Oficiais do Symfony recomendam o uso do Assetic para
:doc:`gerenciar assets web </best_practices/web-assets>`, a menos que você se sinta
confortável com ferramentas front-end baseadas em JavaScript.

Mesmo se estas soluções baseadas em JavaScript forem as mais adequadas do
ponto de vista técnico, o uso de bibliotecas alternativas em PHP puro pode ser útil em
alguns cenários:

* Se você não pode instalar ou usar o ``npm`` e as outras soluções JavaScript;
* Se você prefere limitar a quantidade de tecnologias diferentes usadas em suas
  aplicações;
* Se você quer simplificar a implantação da aplicação.

Neste artigo, você aprenderá como combinar e minificar arquivos CSS e JavaScript
e como compilar arquivos Sass usando bibliotecas puramente em PHP com o Assetic.

Instalando as Bibliotecas de Compressão de Terceiros
----------------------------------------------------

O Assetic inclui vários filtros prontos para usar, mas não inclui as bibliotecas associadas
a estes filtros. Portanto, antes de habilitar os filtros usados neste artigo,
você deve instalar duas bibliotecas. Abra um console de comando, navegue até o diretório do seu
projeto e execute os seguintes comandos:

.. code-block:: terminal

    $ composer require leafo/scssphp
    $ composer require patchwork/jsqueeze

Organizando seus Arquivos Asset Web
-----------------------------------

Este exemplo irá incluir uma configuração usando o framework Bootstrap, jQuery, FontAwesome
e alguns arquivos CSS e JavaScript comuns da aplicação (chamados ``main.css`` e
``main.js``). A estrutura de diretório recomendada para esta configuração parece com a seguinte:

.. code-block:: text

    web/assets/
    ├── css
    │   ├── main.css
    │   └── code-highlight.css
    ├── js
    │   ├── bootstrap.js
    │   ├── jquery.js
    │   └── main.js
    └── scss
        ├── bootstrap
        │   ├── _alerts.scss
        │   ├── ...
        │   ├── _variables.scss
        │   ├── _wells.scss
        │   └── mixins
        │       ├── _alerts.scss
        │       ├── ...
        │       └── _vendor-prefixes.scss
        ├── bootstrap.scss
        ├── font-awesome
        │   ├── _animated.scss
        │   ├── ...
        │   └── _variables.scss
        └── font-awesome.scss

Combinando e Minificando Arquivos CSS e Compilando Arquivos SCSS
----------------------------------------------------------------

Primeiro, configure um novo filtro ``scssphp`` do Assetic:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                scssphp:
                    formatter: 'Leafo\ScssPhp\Formatter\Compressed'
                # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config>
                <assetic:filter name="scssphp" formatter="Leafo\ScssPhp\Formatter\Compressed" />
                <!-- ... -->
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                 'scssphp' => array(
                     'formatter' => 'Leafo\ScssPhp\Formatter\Compressed',
                 ),
                 // ...
            ),
        ));

O valor da opção ``formatter`` é o nome de classe totalmente qualificado do
formatador usado pelo filtro para produzir o arquivo CSS compilado. O uso do
formatador compactado irá minificar o arquivo resultante, independentemente dos
arquivos originais serem arquivos CSS comuns ou arquivos SCSS.

Em seguida, atualize seu template Twig para adicionar a tag ``{% stylesheets %}`` definida
pelo Assetic:

.. code-block:: html+twig

    {# app/Resources/views/base.html.twig #}
    <!DOCTYPE html>
    <html>
        <head>
            <!-- ... -->

            {% stylesheets filter="scssphp" output="css/app.css"
                "assets/scss/bootstrap.scss"
                "assets/scss/font-awesome.scss"
                "assets/css/*.css"
            %}
                <link rel="stylesheet" href="{{ asset_url }}" />
            {% endstylesheets %}

Esta simples configuração compila, combina e minifica os arquivos SCSS em um
arquivo CSS comum que é colocado em ``web/css/app.css``. Este é o único arquivo CSS
que será servido aos seus visitantes.

Combinando e Minificando Arquivos JavaScript
--------------------------------------------

Primeiro, configure um novo filtro ``jsqueeze`` do Assetic da seguinte maneira:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jsqueeze: ~
                # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config>
                <assetic:filter name="jsqueeze" />
                <!-- ... -->
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                 'jsqueeze' => null,
                 // ...
            ),
        ));

Em seguida, atualize o código do seu template Twig para adicionar a tag ``{% javascripts %}``
definida pelo Assetic:

.. code-block:: html+twig

    <!-- ... -->

        {% javascripts filter="?jsqueeze" output="js/app.js"
            "assets/js/jquery.js"
            "assets/js/bootstrap.js"
            "assets/js/main.js"
        %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

        </body>
    </html>

Esta simples configuração combina todos os arquivos JavaScript, minifica o conteúdo
e salva a saída no arquivo ``web/js/app.js``, que é o arquivo
servido aos seus visitantes.

O caractere ``?`` inicial no nome do filtro ``jsqueeze`` diz ao Assetic para aplicar
o filtro apenas quando *não estiver* em modo ``debug``. Na prática, isso significa que você
verá arquivos não minificados enquanto desenvolve e arquivos minificados no ambiente ``prod``.
