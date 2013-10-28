.. index::
   single: Propel

Banco de Dados e Propel
====================

Uma das mais comuns e desafiadoras tarefas para qualquer aplicação está
envolvida em persistência e leitura de informações de um banco de dados.
Symfony2 não faz essa integração de ORM, mas o Propel faz facilmente.
Para instalar o Propel, leia `Trabalhando com Symfony2`_ na documentação
do Propel.

Um Simples Exemplo: Um Produto
---------------------------

Nesta seção, você configurará seu banco de dados, criará um objeto ``Product``,
o persistrá e o buscará no banco de dados.

.. sidebar:: Código de exemplo

    Acompanhando o exemplo deste capítulo, crie um ``AcmeStoreBundle`` através:
    
    .. code-block:: bash

        $ php app/console generate:bundle --namespace=Acme/StoreBundle

Configurando o Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~

Antes de começar, precisamos configurar a conexão com o banco de dados.
Por conveção, esta informação é configurada no arquivo 
``app/config/parameters.yml``:

.. code-block:: yaml

    # app/config/parameters.yml
    parameters:
        database_driver:   mysql
        database_host:     localhost
        database_name:     test_project
        database_user:     root
        database_password: password
        database_charset:  UTF8

.. note::

    A configuração através do ``parameters.yml`` é apenas uma convenção.
    Os parâmetros definidos para o Propel devem ser referenciados na
    configuração principal:

Estes parâmetros definidos no ``parameters.yml`` podem ser incluídos no
arquivo de configuração (``config.yml``):

.. code-block:: yaml

    propel:
        dbal:
            driver:     "%database_driver%"
            user:       "%database_user%"
            password:   "%database_password%"
            dsn:        "%database_driver%:host=%database_host%;dbname=%database_name%;charset=%database_charset%"

Agora o Propel conhece seu banco de dados e o Symfony2 pode criar o
banco de dados para você:

.. code-block:: bash

    $ php app/console propel:database:create

.. note::

    Neste exemplo, temos apenas uma conexão, nomeada ``default``. Se precisar
    configurar mais de uma conexão, leia o `Configuração do PropelBundle`_.

Criando uma Classe Model
~~~~~~~~~~~~~~~~~~~~~~

No mundo Propel, classes de ActiveRecord são conhecidas como **models**, porque 
essas classes geradas pelo Propel contém algumas lógicas de negócio.

.. note::

    Para as pessoas que usam Symfony2 com Doctrine2, **models** são equivalentes
    às **entities**.

Supondo que você está construindo uma aplicação onde os produtos precisam ser
exibidos. Primeiro, crie um arquivo ``schema.xml`` dentro do diretório
``Resources/config`` do seu ``AcmeStoreBundle``:

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8" ?>
    <database name="default"
        namespace="Acme\StoreBundle\Model"
        defaultIdMethod="native">
        <table name="product">
            <column name="id"
                type="integer"
                required="true"
                primaryKey="true"
                autoIncrement="true"
            />
            <column name="name"
                type="varchar"
                primaryString="true"
                size="100"
            />
            <column name="price"
                type="decimal"
            />
            <column name="description"
                type="longvarchar"
            />
        </table>
    </database>

Criando o Model
~~~~~~~~~~~~~~~~~~

Depois de criar seu ``schema.xml``, gere seu model executando o comando:

.. code-block:: bash

    $ php app/console propel:model:build

Este comando gera cada classe de model, para serem alteradas em sua aplicação, no
diretório ``Model/`` no bundle ``AcmeStoreBundle``.

Criando Tabelas/Schemas no Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora você tem uma classe ``Product`` usável e tudo que precisa é persistí-la.
Claro que você não tem a tabela ``product`` correspondente em seu banco de dados.
Felizmente, o Propel pode criar automaticamente todas as tabelas necessárias
no banco de dados para seus models da aplicação. Execute estes comandos para criar 
a tabela:

.. code-block:: bash

    $ php app/console propel:sql:build
    $ php app/console propel:sql:insert --force

Agora seu banco de dados tem a tabela ``product`` com as colunas que coincidem
com seu schema especificado.

.. tip::

    Você pode executar os últimos três comandos combinados, apenas com o seguinte
    comando: ``php app/console propel:build --insert-sql``.

Persistindo Objeto no Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora você tem um objeto ``Product`` e a tabela ``product`` correspondente,
você está pronto para persistir dados no banco de dados. Pode fazer isso facilmente
a partir de dentro de um controller. Adicione um método no ``DefaultController`` do
bundle::

    // src/Acme/StoreBundle/Controller/DefaultController.php

    // ...
    use Acme\StoreBundle\Model\Product;
    use Symfony\Component\HttpFoundation\Response;

    public function createAction()
    {
        $product = new Product();
        $product->setName('A Foo Bar');
        $product->setPrice(19.99);
        $product->setDescription('Lorem ipsum dolor');

        $product->save();

        return new Response('Created product id '.$product->getId());
    }

