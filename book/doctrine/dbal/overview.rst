.. index::
   pair: Doctrine; DBAL

DBAL
====

O `Doctrine`_ Database Abstraction Layer (DBAL) é uma camada de abstração
que senta no topo do `PDO`_ e oferece uma API intuitiva e flexível para
comunicação com os bancos de dados relacionais mais populares que existem hoje!

.. tip::

	Você pode ler mais sobre o Doctrine DBAL no website da `documentação`_  oficial.

Para iniciar você só precisa habilitar e configurar o DBAL:

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

Você pode então acessar a conexão Doctrine DBAL acessando o 
serviço ``database_connection``:

.. code-block:: php

    class UserController extends Controller
    {
        public function indexAction()
        {
            $conn = $this->get('database_connection');

            $users = $conn->fetchAll('SELECT * FROM users');
        }
    }

.. _PDO:           http://www.php.net/pdo
.. _documentação:  http://www.doctrine-project.org/docs/dbal/2.0/en
.. _Doctrine:      http://www.doctrine-project.org
