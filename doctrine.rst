.. index::
   single: Doctrine

Bancos de Dados e Doctrine
==========================

Temos que dizer, uma das tarefas mais comuns e desafiadoras em qualquer
aplicação envolve persistir e ler informações de um banco de dados. Felizmente
o Symfony vem integrado com o `Doctrine`_, uma biblioteca cujo único objetivo é
fornecer poderosas ferramentas que tornem isso fácil. Nesse capítulo você 
aprenderá a filosofia básica por trás do Doctrine e verá quão fácil pode ser
trabalhar com um banco de dados.

.. note::

    O Doctrine é totalmente desacoplado do Symfony, e seu uso é opcional. Esse
    capítulo é totalmente sobre o Doctrine ORM, que visa permitir fazer
    mapeamento de objetos para um banco de dados relacional (como o *MySQL*,
    *PostgreSQL* ou o *Microsoft SQL*). É fácil usar consultas SQL puras se 
    você preferir, isso é explicado na entrada do cookbook
    ":doc:`/cookbook/doctrine/dbal`".
    
    Você também pode persistir dados no `MongoDB`_ usando a biblioteca Doctrine
    ODM. Para mais informações, leia a documentação
    ":doc:`/bundles/DoctrineMongoDBBundle/index`".

Um Exemplo Simples: Um Produto
------------------------------

O jeito mais fácil de entender como o Doctrine trabalha é vendo-o em ação.
Nessa seção, você configurará seu banco de dados, criará um objeto ``Product``,
fará sua persistência no banco e depois irá retorná-lo.

.. sidebar:: Codifique seguindo o exemplo

    Se quiser seguir o exemplo deste capítulo, crie um ``AcmeStoreBundle`` via:
    
    .. code-block:: bash
    
        php app/console generate:bundle --namespace=Acme/StoreBundle

Configurando o Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Antes de começar realmente, você precisa configurar a informação de conexão do
seu banco. Por convenção, essa informação geralmente é configurada no arquivo
``app/config/parameters.yml``:

.. code-block:: yaml

    # app/config/parameters.yml
    parameters:
        database_driver:   pdo_mysql
        database_host:     localhost
        database_name:     test_project
        database_user:     root
        database_password: password

.. note::
    
    Definir a configuração pelo ``parameters.yml`` é apenas uma convenção. Os
    parâmetros definidos naquele arquivo são referenciados pelo arquivo de
    configuração principal na configuração do Doctrine:
    
    .. code-block:: yaml
    
        doctrine:
            dbal:
                driver:   %database_driver%
                host:     %database_host%
                dbname:   %database_name%
                user:     %database_user%
                password: %database_password%
    
    Colocando a informação do banco de dados em um arquivo separado, você pode
    manter de forma fácil diferentes versões em cada um dos servidores. Você
    pode também guardar facilmente a configuração de banco (ou qualquer outra
    informação delicada) fora do seu projeto, por exemplo dentro do seu
    diretório de configuração do Apache. Para mais informações, de uma olhada
    em :doc:`/cookbook/configuration/external_parameters`.

Agora que o Doctrine sabe sobre seu banco, pode deixar que ele faça a criação
dele para você:

.. code-block:: bash

    php app/console doctrine:database:create

Criando uma Classe Entidade
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Suponha que você esteja criando uma aplicação onde os produtos precisam ser
mostrados. Antes mesmo de pensar sobre o Doctrine ou banco de dados, você já
sabe que irá precisar de um objeto ``Product`` para representar esses produtos.
Crie essa classe dentro do diretório ``Entity`` no seu bundle
``AcmeStoreBundle``::

    // src/Acme/StoreBundle/Entity/Product.php    
    namespace Acme\StoreBundle\Entity;

    class Product
    {
        protected $name;

        protected $price;

        protected $description;
    }

A classe - frequentemente chamada de "entidade", que significa *uma classe básica
para guardar dados* - é simples e ajuda a cumprir o requisito de negócio
referente aos produtos na sua aplicação. Essa classe ainda não pode ser
persistida no banco de dados - ela é apenas uma classe PHP simples.

.. tip::

    Depois que você aprender os conceitos por trás do Doctrine, você pode
    deixá-lo criar essa classe entidade para você:
    
    .. code-block:: bash
        
        php app/console doctrine:generate:entity --entity="AcmeStoreBundle:Product" --fields="name:string(255) price:float description:text"

.. index::
    single: Doctrine; Adding mapping metadata

.. _book-doctrine-adding-mapping:

Adicionando Informações de Mapeamento
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O Doctrine permite que você trabalhe de uma forma muito mais interessante com
banco de dados do que apenas buscar registros de uma tabela baseada em colunas
para um array. Em vez disso, o Doctrine permite que você persista *objetos*
inteiros no banco e recupere objetos inteiros do banco de dados. Isso funciona
mapeando uma classe PHP com uma tabela do banco, e as propriedades dessa classe
com as colunas da tabela:

.. image:: /images/book/doctrine_image_1.png
   :align: center
   
Para o Doctrine ser capaz disso, você tem apenas que criar "metadados", em
outras palavras a configuração que diz ao Doctrine exatamente como a classe
``Product`` e suas propriedades devem ser *mapeadas* com o banco de dados.
Esses metadados podem ser especificados em vários diferentes formatos incluindo
YAML, XML ou diretamente dentro da classe ``Product`` por meio de annotations:

.. note::

    Um bundle só pode aceitar um formato para definição de metadados. Por
    exemplo, não é possível misturar definições em YAML com definições
    por annotations nas classes entidade.

