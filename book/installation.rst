.. index::
   single: Installation

Instalando e Configurando o Symfony
===================================

O objetivo deste capítulo é te deixar com uma aplicação pronta e funcionando,
feita com o Symfony. Felizmente, o Symfony oferece o que chamamos de "distribuições",
que são projetos básicos e funcionais que você pode baixar e utilizar como base para
começar a desenvolver imediatamente.

.. tip::

    Se o que você procura são instruções sobre a melhor maneira de criar um
    projeto e armazena-lo por meio de um sistema de controle de versão, veja
    `Utilizando Controle de Versão`_.

Instalando uma Distribuição do Symfony2
---------------------------------------

.. tip::

    Primeiro, certifique-se de que você tem um servidor web (Apache, por exemplo)
    com a versão mais recente possível do PHP (é recomendado o PHP 5.3.8 ou superior). 
    Para mais informações sobre os requisitos do Symfony2, veja a :doc:`referência 
    sobre requisitos</reference/requirements>`. Para informações sobre a configuração
    de seu específico root do servidor web, verifique a seguinte documentação:
    `Apache`_ | `Nginx`_ .

O Symfony2 tem pacotes chamados de "distribuições", que são aplicações totalmente
funcionais que já vem com as bibliotecas básicas do framework, uma seleção de
alguns pacotes úteis, uma estrutura de diretórios com tudo o necessário e
algumas configurações padrão. Ao baixar uma distribuição, você está baixando o
esqueleto de uma aplicação funcional que pode ser utilizado imediatamente
para começar a desenvolver.

Comece acessando a página de download do Symfony2 em `http://symfony.com/download`_.
Nessa página, você verá *Symfony Standard Edition*, que é a principal distribuição
do Symfony2. Existem duas formas de iniciar o seu projeto:

Opção 1) Composer
~~~~~~~~~~~~~~~~~

`Composer`_  é uma biblioteca de gerenciamento de dependências para PHP, que você pode usar 
para baixar a Edição Standard do Symfony2.

Comece fazendo o `download do Composer`_ em qualquer lugar em seu computador local. Se você 
tem o curl instalado, é tão fácil como:

.. code-block:: bash

    curl -s https://getcomposer.org/installer | php

.. note::

     Se o seu computador não está pronto para usar o Composer, você verá algumas recomendações 
     ao executar este comando. Siga as recomendações para que o Composer funcione corretamente.

O Composer é um arquivo executável PHAR, que você pode usar para baixar a Distribuição Standard:

.. code-block:: bash

    php composer.phar create-project symfony/framework-standard-edition /path/to/webroot/Symfony 2.1.x-dev

.. tip::

    Para uma versão exata, substitua `2.1.x-dev` com a versão mais recente do Symfony (por exemplo, 2.1.1). 
    Para mais detalhes, consulte a `Página de Instalação do Symfony`_

Este comando pode demorar alguns minutos para ser executado pois o Composer baixa a Distribuição Padrão, 
juntamente com todas as bibliotecas vendor de que ela precisa. Quando terminar, você deve ter um diretório
parecido com o seguinte:

.. code-block:: text

    path/to/webroot/ <- your web root directory
        Symfony/ <- the new directory
            app/
                cache/
                config/
                logs/
            src/
                ...
            vendor/
                ...
            web/
                app.php
                ...

Opção 2) Fazer download de um arquivo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode fazer download de um arquivo da Edição Standard. Aqui, você vai
precisar fazer duas escolhas:

* Faça o download do arquivo ``tgz`` ou ``zip`` - ambos são equivalentes, faça o download
  daquele que você está mais confortável em usar;

* Faça o download da distribuição com ou sem vendors. Se você está pensando em usar mais
  bibliotecas de terceiros ou bundles e gerenciá-los através do Composer, você 
  provavelmente deve baixar "sem vendors".

