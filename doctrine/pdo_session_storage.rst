.. index::
    single: Sessão; Armazenamento em Banco de Dados

Como usar o PdoSessionHandler para Armazenar as Sessões no Banco de Dados
=========================================================================

O armazenamento de sessão padrão do Symfony grava as informações da sessão em arquivos.
A maioria dos sites médios e grandes usam um banco de dados para armazenar os valores de sessão
em vez de arquivos, porque os bancos de dados são mais fáceis de usar e escalam em um
ambiente de vários servidores web.

O Symfony tem uma solução interna para o armazenamento de sessão em banco de dados chamado
:class:`Symfony\\Component\\HttpFoundation\\Session\\Storage\\Handler\\PdoSessionHandler`.
Para usá-lo, você só precisa mudar alguns parâmetros no arquivo de configuração principal:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session:
                # ...
                handler_id: session.handler.pdo

        services:
            session.handler.pdo:
                class:     Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler
                public:    false
                arguments:
                    - 'mysql:dbname=mydatabase'
                    - { db_username: myuser, db_password: mypassword }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:session handler-id="session.handler.pdo" cookie-lifetime="3600" auto-start="true"/>
        </framework:config>

        <services>
            <service id="session.handler.pdo" class="Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler" public="false">
                <argument>mysql:dbname=mydatabase</agruement>
                <argument type="collection">
                    <argument key="db_username">myuser</argument>
                    <argument key="db_password">mypassword</argument>
                </argument>
            </service>
        </services>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->loadFromExtension('framework', array(
            ...,
            'session' => array(
                // ...,
                'handler_id' => 'session.handler.pdo',
            ),
        ));

        $storageDefinition = new Definition('Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler', array(
            'mysql:dbname=mydatabase',
            array('db_username' => 'myuser', 'db_password' => 'mypassword')
        ));
        $container->setDefinition('session.handler.pdo', $storageDefinition);

Configurando os Nomes de Tabelas e Colunas
------------------------------------------

Isto irá esperar uma tabela ``sessions`` com um número de diferentes colunas.
O nome da tabela, e de todos os nomes de coluna, podem ser configurados passando
um segundo argumento array para ``PdoSessionHandler``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            # ...
            session.handler.pdo:
                class:     Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler
                public:    false
                arguments:
                    - 'mysql:dbname=mydatabase'
                    - { db_table: sessions, db_username: myuser, db_password: mypassword }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <services>
            <service id="session.handler.pdo" class="Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler" public="false">
                <argument>mysql:dbname=mydatabase</agruement>
                <argument type="collection">
                    <argument key="db_table">sessions</argument>
                    <argument key="db_username">myuser</argument>
                    <argument key="db_password">mypassword</argument>
                </argument>
            </service>
        </services>

    .. code-block:: php

        // app/config/config.php

        use Symfony\Component\DependencyInjection\Definition;
        // ...

        $storageDefinition = new Definition('Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler', array(
            'mysql:dbname=mydatabase',
            array('db_table' => 'sessions', 'db_username' => 'myuser', 'db_password' => 'mypassword')
        ));
        $container->setDefinition('session.handler.pdo', $storageDefinition);

Estes são os parâmetros que você deve configurar:

``db_table`` (default ``sessions``):
    O nome da tabela de sessão no banco de dados;

``db_id_col`` (default ``sess_id``):
    O nome da coluna id em sua tabela de sessão (VARCHAR(128));

``db_data_col`` (default ``sess_data``):
    O nome da coluna value em sua tabela de sessão (BLOB);

``db_time_col`` (default ``sess_time``):
    O nome da coluna time em sua tabela de sessão (INTEGER);

``db_lifetime_col`` (default ``sess_lifetime``):
    O nome da coluna lifetime em sua tabela de sessão (INTEGER).


Compartilhando suas Informações de Conexão do Banco de Dados
------------------------------------------------------------

Com a configuração fornecida, as configurações de conexão do banco de dados são definidas somente
para a conexão de armazenamento de sessão. Isso está correto quando você usa um banco de dados
separado para os dados da sessão.

