.. index::
   pair: Doctrine; MongoDB ODM

ODM MongoDB
===========

O `Object Document Mapper` `MongoDB`_ é muito parecido com o ORM Doctrine2 na forma como
ele funciona e na sua arquitetura. Você lida somente com objetos PHP simples e eles
são persistidos transparente sem impor em seu modelo de domínio.

.. tip::

   Você pode ler mais sobre o Doctrine `Object Document Mapper` MongoDB na 
   `documentação`_ dos projetos.

Para começar a trabalhar com o Doctrine e o `Object Document Mapper` MongoDB basta 
ativá-los e especificar o bundle que contém os documentos mapeados:

.. code-block:: yaml

    # app/config/config.yml

    doctrine_mongo_db:
        document_managers:
            default:
                mappings:
                    AcmeHelloBundle: ~

.. configuration-block::

    .. code-block:: php-annotations

        // Acme/HelloBundle/Document/User.php

        namespace Acme\HelloBundle\Document;

        /**
         * @mongodb:Document(collection="users")
         */
        class User
        {
            /**
             * @mongodb:Id
             */
            protected $id;

            /**
             * @mongodb:Field(type="string")
             */
            protected $name;

            /**
             * Get id
             *
             * @return integer $id
             */
            public function getId()
            {
                return $this->id;
            }

            /**
             * Set name
             *
             * @param string $name
             */
            public function setName($name)
            {
                $this->name = $name;
            }

            /**
             * Get name
             *
             * @return string $name
             */
            public function getName()
            {
                return $this->name;
            }
        }

    .. code-block:: yaml

        # Acme/HelloBundle/Resources/config/doctrine/metadata/mongodb/Acme.HelloBundle.Document.User.dcm.yml
        Acme\HelloBundle\Document\User:
            type: document
            collection: user
            fields:
                id:
                    id: true
                name:
                    type: string
                    length: 255

    .. code-block:: xml

        <!-- Acme/HelloBundle/Resources/config/doctrine/metadata/mongodb/Acme.HelloBundle.Document.User.dcm.xml -->
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
              xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                            http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <document name="Acme\HelloBundle\Document\User" collection="user">
                <field name="id" id="true" />
                <field name="name" type="string" length="255" />
            </document>

        </doctrine-mapping>

.. note::

    Ao utilizar annotations em seu projeto com o Symfony2 você deve informar os namespaces 
    de todas as annotations do Doctrine MongoDB com o prefixo ``mongodb:``.

.. tip::

    Se você utiliza YAML ou XML para descrever seus documentos, você pode omitir a 
    criação da classe Document, e deixar o comando ``doctrine:generate:documents``
    fazer isso por você.

Agora, use o seu documento e gerencie a persistência com o Doctrine:

.. code-block:: php

    use Acme\HelloBundle\Document\User;

    class UserController extends Controller
    {
        public function createAction()
        {
            $user = new User();
            $user->setName('Jonathan H. Wage');

            $dm = $this->get('doctrine.odm.mongodb.document_manager');
            $dm->persist($user);
            $dm->flush();

            // ...
        }

        public function editAction($id)
        {
            $dm = $this->get('doctrine.odm.mongodb.document_manager');
            $user = $dm->createQuery('find all from AcmeHelloBundle:User where id = ?', $id);
            $user->setBody('new body');
            $dm->flush();

            // ...
        }

        public function deleteAction($id)
        {
            $dm = $this->get('doctrine.odm.entity_manager');
            $user = $dm->createQuery('find all from AcmeHelloBundle:User where id = ?', $id);
            $dm->remove($user);
            $dm->flush();

            // ...
        }
    }

.. _MongoDB:      http://www.mongodb.org/
.. _documentação: http://www.doctrine-project.org/docs/mongodb_odm/1.0/en
