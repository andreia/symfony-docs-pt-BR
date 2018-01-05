.. index::
   single: Assetic; Introdução

Como Usar o Assetic para o Gerenciamento de Assets
==================================================

Instalando e Ativando o Assetic
-------------------------------

A partir do Symfony 2.8, o Assetic não é mais incluído por padrão na
Edição Standard do Symfony. Antes de utilizar qualquer um de seus recursos, instale o
AsseticBundle executando o seguinte comando de console no seu projeto:

.. code-block:: bash

    $ composer require symfony/assetic-bundle

Então, ative o bundle no arquivo ``AppKernel.php`` da sua aplicação Symfony::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        // ...

        public function registerBundles()
        {
            $bundles = array(
                // ...
                new Symfony\Bundle\AsseticBundle\AsseticBundle(),
            );

            // ...
        }
    }

Finalmente, adicione a seguinte configuração mínima para ativar o suporte ao Assetic na
sua aplicação:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            debug:          '%kernel.debug%'
            use_controller: '%kernel.debug%'
            filters:
                cssrewrite: ~

        # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config debug="%kernel.debug%" use-controller="%kernel.debug%">
                <assetic:filters cssrewrite="null" />
            </assetic:config>

            <!-- ... -->
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'debug' => '%kernel.debug%',
            'use_controller' => '%kernel.debug%',
            'filters' => array(
                'cssrewrite' => null,
            ),
            // ...
        ));

        // ...

Apresentando o Assetic
----------------------

O Assetic combina duas idéias principais: :ref:`assets <assetic-assets>` e
:ref:`filtros <assetic-filters>`. Os assets são arquivos como CSS,
JavaScript e arquivos de imagem. Os filtros podem ser aplicados a
estes arquivos antes deles serem servidos ao navegador. Isso permite uma separação
entre os arquivos asset armazenados na aplicação e os arquivos que são efetivamente apresentados
ao usuário.

Sem o Assetic, você apenas serve os arquivos que estão armazenados na aplicação
diretamente:

.. configuration-block::

    .. code-block:: html+twig

        <script src="{{ asset('js/script.js') }}"></script>

    .. code-block:: php

        <script src="<?php echo $view['assets']->getUrl('js/script.js') ?>"></script>

Mas *com* o Assetic, você pode manipular estes assets da forma que desejar (ou
carregá-los de qualquer lugar) antes de servi-los. Isso significa que você pode:

* Minificar e combinar todos os seus arquivos CSS e JS

* Passar todos (ou apenas alguns) dos seus arquivos CSS ou JS através de algum tipo de compilador,
  como o LESS, SASS ou CoffeeScript

* Executar otimizações em suas imagens

.. _assetic-assets:

Assets
------

O uso do Assetic oferece muitas vantagens sobre servir diretamente os arquivos.
Os arquivos não precisam ser armazenados onde eles são servidos e podem ser
obtidos a partir de várias fontes, como, por exemplo, a partir de um bundle.

Você pode usar o Assetic para processar :ref:`folhas de estilo CSS <assetic-including-css>`,
:ref:`arquivos JavaScript <assetic-including-javascript>` e
:ref:`imagens <assetic-including-image>`. A filosofia
por trás da adição é basicamente a mesma, mas com uma sintaxe ligeiramente diferente.

.. _assetic-including-javascript:

Incluindo Arquivos JavaScript
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Para incluir arquivos JavaScript, use a tag ``javascripts`` em qualquer template:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    Se os templates da sua aplicação usam os nomes de bloco padrão da Edição
    Standard do Symfony, a tag ``javascripts`` provavelmente estará no bloco
    ``javascripts``:

    .. code-block:: html+twig

        {# ... #}
        {% block javascripts %}
            {% javascripts '@AppBundle/Resources/public/js/*' %}
                <script src="{{ asset_url }}"></script>
            {% endjavascripts %}
        {% endblock %}
        {# ... #}

.. tip::

    Você também pode incluir folhas de estilo CSS: consulte :ref:`assetic-including-css`.

Neste exemplo, todos os arquivos no diretório ``Resources/public/js/`` do
AppBundle serão carregados e servidos a partir de um local diferente. A tag
renderizada real pode ser simplesmente:

.. code-block:: html

    <script src="/app_dev.php/js/abcd123.js"></script>

Este é um ponto chave: uma vez que você deixe o Assetic lidar com seus assets, os arquivos são
servidos a partir de um local diferente. Isto *causará* problemas com arquivos CSS
que referenciam imagens pelo seu caminho relativo. Consulte :ref:`assetic-cssrewrite`.

.. _assetic-including-css:

Incluindo Folhas de Estilo CSS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Para incluir folhas de estilo CSS, você pode usar a mesma técnica explicada acima,
mas com a tag ``stylesheets``:

.. configuration-block::

    .. code-block:: html+twig

        {% stylesheets 'bundles/app/css/*' filter='cssrewrite' %}
            <link rel="stylesheet" href="{{ asset_url }}" />
        {% endstylesheets %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->stylesheets(
            array('bundles/app/css/*'),
            array('cssrewrite')
        ) as $url): ?>
            <link rel="stylesheet" href="<?php echo $view->escape($url) ?>" />
        <?php endforeach ?>

.. note::

    Se os templates da sua aplicação usam os nomes de bloco padrão da Edição
    Standard do Symfony, a tag ``stylesheets`` provavelmente estará no bloco
    ``stylesheets``:

    .. code-block:: html+twig

        {# ... #}
        {% block stylesheets %}
            {% stylesheets 'bundles/app/css/*' filter='cssrewrite' %}
                <link rel="stylesheet" href="{{ asset_url }}" />
            {% endstylesheets %}
        {% endblock %}
        {# ... #}

Mas devido ao Assetic mudar os caminhos para seus assets, isto *quebrará* quaisquer
imagens de background (ou outros caminhos) que usam caminhos relativos, a menos que você use
o filtro :ref:`cssrewrite <assetic-cssrewrite>`.

.. note::

    Observe que exemplo original que incluiu arquivos JavaScript, você
    se referiu aos arquivos usando um caminho como ``@AppBundle/Resources/public/file.js``,
    mas que neste exemplo, você se referiu aos arquivos CSS usando seus caminhos reais,
    publicamente acessíveis: ``bundles/app/css``. Você pode usar qualquer um, exceto
    que existe um problema conhecido que faz com que o filtro ``cssrewrite`` falhe
    ao usar a sintaxe ``@AppBundle`` para folhas de estilo CSS.

.. _assetic-including-image:

Incluindo Imagens
~~~~~~~~~~~~~~~~~

Para incluir uma imagem você pode usar a tag ``image``.

.. configuration-block::

    .. code-block:: html+twig

        {% image '@AppBundle/Resources/public/images/example.jpg' %}
            <img src="{{ asset_url }}" alt="Example" />
        {% endimage %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->image(
            array('@AppBundle/Resources/public/images/example.jpg')
        ) as $url): ?>
            <img src="<?php echo $view->escape($url) ?>" alt="Example" />
        <?php endforeach ?>

Você pode também usar o Assetic para otimização de imagens. Mais informações em
:doc:`/frontend/assetic/jpeg_optimize`.

.. tip::

    Ao invés de usar o Assetic para incluir imagens, você pode considerar usar o bundle
    `LiipImagineBundle`_ da comunidade, que permite comprimir e
    manipular imagens (rotacionar, redimencionar, aplicar marca d'água, etc.) antes de servi-las.

.. _assetic-cssrewrite:

Corrigindo Caminhos CSS com o Filtro ``cssrewrite``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Como o Assetic gera novos URLs para seus assets, qualquer caminho relativo dentro dos
seus arquivos CSS irá quebrar. Para corrigir isso, certifique-se de usar o filtro ``cssrewrite``
com sua tag ``stylesheets``. Ele analisa seus arquivos CSS e corrige
os caminhos internamente para refletir a nova localização.

Você pode ver um exemplo na seção anterior.

.. caution::

    Ao usar o filtro ``cssrewrite``, não referencie seus arquivos CSS usando
    a sintaxe ``@AppBundle``. Veja a nota na seção anterior para obter detalhes.

Combinando Assets
~~~~~~~~~~~~~~~~~

Um recurso do Assetic é que ele irá combinar vários arquivos em um único. Isto ajuda
a reduzir o número de requisições HTTP, o que é ótimo para o desempenho front-end.
Ele também permite que você mantenha os arquivos mais facilmente, dividindo-os em
partes gerenciáveis. Isso pode ajudar com a reusabilidade, uma vez que você pode facilmente separar
os arquivos específicos do projeto daqueles que podem ser usados em outras aplicações,
mas ainda servi-los como um único arquivo:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts
            '@AppBundle/Resources/public/js/*'
            '@AcmeBarBundle/Resources/public/js/form.js'
            '@AcmeBarBundle/Resources/public/js/calendar.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@AppBundle/Resources/public/js/*',
                '@AcmeBarBundle/Resources/public/js/form.js',
                '@AcmeBarBundle/Resources/public/js/calendar.js',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

No ambiente ``dev``, cada arquivo ainda é servido individualmente, de modo que
você possa depurar problemas mais facilmente. No entanto, no ambiente ``prod``
(ou mais especificamente, quando a flag ``debug`` é ``false``), eles serão
renderizados como uma única tag ``script``, que contém o conteúdo de todos
os arquivos JavaScript.

.. tip::

    Se você é novo no Assetic e tentar usar sua aplicação no ambiente
    ``prod`` (utilizando o controlador ``app.php``), você provavelmente verá
    que todos os seus CSS e JS quebram. Não se preocupe! Isso é de propósito.
    Para detalhes sobre a utilização do Assetic no ambiente ``prod``, consulte :ref:`assetic-dumping`.

A combinação de arquivos não se aplica apenas a *seus* arquivos. Você também pode usar o Assetic para
combinar assets de terceiros, tais como jQuery, com os seus próprios assets em um único arquivo:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts
            '@AppBundle/Resources/public/js/thirdparty/jquery.js'
            '@AppBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@AppBundle/Resources/public/js/thirdparty/jquery.js',
                '@AppBundle/Resources/public/js/*',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Usando Assets Nomeados
~~~~~~~~~~~~~~~~~~~~~~

As diretivas de configuração do AsseticBundle permitem que você defina conjuntos de assets nomeados.
Você pode fazer isso definindo os arquivos de entrada, filtros e arquivos de saída em sua
configuração na seção ``assetic``. Leia mais na
:doc:`Referência da Configuração do Assetic </reference/configuration/assetic>`.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            assets:
                jquery_and_ui:
                    inputs:
                        - '@AppBundle/Resources/public/js/thirdparty/jquery.js'
                        - '@AppBundle/Resources/public/js/thirdparty/jquery.ui.js'

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
                <assetic:asset name="jquery_and_ui">
                    <assetic:input>@AppBundle/Resources/public/js/thirdparty/jquery.js</assetic:input>
                    <assetic:input>@AppBundle/Resources/public/js/thirdparty/jquery.ui.js</assetic:input>
                </assetic:asset>
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'assets' => array(
                'jquery_and_ui' => array(
                    'inputs' => array(
                        '@AppBundle/Resources/public/js/thirdparty/jquery.js',
                        '@AppBundle/Resources/public/js/thirdparty/jquery.ui.js',
                    ),
                ),
            ),
        ));

Depois de ter definido os assets nomeados, você pode referenciá-los em seus templates
com a notação ``@asset_nomeado``:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts
            '@jquery_and_ui'
            '@AppBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@jquery_and_ui',
                '@AppBundle/Resources/public/js/*',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. _assetic-filters:

Filtros
-------

Uma vez que são gerenciados pelo Assetic, você pode aplicar filtros em seus assets antes
deles serem servidos. Isso inclui filtros que comprimem a saída de seus assets
para tamanhos de arquivos menores (e melhor otimização do front-end). Outros filtros
podem compilar arquivos CoffeeScript para JavaScript e processar SASS em CSS.
Na verdade, o Assetic tem uma longa lista de filtros disponíveis.

Muitos dos filtros não fazem o trabalho diretamente, mas usam bibliotecas existentes
de terceiros para fazer o trabalho pesado. Isto significa que muitas vezes você precisará instalar
uma biblioteca de terceiro para usar um filtro. A grande vantagem de usar o Assetic
para chamar estas bibliotecas (ao invés de usá-las diretamente) é que, em vez
de ter que executá-las manualmente depois de trabalhar nos arquivos, o Assetic irá
cuidar disso para você e remover completamente esta etapa dos seus processos de desenvolvimento
e implantação.

Para usar um filtro, primeiro você precisa especificá-lo na configuração do Assetic.
Adicionar um filtro aqui não significa que ele está sendo usado - apenas significa que está
disponível para uso (vamos usar o filtro abaixo).

Por exemplo, para usar o minificador de JavaScript UglifyJS, a seguinte configuração
deve ser definida:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                uglifyjs2:
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
                    'bin' => '/usr/local/bin/uglifyjs',
                ),
            ),
        ));