Nesta parte do código, você instancia e trabalha com o objeto ``$product``.
Quando você chamar o método ``save()``, os dados serão persistidos no banco 
de dados. Não precisa usar outros serviços, o objeto sabe como ser persistido.

.. note::

    Acompanhando este exemplo, você precisará criar uma :doc:`rota <routing>` 
    que aponta para esta action para vê-la em ação.

Buscando Objetos a partir do Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Buscar um objeto a partir do banco de dados é ainda mais fácil. Por exemplo, 
suponha que você tem configurada uma rota baseada no valor do ``id`` de um
``Product`` específico e quer exibí-lo::

    // ...
    use Acme\StoreBundle\Model\ProductQuery;

    public function showAction($id)
    {
        $product = ProductQuery::create()
            ->findPk($id);

        if (!$product) {
            throw $this->createNotFoundException(
                'No product found for id '.$id
            );
        }

        // ... faça algo, como passar o objeto $product para dentro de um template
    }

Autalizando um Objeto
~~~~~~~~~~~~~~~~~~

Uma vez que você foi buscar o objeto a partir do Propel, atualizá-lo é fácil.
Supondo que você possui a rota mapeada a um id do Product para um método 
``updateAction`` no controller::

    // ...
    use Acme\StoreBundle\Model\ProductQuery;

    public function updateAction($id)
    {
        $product = ProductQuery::create()
            ->findPk($id);

        if (!$product) {
            throw $this->createNotFoundException(
                'No product found for id '.$id
            );
        }

        $product->setName('New product name!');
        $product->save();

        return $this->redirect($this->generateUrl('homepage'));
    }

Atualizar um objeto envolve três passos:

#. buscar o objeto com Propel (line 6 - 13);
#. modificar o objeto (line 15);
#. salvá-lo (line 16).

Removendo um Obejto
~~~~~~~~~~~~~~~~~~

Remover um objeto é similar ao atualizar, mas requer uma chamada para o método
``delete()`` no objeto::

    $product->delete();

Consultando Objetos
--------------------

Propel providencia classes de ``Query`` para executar consultas básicas e complexas
sem muito trabalho::

    \Acme\StoreBundle\Model\ProductQuery::create()->findPk($id);

    \Acme\StoreBundle\Model\ProductQuery::create()
        ->filterByName('Foo')
        ->findOne();

Imagine que você precisa consultar produtos com custo acima de 19,99, ordenados
do mais barato para o mais caro. Dentro do controller, faça o seguinte::

    $products = \Acme\StoreBundle\Model\ProductQuery::create()
        ->filterByPrice(array('min' => 19.99))
        ->orderByPrice()
        ->find();

Na primeira linha, você pega seus produtos totalmente orientado a objetos. Não
precisa perder seu tempo com SQL ou qualquer coisa do tipo, Symfony2 oferece
uma programação totalmente orientada a objetos e o Propel segue a mesma filosofia
oferecendo uma incrível camada de abstração.

Se você precisa reusar alguma consulta, você pode adicionar novos métodos 
na classe ``ProductQuery``::

    // src/Acme/StoreBundle/Model/ProductQuery.php
    class ProductQuery extends BaseProductQuery
    {
        public function filterByExpensivePrice()
        {
            return $this
                ->filterByPrice(array('min' => 1000));
        }
    }

Mas perceba que o Propel gera vários métodos para você e um simples
``findAllOrderedByName()`` pode ser escrito sem qualquer esforço::

    \Acme\StoreBundle\Model\ProductQuery::create()
        ->orderByName()
        ->find();

Relacionamentos/Associações
--------------------------

Suponha que os produtos em sua aplicação são todos de uma "categoria". Neste caso,
você precisará de um obejto ``Category`` e uma relação com um objeto ``Product``

Inicie adicionando a definição de ``category`` no seu ``schema.xml``:

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8" ?>
    <database name="default"
        namespace="Acme\StoreBundle\Model"
        defaultIdMethod="native">
        <table name="product">
            <column name="id"
                type="integer"
                required="true"
                primaryKey="true"
                autoIncrement="true" />

            <column name="name"
                type="varchar"
                primaryString="true"
                size="100" />

            <column name="price"
                type="decimal" />

            <column name="description"
                type="longvarchar" />

            <column name="category_id"
                type="integer" />

            <foreign-key foreignTable="category">
                <reference local="category_id" foreign="id" />
            </foreign-key>
        </table>

        <table name="category">
            <column name="id"
                type="integer"
                required="true"
                primaryKey="true"
                autoIncrement="true" />

            <column name="name"
                type="varchar"
                primaryString="true"
                size="100" />
       </table>
    </database>

