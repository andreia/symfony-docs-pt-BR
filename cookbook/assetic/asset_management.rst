.. index::
   single: Assetic; Introdução

Como usar o Assetic para o Gerenciamento de Assets
==================================================

O Assetic combina duas idéias principais: assets e filtros. Os assets são arquivos
CSS, JavaScript e arquivos de imagem. Os filtros são coisas que podem ser aplicadas 
à esses arquivos antes deles serem servidos ao navegador. Isto permite uma separação 
entre os arquivos asset armazenados na aplicação e os arquivos que são efetivamente 
apresentados ao usuário.

Sem o Assetic, você somente poderia servir os arquivos que são armazenados diretamente 
na aplicação:

.. configuration-block::

    .. code-block:: html+jinja

        <script src="{{ asset('js/script.js') }}" type="text/javascript" />

    .. code-block:: php

        <script src="<?php echo $view['assets']->getUrl('js/script.js') ?>" type="text/javascript" />

Mas *com* o Assetic, você pode manipular esses assets da forma que desejar (ou
carregá-los de qualquer lugar) antes de serví-los. Isto significa que você pode:

* Minificar e combinar todos os seus arquivos CSS e JS

* Executar todos (ou apenas alguns) dos seus arquivos CSS ou JS através de algum tipo de 
  compilador, como o LESS, SASS ou CoffeeScript

* Executar otimizações em suas imagens

Assets
------