Baixe um dos arquivos em algum lugar sob o diretório raiz do seu servidor web local e descompacte-o. 
A partir de uma linha de comando UNIX, isto pode ser feito com um dos seguintes comandos (substituindo 
``###`` com o seu nome real do arquivo):

.. code-block:: bash

    # for .tgz file
    $ tar zxvf Symfony_Standard_Vendors_2.1.###.tgz

    # for a .zip file
    $ unzip Symfony_Standard_Vendors_2.1.###.zip

Se você baixou o arquivo "sem vendors", você definitivamente precisa ler a próxima seção.

.. note ::

     Você pode facilmente substituir a estrutura de diretórios padrão. Veja
     :doc:`/cookbook/configuration/override_dir_structure` para mais
     informações.

.. _installation-updating-vendors:

Atualizando os Vendors
~~~~~~~~~~~~~~~~~~~~~~

Neste ponto, você baixou um projeto Symfony totalmente funcional em que
você vai começar a desenvolver a sua própria aplicação. Um projeto Symfony depende
de um número de bibliotecas externas. Estas são baixadas no diretório `vendor/`
do seu projeto através de uma biblioteca chamada `Composer_`.

Dependendo de como você baixou o Symfony, você pode ou não precisar fazer a atualização
de seus vendors agora. Mas, a atualização de seus vendors é sempre segura, e garante
que você tem todas as bibliotecas vendor que você precisa.

Passo 1: Obter o `Composer` _ (O excelente novo sistema de pacotes do PHP)

.. code-block:: bash

    curl -s http://getcomposer.org/installer | php

Certifique-se de que você baixou o ``composer.phar`` no mesmo diretório onde
o arquivo ``composer.json`` encontra-se (por padrão, no raiz de seu projeto 
Symfony).

Passo 2: Instalar os vendors

.. code-block:: bash

    $ php composer.phar install

Este comando faz o download de todas as bibliotecas vendor necessárias - incluindo
o Symfony em si - dentro do diretório ``vendor/``.

.. note ::

    Se você não tem o ``curl`` instalado, você também pode apenas baixar o arquivo ``installer``
    manualmente em http://getcomposer.org/installer. Coloque este arquivo em seu
    projeto e execute:

    .. code-block:: bash

        php installer
        php composer.phar install

.. tip::

    Ao executar ``php composer.phar install`` ou ``php composer.phar update``,
    o composer vai executar comandos de pós instalação/atualização para limpar o cache
    e instalar os assets. Por padrão, os assets serão copiados para o seu diretório 
    ``web``. Para criar links simbólicos em vez de copiar os assets, você pode
    adicionar uma entrada no nó ``extra`` do seu arquivo composer.json com a chave
    ``symfony-assets-install``  e o valor ``symlink``:

    .. code-block:: json

        "extra": {
            "symfony-app-dir": "app",
            "symfony-web-dir": "web",
            "symfony-assets-install": "symlink"
        }

    Ao passar ``relative` ao invés de ``symlink`` para o symfony-assets-install,
    o comando irá gerar links simbólicos relativos.

Configuração e Instalação
~~~~~~~~~~~~~~~~~~~~~~~~~

Nesse ponto, todas as bibliotecas de terceiros necessários encontram-se no
diretório ``vendor/``. Você também tem um instalação padrão da aplicação em ``app/``
e alguns códigos de exemplo no diretório ``src/``.

O Symfony2 tem um script para testar as configuração do servidor de forma visual,
que ajuda garantir que o servidor web e o PHP estão configurados para o framework.
Utilize a seguinte URL para verificar a sua configuração:

.. code-block:: text

    http://localhost/config.php

Se algum problema foi encontrado, ele deve ser corrigido agora, antes de prosseguir.

.. sidebar:: Configurando as Permissões

    Um problema comum é que os diretórios ``app/cache`` e ``app/logs`` devem
    ter permissão de escrita para o servidor web e para o usuário da linha de
    comando. Em um sistema UNIX, se o usuário do seu servidor web for diferente
    do seu usuário da linha de comando, você pode executar os seguintes comandos
    para garantir que as permissões estejam configuradas corretamente. Mude o
    ``www-data`` para o usuário do servidor web e o ``yourname`` para o usuário
    da linha de comando:

    **1. Utilizando ACL em um sistema que suporta chmod +a**

    Muitos sistemas permitem que você utilize o comando ``chmod +a``. Tente ele
    primeiro e se der erro tente o próximo método:

    .. code-block:: bash

        rm -rf app/cache/*
        rm -rf app/logs/*

        sudo chmod +a "www-data allow delete,write,append,file_inherit,directory_inherit" app/cache app/logs
        sudo chmod +a "yourname allow delete,write,append,file_inherit,directory_inherit" app/cache app/logs

    **2. Utilizando ACL em um sistema que não suporta chmod +a**

    Alguns sistemas não suportam o comando ``chmod +a``, mas suportam um outro
    chamado ``setfacl``. Pode ser necessário que você `habilite o suporte a ACL`_
    na sua partição e instale o setfacl antes de utiliza-lo (esse é o caso no Ubuntu,
    por exemplo) da seguinte maneira:

    .. code-block:: bash

        sudo setfacl -R -m u:www-data:rwx -m u:yourname:rwx app/cache app/logs
        sudo setfacl -dR -m u:www-data:rwx -m u:yourname:rwx app/cache app/logs

    **3. Sem utilizar ACL**

    Se você não tem acesso para alterar a ACL de diretórios, será necessário
    alterar a umask para que os diretórios de cache e log tenham permissão de
    escrita para o grupo ou para todos (vai depender se o usuário do servidor web
    e o usuário da linha de comando estão no mesmo grupo). Para isso, coloque a
    seguinte linha no começo dos arquivos ``app/console``, ``web/app.php`` e
    ``web/app_dev.php``:

    .. code-block:: php

        umask(0002); // This will let the permissions be 0775

        // or

        umask(0000); // This will let the permissions be 0777

    Note que se você tem acesso a ACL no seu servidor, esse será o método recomendado,
    uma vez que alterar a umask não é uma operação thread-safe.       

Quando tudo estiver feito, clique em "Go to the Welcome page" para acessar a sua
primeira webpage Symfony2 "real":

.. code-block:: text

    http://localhost/app_dev.php/

O Symfony2 deverá lhe dar as boas vindas e parabeniza-lo pelo trabalho duro até agora!

.. image:: /images/quick_tour/welcome.jpg

Iniciando o Desenvolvimento
---------------------------

Agora que você tem uma aplicação Symfony2 totalmente funcional, você pode começar
o desenvolvimento! A sua distribuição deve conter alguns códigos de exemplo -
verifique o arquivo ``README.rst`` incluído na distribuição (você pode abri-lo
como um arquivo de texto) para aprender sobre os exemplos incluídos e como
você pode removê-los mais tarde.

Se você é novo no Symfony, junte-se a nós em ":doc:`page_creation`", onde você
aprenderá como criar páginas, mudar configurações e tudo mais que precisará para
a sua nova aplicação.

Certifique-se também verificar o :doc:`Cookbook</cookbook/index>`, que contém 
uma grande variedade de artigos sobre a resolução de problemas específicos com Symfony.

Utilizando Controle de Versão
-----------------------------

Se você está utilizando um sistema de controle de versão como ``Git`` ou ``Subversion``,
você pode instala-lo e começar a realizar os commits do seu projeto normalmente.
A edição padrão do Symfony *é* o ponto inicial para o seu novo projeto.

Para instruções específicas sobre a melhor maneira de configurar o seu projeto
para ser armazenado no git, veja :doc:`/cookbook/workflow/new_project_git`.

Ignorando o diretório ``vendor/``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você baixou o arquivo *sem itens de terceiros* (without vendors), você pode
ignorar todo o diretório ``vendor/`` com segurança e não enviá-lo para o controle
de versão. No ``Git``, isso é feito criando e o arquivo ``.gitignore`` e adicionando
a seguinte linha: 

.. code-block:: text

    /vendor/

Agora, o diretório vendor não será enviado para o controle de versão. Isso é bom
(na verdade, é ótimo!) porque quando alguém clonar ou fizer check out do projeto,
ele/ela poderá simplesmente executar o script ``php composer.phar install`` para
instalar todas as bibliotecas vendor necessárias.

.. _`habilite o suporte a ACL`: https://help.ubuntu.com/community/FilePermissionsACLs
.. _`http://symfony.com/download`: http://symfony.com/download
.. _`Git`: http://git-scm.com/
.. _`GitHub Bootcamp`: http://help.github.com/set-up-git-redirect
.. _`Composer`: http://getcomposer.org/
.. _`download do Composer`: http://getcomposer.org/download/
.. _`Apache`: http://httpd.apache.org/docs/current/mod/core.html#documentroot
.. _`Nginx`: http://wiki.nginx.org/Symfony
.. _`Página de Inslatação do Symfony`:    http://symfony.com/download