.. configuration-block::

    .. code-block:: php-annotations

        // src/Acme/StoreBundle/Entity/Product.php
        namespace Acme\StoreBundle\Entity;

        use Doctrine\ORM\Mapping as ORM;

        /**
         * @ORM\Entity
         * @ORM\Table(name="product")
         */
        class Product
        {
            /**
             * @ORM\Id
             * @ORM\Column(type="integer")
             * @ORM\GeneratedValue(strategy="AUTO")
             */
            protected $id;

            /**
             * @ORM\Column(type="string", length=100)
             */
            protected $name;

            /**
             * @ORM\Column(type="decimal", scale=2)
             */
            protected $price;

            /**
             * @ORM\Column(type="text")
             */
            protected $description;
        }

    .. code-block:: yaml

        # src/Acme/StoreBundle/Resources/config/doctrine/Product.orm.yml
        Acme\StoreBundle\Entity\Product:
            type: entity
            table: product
            id:
                id:
                    type: integer
                    generator: { strategy: AUTO }
            fields:
                name:
                    type: string
                    length: 100
                price:
                    type: decimal
                    scale: 2
                description:
                    type: text

    .. code-block:: xml

        <!-- src/Acme/StoreBundle/Resources/config/doctrine/Product.orm.xml -->
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
              xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                            http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="Acme\StoreBundle\Entity\Product" table="product">
                <id name="id" type="integer" column="id">
                    <generator strategy="AUTO" />
                </id>
                <field name="name" column="name" type="string" length="100" />
                <field name="price" column="price" type="decimal" scale="2" />
                <field name="description" column="description" type="text" />
            </entity>
        </doctrine-mapping>

.. tip::

    O nome da tabela é opcional e, se omitido, será determinado automaticamente
    baseado no nome da classe entidade.
    
O Doctrine permite que você escolha entre uma grande variedade de diferentes
tipos de campo, cada um com suas opções específicas. Para informações sobre os
tipos de campos disponíveis, dê uma olhada na seção
:ref:`book-doctrine-field-types`.

.. seealso::

    Você também pode conferir a `Documentação Básica sobre Mapeamento do
    Doctrine`_ para todos os detalhes sobre o tema. Se você usar annotations,
    irá precisar prefixar todas elas com ``ORM\`` (i.e. ``ORM\Column(..)``),
    o que não é citado na documentação do Doctrine. Você também irá precisar
    incluir o comando ``use Doctrine\ORM\Mapping as ORM;``, que *importa* o
    prefixo ``ORM`` das annotations.

.. caution::

    Tenha cuidado para que o nome da sua classe e suas propriedades não estão
    mapeadas com o nome de um comando SQL protegido (como ``group``ou
    ``user``). Por exemplo, se o nome da sua classe entidade é ``Group`` então,
    por padrão, o nome da sua tabela será ``group``, que causará um erro de
    SQL em alguns dos bancos de dados. Dê uma olhada na `Documentação sobre
    os nomes de comandos SQL reservados`_ para ver como escapar adequadamente
    esses nomes. Alternativamente, se você pode escolher livremente seu 
    esquema de banco de dados, simplesmente mapeie para um nome de tabela 
    ou nome de coluna diferente. Veja a documentação do Doctrine sobre
    `Classes persistentes`_ e `Mapeamento de propriedades`_
    
.. note::

    Quando usar outra biblioteca ou programa (i.e. Doxygen) que usa annotations
    você dever colocar a annotation ``@IgnoreAnnotation`` na classe para indicar
    que annotations o Symfony deve ignorar.
    
    Por exemplo, para prevenir que a annotation ``@fn`` gere uma exceção, inclua
    o seguinte:

        /**
         * @IgnoreAnnotation("fn")
         */
        class Product

