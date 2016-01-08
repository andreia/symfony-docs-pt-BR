.. index::
    double: Composer; Instalação

Instalando o Composer
=====================

`Composer`_ é o gerenciador de dependências usado por aplicações PHP modernas. Use o Compositor
para gerenciar dependências em suas aplicações Symfony e para instalar componentes Symfony
em seus projetos PHP.

É recomendado instalar o Composer globalmente em seu sistema como explicado nas
seções seguintes.

Instalar o Composer no Linux e no Mac OS X
------------------------------------------

Para instalar o Composer no Linux ou Mac OS X, execute os seguintes comandos:

.. code-block:: bash

    $ curl -sS https://getcomposer.org/installer | php
    $ sudo mv composer.phar /usr/local/bin/composer

.. note::

    Se você não tem o ``curl`` instalado, você também pode simplesmente baixar o
    arquivo ``installer`` manualmente em https://getcomposer.org/installer e
    em seguida, executar:

    .. code-block:: bash

        $ php installer
        $ sudo mv composer.phar /usr/local/bin/composer

Instalar o Composer no Windows
------------------------------

Faça o download do instalador em `getcomposer.org/download`_, execute ele e siga
as instruções.

Saiba mais
----------

Leia a `documentação do Composer`_ para saber mais sobre a sua utilização e características.

.. _`Composer`: https://getcomposer.org/
.. _`getcomposer.org/download`: https://getcomposer.org/download
.. _`documentação do Composer`: https://getcomposer.org/doc/00-intro.md