O uso do Assetic oferece muitas vantagens sobre servir diretamente os arquivos.
Os arquivos não precisam ser armazenados onde eles são servidos e podem ser
buscados a partir de várias fontes, como, por exemplo, a partir de um bundle:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/*' %}
            <script type="text/javascript" src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/*')) as $url): ?>
            <script type="text/javascript" src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>

.. tip::

    Para buscar folhas de estilo CSS, você pode usar as mesmas metodologias vistas
    aqui, exceto com a tag `stylesheets` :

    .. configuration-block::

        .. code-block:: html+jinja

            {% stylesheets '@AcmeFooBundle/Resources/public/css/*' %}
                <link rel="stylesheet" href="{{ asset_url }}" />
            {% endstylesheets %}

        .. code-block:: html+php

            <?php foreach ($view['assetic']->stylesheets(
                                                 array('@AcmeFooBundle/Resources/public/css/*')
                                             ) as $url): ?>
                <link rel="stylesheet" href="<?php echo $view->escape($url) ?>" />
            <?php endforeach; ?>

Neste exemplo, todos os arquivos no diretório ``Resources/public/js/``
do ``AcmeFooBundle`` serão carregados e servidos em um local diferente.
A tag atual renderizada pode parecer simplesmente com:

.. code-block:: html

    <script src="/app_dev.php/js/abcd123.js"></script>

.. note::

    Este é um ponto-chave: uma vez que você deixar o Assetic lidar com seus assets, os arquivos são
    servidos a partir de um local diferente. Isto *pode* causar problemas com os arquivos CSS
    que referenciam imagens pelo seu caminho relativo. No entanto, isso pode ser corrigido
    usando o filtro ``cssrewrite``, que atualiza os caminhos nos arquivos CSS
    para refletir a sua nova localização.

Combinando Assets
~~~~~~~~~~~~~~~~~

Você também pode combinar vários arquivos em um único. Isto ajuda a reduzir o número
de solicitações HTTP, o que é ótimo para o desempenho front-end. Também permite que 
você mantenha os arquivos mais facilmente, dividindo-os em partes gerenciáveis.
Isso pode ajudar com a possibilidade de reutilização, uma vez que você pode facilmente 
dividir os arquivos específicos do projeto daqueles que podem ser usados ​​em outras aplicações, 
mas ainda serví-los como um único arquivo:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts
            '@AcmeFooBundle/Resources/public/js/*'
            '@AcmeBarBundle/Resources/public/js/form.js'
            '@AcmeBarBundle/Resources/public/js/calendar.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/*',
                  '@AcmeBarBundle/Resources/public/js/form.js',
                  '@AcmeBarBundle/Resources/public/js/calendar.js')) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>

No ambiente `dev`, cada arquivo ainda é servido individualmente, de modo que
você pode depurar problemas mais facilmente. No entanto, no ambiente `prod`, serão
processados como uma única tag `script`.

.. tip::

    Se você é novo no Assetic e tentar usar a sua aplicação no ambiente 
    ``prod`` (utilizando o controlador ``app.php``), você provavelmente verá
    que todos os seus CSS e JS estão corrompidos. Não se preocupe! Isso é de propósito.
    Para detalhes sobre a utilização do Assetic no ambiente `prod`, consulte :ref:`cookbook-assetic-dumping`.

E a combinação de arquivos não se aplica apenas para *seus* arquivos. Você também pode usar o Assetic
para combinar assets de terceiros, tais como jQuery, como seu próprio em um único arquivo:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts
            '@AcmeFooBundle/Resources/public/js/thirdparty/jquery.js'
            '@AcmeFooBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/thirdparty/jquery.js',
                  '@AcmeFooBundle/Resources/public/js/*')) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>

Filtros 
-------

Uma vez que são gerenciados pelo Assetic, você pode aplicar filtros em seus assets antes
deles serem servidos. Isso inclui filtros que comprimem a saída de seus assets
para tamanhos de arquivos menores (e melhor otimização do front-end). Outros filtros
podem compilar os arquivos JavaScript a partir de arquivos CoffeeScript e processar SASS em CSS.
Na verdade, o Assetic tem uma longa lista de filtros disponíveis.

Muitos dos filtros não fazem o trabalho diretamente, mas usam bibliotecas existentes de terceiros
para fazer o trabalho pesado. Isto significa que muitas vezes você vai precisar instalar
uma biblioteca de terceiro para usar um filtro.  A grande vantagem de usar o Assetic
para chamar estas bibliotecas (em oposição a usá-las diretamente) é que, em vez
de ter que executá-las manualmente depois de trabalhar nos arquivos, o Assetic irá
cuidar disto para você e remover completamente esta etapa do seu processo de desenvolvimento
e implantação.

Para usar um filtro, primeiro você precisa especificá-lo na configuração do Assetic.
Adicionar um filtro aqui não significa que ele está sendo usado - apenas significa que está
disponível para uso (vamos usar o filtro abaixo).

Por exemplo, para usar o JavaScript YUI Compressor, a configuração seguinte deve
ser acrescentada:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                yui_js:
                    jar: "%kernel.root_dir%/Resources/java/yuicompressor.jar"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="yui_js"
                jar="%kernel.root_dir%/Resources/java/yuicompressor.jar" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'yui_js' => array(
                    'jar' => '%kernel.root_dir%/Resources/java/yuicompressor.jar',
                ),
            ),
        ));

Agora, para efetivamente *usar* o filtro em um grupo de arquivos JavaScript, adicione-o
em seu template:

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

Um guia mais detalhado sobre a configuração e uso dos filtros Assetic, bem como
detalhes do modo de depuração do Assetic pode ser encontrado em :doc:`/cookbook/assetic/yuicompressor`.

Controlando a URL usada
-----------------------

Se desejar, você pode controlar as URLs que o Assetic produz. Isto é
feito a partir do template e é relativo à raiz do documento público:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/*' output='js/compiled/main.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/*'),
            array(),
            array('output' => 'js/compiled/main.js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>

.. note::

    O Symfony também contém um método de *busting* de cache, onde a URL final
    gerada pelo Assetic contém um parâmetro de query, que pode ser incrementado
    através de configuração em cada implantação. Para mais informações, consulte 
    a opção de configuração :ref:`ref-framework-assets-version`.

.. _cookbook-assetic-dumping:

Dump dos arquivos de asset
--------------------------

No ambiente ``dev``, o Assetic gera caminhos para os arquivos CSS e JavaScript
que não existem fisicamente em seu computador. Mas, eles renderizam mesmo assim 
porque um controlador interno do Symfony abre os arquivos e serve de volta o
conteúdo (após a execução de quaisquer filtros).

Este tipo de publicação dinâmica dos assets processados ​​é ótima porque significa que 
você pode ver imediatamente o novo estado de quaisquer arquivos de assets que foram alterados.
Também é ruim, porque pode ser muito lento. Se você estiver usando uma série de filtros,
pode ser francamente frustrante.

Felizmente, o Assetic fornece uma forma de fazer o dump de seus assets para arquivos reais, 
em vez de ser gerado dinamicamente.

Dump dos arquivos asset no ambiente ``prod``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No ambiente ``prod``, seus JS e CSS são representados por uma única tag cada. 
Em outras palavras, em vez de ver cada arquivo JavaScript que você está incluindo
no seu código fonte, é provável que você só veja algo semelhante a:

.. code-block:: html

    <script src="/app_dev.php/js/abcd123.js"></script>

Além disso, esse arquivo **não** existe realmente, nem é renderizado de forma dinâmica
pelo Symfony (pois os arquivos de asset estão no ambiente ``dev``). Isto é de 
propósito - deixar o Symfony gerar esses arquivos dinamicamente em um ambiente de produção
é muito lento.

Em vez disso, cada vez que você usar a sua aplicação no ambiente ``prod `` (e, portanto,
cada vez que você implantar), você deve executar o seguinte comando:

.. code-block:: bash

    $ php app/console assetic:dump --env=prod --no-debug

Isso vai gerar fisicamente e escrever cada arquivo que você precisa (por exemplo, ``/js/abcd123.js``).
Se você atualizar qualquer um de seus assets, é necessário executar o comando novamente para gerar 
o novo arquivo.

Dump dos arquivos de asset no ambiente ``dev``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, cada caminho de asset gerado no ambiente ``dev`` é gerenciado
dinamicamente pelo Symfony. Isso não tem desvantagem (você pode ver as suas alterações
imediatamente), com exceção de que os assets podem visivelmente carregar mais lentos. Se você 
sentir que seus assets estão carregando muito lentamente, siga este guia.

Primeiro, diga ao Symfony para parar de tentar processar estes arquivos dinamicamente. Faça
a seguinte alteração em seu arquivo ``config_dev.yml``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        assetic:
            use_controller: false

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->
        <assetic:config use-controller="false" />

    .. code-block:: php

        // app/config/config_dev.php
        $container->loadFromExtension('assetic', array(
            'use_controller' => false,
        ));

Em seguida, uma vez que o Symfony não está mais gerando esses assets para você, você vai
precisar fazer o dump deles manualmente. Para isso, execute o seguinte:

.. code-block:: bash

    $ php app/console assetic:dump

Esta fisicamente grava todos os arquivos ativos que você precisa para seu `` `` dev
produção. A grande desvantagem é que você precisa executar este cada vez
você atualizar um ativo. Felizmente, passando o `` - assistir opção ``, o
comando automaticamente regenerar ativos * como eles mudam *:

.. code-block:: bash

    $ php app/console assetic:dump --watch

Uma vez que executar este comando no ambiente ``dev`` pode gerar vários arquivos,  
geralmente é uma boa idéia apontar os seus arquivos assets gerados para algum diretório 
isolado (por exemplo, ``/js/compiled``), para manter as coisas organizadas:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AcmeFooBundle/Resources/public/js/*' output='js/compiled/main.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AcmeFooBundle/Resources/public/js/*'),
            array(),
            array('output' => 'js/compiled/main.js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach; ?>
