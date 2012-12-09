.. index::
    single: Console; Uso

Como usar o Console
===================

A página :doc:`/components/console/usage` da documentação dos componentes aborda
sobre as opções globais do console. Quando você usar o console como parte do
framework full stack, algumas opções globais adicionais também estão disponíveis.

Por padrão, os comandos de console executam no ambiente ``dev`` e você pode desejar
alterar isso para alguns comandos. Por exemplo, se você deseja executar alguns comandos
no ambiente ``prod`` por motivos de desempenho. Além disso, o resultado de alguns comandos
será diferente, dependendo do ambiente. Por exemplo, o comando ``cache:clear``
irá limpar e fazer o warm do cache apenas para o ambiente especificado. Para
limpar e realizar o warm do cache de ``prod`` você precisa executar:

.. code-block:: bash

    $ php app/console cache:clear --env=prod

ou o equivalente:

.. code-block:: bash

    $ php app/console cache:clear -e=prod

Além de alterar o ambiente, você pode optar também por desativar o modo de depuração.
Isto pode ser útil quando deseja-se executar comandos no ambiente ``dev``
mas evitar o impacto no desempenho devido a coleta de dados de depuração:

.. code-block:: bash

    $ php app/console list --no-debug

Há um shell interativo que permite à você digitar os comandos sem ter que
especificar ``php app/console`` toda vez, o que é útil se você precisa executar
vários comandos. Para entrar no shell execute:

.. code-block:: bash

    $ php app/console --shell
    $ php app/console -s

Agora você pode simplesmente executar comandos com o nome do comando:

.. code-block:: bash

    Symfony > list

Ao usar o shell você pode escolher em executar cada comando em um processo separado:

.. code-block:: bash

    $ php app/console --shell --process-isolation
    $ php app/console -s --process-isolation

Quando fizer isso, a saída não será colorida e a interatividade não é suportada,
então, você vai precisar passar todos os parâmetros de comando explicitamente.

.. note::

    A menos que você estiver usando processos isolados, limpar o cache no shell
    não terá efeito sobre os comandos subsequentes que você executar. Isto é porque
    os arquivos originais em cache ainda estão sendo usados.