Gerando os Getters e Setters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Apesar do Doctrine agora saber como persistir um objeto ``Product`` num banco
de dados, a classe ainda não é realmente útil. Como ``Product`` é apenas uma
classe PHP usual, você precisa criar os métodos getters e setters (i.e.
``getName()``, ``setName()`` para acessar sua suas propriedades (até as
propriedades ``protected``). Felizmente o Doctrine pode fazer isso por você
executando:

.. code-block:: bash

    php app/console doctrine:generate:entities Acme/StoreBundle/Entity/Product
    
Esse comando garante que todos os getters e setters estão criados na classe
``Product``. Ele é um comando seguro - você pode executá-lo muitas e muitas
vezes: ele apenas gera getters e setters que ainda não existem (i.e. ele não
altera os models já existentes).

.. caution::

    O comando ``doctrine:generate:entities`` gera um backup do ``Product.php``
    original chamado de ``Product.php~`. Em alguns casos, a presença desse
    arquivo pode causar um erro "Cannot redeclare class`.  É seguro removê-lo.

Você pode gerar todas as entidades que são conhecidas por um bundle (i.e. cada
classe PHP com a informação de mapeamento do Doctrine) ou de um namespace
inteiro.

.. code-block:: bash

    php app/console doctrine:generate:entities AcmeStoreBundle
    php app/console doctrine:generate:entities Acme

.. note::

    O Doctrine não se importa se as suas propriedades são ``protected`` ou
    ``private``, ou se você não tem um método getter ou setter. Os getters e
    setters são gerados aqui apenas porque você irá precisar deles para
    interagir com o seu objeto PHP.
    

Criando as Tabelas/Esquema do Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora você tem uma classe utilizável ``Product`` com informação de mapeamento
assim o Doctrine sabe exatamente como fazer a persistência dela. É claro, você
ainda não tem a tabela correspondente ``product`` no seu banco de dados.
Felizmente, o Doctrine pode criar automaticamente todas as tabelas necessárias
no banco para cada uma das entidades conhecidas da sua aplicação. Para isso,
execute:

.. code-block:: bash

    php app/console doctrine:schema:update --force

.. tip::

    Na verdade, esse comando é extremamente poderoso. Ele compara o que o banco
    de dados *deveria* se parecer (baseado na informação de mapeamento das suas
    entidades) com o que ele *realmente* se parece, e gera os comandos SQL
    necessários para *atualizar* o banco para o que ele deveria ser. Em outras
    palavras, se você adicionar uma nova propriedade com metadados de
    mapeamento na classe ``Product``e executar esse comando novamente, ele irá
    criar a instrução ''alter table'' para adicionar as novas colunas na tabela
    ``product`` existente.
    
    Uma maneira ainda melhor de se aproveitar dessa funcionalidade é por meio
    das :doc:`migrations</bundles/DoctrineMigrationsBundle/index>`, que lhe
    permitem criar essas instruções SQL e guardá-las em classes migration que
    podem ser rodadas de forma sistemática no seu servidor de produção para que
    se possa acompanhar e migrar o schema do seu banco de dados de uma forma
    mais segura e confiável.

Seu banco de dados agora tem uma tabela ``product`` totalmente funcional com
as colunas correspondendo com os metadados que foram especificados.

Persistindo Objetos no Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora que você tem uma entidade ``Product`` mapeada e a tabela correspondente
``product``, já está pronto para persistir os dados no banco. De dentro de um
controller, isso é bem simples. Inclua o seguinte método no
``DefaultController`` do bundle:

.. code-block:: php
    :linenos:

    // src/Acme/StoreBundle/Controller/DefaultController.php
    use Acme\StoreBundle\Entity\Product;
    use Symfony\Component\HttpFoundation\Response;
    // ...
    
    public function createAction()
    {
        $product = new Product();
        $product->setName('A Foo Bar');
        $product->setPrice('19.99');
        $product->setDescription('Lorem ipsum dolor');

        $em = $this->getDoctrine()->getManager();
        $em->persist($product);
        $em->flush();

        return new Response('Created product id '.$product->getId());
    }

.. note::

    Se você estiver seguindo o exemplo na prática, precisará criar a rota que
    aponta para essa action se quiser vê-la funcionando.

Vamos caminhar pelo exemplo:

* **linhas 8-11** Nessa parte você instancia o objeto ``$product`` como
  qualquer outro objeto PHP normal;

* **linha 13** Essa linha recuperar o objeto *entity manager* do Doctrine, que
  é o responsável por lidar com o processo de persistir e retornar objetos do
  e para o banco de dados;

* **linha 14** O método ``persist()`` diz ao Doctrine para ''gerenciar'' o
  objeto ``$product``. Isso não gera (ainda) um comando real no banco de dados.

* **linha 15** Quando o método ``flush()`` é chamado, o Doctrine verifica em
  todos os objetos que ele gerencia para ver se eles necessitam ser persistidos
  no banco. Nesse exemplo, o objeto ``$product`` ainda não foi persistido, por
  isso o entity manager executa um comando ``INSERT`` e um registro é criado
  na tabela ``product``.

.. note::

  Na verdade, como o Doctrine conhece todas as entidades gerenciadas,
  quando você chama o método ``flush()``, ele calcula um changeset geral e
  executa o comando ou os comandos mais eficientes possíveis. Por exemplo,
  se você vai persistir um total de 100 objetos ``Product`` e em seguida
  chamar o método ``flush()``, o Doctrine irá criar um *único* prepared statment
  e reutilizá-lo para cada uma das inserções. Esse padrão é chamado de *Unit of
  Work*, e é utilizado porque é rápido e eficiente.
  
Na hora de criar ou atualizar objetos, o fluxo de trabalho é quase o mesmo. Na
próxima seção, você verá como o Doctrine é inteligente o suficiente para rodar
uma instrução ``UPDATE`` de forma automática se o registro já existir no banco.

.. tip::
    
    O Doctrine fornece uma biblioteca que permite a você carregar
    programaticamente dados de teste no seu projeto (i.e. "fixture data"). Para
    mais informações, veja :doc:`/bundles/DoctrineFixturesBundle/index`.

Trazendo Objetos do Banco de Dados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Trazer um objeto a partir do banco é ainda mais fácil. Por exemplo, suponha
que você tenha configurado uma rota para mostrar um ``Product`` específico
baseado no seu valor ``id``::

    public function showAction($id)
    {
        $product = $this->getDoctrine()
            ->getRepository('AcmeStoreBundle:Product')
            ->find($id);
        
        if (!$product) {
            throw $this->createNotFoundException('No product found for id '.$id);
        }

        // faz algo, como passar o objeto $product para um template
    }

Quando você busca um tipo de objeto em particular, você sempre usa o que
chamamos de "repositório". Você pode pensar num repositório como uma classe
PHP cuja única função é auxiliar a trazer entidades de uma determinada classe.
Você pode acessar o objeto repositório por uma classe entidade dessa forma::

    $repository = $this->getDoctrine()
        ->getRepository('AcmeStoreBundle:Product');

.. note::

    A string ``AcmeStoreBundle:Product`` é um atalho que você pode usar
    em qualquer lugar no Doctrine em vez do nome completo da classe entidade
    (i.e ``Acme\StoreBundle\Entity\Product``). Desde que sua entidade esteja
    sob o namespace ``Entity`` do seu bundle, isso vai funcionar.

Uma vez que você tiver seu repositório, terá acesso a todos os tipos de métodos
úteis::

    // Busca pela chave primária (geralmente "id")
    $product = $repository->find($id);

    // nomes de métodos dinâmicos para busca baseados no valor de uma coluna
    $product = $repository->findOneById($id);
    $product = $repository->findOneByName('foo');

    // busca *todos* os produtos
    $products = $repository->findAll();

    // busca um grupo de produtos baseada numa valor arbitrário de coluna
    $products = $repository->findByPrice(19.99);

.. note::

    Naturalmente, você pode também pode rodar consultas complexas, vamos
    aprender mais sobre isso na seção :ref:`book-doctrine-queries`.

Você também pode se aproveitar dos métodos bem úteis ``findBy`` e
``findOneBy`` para retornar facilmente objetos baseando-se em múltiplas
condições::

    // busca por um produto que corresponda a um nome e um preço
    $product = $repository->findOneBy(array('name' => 'foo', 'price' => 19.99));

    // busca por todos os produtos correspondentes a um nome, ordenados por
    // preço
    $product = $repository->findBy(
        array('name' => 'foo'),
        array('price' => 'ASC')
    );

.. tip::

    Quando você renderiza uma página, você pode ver quantas buscas foram feitas
    no canto inferior direito da web debug toolbar.

    .. image:: /images/book/doctrine_web_debug_toolbar.png
       :align: center
       :scale: 50
       :width: 350

    Se você clicar no ícone, irá abrir o profiler, mostrando a você as
    consultas exatas que foram feitas.

Atualizando um Objeto
~~~~~~~~~~~~~~~~~~~~~

Depois que você trouxe um objeto do Doctrine, a atualização é fácil. Suponha
que você tenha uma rota que mapeia o id de um produto para uma action de
atualização em um controller::

    public function updateAction($id)
    {
        $em = $this->getDoctrine()->getManager();
        $product = $em->getRepository('AcmeStoreBundle:Product')->find($id);

        if (!$product) {
            throw $this->createNotFoundException('No product found for id '.$id);
        }

        $product->setName('New product name!');
        $em->flush();

        return $this->redirect($this->generateUrl('homepage'));
    }

Atualizar um objeto envolve apenas três passos:

1. retornar um objeto do Doctrine;
2. modificar o objeto;
3. chamar ``flush()`` no entity manager

Observe que não é necessário chamar ``$em->persist($product)``. Chamar novamente
esse método apenas diz ao Doctrine para gerenciar ou "ficar de olho" no objeto
``$product``. Nesse caso, como o objeto ``$product`` foi trazido do Doctrine,
ele já está sendo gerenciado.

Excluindo um Objeto
~~~~~~~~~~~~~~~~~~~

Apagar um objeto é muito semelhante, mas requer um chamada ao método
``remove()`` do entity manager::

    $em->remove($product);
    $em->flush();

Como você podia esperar, o método ``remove()`` notifica o Doctrine que você
quer remover uma determinada entidade do banco. A consulta real ``DELETE``, no
entanto, não é executada de verdade até que o método ``flush()`` seja chamado.

.. _`book-doctrine-queries`:

Consultando Objetos
-------------------

Você já viu como o repositório objeto permite que você execute consultas
básicas sem nenhum esforço::

    $repository->find($id);
    
    $repository->findOneByName('Foo');

É claro, o Doctrine também permite que se escreva consulta mais complexas
usando o Doctrine Query Language (DQL). O DQL é similar ao SQL exceto que você
deve imaginar que você está consultando um ou mais objetos de uma classe entidade
(i.e. ``Product``) em vez de consultar linhas em uma tabela (i.e. ``product``).

Quando estiver consultando no Doctrine, você tem duas opções: escrever
consultas Doctrine puras ou usar o Doctrine's Query Builder.

Consultando Objetos com DQL
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Imagine que você queira buscar por produtos, mas retornar apenas produtos que
custem menos que ``19,99``, ordenados do mais barato para o mais caro. De um
controller, faça o seguinte::

    $em = $this->getDoctrine()->getManager();
    $query = $em->createQuery(
        'SELECT p FROM AcmeStoreBundle:Product p WHERE p.price > :price ORDER BY p.price ASC'
    )->setParameter('price', '19.99');
    
    $products = $query->getResult();

Se você se sentir confortável com o SQL, então o DQL deve ser bem natural. A
grande diferença é que você precisa pensar em termos de "objetos" em vez de
linhas no banco de dados. Por esse motivo, você faz um "select" *from*
``AcmeStoreBundle:Product`` e dá para ele o alias ``p``.

O método ``getResult()`` retorna um array de resultados. Se você estiver
buscando por apenas um objeto, você pode usar em vez disso o método
``getSingleResult()``::

    $product = $query->getSingleResult();

.. caution::

    O método ``getSingleResult()`` gera uma exceção
    ``Doctrine\ORM\NoResultException`` se nenhum resultado for retornado e uma
    ``Doctrine\ORM\NonUniqueResultException`` se *mais* de um resultado for
    retornado. Se você usar esse método, você vai precisar envolvê-lo em um
    bloco try-catch e garantir que apenas um resultado é retornado (se estiver
    buscando algo que possa de alguma forma retornar mais de um resultado)::
    
        $query = $em->createQuery('SELECT ....')
            ->setMaxResults(1);
        
        try {
            $product = $query->getSingleResult();
        } catch (\Doctrine\Orm\NoResultException $e) {
            $product = null;
        }
        // ...

A sintaxe DQL é incrivelmente poderosa, permitindo que você faça junções
entre entidades facilmente (o tópico de 
:ref:`relacionamentos<book-doctrine-relations>` será coberto posteriormente),
grupos etc. Para mais informações, veja a documentação oficial do
`Doctrine Query Language`_.

.. sidebar:: Configurando parâmetros

    Tome nota do método ``setParameter()``. Quando trabalhar com o Doctrine,
    é sempre uma boa ideia configurar os valores externos como
    ``placeholders``, o que foi feito na consulta acima:
    
    .. code-block:: text

        ... WHERE p.price > :price ...

    Você pode definir o valor do placeholder ``price``chamando o método
    ``setParameter()``::

        ->setParameter('price', '19.99')

    Usar parâmetros em vez de colocar os valores diretamente no texto da
    consulta é feito para prevenir ataques de SQL injection e deve ser feito
    *sempre*. Se você estiver usando múltiplos parâmetros, você pode definir seus
    valores de uma vez só usando o método ``setParameters()``::

        ->setParameters(array(
            'price' => '19.99',
            'name'  => 'Foo',
        ))

Usando o Doctrine's Query Builder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Em vez de escrever diretamente suas consultas, você pode alternativamente usar
o ``QueryBuilder`` do Doctrine para fazer o mesmo serviço usando uma bela
interface orientada a objetos. Se você utilizar uma IDE, pode também se
beneficiar do auto-complete à medida que você digita o nome dos métodos. A
partir de um controller::

    $repository = $this->getDoctrine()
        ->getRepository('AcmeStoreBundle:Product');

    $query = $repository->createQueryBuilder('p')
        ->where('p.price > :price')
        ->setParameter('price', '19.99')
        ->orderBy('p.price', 'ASC')
        ->getQuery();
    
    $products = $query->getResult();

O objeto ``QueryBuilder`` contém todos os métodos necessários para criar sua
consulta. Ao chamar o método ``getQuery(), o query builder retorna um objeto
``Query`` normal, que é o mesmo objeto que você criou diretamente na seção
anterior.

Para mais informações, consulte a documentação do `Query Builder`_ do Doctrine.

Classes Repositório Personalizadas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Nas seções anteriores, você começou a construir e usar consultas mais complexas
de dentro de um controller. De modo a isolar, testar e reutilizar essas
consultas, é uma boa ideia criar uma classe repositório personalizada para sua
entidade e adicionar métodos com sua lógica de consultas lá dentro.

Para fazer isso, adicione o nome da classe repositório na sua definição de
mapeamento.

.. configuration-block::

    .. code-block:: php-annotations

        // src/Acme/StoreBundle/Entity/Product.php
        namespace Acme\StoreBundle\Entity;

        use Doctrine\ORM\Mapping as ORM;

        /**
         * @ORM\Entity(repositoryClass="Acme\StoreBundle\Repository\ProductRepository")
         */
        class Product
        {
            //...
        }

    .. code-block:: yaml

        # src/Acme/StoreBundle/Resources/config/doctrine/Product.orm.yml
        Acme\StoreBundle\Entity\Product:
            type: entity
            repositoryClass: Acme\StoreBundle\Repository\ProductRepository
            # ...

    .. code-block:: xml

        <!-- src/Acme/StoreBundle/Resources/config/doctrine/Product.orm.xml -->
        <!-- ... -->
        <doctrine-mapping>

            <entity name="Acme\StoreBundle\Entity\Product"
                    repository-class="Acme\StoreBundle\Repository\ProductRepository">
                    <!-- ... -->
            </entity>
        </doctrine-mapping>

O Doctrine pode gerar para você a classe repositório usando o mesmo comando
utilizado anteriormente para criar os métodos getters e setters que estavam
faltando:

.. code-block:: bash

    php app/console doctrine:generate:entities Acme

Em seguida, adicione um novo método - ``findAllOrderedByName()`` - para sua
recém-gerada classe repositório. Esse método irá buscar por todas as
entidades ``Product``, ordenadas alfabeticamente.

.. code-block:: php

    // src/Acme/StoreBundle/Repository/ProductRepository.php
    namespace Acme\StoreBundle\Repository;

    use Doctrine\ORM\EntityRepository;

    class ProductRepository extends EntityRepository
    {
        public function findAllOrderedByName()
        {
            return $this->getEntityManager()
                ->createQuery('SELECT p FROM AcmeStoreBundle:Product p ORDER BY p.name ASC')
                ->getResult();
        }
    }

.. tip::

    O entity manager pode ser acessado via ``$this->getEntityManager()`` de 
    dentro do repositório.

Você pode usar esse novo método da mesma forma que os métodos padrões "find"
do repositório::

    $em = $this->getDoctrine()->getManager();
    $products = $em->getRepository('AcmeStoreBundle:Product')
                ->findAllOrderedByName();

.. note::
    
    Quando estiver usando uma classe repositório personalizada, você continua
    tendo acesso aos métodos padrões finder com ``find()`` e ``findAll()``.

.. _`book-doctrine-relations`:

Relacionamentos/Associações de Entidades
----------------------------------------

Suponha que todos os produtos na sua aplicação pertençam exatamente a uma
"categoria". Nesse caso, você precisa de um objeto ``Category`` e de uma forma
de relacionar um objeto ``Produto`` com um objeto ``Category``. Comece criando
uma entidade ``Category``. Como você sabe que irá eventualmente precisar de fazer
a persistência da classe através do Doctrine, você pode deixá-lo criar a classe por
você.

.. code-block:: bash

    php app/console doctrine:generate:entity --entity="AcmeStoreBundle:Category" --fields="name:string(255)"
    
Esse comando gera a entidade ``Category`` para você, com um campo ``id``, um
campo ``name`` e as funções getters e setters relacionadas.

Metadado para Mapeamento de Relacionamentos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Para relacionar as entidades ``Category`` e ``Product``, comece criando a
propriedade ``products`` na classe ``Category``::

    // src/Acme/StoreBundle/Entity/Category.php
    // ...
    use Doctrine\Common\Collections\ArrayCollection;
    
    class Category
    {
        // ...
        
        /**
         * @ORM\OneToMany(targetEntity="Product", mappedBy="category")
         */
        protected $products;

        public function __construct()
        {
            $this->products = new ArrayCollection();
        }
    }

Primeiro, como o objeto ``Category`` irá se relacionar a vários objetos
``Product`, uma propriedade array ``products`` é adicionada para guardar esses
objetos ``Product``. Novamente, isso não é feito porque o Doctrine precisa
dele, mas na verdade porque faz sentido dentro da aplicação guardar um array de
objetos ``Product``.

.. note::

    O código no método ``__construct()`` é importante porque o Doctrine requer
    que a propriedade ``$products``seja um objeto ``ArrayCollection``. Esse
    objeto se parece e age quase *exatamente* como um array, mas tem mais um
    pouco de flexibilidade embutida. Se isso te deixa desconfortável, não se
    preocupe. Apenas imagine que ele é um ``array`` e você estará em boas mãos.

Em seguida, como cada classe ``Product`` pode se relacionar exatamente com um
objeto ``Category``, você irá querer adicionar uma propriedade ``$category`` na
classe ``Product``::

    // src/Acme/StoreBundle/Entity/Product.php
    // ...

    class Product
    {
        // ...
    
        /**
         * @ORM\ManyToOne(targetEntity="Category", inversedBy="products")
         * @ORM\JoinColumn(name="category_id", referencedColumnName="id")
         */
        protected $category;
    }

Finalmente, agora que você adicionou um nova propriedade tanto na classe
``Category`` quanto na ``Product``, diga ao Doctrine para gerar os métodos
getters e setters que estão faltando para você:

.. code-block:: bash

    php app/console doctrine:generate:entities Acme

Ignore o metadado do Doctrine por um instante. Agora você tem duas classes -
``Category`` e ``Product`` com um relacionamento natural um-para-muitos. A
classe categoria contém um array de objetos ``Product`` e o objeto ``Product``
pode conter um objeto ``Category``. Em outras palavras - você construiu suas
classes de um jeito que faz sentido para as suas necessidades. O fato de que
os dados precisam ser persistidos no banco é sempre secundário.

Agora, olhe o metadado acima da propriedade ``$category`` na classe
``Product``. A informação aqui diz para o Doctrine que a classe relacionada é a
``Category`` e que ela deve guardar o ``id`` do registro categoria em um campo
``category_id`` que fica na tabela ``product``. Em outras palavras, o objeto
``Category`` será guardado na propriedade ``$category``, mas nos bastidores, o
Doctrine irá persistir esse relacionamento guardando o valor do id da categoria
na coluna ``category_id`` da tabela ``product``.

.. image:: /images/book/doctrine_image_2.png
   :align: center

O metadado acima da propriedade ``$products`` do objeto ``Category`` é menos
importante, e simplesmente diz ao Doctrine para olhar a propriedade
``Product.category`` para descobrir como o relacionamento é mapeado.

Antes de continuar, tenha certeza de dizer ao Doctrine para adicionar uma nova
tabela ``category``, além de uma coluna ``product.category_id`` e uma nova
chave estrangeira:

.. code-block:: bash

    php app/console doctrine:schema:update --force

.. note::

    Esse comando deve ser usado apenas durante o desenvolvimento. Para um
    método mais robusto de atualização sistemática em um banco de dados de
    produção, leia sobre as
    :doc:`Doctrine migrations</bundles/DoctrineMigrationsBundle/index>`.

Salvando as Entidades Relacionadas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora é o momento de ver o código em ação. Imagine que você está dentro de um
controller::

    // ...
    use Acme\StoreBundle\Entity\Category;
    use Acme\StoreBundle\Entity\Product;
    use Symfony\Component\HttpFoundation\Response;
    // ...

    class DefaultController extends Controller
    {
        public function createProductAction()
        {
            $category = new Category();
            $category->setName('Main Products');
            
            $product = new Product();
            $product->setName('Foo');
            $product->setPrice(19.99);
            // relaciona a categoria com esse produto
            $product->setCategory($category);
            
            $em = $this->getDoctrine()->getManager();
            $em->persist($category);
            $em->persist($product);
            $em->flush();
            
            return new Response(
                'Created product id: '.$product->getId().' and category id: '.$category->getId()
            );
        }
    }

Agora, um registro único é adicionado para ambas tabelas ``category`` e
``product``. A coluna ``product.category_id`` para o novo produto é definida
como o que for definido como ``id`` na nova categoria. O Doctrine gerencia a
persistência desse relacionamento para você.

Retornando Objetos Relacionados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando você precisa pegar objetos associados, seu fluxo de trabalho é parecido
com o que foi feito anteriormente. Primeiro, consulte um objeto ``$product`` e
então acesse seu o objeto ``Category`` relacionado::

    public function showAction($id)
    {
        $product = $this->getDoctrine()
            ->getRepository('AcmeStoreBundle:Product')
            ->find($id);

        $categoryName = $product->getCategory()->getName();
        
        // ...
    }

Nesse exemplo, você primeiro busca por um objeto ``Product`` baseado no ``id``
do produto. Isso gera uma consulta *apenas* para os dados do produto e faz um
hydrate do objeto ``$product`` com esses dados. Em seguida, quando você chamar
``$product->getCategory()->getName()``, o Doctrine silenciosamente faz uma
segunda consulta para buscar a ``Category`` que está relacionada com esse
``Product``.  Ele prepara o objeto ``$category`` e o retorna para você.

.. image:: /images/book/doctrine_image_3.png
   :align: center

O que é importante é o fato de que você tem acesso fácil as categorias
relacionadas com os produtos, mas os dados da categoria não são realmente
retornados até que você peça pela categoria (i.e. sofre "lazy load").

Você também pode buscar na outra direção::

    public function showProductAction($id)
    {
        $category = $this->getDoctrine()
            ->getRepository('AcmeStoreBundle:Category')
            ->find($id);

        $products = $category->getProducts();
    
        // ...
    }

Nesse caso, ocorre a mesma coisa: primeiro você busca por um único objeto
``Category``, e então o Doctrine faz uma segunda busca para retornar os objetos
``Product`` relacionados, mas apenas se você pedir por eles (i.e. quando você
chama ``->getProducts()``). A variável ``$products`` é uma array de todos os
objetos ``Product`` que estão relacionados com um dado objeto ``Category`` por
meio do valor de seu campo ``category_id``.

.. sidebar:: Relacionamentos e Classes Proxy

    O "lazy loading" é possível porque, quando necessário, o Doctrine retorna
    um objeto "proxy" no lugar do objeto real. Olhe novamente o exemplo acima::
    
        $product = $this->getDoctrine()
            ->getRepository('AcmeStoreBundle:Product')
            ->find($id);

        $category = $product->getCategory();

        // prints "Proxies\AcmeStoreBundleEntityCategoryProxy"
        echo get_class($category);

    Esse objeto proxy estende o verdadeiro objeto ``Category``, e se parece e
    age exatamente como ele. A diferença é que, por usar um objeto proxy,
    o Doctrine pode retardar a busca pelos dados reais da ``Category``até que
    você realmente precise daqueles dados (e.g. até que você chame
    ``$category->getName()``).
    
    As classes proxy são criadas pelo Doctrine e armazenadas no diretório
    cache. E apesar de que você provavelmente nunca irá notar que o seu objeto
    ``$category`` é na verdade um objeto proxy, é importante manter isso em
    mente.

    Na próxima seção, quando você retorna os dados do produto e categoria todos
    de uma vez (via um *join*), o Doctrine irá retornar o *verdadeiro* objeto
    ``Category`, uma vez que nada precisa ser carregado de modo "lazy load".


Juntando Registros Relacionados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Nos exemplos acima, duas consultas foram feitas - uma para o objeto original
(e.g uma ``Category``) e uma para os objetos relacionados (e.g. os objetos
``Product``).

.. tip::

    Lembre que você pode visualizar todas as consultas feitas durante uma
    requisição pela web debug toolbar.

É claro, se você souber antecipadamente que vai precisar acessar ambos os
objetos, você pode evitar a segunda consulta através da emissão de um "join"
na consulta original. Inclua o método seguinte na classe
``ProductRepository``::

    // src/Acme/StoreBundle/Repository/ProductRepository.php
    
    public function findOneByIdJoinedToCategory($id)
    {
        $query = $this->getEntityManager()
            ->createQuery('
                SELECT p, c FROM AcmeStoreBundle:Product p
                JOIN p.category c
                WHERE p.id = :id'
            )->setParameter('id', $id);
        
        try {
            return $query->getSingleResult();
        } catch (\Doctrine\ORM\NoResultException $e) {
            return null;
        }
    }

Agora, você pode usar esse método no seu controller para buscar um objeto
``Product`` e sua ``Category`` relacionada com apenas um consulta::

    public function showAction($id)
    {
        $product = $this->getDoctrine()
            ->getRepository('AcmeStoreBundle:Product')
            ->findOneByIdJoinedToCategory($id);

        $category = $product->getCategory();
    
        // ...
    }    


Mais Informações sobre Associações
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Essa seção foi uma introdução para um tipo comum de relacionamento de
entidades, o um-para-muitos. Para detalhes mais avançados e exemplos de como
usar outros tipos de relacionamentos (i.e. ``um-para-um,
``muitos-para-muitos``), verifique a `Documentação sobre Mapeamento e Associações`_ do
Doctrine.

.. note::

    Se você estiver usando annotations, irá precisar prefixar todas elas com
    ``ORM\`` (e.g ``ORM\OneToMany``), o que não está descrito na documentação
    do Doctrine. Você também precisará incluir a instrução
    ``use Doctrine\ORM\Mapping as ORM;``, que faz a *importação* do prefixo
    ``ORM`` das annotations.

Configuração
------------

O Doctrine é altamente configurável, embora você provavelmente não vai precisar
se preocupar com a maioria de suas opções. Para saber mais sobre a configuração
do Doctrine, veja a seção Doctrine do
:doc:`reference manual</reference/configuration/doctrine>`.

Lifecycle Callbacks
-------------------

Às vezes, você precisa executar uma ação justamente antes ou depois de uma entidade
ser inserida, atualizada ou apagada. Esses tipos de ações são conhecidas como
"lifecycle" callbacks, pois elas são métodos callbacks que você precisa
executar durante diferentes estágios do ciclo de vida de uma entidade (i.e. a
entidade foi inserida, atualizada, apagada, etc.).

Se você estiver usando annotations para seus metadados, comece habilitando esses
callbacks. Isso não é necessário se estiver utilizando YAML ou XML para seus
mapeamentos:

.. code-block:: php-annotations

    /**
     * @ORM\Entity()
     * @ORM\HasLifecycleCallbacks()
     */
    class Product
    {
        // ...
    }

Agora, você pode dizer ao Doctrine para executar um método em cada um dos
eventos de ciclo de vida disponíveis. Por exemplo, suponha que você queira
definir uma coluna ``created`` do tipo data para a data atual, apenas quando for
a primeira persistência da entidade (i.e. inserção):

.. configuration-block::

    .. code-block:: php-annotations

        /**
         * @ORM\prePersist
         */
        public function setCreatedValue()
        {
            $this->created = new \DateTime();
        }

    .. code-block:: yaml

        # src/Acme/StoreBundle/Resources/config/doctrine/Product.orm.yml
        Acme\StoreBundle\Entity\Product:
            type: entity
            # ...
            lifecycleCallbacks:
                prePersist: [ setCreatedValue ]

    .. code-block:: xml

        <!-- src/Acme/StoreBundle/Resources/config/doctrine/Product.orm.xml -->
        <!-- ... -->
        <doctrine-mapping>

            <entity name="Acme\StoreBundle\Entity\Product">
                    <!-- ... -->
                    <lifecycle-callbacks>
                        <lifecycle-callback type="prePersist" method="setCreatedValue" />
                    </lifecycle-callbacks>
            </entity>
        </doctrine-mapping>

.. note::

    O exemplo acima presume que você tenha criado e mapeado uma propriedade
    ``created`` (que não foi mostrada aqui).
    
Agora, logo no momento anterior a entidade ser persistida pela primeira vez, o
Doctrine irá automaticamente chamar esse método e o campo ``created`` será
preenchido com a data atual.

Isso pode ser repetido para qualquer um dos outros eventos de ciclo de vida,
que incluem:

* ``preRemove``
* ``postRemove``
* ``prePersist``
* ``postPersist``
* ``preUpdate``
* ``postUpdate``
* ``postLoad``
* ``loadClassMetadata``

Para mais informações sobre o que esses eventos significam e sobre os lifecycle
callbacks em geral, veja a `Documentação sobre Lifecycle Events`_ do Doctrine.

.. sidebar:: Lifecycle Callbacks e Event Listeners

    Observe que o método ``setCreatedValue()`` não recebe nenhum argumento.
    Esse é o comportamento usual dos lifecycle callbacks e é intencional: eles
    devem ser métodos simples que estão preocupados com as transformações
    internas dos dados na entidade (e.g. preencher um campo created/updated ou
    gerar um valor slug).
    
    Se você precisar fazer algo mais pesado - como rotinas de log ou mandar um
    e-mail - você deve registrar uma classe externa como um event listener ou
    subscriber e dar para ele acesso aos recursos que precisar. Para mais
    informações, veja :doc:`/cookbook/doctrine/event_listeners_subscribers`.
    

Extensões do Doctrine: Timestampable, Sluggable, etc.
-----------------------------------------------------

O Doctrine é bastante flexível, e um grande número de extensões de terceiros
está disponível o que permirte que você execute facilmente tarefas repetitivas
e comuns nas suas entidades. Isso inclui coisas como *Sluggable*,
*Timestampable*, *Loggable*, *Translatable* e *Tree*.

Para mais informações sobre como encontrar e usar essas extensões, veja o
artigo no cookbook sobre
:doc:`using common Doctrine extensions</cookbook/doctrine/common_extensions>`.

.. _book-doctrine-field-types:

Referência dos Tipos de Campos do Doctrine
------------------------------------------

O Doctrine já vem com um grande número de tipos de campo disponível. Cada um
deles mapeia um tipo de dados do PHP para um tipo de coluna específico em
qualquer banco de dados que você estiver utilizando. Os seguintes tipos são
suportados no Doctrine:

* **Strings**

  * ``string`` (usado para strings curtas)
  * ``text`` (usado para strings longas)

* **Números**

  * ``integer``
  * ``smallint``
  * ``bigint``
  * ``decimal``
  * ``float``

* **Datas e Horários** (usa um objeto `DateTime`_ para esses campos no PHP)

  * ``date``
  * ``time``
  * ``datetime``

* **Outros Tipos**

  * ``boolean``
  * ``object`` (serializado e armazenado em um campo ``CLOB``)
  * ``array`` (serializado e guardado em um campo ``CLOB``)

Para mais informações, veja a `Documentação sobre Tipos de Mapeamento`_ do Doctrine.

Opções de Campo
~~~~~~~~~~~~~~~

Cada campo pode ter um conjunto de opções aplicado sobre ele. As opções
disponíveis incluem ``type`` (o padrão é ``string``), ``name``, ``lenght``,
``unique`` e ``nullable``.  Olhe alguns exemplos de annotations:

.. code-block:: php-annotations

    /**
     * Um campo string com tamanho 255 que não pode ser nulo
     * (segue os valores padrões para "type", "length" e *nullable* options)
     * 
     * @ORM\Column()
     */
    protected $name;

    /**
     * Um campo string com tamanho 150 persistido na coluna "email_adress"
     * e com um índice único
     *
     * @ORM\Column(name="email_address", unique="true", length="150")
     */
    protected $email;

.. note::

    Existem mais algumas opções que não estão listadas aqui. Para mais detalhes,
    veja a `Documentação sobre Mapeamento de Propriedades`_ do Doctrine.

.. index::
   single: Doctrine; ORM Console Commands
   single: CLI; Doctrine ORM

Comandos de Console
-------------------

A integração com o Doctrine2 ORM fornece vários comandos de console no
namespace ``doctrine``. Para ver a lista de comandos, você pode executar o
console sem nenhum argumento:

.. code-block:: bash

    php app/console

A lista dos comandos disponíveis será mostrada, muitos dos quais começam com o
prefixo ``doctrine``. Você pode encontrar mais informações sobre qualquer um
desses comandos (e qualquer comando do Symfony) rodando o comando ``help``.
Por exemplo, para pegar detalhes sobre o comando ``doctrine:database:create``,
execute:

.. code-block:: bash

    php app/console help doctrine:database:create

Alguns comandos interessantes e notáveis incluem:

* ``doctrine:ensure-production-settings`` - verifica se o ambiente atual está
  configurado de forma eficiente para produção. Deve ser sempre executado no
  ambiente ``prod``:
  
  .. code-block:: bash
  
    php app/console doctrine:ensure-production-settings --env=prod

* ``doctrine:mapping:import`` - permite ao Doctrine fazer introspecção de um
  banco de dados existente e criar a informação de mapeamento. Para mais
  informações veja :doc:`/cookbook/doctrine/reverse_engineering`.

* ``doctrine:mapping:info`` - diz para você todas as entidades que o Doctrine
  tem conhecimento e se existe ou não algum erro básico com o mapeamento.

* ``doctrine:query:dql`` and ``doctrine:query:sql`` - permite que você execute
  consultas DQL ou SQL diretamente na linha de comando.

.. note::

   Para poder carregar data fixtures para seu banco de dados, você precisa ter
   o bundle ``DoctrineFixturesBundle`` instalado. Para aprender como fazer
   isso, leia a entrada ":doc:`/bundles/DoctrineFixturesBundle/index`" da
   documentação.
   
Sumário
-------

Com o Doctrine, você pode se focar nos seus objetos e como eles podem ser úteis
na sua aplicação, deixando a preocupação com a persistência de banco de dados
em segundo plano. Isso porque o Doctrine permite que você use qualquer objeto
PHP para guardar seus dados e se baseia nos metadados de mapeamento para mapear
os dados de um objetos para um tabela específica no banco.

E apesar do Doctrine girar em torno de um conceito simples, ele é incrivelmente
poderoso, permitindo que você crie consultas complexas e faça subscrição em
eventos que permitem a você executar ações diferentes à medida que os objetos
vão passando pelo seu ciclo de vida de persistência.

Aprenda mais
------------

.. toctree::
    :maxdepth: 1
    :glob:

    doctrine/*

* `DoctrineFixturesBundle`_
* `DoctrineMongoDBBundle`_

.. _`Doctrine`: http://www.doctrine-project.org/
.. _`MongoDB`: https://www.mongodb.org/
.. _`Documentação Básica sobre Mapeamento do Doctrine`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html
.. _`Query Builder`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/query-builder.html
.. _`Doctrine Query Language`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/dql-doctrine-query-language.html
.. _`Documentação sobre Tipos de Mapeamento`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#property-mapping
.. _`Mapeamento de propriedades`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#property-mapping
.. _`Documentação sobre os nomes de comandos SQL reservados`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#quoting-reserved-words
.. _`Creating Classes for the Database`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#creating-classes-for-the-database
.. _`DoctrineMongoDBBundle`: https://symfony.com/doc/current/bundles/DoctrineMongoDBBundle/index.html
.. _`migrations`: https://symfony.com/doc/current/bundles/DoctrineMigrationsBundle/index.html
.. _`DoctrineFixturesBundle`: https://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html
.. _`FrameworkExtraBundle documentation`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`newer utf8mb4 character set`: https://dev.mysql.com/doc/refman/5.5/en/charset-unicode-utf8mb4.html
.. _`Transactions and Concurrency`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/transactions-and-concurrency.html
