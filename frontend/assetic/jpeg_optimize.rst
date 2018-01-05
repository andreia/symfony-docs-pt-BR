.. index::
   single: Assetic; Otimização de Imagens

Como Usar o Assetic para Otimização de imagens com Funções do Twig
==================================================================

.. include:: /assetic/_standard_edition_warning.rst.inc

Dentre os seus vários filtros, o Assetic possui quatro que podem ser utilizados para a otimização 
de imagens on-the-fly. Isso permite obter os benefícios de tamanhos menores de arquivos
sem ter que usar um editor de imagens para processar cada imagem. Os resultados
são armazenados em cache e pode ser feito o dump para produção de modo que não haja impacto no desempenho
para seus usuários finais.

Usando o Jpegoptim
------------------

`Jpegoptim`_ é um utilitário para otimizar arquivos JPEG. Para usá-lo com o Assetic, certifique-se
de já tê-lo instalado em seu sistema, e então configure sua localização
usando a opção ``bin`` do filtro ``jpegoptim``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jpegoptim:
                    bin: path/to/jpegoptim

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
                    name="jpegoptim"
                    bin="path/to/jpegoptim" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin' => 'path/to/jpegoptim',
                ),
            ),
        ));

Ele agora pode ser usado em um template:

.. configuration-block::

    .. code-block:: html+twig

        {% image '@AppBundle/Resources/public/images/example.jpg'
            filter='jpegoptim' output='/images/example.jpg' %}
            <img src="{{ asset_url }}" alt="Example"/>
        {% endimage %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->image(
            array('@AppBundle/Resources/public/images/example.jpg'),
            array('jpegoptim')
        ) as $url): ?>
            <img src="<?php echo $view->escape($url) ?>" alt="Example"/>
        <?php endforeach ?>

Removendo Todos os Dados EXIF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, o filtro ``jpegoptim`` remove apenas algumas das meta informações armazenadas
na imagem. Para remover todos os dados EXIF e comentários, configure a opção ``strip_all``
para ``true``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jpegoptim:
                    bin: path/to/jpegoptim
                    strip_all: true

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
                    name="jpegoptim"
                    bin="path/to/jpegoptim"
                    strip-all="true" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin'       => 'path/to/jpegoptim',
                    'strip_all' => 'true',
                ),
            ),
        ));

Diminuindo a Qualidade Máxima
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, o filtro ``jpegoptim`` não altera o nível de qualidade da imagem
JPEG. Use a opção ``max`` para configurar a qualidade máxima (numa
escala de ``0`` a ``100``). A redução no tamanho do arquivo da imagem será, claro,
ao custo da sua qualidade:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jpegoptim:
                    bin: path/to/jpegoptim
                    max: 70

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
                    name="jpegoptim"
                    bin="path/to/jpegoptim"
                    max="70" />
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin' => 'path/to/jpegoptim',
                    'max' => '70',
                ),
            ),
        ));

Sintaxe Curta: Função Twig
--------------------------

Se você estiver usando o Twig, é possível conseguir tudo isso com uma sintaxe
curta, ao habilitar e usar uma função especial do Twig. Comece adicionando a
seguinte configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jpegoptim:
                    bin: path/to/jpegoptim
            twig:
                functions:
                    jpegoptim: ~

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
                    name="jpegoptim"
                    bin="path/to/jpegoptim" />
                <assetic:twig>
                    <assetic:function
                        name="jpegoptim" />
                </assetic:twig>
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin' => 'path/to/jpegoptim',
                ),
            ),
            'twig' => array(
                'functions' => array('jpegoptim'),
            ),
        ));

O template do Twig agora pode ser alterado para o seguinte:

.. code-block:: html+twig

    <img src="{{ jpegoptim('@AppBundle/Resources/public/images/example.jpg') }}" alt="Example"/>

Você também pode especificar o diretório de saída para imagens no arquivo de configuração do
Assetic:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jpegoptim:
                    bin: path/to/jpegoptim
            twig:
                functions:
                    jpegoptim: { output: images/*.jpg }

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
                    name="jpegoptim"
                    bin="path/to/jpegoptim" />
                <assetic:twig>
                    <assetic:function
                        name="jpegoptim"
                        output="images/*.jpg" />
                </assetic:twig>
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin' => 'path/to/jpegoptim',
                ),
            ),
            'twig' => array(
                'functions' => array(
                    'jpegoptim' => array(
                        'output' => 'images/*.jpg',
                    ),
                ),
            ),
        ));

.. tip::

    Você pode comprimir e manipular imagens carregadas por upload usando o
    bundle `LiipImagineBundle`_ da comunidade.

.. _`Jpegoptim`: http://www.kokkonen.net/tjko/projects.html
.. _`LiipImagineBundle`: https://github.com/liip/LiipImagineBundle
