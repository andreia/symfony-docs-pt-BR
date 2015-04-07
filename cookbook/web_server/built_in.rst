.. index::
    single: Servidor Web; Servidor Web Embutido

Como Usar o Servidor Web Embutido do PHP
========================================

.. versionadded:: 2.6
    A capacidade de executar o servidor como um processo em background foi introduzido
    no Symfony 2.6.

Desde o PHP 5.4 o CLI SAPI vem com um `servidor web embutido`_. Ele pode ser usado
para executar suas aplicações PHP localmente durante o desenvolvimento, para testar ou para
demonstrações da aplicação. Desta forma, você não tem que se preocupar em configurar
um servidor web com todos os recursos, como o
:doc:`Apache ou o Nginx </cookbook/configuration/web_server_configuration>`.

.. caution::

    O servidor web embutido é feito para ser executado em um ambiente controlado.
    Ele não foi projetado para ser usado em redes públicas.

Iniciando o Servidor Web
------------------------

Executar uma aplicação Symfony utilizando o servidor embutido do PHP é tão fácil quanto
executar o comando ``server:start``:

.. code-block:: bash

    $ php app/console server:start

Isso inicia o servidor web em background no endereço ``localhost:8000`` que serve
a sua aplicação Symfony.

Por padrão, o servidor web escuta na porta 8000 no dispositivo de loopback. Você
pode mudar o socket passando um endereço IP e uma porta como um argumento de linha de comando:

.. code-block:: bash

    $ php app/console server:run 192.168.0.1:8080

.. note::

    Você pode usar o comando ``server:status`` para verificar se um servidor web está
    escutando em um determinado socket:

    .. code-block:: bash

        $ php app/console server:status

        $ php app/console server:status 192.168.0.1:8080

    O primeiro comando mostra se a sua aplicação Symfony será um servidor em
    ``localhost:8000``, o segundo faz o mesmo para ``192.168.0.1:8080``.

.. note::

    Antes do Symfony 2.6, o comando ``server:run`` era utilizado para iniciar o servidor
    web embutido. Esse comando ainda está disponível e se comporta de forma ligeiramente diferente.
    Em vez de iniciar o servidor em background, ele irá bloquear o
    terminal atual até você terminá-lo (isso geralmente é feito pressionando Ctrl
    e C).

.. sidebar:: Usando o servidor web embutido dentro de uma máquina virtual

    Se você deseja usar o servidor web embutido dentro de uma máquina virtual
    e, em seguida, carregar o site a partir de um navegador em sua máquina host, você vai precisar
    escutar no endereço ``0.0.0.0:8000`` (ou seja, em todos os endereços IP que
    são atribuídos à máquina virtual):

    .. code-block:: bash

        $ php app/console server:start 0.0.0.0:8000

    .. caution::

        Você **NUNCA** deve ouvir todas as interfaces em um computador que é
        acessível diretamente a partir da Internet. O servidor web embutido não é
        projetado para ser usado em redes públicas.

Opções de Comando
~~~~~~~~~~~~~~~~~

O servidor web embutido espera um script "router" (leia sobre o script "router"
em `php.net`_) como um argumento. O Symfony já passa um script router
quando o comando é executado no ambiente ``prod`` ou ``dev``.
Use a opção ``--router`` em qualquer outro ambiente ou use outro
script router:

.. code-block:: bash

    $ php app/console server:start --env=test --router=app/config/router_test.php

Se o diretório raiz da sua aplicação é diferente do sistema de diretórios padrão,
você tem que passar o local correto usando a opção ``--docroot``:

.. code-block:: bash

    $ php app/console server:start --docroot=public_html

Parando o Servidor
------------------

Quando terminar, você pode simplesmente parar o servidor web usando o comando
``server:stop``:

.. code-block:: bash

    $ php app/console server:stop

Assim como o comando start, se você omitir a informação socket, o Symfony irá
parar o servidor web associado a ``localhost:8000``. Basta passar as informações de socket
quando o servidor web escuta outro endereço IP ou a outra porta:

.. code-block:: bash

    $ php app/console server:stop 192.168.0.1:8080

.. _`servidor web embutido`: http://www.php.net/manual/en/features.commandline.webserver.php
.. _`php.net`: http://php.net/manual/en/features.commandline.webserver.php#example-401