Agora, para efetivamente *usar* o filtro em um grupo de arquivos JavaScript, adicione-o
em seu template:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' filter='uglifyjs2' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('uglifyjs2')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Um guia mais detalhado sobre a configuração e uso dos filtros do Assetic, bem como
detalhes do modo de depuração do Assetic pode ser encontrado em :doc:`/frontend/assetic/uglifyjs`.

Controlando o URL Usado
-----------------------

Se desejar, você pode controlar os URLs que o Assetic produz. Isto é
feito a partir do template e é relativo ao diretório público raiz:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' output='js/compiled/main.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array(),
            array('output' => 'js/compiled/main.js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    O Symfony fornece várias implementações de cache busting através das
    opções de configuração
    :ref:`version <reference-framework-assets-version>`,
    :ref:`version_format <reference-assets-version-format>` e
    :ref:`json_manifest_path <reference-assets-json-manifest-path>`.

.. _assetic-dumping:

Fazendo o Dump dos Arquivos Asset
---------------------------------

No ambiente ``dev``, o Assetic gera caminhos para os arquivos CSS e JavaScript
que não existem fisicamente em seu computador. Mas eles renderizam mesmo assim
porque um controlador interno do Symfony abre os arquivos e serve de volta o
conteúdo (após a execução de quaisquer filtros).

Este tipo de publicação dinâmica dos assets processados é ótima porque significa que
você pode ver imediatamente o novo estado de quaisquer arquivos asset que você altere.
Também é ruim, porque pode ser muito lento. Se você estiver usando uma série de filtros,
pode ser francamente frustrante.

Felizmente, o Assetic fornece uma forma de fazer o dump de seus assets para arquivos reais, ao invés
de gerá-los dinamicamente.

Fazendo o Dump dos Arquivos Asset no Ambiente ``prod``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No ambiente ``prod``, seus arquivos JS e CSS são representados por uma única
tag cada. Em outras palavras, em vez de ver cada arquivo JavaScript que você está incluindo
no seu código fonte, é provável que você só veja algo semelhante a:

.. code-block:: html

    <script src="/js/abcd123.js"></script>

Além disso, esse arquivo **não** existe realmente, nem é renderizado de forma dinâmica
pelo Symfony (como os arquivos asset são renderizados no ambiente ``dev``). Isto é de
propósito - deixar o Symfony gerar esses arquivos dinamicamente em um ambiente
de produção é muito lento.

.. _assetic-dump-prod:

Em vez disso, cada vez que você usar sua aplicação no ambiente ``prod `` (e, portanto,
cada vez que você implantar), você deve executar o seguinte comando:

.. code-block:: terminal

    $ php bin/console assetic:dump --env=prod --no-debug

Isso vai gerar fisicamente e escrever cada arquivo que você precisa (por exemplo, ``/js/abcd123.js``).
Se você atualizar qualquer um de seus assets, você precisará executar o comando novamente para gerar
o novo arquivo.

Fazendo o Dump dos Arquivos Asset no Ambiente ``dev``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, cada caminho de asset gerado no ambiente ``dev`` é gerenciado
dinamicamente pelo Symfony. Isso não tem desvantagem (você pode ver as suas alterações
imediatamente), exceto que os assets podem carregar visivelmente lentos. Se você sentir
que seus assets estão carregando muito lentamente, siga este guia.

Primeiro, diga ao Symfony para parar de tentar processar estes arquivos dinamicamente. Faça
a seguinte alteração em seu arquivo ``config_dev.yml``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        assetic:
            use_controller: false

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/assetic
                http://symfony.com/schema/dic/assetic/assetic-1.0.xsd">

            <assetic:config use-controller="false" />
        </container>

    .. code-block:: php

        // app/config/config_dev.php
        $container->loadFromExtension('assetic', array(
            'use_controller' => false,
        ));

Em seguida, uma vez que o Symfony não está mais gerando esses assets para você, você
precisará fazer o dump deles manualmente. Para fazer isso, execute o seguinte comando:

.. code-block:: terminal

    $ php bin/console assetic:dump

Isso fisicamente grava todos os arquivos asset que você precisa para seu ambiente
``dev``. A grande desvantagem é que você precisa executar isso cada vez que
você atualizar um asset. Felizmente, ao usar o comando ``assetic:watch``,
os assets serão gerados de novo automaticamente *enquanto eles mudam*:

.. code-block:: terminal

    $ php bin/console assetic:watch

O comando ``assetic:watch`` foi introduzido no AsseticBundle 2.4. Em versões
anteriores, você tinha que usar a opção ``--watch`` do comando ``assetic:dump``
para o mesmo comportamento.

Uma vez que executar este comando no ambiente ``dev`` pode gerar vários
arquivos, geralmente é uma boa idéia apontar os seus arquivos assets gerados para
algum diretório isolado (por exemplo, ``/js/compiled``), para manter as coisas organizadas:

.. configuration-block::

    .. code-block:: html+twig

        {% javascripts '@AppBundle/Resources/public/js/*' output='js/compiled/main.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array(),
            array('output' => 'js/compiled/main.js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. _`LiipImagineBundle`: https://github.com/liip/LiipImagineBundle
