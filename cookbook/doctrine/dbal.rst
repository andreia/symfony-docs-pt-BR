.. index::
   pair: Doctrine; DBAL

Como usar a Camada DBAL do Doctrine
===================================

.. note::

    Este artigo é sobre a camada DBAL do Doctrine. Normalmente, você vai trabalhar com
    a camada de alto nível ORM do Doctrine, que simplesmente usa o DBAL, nos
    bastidores, para comunicar-se com o banco de dados. Para ler mais sobre
    o ORM Doctrine, consulte ":doc:`/book/doctrine`".

A Camada de Abstração de Banco de Dados `Doctrine`_ (DBAL) é uma camada de abstração que
fica situada no topo do `PDO`_ e oferece uma API intuitiva e flexível para se comunicar
com os bancos de dados relacionais mais populares. Em outras palavras, a biblioteca DBAL
torna mais fácil a execução de consultas e de outras ações de banco de dados.

.. tip::

    Leia a `Documentação oficial do DBAL`_ para aprender todos os detalhes
    e as capacidades da biblioteca DBAL do Doctrine.

Para começar, configure os parâmetros de conexão do banco de dados:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        doctrine:
            dbal:
                driver:   pdo_mysql
                dbname:   Symfony2
                user:     root
                password: null
                charset:  UTF8

    .. code-block:: xml

        // app/config/config.xml
        <doctrine:config>
            <doctrine:dbal
                name="default"
                dbname="Symfony2"
                user="root"
                password="null"
                driver="pdo_mysql"
            />
        </doctrine:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('doctrine', array(
            'dbal' => array(
                'driver'    => 'pdo_mysql',
                'dbname'    => 'Symfony2',
                'user'      => 'root',
                'password'  => null,
            ),
        ));

Para ver todas as opções de configuração do DBAL, consulte :ref:`reference-dbal-configuration`.

Você pode acessar a conexão DBAL do Doctrine acessando o
serviço ``database_connection``:

.. code-block:: php

    class UserController extends Controller
    {
        public function indexAction()
        {
            $conn = $this->get('database_connection');
            $users = $conn->fetchAll('SELECT * FROM users');

            // ...
        }
    }

Registrando Tipos de Mapeamento Personalizados
----------------------------------------------

Você pode registrar tipos de mapeamento personalizados através de configuração do symfony. Eles 
serão adicionados à todas as conexões configuradas. Para mais informações sobre tipos de 
mapeamento personalizados, leia a seção `Tipos de Mapeamento Personalizados`_ na documentação do Doctrine.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        doctrine:
            dbal:
                types:
                    custom_first: Acme\HelloBundle\Type\CustomFirst
                    custom_second: Acme\HelloBundle\Type\CustomSecond

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal>
                    <doctrine:type name="custom_first" class="Acme\HelloBundle\Type\CustomFirst" />
                    <doctrine:type name="custom_second" class="Acme\HelloBundle\Type\CustomSecond" />
                </doctrine:dbal>
            </doctrine:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('doctrine', array(
            'dbal' => array(
                'types' => array(
                    'custom_first'  => 'Acme\HelloBundle\Type\CustomFirst',
                    'custom_second' => 'Acme\HelloBundle\Type\CustomSecond',
                ),
            ),
        ));

Registrando Tipos de Mapeamento Personalizados no SchemaTool
------------------------------------------------------------

O SchemaTool é usado para inspecionar o banco de dados para comparar o esquema. Para
realizar esta tarefa, ele precisa saber que tipo de mapeamento precisa ser usado
para cada um dos tipos do banco de dados. O registro de novos pode ser feito através de configuração.

Vamos mapear o tipo ENUM (por padrão não suportado pelo DBAL) para um tipo de mapeamento
``string``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        doctrine:
            dbal:
                connections:
                    default:
                        // Other connections parameters
                        mapping_types:
                            enum: string

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal>
                <doctrine:dbal default-connection="default">
                    <doctrine:connection>
                        <doctrine:mapping-type name="enum">string</doctrine:mapping-type>
                    </doctrine:connection>
                </doctrine:dbal>
            </doctrine:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('doctrine', array(
            'dbal' => array(
                'connections' => array(
                    'default' => array(
                        'mapping_types' => array(
                            'enum'  => 'string',
                        ),
                    ),
                ),
            ),
        ));

.. _`PDO`:           http://www.php.net/pdo
.. _`Doctrine`:      http://www.doctrine-project.org
.. _`Documentação oficial do DBAL`: http://docs.doctrine-project.org/projects/doctrine-dbal/en/latest/index.html
.. _`Tipos de Mapeamento Personalizados`: http://docs.doctrine-project.org/projects/doctrine-dbal/en/latest/reference/types.html#custom-mapping-types
