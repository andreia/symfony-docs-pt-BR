Gerenciando Bibliotecas Vendor com o ``composer.json``
------------------------------------------------------

Como funciona?
~~~~~~~~~~~~~~

Cada projeto Symfony utiliza um grupo de bibliotecas "vendor" de terceiros. De
qualquer forma, o objetivo é fazer o download desses arquivos em seu diretório
``vendor/`` e, idealmente, fornecer alguma forma sã para gerenciar a versão exata
que você precisa para cada um.

Por padrão, essas bibliotecas são baixadas executando um binário "downloader"
``composer install``. Esse arquivo ``composer`` é de uma biblioteca chamada `Composer`_
e você pode ler mais sobre como instalá-la no capítulo
:ref:`Instalação <installation-updating-vendors>`.

O comando ``composer`` lê a partir do arquivo ``composer.json`` no raiz
do seu projeto. Ele é um arquivo formatado em JSON, que contém uma lista de cada
um dos pacotes externos que você precisa, a versão a ser baixada e muito mais.
O ``composer`` também lê a partir de um arquivo ``composer.lock``, que lhe permite
fixar cada biblioteca para uma versão **exata**. Na verdade, se um arquivo ``composer.lock``
existe, as versões dentro dele substituirão aquelas em ``composer.json``.
Para atualizar suas bibliotecas para novas versões, execute ``composer update``.

.. tip::

    Se você quiser adicionar um novo pacote na sua aplicação, execute o comando composer
    ``require``:

    .. code-block:: bash

        $ composer require doctrine/doctrine-fixtures-bundle

Para saber mais sobre o Composer, veja `GetComposer.org`_:

É importante perceber que essas bibliotecas vendor *não* são realmente parte
do *seu* repositório. Em vez disso, elas são simplesmente arquivos não monitorados que são baixados
para o diretório ``vendor/``. Mas, uma vez que todas as informações necessárias para baixar esses
arquivos são guardadas em ``composer.json`` e ``composer.lock`` (que *são* armazenados
no repositório), qualquer outro desenvolvedor pode usar o projeto, executando ``composer install``,
e baixando exatamente o mesmo conjunto de bibliotecas vendor. Isso significa que você está
controlando exatamente como cada biblioteca vendor parece, sem a necessidade de
realmente fazer o commit delas para *seu* repositório.

Assim, sempre que um desenvolvedor usa o seu projeto, deve executar o secript ``composer install``
para assegurar que todas as bibliotecas vendor necessários são baixadas.

.. sidebar:: Atualizando o Symfony

    Uma vez que o Symfony é apenas um grupo de bibliotecas de terceiros e que essas
    bibliotecas são inteiramente controladas através de ``composer.json`` e ``composer.lock``,
    a atualização do Symfony significa simplesmente atualizar cada um desses arquivos para coincidir
    seu estado com a última Edição Standard do Symfony.

    Claro, se você adicionou novas entradas no ``composer.json``, certifique-se
    de substituir apenas as partes originais (ou seja, cuidado para não excluir também qualquer uma
    de suas entradas personalizadas).

.. _Composer: https://getcomposer.org/
.. _GetComposer.org: https://getcomposer.org/
