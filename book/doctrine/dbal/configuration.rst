.. index::
   single: Configuration; Doctrine DBAL
   single: Doctrine; DBAL configuration

Configuração
=============

Um exemplo de configuração com banco de dados MySQL pode parecer como o 
seguinte exemplo:

.. code-block:: yaml

    # app/config/config.yml
    doctrine:
        dbal:
            connections:
                default:
                    driver:   pdo_mysql
                    dbname:   Symfony2
                    user:     root
                    password: null

O DoctrineBundle suporta todos os parâmetros que todos os drivers padrões do doctrine
aceitam, convertidos para os padrões de nomenclatura do XML ou YAML que o Symfony aplica.
Veja a `documentation`_ do Doctrine DBAL para mais informações. Além disso, existem algumas 
opções relacionadas ao Symfony que você pode configurar. O bloco a seguir mostra todas
as chaves de configuração possíveis, sem maiores explicacações de seus significados.

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            dbal:
                connections:
                    default:
                        dbname:               database
                        host:                 localhost
                        port:                 1234
                        user:                 user
                        password:             secret
                        driver:               pdo_mysql
                        driver_class:         MyNamespace\MyDriverImpl
                        options:
                            foo: bar
                        path:                 %kernel.data_dir%/data.sqlite
                        memory:               true
                        unix_socket:          /tmp/mysql.sock
                        wrapper_class:        MyDoctrineDbalConnectionWrapper
                        charset:              UTF8
                        logging:              %kernel.debug%
                        platform_service:     MyOwnDatabasePlatformService

    .. code-block:: xml

        <!-- xmlns:doctrine="http://symfony.com/schema/dic/doctrine" -->
        <!-- xsi:schemaLocation="http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd"> -->

        <doctrine:config>
            <doctrine:dbal>
                <doctrine:connection
                    name="default"
                    dbname="database"
                    host="localhost"
                    port="1234"
                    user="user"
                    password="secret"
                    driver="pdo_mysql"
                    driver-class="MyNamespace\MyDriverImpl"
                    path="%kernel.data_dir%/data.sqlite"
                    memory="true"
                    unix-socket="/tmp/mysql.sock"
                    wrapper-class="MyDoctrineDbalConnectionWrapper"
                    charset="UTF8"
                    logging="%kernel.debug%"
                    platform-service="MyOwnDatabasePlatformService"
                />
            </doctrine:dbal>
        </doctrine:config>

Também existem um monte de parâmetros do dependency injection container
que permitem a você especificar quais classes são usadas (com seus valores padrões):

.. code-block:: yaml

    parameters:
        doctrine.dbal.logger_class: Symfony\Bundle\DoctrineBundle\Logger\DbalLogger
        doctrine.dbal.configuration_class: Doctrine\DBAL\Configuration
        doctrine.data_collector.class: Symfony\Bundle\DoctrineBundle\DataCollector\DoctrineDataCollector
        doctrine.dbal.event_manager_class: Doctrine\Common\EventManager
        doctrine.dbal.events.mysql_session_init.class: Doctrine\DBAL\Event\Listeners\MysqlSessionInit
        doctrine.dbal.events.oracle_session_init.class: Doctrine\DBAL\Event\Listeners\OracleSessionInit

Se você quer configurar multiplas conexões, você pode fazer isso simplesmente listando
elas sob a chave ``connections``. Todos os parâmetros mostrados acima podem
também ser especificados nas sub-chaves da conexão.

.. code-block:: yaml

    doctrine:
        dbal:
            default_connection:       default
            connections:
                default:
                    dbname:           Symfony2
                    user:             root
                    password:         null
                    host:             localhost
                customer:
                    dbname:           customer
                    user:             root
                    password:         null
                    host:             localhost

Se você tem definidas multiplas conexões, você pode usar o 
``$this->get('doctrine.dbal.[connectionname]_connection')`` também, 
mas você deve passar a ele um argumento com o nome 
da conexão que você quer ter

.. code-block:: php

    class UserController extends Controller
    {
        public function indexAction()
        {
            $defaultConn1 = $this->get('doctrine.dbal.connection');
            $defaultConn2 = $this->get('doctrine.dbal.default_connection');
            // $defaultConn1 === $defaultConn2

            $customerConn = $this->get('doctrine.dbal.customer_connection');
        }
    }

.. _documentation: http://www.doctrine-project.org/docs/dbal/2.0/en
