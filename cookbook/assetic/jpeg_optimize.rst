Como usar o Assetic para otimização de imagem com funções do Twig 
=================================================================

Dentre os seus vários filtros, o Assetic possui quatro que podem ser utilizados para a otimização 
de imagens on-the-fly. Isso permite obter os benefícios de tamanhos menores dos arquivos
sem ter que usar um editor de imagens para processar cada imagem. Os resultados são armazenados 
em cache e pode ser feito o dump para produção de modo que não há impacto no desempenho
para seus usuários finais.

Usando o jpegoptim
------------------

`Jpegoptim`_ é um utilitário para otimizar arquivos JPEG. Para usá-lo com o Assetic,
adicione o seguinte na configuração do Assetic:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jpegoptim:
                    bin: path/to/jpegoptim

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="jpegoptim"
                bin="path/to/jpegoptim" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin' => 'path/to/jpegoptim',
                ),
            ),
        ));

.. note::

    Observe que, para usar o jpegoptim, você deve instalá-lo em seu
    sistema. A opção ``bin`` aponta para a localização do binário compilado.

Ele agora pode ser usado em um template:

.. configuration-block::

    .. code-block:: html+jinja

        {% image '@AcmeFooBundle/Resources/public/images/example.jpg'
            filter='jpegoptim' output='/images/example.jpg'
        %}
        <img src="{{ asset_url }}" alt="Example"/>
        {% endimage %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->images(
            array('@AcmeFooBundle/Resources/public/images/example.jpg'),
            array('jpegoptim')) as $url): ?>
        <img src="<?php echo $view->escape($url) ?>" alt="Example"/>
        <?php endforeach; ?>

Removendo todos os dados EXIF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, a execução desse filtro remove apenas algumas das informações meta
armazenadas no arquivo. Os dados EXIF ​​e comentários não são removidos, mas você pode
removê-los usando a opção ``strip_all``:

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
        <assetic:config>
            <assetic:filter
                name="jpegoptim"
                bin="path/to/jpegoptim"
                strip_all="true" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'jpegoptim' => array(
                    'bin' => 'path/to/jpegoptim',
                    'strip_all' => 'true',
                ),
            ),
        ));

Diminuindo a qualidade máxima
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, o nível de qualidade do JPEG não é afetado. Você pode ganhar reduções 
adicionais no tamanho dos arquivos ao ajustar a configuração de qualidade máxima 
para um valor inferior ao nível atual das imagens. Isto irá, claro, custar a
qualidade de imagem:

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
        <assetic:config>
            <assetic:filter
                name="jpegoptim"
                bin="path/to/jpegoptim"
                max="70" />
        </assetic:config>

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

Sintaxe curta: Função Twig 
--------------------------

Se você estiver usando o Twig, é possível conseguir tudo isso com uma sintaxe 
curta, ao habilitar e usar uma função especial do Twig. Comece adicionando 
a seguinte configuração:

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
        <assetic:config>
            <assetic:filter
                name="jpegoptim"
                bin="path/to/jpegoptim" />
            <assetic:twig>
                <assetic:twig_function
                    name="jpegoptim" />
            </assetic:twig>
        </assetic:config>

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
            ),
        ));

O template Twig pode agora ser alterado para o seguinte:

.. code-block:: html+jinja

    <img src="{{ jpegoptim('@AcmeFooBundle/Resources/public/images/example.jpg') }}"
         alt="Example"/>

Você pode especificar o diretório de saída na configuração da seguinte forma:

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
        <assetic:config>
            <assetic:filter
                name="jpegoptim"
                bin="path/to/jpegoptim" />
            <assetic:twig>
                <assetic:twig_function
                    name="jpegoptim"
                    output="images/*.jpg" />
            </assetic:twig>
        </assetic:config>

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
                        output => 'images/*.jpg'
                    ),
                ),
            ),
        ));

.. _`Jpegoptim`: http://www.kokkonen.net/tjko/projects.html
