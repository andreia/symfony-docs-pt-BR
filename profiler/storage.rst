.. index::
    single: Profiling; Configuração de Armazenamento

Alterando o Armazenamento do Profiler
=====================================

Por padrão o profile armazena os dados coletados em arquivos no diretório ``%kernel.cache_dir%/profiler/``.
Você pode controlar o armazenamento que está sendo usado através das opções ``dsn``, ``username``,
``password`` e ``lifetime``. Por exemplo, a seguinte configuração
usa o MySQL como armazenamento para o profiler com um tempo de vida de uma hora:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            profiler:
                dsn:      'mysql:host=localhost;dbname=%database_name%'
                username: '%database_user%'
                password: '%database_password%'
                lifetime: 3600

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >
            <framework:config>
                <framework:profiler
                    dsn="mysql:host=localhost;dbname=%database_name%"
                    username="%database_user%"
                    password="%database_password%"
                    lifetime="3600"
                />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php

        // ...
        $container->loadFromExtension('framework', array(
            'profiler' => array(
                'dsn'      => 'mysql:host=localhost;dbname=%database_name%',
                'username' => '%database_user',
                'password' => '%database_password%',
                'lifetime' => 3600,
            ),
        ));

O :doc:`componente HttpKernel </components/http_kernel/introduction>` atualmente
suporta os seguintes drivers de armazenamento para o profiler:

* file
* sqlite
* mysql
* mongodb
* memcache
* memcached
* redis
