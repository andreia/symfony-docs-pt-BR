.. index::
   pair: Doctrine; Migrations

Migrações
=========

O recurso de migrações de banco de dados é uma extensão da camada de abstração 
de dados e oferece a capacidade de programar a implantação de novas versões de 
seu esquema de banco de dados de forma segura e padronizada.

.. tip::

   Você pode ler mais sobre Migrações de Banco de Dados com o Doctrine na 
   `documentação`_ dos projetos.

Todas as funcionalidades de migrações estão contidas em alguns comandos de console:

.. code-block:: bash

    doctrine:migrations    
      :diff     Gera uma migração comparando seu banco de dados atual com a informação de mapeamento.
      :execute  Executa uma versão individual `up` ou `down` de migração manualmente.
      :generate Gera uma classe de migração em branco.
      :migrate  Executa uma migração para uma versão especificada ou a última versão disponível.
      :status   Visualiza o status de um conjunto de migrações.
      :version  Adiciona e deleta manualmente versões de migração da tabela de versão.

Cada pacote gerencia as suas próprias migrações assim, ao trabalhar com os comandos acima, 
você deve especificar o pacote que deseja trabalhar. Por exemplo, para ver o status dos
pacotes de migrações, você pode executar o comando ``status``:

.. code-block:: bash

    $ php app/console doctrine:migrations:status

     == Configuration

        >> Name:                                               HelloBundle Migrations
        >> Configuration Source:                               manually configured
        >> Version Table Name:                                 hello_bundle_migration_versions
        >> Migrations Namespace:                               Application\Migrations
        >> Migrations Directory:                               /path/to/symfony-sandbox/app/DoctrineMigrations
        >> Current Version:                                    0
        >> Latest Version:                                     0
        >> Executed Migrations:                                0
        >> Available Migrations:                               0
        >> New Migrations:                                     0

Agora, podemos começar a trabalhar com as migrações gerando uma nova classe de migração 
em branco:

.. code-block:: bash

    $ php console doctrine:migrations:generate --bundle="Application\HelloBundle"
    Generated new migration class to "/path/to/symfony-sandbox/src/Bundle/HelloBundle/DoctrineMigrations/Version20100621140655.php"

.. tip::

    Pode ser necessário criar o diretório ``/path/to/project/app/DoctrineMigrations``
    antes de executar o comando ``doctrine:migrations:generate``.

Dê uma olhada na classe de migração recém-gerada e você verá o código 
semelhante ao seguinte::

    namespace Application\Migrations;

    use Doctrine\DBAL\Migrations\AbstractMigration,
        Doctrine\DBAL\Schema\Schema;

    class Version20100621140655 extends AbstractMigration
    {
        public function up(Schema $schema)
        {

        }

        public function down(Schema $schema)
        {

        }
    }

Se você executar o comando ``status`` para o ``HelloBundle`` ele irá mostrar que 
você tem uma nova migração para executar:

.. code-block:: bash

    $ php app/console doctrine:migrations:status

     == Configuration

       >> Name:                                               HelloBundle Migrations
       >> Configuration Source:                               manually configured
       >> Version Table Name:                                 hello_bundle_migration_versions
       >> Migrations Namespace:                               Application\Migrations
       >> Migrations Directory:                               /path/to/symfony-sandbox/app/DoctrineMigrations
       >> Current Version:                                    0
       >> Latest Version:                                     2010-06-21 14:06:55 (20100621140655)
       >> Executed Migrations:                                0
       >> Available Migrations:                               1
       >> New Migrations:                                     1

    == Migration Versions

       >> 2010-06-21 14:06:55 (20100621140655)                not migrated

Agora, você pode adicionar algum código de migração aos métodos ``up()`` e ``down()`` e
migrar:

.. code-block:: bash

    $ php app/console doctrine:migrations:migrate

.. _documentação: http://www.doctrine-project.org/docs/migrations/2.0/en
