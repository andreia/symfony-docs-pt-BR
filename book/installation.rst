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

Baixando uma Distribuição do Symfony2
-------------------------------------

.. tip::

    Primeiro, certifique-se de que você tem um servidor web (Apache, por exemplo)
    com o PHP 5.3.2 ou superior instalado e configurado. Para mais informações
    sobre os requisitos do Symfony2, veja a :doc:`referência sobre requisitos</reference/requirements>`.

O Symfony2 tem pacotes chamados de "distribuições", que são aplicações totalmente
funcionais que já vem com as bibliotecas básicas do framework, uma seleção de
alguns pacotes úteis, uma estrutura de diretórios com tudo o necessário e
algumas configurações padrão. Ao baixar uma distribuição, você está baixando o
esqueleto de uma aplicação funcional que pode ser utilizado imediatamente
para começar a desenvolver.

Comece acessando a página de download do Symfony2 em `http://symfony.com/download`_.
Nessa página, você verá *Symfony Standard Edition*, que é a principal distribuição
do Symfony2. Aqui você precisará fazer duas escolhas:

* Baixar o arquivo ``.tgz`` ou o ``.zip``. Os dois são equivalentes, portanto,
  baixe o arquivo no formato que você estiver mais acostumado a utilizar.

* Baixar a distribuição com ou sem itens de terceiros (vendors). Se você tem o
  `Git`_ instalado em seu computador, você deve optar pela opção sem itens de
  terceiros (without vendors), uma vez que isso te dará um pouco mais de flexibilidade
  ao trabalhar com bibliotecas de terceiros.

Baixe um dos arquivos para um local dentro do diretório raiz do seu servidor web
e o descompacte. Utilizando a linha de comando em um sistema UNIX, isso pode ser
feito da seguinte maneira (troque o ``###`` pelo nome do arquivo):

.. code-block:: bash

    # for .tgz file
    tar zxvf Symfony_Standard_Vendors_2.0.###.tgz

    # for a .zip file
    unzip Symfony_Standard_Vendors_2.0.###.zip

Ao terminar, você deverá ter um diretório chamado ``Symfony/`` parecido com isso:

.. code-block:: text

    www/ <- your web root directory
        Symfony/ <- the unpacked archive
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

Atualizando bibliotecas de terceiros
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por fim, se você baixou a versão sem itens de terceiros (without vendors),
instale esses itens via linha de comando utilizando:

.. code-block:: bash

    php bin/vendors install

Esse comando baixa todas as bibliotecas de terceiros necessários - incluindo o
próprio Symfony - para o diretório ``vendor/``. Para informações sobre como as
bibliotecas de terceiros são gerenciadas dentro do Symfony2, veja ":ref:`cookbook-managing-vendor-libraries`".

Configuração e Instalação
~~~~~~~~~~~~~~~~~~~~~~~~~

Nesse ponto, todas as bibliotecas de terceiros necessários encontram-se no
diretório ``vendor/``. Você também tem um instalação padrão da aplicação em ``app/``
e alguns códigos de exemplo no diretório ``src/``.

O Symfony2 tem um script para testar as configuração do servidor de forma visual,
que ajuda garantir que o servidor web e o PHP estão configurados para o framework.
Utilize a seguinte URL para verificar a sua configuração:

.. code-block:: text

    http://localhost/Symfony/web/config.php

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

    http://localhost/Symfony/web/app_dev.php/

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

Utilizando Controle de Versão
-----------------------------

Se você está utilizando um sistema de controle de versão como ``Git`` ou ``Subversion``,
você pode instala-lo e começar a realizar os commits do seu projeto normalmente.
A edição padrão do Symfony *é* o ponto inicial para o seu novo projeto.

Para instruções específicas sobre a melhor maneira de configurar o seu projeto
para ser armazenado no git, veja :doc:`/cookbook/workflow/new_project_git`.

Ignorando o diretório ``vendor/``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você baixou o arquivo *sem itens de terceiros* (without vendors), você pode
ignorar todo o diretório ``vendor/`` com segurança e não enviá-lo para o controle
de versão. No ``Git``, isso é feito criando e o arquivo ``.gitignore`` e adicionando
a seguinte linha: 

.. code-block:: text

    vendor/

Agora, o diretório vendor não será enviado para o controle de versão. Isso é bom
(na verdade, é ótimo!) porque quando alguém clonar ou fizer check out do projeto,
ele/ela poderá simplesmente executar o script ``php bin/vendors install`` para
baixar todas as bibliotecas de terceiros necessárias.

.. _`habilite o suporte a ACL`: https://help.ubuntu.com/community/FilePermissions#ACLs
.. _`http://symfony.com/download`: http://symfony.com/download
.. _`Git`: http://git-scm.com/
.. _`GitHub Bootcamp`: http://help.github.com/set-up-git-redirect
