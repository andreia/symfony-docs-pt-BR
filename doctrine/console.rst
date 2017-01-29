.. index::
   single: Doctrine; Comandos de console ORM
   single: CLI; ORM Doctrine

Comandos de Console
-------------------

A integração com o ORM Doctrine2 oferece vários comandos do console sob o
namespace ``doctrine``. Para ver a lista de comandos você pode usar o comando
``list``:

.. code-block:: bash

    $ php app/console list doctrine

Uma lista dos comandos disponíveis será impressa. Você pode descobrir mais informações
sobre qualquer um desses comandos (ou qualquer comando Symfony), executando o comando
``help``. Por exemplo, para obter detalhes sobre a tarefa ``doctrine:database:create``
, execute:

.. code-block:: bash

    $ php app/console help doctrine:database:create

Algumas tarefas notáveis ​​ou interessantes incluem:

* ``doctrine:ensure-production-settings`` - verifica se o ambiente
  atual está configurado de forma eficiente para produção. Deve sempre
  ser executado no ambiente ``prod``:

  .. code-block:: bash

      $ php app/console doctrine:ensure-production-settings --env=prod

* ``doctrine:mapping:import`` - permite ao Doctrine a introspecção de um banco de dados
  existente e criar as informações de mapeamento. Para obter mais informações, consulte
  :doc:`/cookbook/doctrine/reverse_engineering`.

* ``doctrine:mapping:info`` - exibe todas as entidades que o Doctrine
  está ciente e se há ou não algum erro básico com o mapeamento.

* ``doctrine:query:dql`` e ``doctrine:query:sql`` - permitem executar
  consultas DQL ou SQL diretamente na linha de comando.
