.. index::
   single: Sessão; Armazenamento em Banco de Dados

Como usar o PdoSessionStorage para armazenar as Sessões no Banco de Dados
=========================================================================

O armazenamento de sessão padrão do Symfony2 grava as informações da sessão em arquivo(s).
A maioria dos sites de médio à grande porte utilizam um banco de dados para armazenar os
valores da sessão, em vez de arquivos, pois os bancos de dados são mais fáceis de usar e
escalam em um ambiente multi-servidor.

O Symfony2 tem uma solução interna para o armazenamento de sessão em banco de dados chamado
:class:`Symfony\\Component\\HttpFoundation\\SessionStorage\\PdoSessionStorage`.
Para usá-lo, você só precisa alterar alguns parâmetros no ``config.yml`` (ou o
formato de configuração de sua escolha):

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session:
                # ...
                storage_id:     session.storage.pdo

        parameters:
            pdo.db_options:
                db_table:    session
                db_id_col:   session_id
                db_data_col: session_value
                db_time_col: session_time

        services:
            pdo:
                class: PDO
                arguments:
                    dsn:      "mysql:dbname=mydatabase"
                    user:     myuser
                    password: mypassword

            session.storage.pdo:
                class:     Symfony\Component\HttpFoundation\SessionStorage\PdoSessionStorage
                arguments: [@pdo, %session.storage.options%, %pdo.db_options%]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:session storage-id="session.storage.pdo" default-locale="en" lifetime="3600" auto-start="true"/>
        </framework:config>

        <parameters>
            <parameter key="pdo.db_options" type="collection">
                <parameter key="db_table">session</parameter>
                <parameter key="db_id_col">session_id</parameter>
                <parameter key="db_data_col">session_value</parameter>
                <parameter key="db_time_col">session_time</parameter>
            </parameter>
        </parameters>

        <services>
            <service id="pdo" class="PDO">
                <argument>mysql:dbname=mydatabase</argument>
                <argument>myuser</argument>
                <argument>mypassword</argument>
            </service>

            <service id="session.storage.pdo" class="Symfony\Component\HttpFoundation\SessionStorage\PdoSessionStorage">
                <argument type="service" id="pdo" />
                <argument>%session.storage.options%</argument>
                <argument>%pdo.db_options%</argument>
            </service>
        </services>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->loadFromExtension('framework', array(
            // ...
            'session' => array(
                ...,
                'storage_id' => 'session.storage.pdo',
            ),
        ));

        $container->setParameter('pdo.db_options', array(
            'db_table'      => 'session',
            'db_id_col'     => 'session_id',
            'db_data_col'   => 'session_value',
            'db_time_col'   => 'session_time',
        ));

        $pdoDefinition = new Definition('PDO', array(
            'mysql:dbname=mydatabase',
            'myuser',
            'mypassword',
        ));
        $container->setDefinition('pdo', $pdoDefinition);

        $storageDefinition = new Definition('Symfony\Component\HttpFoundation\SessionStorage\PdoSessionStorage', array(
            new Reference('pdo'),
            '%session.storage.options%',
            '%pdo.db_options%',
        ));
        $container->setDefinition('session.storage.pdo', $storageDefinition);

* ``db_table``: O nome da tabela de sessão no seu banco de dados
* ``db_id_col``: O nome da coluna id na sua tabela de sessão (VARCHAR (255) ou maior)
* ``db_data_col``: O nome da coluna value na sua tabela de sessão (TEXT ou CLOB)
* ``db_time_col``: O nome da coluna time em sua tabela de sessão (INTEGER)

Compartilhando suas Informações de Conexão do Banco de Dados
------------------------------------------------------------

Com a configuração fornecida, as configurações de conexão do banco de dados são definidas
somente para a conexão de armazenamento de sessão. Isto está OK quando você usa um banco de dados
separado para os dados da sessão.

Mas, se você gostaria de armazenar os dados da sessão no mesmo banco de dados que o resto
dos dados do seu projeto, você pode usar as definições de conexão do
parameter.ini referenciando os parâmetros relacionados ao banco de dados definidos lá:

.. configuration-block::

    .. code-block:: yaml

        pdo:
            class: PDO
            arguments:
                - "mysql:dbname=%database_name%"
                - %database_user%
                - %database_password%

    .. code-block:: xml

        <service id="pdo" class="PDO">
            <argument>mysql:dbname=%database_name%</argument>
            <argument>%database_user%</argument>
            <argument>%database_password%</argument>
        </service>

    .. code-block:: php

        $pdoDefinition = new Definition('PDO', array(
            'mysql:dbname=%database_name%',
            '%database_user%',
            '%database_password%',
        ));

Exemplo de Instruções SQL
-------------------------

MySQL
~~~~~

A instrução SQL para criar a tabela de banco de dados necessária pode ser semelhante a
seguinte (MySQL):

.. code-block:: sql

    CREATE TABLE `session` (
        `session_id` varchar(255) NOT NULL,
        `session_value` text NOT NULL,
        `session_time` int(11) NOT NULL,
        PRIMARY KEY (`session_id`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

PostgreSQL
~~~~~~~~~~

Para o PostgreSQL, a declaração deve ficar assim:

.. code-block:: sql

    CREATE TABLE session (
        session_id character varying(255) NOT NULL,
        session_value text NOT NULL,
        session_time integer NOT NULL,
        CONSTRAINT session_pkey PRIMARY KEY (session_id)
    );