Crie as classes:

.. code-block:: bash

    $ php app/console propel:model:build

Assumindo que você tem produtos no seu banco de dados, você não precisa perdê-los.
Graças as migrations, Propel será capaz de atualizar seu banco de dados sem perder
dados existentes.

.. code-block:: bash

    $ php app/console propel:migration:generate-diff
    $ php app/console propel:migration:migrate

Seu banco de dados foi atualizado, você pode continuar escrevendo sua aplicação.

Salvando Objetos Relacionados
~~~~~~~~~~~~~~~~~~~~~~

Agora, coloque este código na action. Imagine que você está dentro de um controller::

    // ...
    use Acme\StoreBundle\Model\Category;
    use Acme\StoreBundle\Model\Product;
    use Symfony\Component\HttpFoundation\Response;

    class DefaultController extends Controller
    {
        public function createProductAction()
        {
            $category = new Category();
            $category->setName('Main Products');

            $product = new Product();
            $product->setName('Foo');
            $product->setPrice(19.99);
            // relate this product to the category
            $product->setCategory($category);

            // save the whole
            $product->save();

            return new Response(
                'Created product id: '.$product->getId().' and category id: '.$category->getId()
            );
        }
    }

Agora, uma única linha será adicionada à tabelas ``category`` e ``product``.
Na coluna ``product.category_id``, do novo produto, será definido um id qualquer 
da nova cateogria. O Propel gerencia a persistência deste relecionamento para você.

Buscando Objetos Relacionados
~~~~~~~~~~~~~~~~~~~~~~~~

Quando você precisa buscar objetos relacionados, veja como fica o fluxo de trabalho.
Primeiro, busque um objeto ``$product`` e então acesse o objeto ``Category`` relacionado::

    // ...
    use Acme\StoreBundle\Model\ProductQuery;

    public function showAction($id)
    {
        $product = ProductQuery::create()
            ->joinWithCategory()
            ->findPk($id);

        $categoryName = $product->getCategory()->getName();

        // ...
    }

Veja que no exemplo acima, apenas uma consulta foi feita.

Mais informações sobre Associações
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você precisa encontrar mais informações sobre relações lendo o capítulo dedicado 
aos `Relacionamentos`_.

Callbacks do ciclo de vida
-------------------

As vezes, você precisa executar uma certa ação antes ou depois que um objeto
for inserido, atualizado ou apagado. Estes tipos de ações são conhecidas como
callback de ciclo de vida ou "hooks" (ganchos), como eles são métodos de 
callback que você precisa executar durante diferentes estágios do ciclo de 
vida de um objeto (ex. o objeto é inserido, atualizado, apagado, etc).

Para adicionar um hook, apenas adicione um novo método na classe::

    // src/Acme/StoreBundle/Model/Product.php

    // ...
    class Product extends BaseProduct
    {
        public function preInsert(\PropelPDO $con = null)
        {
            // do something before the object is inserted
        }
    }

Propel oferece os seguintes hooks:

* ``preInsert()`` código executado antes de inserir de um novo objeto
* ``postInsert()`` código executado depois de inserir de um novo objeto
* ``preUpdate()`` código executado antes de atualizar um objeto existente
* ``postUpdate()`` código executado depois de atualizar um objeto existente
* ``preSave()`` código executado antes de salvar um objeto (novo ou existente)
* ``postSave()`` código executado depois de salvar um objeto (novo ou existente)
* ``preDelete()`` código executado antes de apagar de um obejto
* ``postDelete()`` código executado depois de apagar de um obejto

Comportamentos
---------

Todos comportamentos empacotados no Propel funcionam com Symfony2.
Para ter mais informações sobre como usá-los, veja 
`Referências de Comportamentos`_.

Comandos
--------

Você deve ler a seção dedicada para `Comandos Propel no Symfony2`_.

.. _`Trabalhando com Symfony2`: http://propelorm.org/Propel/cookbook/symfony2/working-with-symfony2.html#installation
.. _`Configuração do PropelBundle`: http://propelorm.org/Propel/cookbook/symfony2/working-with-symfony2.html#configuration
.. _`Relacionamentos`: http://propelorm.org/documentation/04-relationships.html
.. _`Referências de Comportamentos`: http://propelorm.org/documentation/#behaviors-reference
.. _`Comandos Propel no Symfony2`: http://propelorm.org/Propel/cookbook/symfony2/working-with-symfony2#the-commands