Mas, se você gostaria de armazenar os dados da sessão no mesmo banco de dados com o resto
dos dados do seu projeto, você pode usar as configurações de conexão do arquivo
``parameters.yml`` referenciando os parâmetros relacionados ao banco de dados definidos lá:

.. configuration-block::

    .. code-block:: yaml

        services:
            session.handler.pdo:
                class:     Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler
                public:    false
                arguments:
                    - 'mysql:host=%database_host%;port=%database_port%;dbname=%database_name%'
                    - { db_username: '%database_user%', db_password: '%database_password%' }

    .. code-block:: xml

        <service id="session.handler.pdo" class="Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler" public="false">
            <argument>mysql:host=%database_host%;port=%database_port%;dbname=%database_name%</argument>
            <argument type="collection">
                <argument key="db_username">%database_user%</argument>
                <argument key="db_password">%database_password%</argument>
            </argument>
        </service>

    .. code-block:: php

        $storageDefinition = new Definition('Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler', array(
            'mysql:host=%database_host%;port=%database_port%;dbname=%database_name%',
            array('db_username' => '%database_user%', 'db_password' => '%database_password%')
        ));

.. _example-sql-statements:

Preparando o Banco de Dados para Armazenar as Sessões
-----------------------------------------------------

Antes de armazenar sessões no banco de dados, você deve criar a tabela que armazena
a informação. As seções a seguir contêm alguns exemplos de instruções SQL que
você pode usar para o seu banco de dados específico.

MySQL
~~~~~

.. code-block:: sql

    CREATE TABLE `sessions` (
        `sess_id` VARBINARY(128) NOT NULL PRIMARY KEY,
        `sess_data` BLOB NOT NULL,
        `sess_time` INTEGER UNSIGNED NOT NULL,
        `sess_lifetime` MEDIUMINT NOT NULL
    ) COLLATE utf8_bin, ENGINE = InnoDB;

.. note::

    Um tipo de coluna ``BLOB`` só pode armazenar até 64 kb. Se os dados armazenados em
    uma sessão do usuário excederem esse valor, uma exceção pode ser lançada ou a sessão dele
    será silenciosamente resetada. Considere o uso de um ``MEDIUMBLOB`` se precisar de mais
    espaço.

PostgreSQL
~~~~~~~~~~

.. code-block:: sql

    CREATE TABLE sessions (
        sess_id VARCHAR(128) NOT NULL PRIMARY KEY,
        sess_data BYTEA NOT NULL,
        sess_time INTEGER NOT NULL,
        sess_lifetime INTEGER NOT NULL
    );

Microsoft SQL Server
~~~~~~~~~~~~~~~~~~~~

.. code-block:: sql

    CREATE TABLE [dbo].[sessions](
        [sess_id] [nvarchar](255) NOT NULL,
        [sess_data] [ntext] NOT NULL,
        [sess_time] [int] NOT NULL,
        [sess_lifetime] [int] NOT NULL,
        PRIMARY KEY CLUSTERED(
            [sess_id] ASC
        ) WITH (
            PAD_INDEX  = OFF,
            STATISTICS_NORECOMPUTE  = OFF,
            IGNORE_DUP_KEY = OFF,
            ALLOW_ROW_LOCKS  = ON,
            ALLOW_PAGE_LOCKS  = ON
        ) ON [PRIMARY]
    ) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

.. caution::

    Se os dados da sessão não couberem na coluna de dados, eles podem ser truncados
    pelo mecanismo de banco de dados. Para piorar a situação, quando os dados da sessão ficam
    corrompidos, o PHP ignora os dados sem avisar.

    Se a aplicação armazena grandes quantidades de dados de sessão, esse problema pode
    ser resolvido aumentando o tamanho da coluna (use ``BLOB`` ou mesmo ``MEDIUMBLOB``).
    Ao usar o MySQL como banco de dados, você também pode habilitar o `strict SQL mode`_
    para ser notificado quando tal erro acontecer.

.. _`strict SQL mode`: https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html
