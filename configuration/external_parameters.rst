.. index::
    single: Ambientes; Parâmetros Externos

Como definir Parâmetros Externos no Container de Serviços
=========================================================

No capítulo :doc:`/cookbook/configuration/environments`, você aprendeu como gerenciar
a configuração da sua aplicação. Às vezes, armazenar certas credenciais fora do
código do seu projeto pode beneficiar a sua aplicação. A configuração do banco de dados
é um exemplo. A flexibilidade do container de serviço do Symfony permite à
você fazer isso facilmente.

Variáveis ​​de Ambiente
---------------------

O Symfony vai pegar qualquer variável de ambiente com o prefixo ``SYMFONY__`` e
setá-la como um parâmetro no container de serviço. Sublinhados duplos são substituídos
por um ponto, pois o ponto não é um caracter válido no nome de uma variável de
ambiente.

Por exemplo, se você está usando o Apache, as variáveis ​​de ambiente podem ser definidas 
utilizando a seguinte configuração ``VirtualHost``:

.. code-block:: apache

    <VirtualHost *:80>
        ServerName      Symfony2
        DocumentRoot    "/path/to/symfony_2_app/web"
        DirectoryIndex  index.php index.html
        SetEnv          SYMFONY__DATABASE__USER user
        SetEnv          SYMFONY__DATABASE__PASSWORD secret

        <Directory "/path/to/symfony_2_app/web">
            AllowOverride All
            Allow from All
        </Directory>
    </VirtualHost>

.. note::

    O exemplo acima é para uma configuração Apache, usando a diretiva
    `SetEnv`_.  No entanto, isso vai funcionar para qualquer servidor web que 
    suporte a definição de variáveis ​​de ambiente.

    Além disso, para que o seu console funcione (que não usa Apache), você deve 
    exportar estas como variáveis ​​shell. Em um sistema Unix, você pode executar
    o seguinte:

    .. code-block:: bash

        $ export SYMFONY__DATABASE__USER=user
        $ export SYMFONY__DATABASE__PASSWORD=secret

Agora que você declarou uma variável de ambiente, ela estará presente na 
variável global ``$_SERVER`` do PHP. O Symfony, então, automaticamente define todas 
as variáveis ​​``$_SERVER`` prefixadas com ``SYMFONY__`` como parâmetros no container
de serviços.

Agora, você pode referenciar estes parâmetros em qualquer local onde precisar deles.

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            dbal:
                driver    pdo_mysql
                dbname:   symfony2_project
                user:     "%database.user%"
                password: "%database.password%"

    .. code-block:: xml

        <!-- xmlns:doctrine="http://symfony.com/schema/dic/doctrine" -->
        <!-- xsi:schemaLocation="http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd"> -->

        <doctrine:config>
            <doctrine:dbal
                driver="pdo_mysql"
                dbname="symfony2_project"
                user="%database.user%"
                password="%database.password%"
            />
        </doctrine:config>

    .. code-block:: php

        $container->loadFromExtension('doctrine', array('dbal' => array(
            'driver'   => 'pdo_mysql',
            'dbname'   => 'symfony2_project',
            'user'     => '%database.user%',
            'password' => '%database.password%',
        ));

Constantes
----------

O container também possui suporte para definir constantes do PHP como parâmetros. Para
aproveitar esse recurso, mapeie o nome da sua constante para uma chave de parâmetro
, e defina o tipo como ``constant``.

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8"?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <parameters>
                <parameter key="global.constant.value" type="constant">GLOBAL_CONSTANT</parameter>
                <parameter key="my_class.constant.value" type="constant">My_Class::CONSTANT_NAME</parameter>
            </parameters>
        </container>

.. note::

    Isso funciona somente para a configuração XML. Se você *não* está usando XML, simplesmente
    importe um arquivo XML para aproveitar essa funcionalidade:

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: parameters.xml }

Configurações Diversas
----------------------

A diretiva ``imports`` pode ser usada para puxar os parâmetros armazenados em outro lugar.
Importando um arquivo PHP lhe dá a flexibilidade para adicionar o que for necessário
no container. O seguinte importa um arquivo chamado ``parameters.php``.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: parameters.php }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <imports>
            <import resource="parameters.php" />
        </imports>

    .. code-block:: php

        // app/config/config.php
        $loader->import('parameters.php');

.. note::

    Um arquivo de recursos pode ser um de muitos tipos. PHP, XML, YAML, INI e
    recursos de closure são todos suportados pela directiva ``imports``.

No ``parameters.php``, diga ao container de serviço os parâmetros que você deseja
definir. Isto é útil quando alguma configuração importante está em um formato fora do 
padrão. O exemplo abaixo inclui a configuração de um banco de dados do Drupal no
container de serviço do Symfony.

.. code-block:: php

    // app/config/parameters.php
    include_once('/path/to/drupal/sites/default/settings.php');
    $container->setParameter('drupal.database.url', $db_url);

.. _`SetEnv`: http://httpd.apache.org/docs/current/env.html
