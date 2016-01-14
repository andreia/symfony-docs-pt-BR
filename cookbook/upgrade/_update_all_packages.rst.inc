Atualizando outros Pacotes
~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode desejar atualizar as suas bibliotecas restantes. Caso você tenha feito um
bom trabalho com suas `restrições de versão`_ no ``composer.json``, você pode fazer
isso com segurança executando:

.. code-block:: bash

    $ composer update

.. caution::

    Cuidado, se você possui `restrições de versão`_ inespecíficas em seu
    ``composer.json`` (ex. ``dev-master``), isso pode atualizar algumas
    bibliotecas não-Symfony para novas versões que contêm alterações que quebram a
    compatibilidade com versões anteriores.

.. _`restrições de versão`: https://getcomposer.org/doc/01-basic-usage.md#package-versions
