.. index::
   single: Implantação; Implantando no Platform.sh

Implantando no Platform.sh
==========================

Este cookbook passo-a-passo descreve como implantar uma aplicação web Symfony para o
`Platform.sh`_. Você pode ler mais sobre como usar o Symfony com Platform.sh na
`documentação oficial do Platform.sh`_.

Implantar um Site Existente
---------------------------

Neste guia, presume-se que sua base de código já está versionada com Git.

Obter um Projeto no Platform.sh
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você precisa se inscrever em um `projeto Platform.sh`_. Escolha o plano de desenvolvimento
e siga o processo de checkout. Uma vez que seu projeto está pronto, forneça-lhe um nome
e escolha: **Importar um site existente**.

Prepare a sua Aplicação
~~~~~~~~~~~~~~~~~~~~~~~

Para implantar sua aplicação Symfony no Platform.sh, você simplesmente precisa adicionar um
``.platform.app.yaml`` no raiz de seu repositório Git, que irá dizer ao
Platform.sh como implantar a sua aplicação (leia mais sobre os
`arquivos de configuração do Platform.sh`_).

.. code-block:: yaml

    # .platform.app.yaml

    # This file describes an application. You can have multiple applications
    # in the same project.

    # The name of this app. Must be unique within a project.
    name: myphpproject
    
    # The type of the application to build.
    type: php:5.6
    build:
      flavor: symfony

    # The relationships of the application with services or other applications.
    # The left-hand side is the name of the relationship as it will be exposed
    # to the application in the PLATFORM_RELATIONSHIPS variable. The right-hand
    # side is in the form `<service name>:<endpoint name>`.
    relationships:
        database: 'mysql:mysql'

    # The configuration of app when it is exposed to the web.
    web:
        # The public directory of the app, relative to its root.
        document_root: '/web'
        # The front-controller script to send non-static requests to.
        passthru: '/app.php'

    # The size of the persistent disk of the application (in MB).
    disk: 2048

    # The mounts that will be performed when the package is deployed.
    mounts:
        '/var/cache': 'shared:files/cache'
        '/var/logs': 'shared:files/logs'

    # The hooks that will be performed when the package is deployed.
    hooks:
        build: |
          rm web/app_dev.php
          bin/console --env=prod assetic:dump --no-debug
        deploy: |
          bin/console --env=prod cache:clear

Para melhor prática, você também deve adicionar um diretório ``.platform`` no raiz
do seu repositório Git, que contém os seguintes arquivos:

.. code-block:: yaml

    # .platform/routes.yaml
    "http://{default}/":
        type: upstream
        # the first part should be your project name
        upstream: 'myphpproject:php'

.. code-block:: yaml

    # .platform/services.yaml
    mysql:
        type: mysql
        disk: 2048

Um exemplo dessas configurações podem ser encontrado no `GitHub`_. A lista de
`serviços disponíveis`_ pode ser encontrada na documentação do Platform.sh.

Configurar o Acesso ao Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O Platform.sh sobrescreve a configuração específica do banco de dados através da importação do
seguinte arquivo (você é que deve adicionar este arquivo em sua base de código)::

    // app/config/parameters_platform.php
    <?php
    $relationships = getenv("PLATFORM_RELATIONSHIPS");
    if (!$relationships) {
        return;
    }

    $relationships = json_decode(base64_decode($relationships), true);

    foreach ($relationships['database'] as $endpoint) {
        if (empty($endpoint['query']['is_master'])) {
          continue;
        }

        $container->setParameter('database_driver', 'pdo_' . $endpoint['scheme']);
        $container->setParameter('database_host', $endpoint['host']);
        $container->setParameter('database_port', $endpoint['port']);
        $container->setParameter('database_name', $endpoint['path']);
        $container->setParameter('database_user', $endpoint['username']);
        $container->setParameter('database_password', $endpoint['password']);
        $container->setParameter('database_path', '');
    }

    # Store session into /tmp.
    ini_set('session.save_path', '/tmp/sessions');

Certifique-se que esse arquivo está listado em seu *imports*:

.. code-block:: yaml

    # app/config/config.yml
    imports:
        - { resource: parameters_platform.php }

Implantar sua Aplicação
~~~~~~~~~~~~~~~~~~~~~~~

Agora você precisa adicionar um remoto para o Platform.sh em seu repositório Git (copie o
comando que você vê na UI Web do Platform.sh):

.. code-block:: bash

    $ git remote add platform [PROJECT-ID]@git.[CLUSTER].platform.sh:[PROJECT-ID].git

``PROJECT-ID``
    Identificador único do seu projeto. Algo como ``kjh43kbobssae``
``CLUSTER``
    Localização do servidor onde o projeto está implantado. Pode ser ``eu`` ou ``us``

Faça o commit dos arquivos específicos do Platform.sh criados na seção anterior:

.. code-block:: bash

    $ git add .platform.app.yaml .platform/*
    $ git add app/config/config.yml app/config/parameters_platform.php
    $ git commit -m "Adding Platform.sh configuration files."

Faça o push da sua base de código para o remoto recém-adicionado:

.. code-block:: bash

    $ git push platform master

É isso! Sua aplicação está sendo implantada no Platform.sh e em breve você será
capaz de acessá-la no seu navegador.

Para cada alteração de código que você fizer a partir de agora será feito um push para o Git, a fim de
reimplantar seu ambiente no Platform.sh.

Mais informações sobre como `migrar seu banco de dados e arquivos`_ podem ser encontradas
na documentação do Platform.sh.

Implantar um novo site
----------------------

Você pode começar um novo `projeto Platform.sh`_. Escolha o plano de desenvolvimento e siga
através do processo de checkout.

Uma vez que seu projeto está pronto, forneça-lhe um nome e escolha: **Criar um novo site**.
Escolha a stack *Symfony* e um ponto de partida, como *Standard*.

É isso! Sua aplicação Symfony será inicializada e implantada. Em breve você
poderá vê-la no seu navegador.

.. _`Platform.sh`: https://platform.sh
.. _`documentação oficial do Platform.sh`: https://docs.platform.sh/toolstacks/symfony/symfony-getting-started
.. _`projeto Platform.sh`: https://marketplace.commerceguys.com/platform/buy-now
.. _`arquivos de configuração do Platform.sh`: https://docs.platform.sh/reference/configuration-files
.. _`GitHub`: https://github.com/platformsh/platformsh-examples
.. _`serviços disponíveis`: https://docs.platform.sh/reference/configuration-files/#configure-services
.. _`migrar seu banco de dados e arquivos`: https://docs.platform.sh/toolstacks/php/symfony/migrate-existing-site/
