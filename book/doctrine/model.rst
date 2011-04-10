.. index::
   single: Model

Introduction to the "Model"
===================
Introdução ao "Model"
================

If you wanted to learn more about fashion models and supermodels, then this
section won't be helpful to you. But if you're looking to learn about the
model - the layer in your application that manages data - then keep reading.
The Model description in this section is the one used when talking about
*Model-View-Controller* applications.

Se você queria aprender mais sobre desfiles e top models, então esta seção
não vai ser útil para você. Mas se você está procurando aprender sobre o 
modelo - a camada da sua aplicação que gerencia dados - então continue lendo.
A descrição de Model nesta seção é a única usada para falar sobre aplicações 
*Model-View-Controller*.

.. note::

   Model-View-Controller (MVC) is an application design pattern, that
   was originally introduced by Trygve Reenskaug for the Smalltalk
   platform. The main idea of MVC is separating presentation from the
   data and separating the controller from presentation. This kind of
   separation let's each part of the application focus on exactly one
   goal. The controller focuses on changing the data of the Model, the Model
   exposes its data for the View, and the View focuses on creating
   representations of the Model (e.g. an HTML page displaying "Blog Posts").
   
   Model-View-Controller (MVC) é um padrão de projeto de aplicação, que 
   foi originalmente introduzido por Trygve Reenskaug para a plataforma Smalltalk.
   A idéia principal é separar a apresentação dos dados separando o controlador
   da apresentação. Esse tipo de separação faz cada parte da aplicação focar em
   exatamente um ponto. O Controller foca na mudança dos dados do Model, o Model
   expõe seus dados para a View, e a View foca em criar a representação do Model.
   (Ex. uma página HTML mostrando "Postagens do Blog".

For example, when a user goes to your blog homepage, the user's browser sends
a request, which is passed to the Controller responsible for rendering
posts. The Controller calculates which posts should be displayed, retrieves
``Post`` Models from the database and passes that array to the View. The
View renders HTML that is interpreted by the browser.

Por exemplo, quando um usuário vai para a homepage do seu blog, o browser do usuário
envia uma requisição, que é passada ao Controller responsável por renderizar postagens.
O Controller calcula quais postagens devem ser exibidas, recupera os Models de ``Post`` da
base de dados e passa a array para a View. A View renderiza o HTML que é interpretado pelo
browser.


What is a Model anyway?
--------------------------------------
O que é um Model afinal?
--------------------------------------

The *Model* is what the "M" in "MVC_" stands for. It is one of the three
whales of an MVC application. A model is responsible for changing its
internal state based on requests from the :doc:`controller
</quick_tour/the_controller>` and giving its current state information
to the :doc:`view </book/templating>`. It is the main
application logic container.

Um *Model* é o significado do "M" no "MVC_". É um dos três pilares de 
uma aplicação MVC. Um modelo é responsável por mudar seu estado interno 
baseado em requisições vindas do :doc:`controller
</quick_tour/the_controller>` e fornecer informações sobre seu estado atual
para o :doc:`view </book/templating>`. Ele [o Model] é o principal detentor da
lógica da aplicação.

For example, if you are building a blog, then you'll have a ``Post``
model. If you're building a content management system, then you will
need a ``Page`` model.

Por exemplo, se você está construindo um blog, então você vai ter um modelo 
``Post``. Se você está contruindo um sistema de gerenciamento de conteúdo (CMS), 
então você vai precisar de um modelo ``Page``.

.. code-block:: php
    
    <?php
    
    namespace Blog;
    
    class Post
    {
        private $title;
        private $body;
        private $createdAt;
        private $updatedAt;
        
        public function __construct($title, $body)
        {
            $this->title     = $title;
            $this->body      = $body;
            $this->createdAt = new \DateTime();
        }
        
        public function setTitle($title)
        {
            $this->title     = $title;
            $this->updatedAt = new \DateTime();
        }
        
        public function setBody($body)
        {
            $this->body      = $body;
            $this->updatedAt = new \DateTime();
        }
        
        public function getTitle()
        {
            return $this->title;
        }
        
        public function getBody()
        {
            return $this->body;
        }
    }

It is obvious that the above class is very simple and testable, yet it's
mostly complete and will fulfill all the needs of a simple blogging
engine.

É óbvio que a classe acima é bem simples e testável, no entanto está 
quase completa e vai satisfazer todas as necessidades de um simples
gerenciador de blogs. 

That's it! You now you know what a Model in Symfony2 is: it is any class
that you want to save into some sort of data storage mechanism and
retrieve later. The rest of the chapter is dedicated to explaining how
to interact with the database.

É isso aí! Agora você sabe o que é um Model no Symfony2: é alguma
classe que você quer salvar em algum tipo de sistema de armazenamento e 
recuperar depois. O restante do capítulo é dedicado a explicar como interagir 
com a base de dados.

Databases and Symfony2
----------------------
Bancos de dados e o Symfony2
----------------------

It is worth noting that Symfony2 doesn't come with an ORM or database
abstraction library of its own, this is just not the problem Symfony2 is
meant to solve. However, it provides deep integration with libraries
like Doctrine_ and Propel_, letting you use whichever one you like best.

É uma pena observar que o Symfony2 não vem com seu próprio ORM ou biblioteca
de abstração de banco de dados, isso apenas não é da conta do Symfony2. 
De qualquer maneira, ele fornece profunda integração com bibiliotecas como 
Doctrine_ and Propel_, deixando que você escolha utilizar a qual preferir.

.. note::

   The acronym "ORM" stands for "Object Relational Mapping" and
   represents a programming technique of converting data between
   incompatible type systems. Say we have a ``Post``, which is stored as
   a set of columns in a database, but represented by an instance of
   class ``Post`` in your application. The process of transforming the
   from the database table into an object is called *object relation mapping*.
   We will also see that this term is slightly outdated as it is used in
   dealing with relational database management systems. Nowadays there are
   tons of non-relational data storage mechanism available. One such mechanism
   is the *document oriented database* (e.g. MongoDB), for which we invented a
   new term "ODM" or "Object Document Mapping".

Going forward, you'll learn about the `Doctrine2 ORM`_ and Doctrine2
`MongoDB ODM`_ (which serves as an ODM for MongoDB_ - a popular document
store) as both have the deepest integration with Symfony2 at the time of
this writing.

A Model is not a Table
---------------------------------
Um Model não é uma tabela
------------------------------------------

The perception of a model class as a database table, and each individual
instance as a row was popularized by the Ruby on Rails framework. It's
a good way of thinking about the model at first and it will get you far
enough, if you're exposing a simple `CRUD`_ (create, retrieve, update,
delete) interface in your application for modifying the data of a model.

A percepção de um modelo de classe como uma tabela de banco de dados,
e cada instância individual como uma tupla foi popularizada pelo
framework Ruby on Rails. Issa é uma boa forma de pensar sobre o
modelo primeiro e isso levará você longe o bastante, se você está
expondo uma simples interface `CRUD`_ (create, retrieve, updade, delete)
na sua aplicação para modificar os dados de um modelo.

This approach can actually cause problems once you're past the CRUD part
of your application and are trying to add more business logic. Here are
the common limitations of the above-described approach:

Esta abordagem pode atualmente causar problemas ao menos que você
esteja passando a parte do CRUD da sua aplicação e está tentando 
adicionar mais regras de negócio. Estas são as limitações comuns da
abordagem acima descrita:

* Designing schema before software that will utilize it is like digging
  a hole before you've identified what you need to bury. The item might
  fit, but most probably it won't.

* Projetar o esquema antes do software que irá utilizá-lo é como cavar 
  um buraco antes de saber o que você irá precisar enterrar nele.

* Database should be tailored to fit your application's needs, not the other way around.

* Bancos de dados precisam ser adaptados para atender as necessidades 
  da sua aplicação, não o contrário.

* Some data storage engines don't have a notion of tables, rows or even
  schema, which makes it hard to use them if your perception of a model
  is that it represents a table.

* Alguns mecanismos de armazenamento de dados não têm uma noção de 
  tabelas, linhas ou até mesmo de esquema, o que torna difícil usá-los se a 
  sua percepção de um modelo é que ele representa uma tabela.

* Keeping database schema in your head while designing your application
  domain is problematic, and following the rule of the lowest common
  denominator will give you the worst of both worlds.

* Manter o esquema de banco de dados na sua cabeça enquanto
  planeja o domínio da sua aplicação é problemático, e seguindo a regra
  do menor denominador comum vai lhe trazer o pior dos dois mundos.

The `Doctrine2 ORM`_ is designed to remove the need to keep database
structure in mind and let you concentrate on writing the cleanest
possible models that will satisfy your business needs. It lets you design
your classes and their interactions, allowing you to postpone persistence
decisions until you're ready.

O `Doctrine2 ORM`_ é concebido para remover a necessidade de manter
a estrutura de banco de dados em mente e deixar você concentrar-se em
os modelos mais simples possíveis e que satisfarão as necessidades do
seu negócio. Ele deixa você projetar suas classes e as interações delas,
possibilitando que você adie decisões sobre a persistência até que você 
esteja pronto.

Paradigm Shift
--------------------------------------
Mudança de Paradigma
--------------------------------------

With the introduction of Doctrine2, some of the core paradigms have
shifted. `Domain Driven Design`_ teaches us that objects are best
modeled when modeled after their real-world prototypes. For example a ``Car``
object is best modeled to contain ``Engine``, four instances of
``Tire``, etc. and should be produced by ``CarFactory`` - something that
knows how to assemble all the parts together. Domain driven design deserves
a book in its own, as the concept is rather broad. However, for the purposes
of this guide it should be clear, that a car cannot start by itself, there
must be an external impulse to start it. In a similar manner, a model cannot
save itself without an external impulse, therefore the following piece of
code violates DDD and will be troublesome to redesign in a clean, testable way.

Com a introdução do Doctrine2, muitos dos paradigmas fundamentais foram alterados.
`Domain Driven Design`_ nos ensida que objetos são melhores modelados quando
modelados após seus protótipos do mundo real. Por exemplo um objeto `Carro` é
melhor modelado contendo `Motor`, quatro instâncias de `Pneu`, etc. e deve ser
produzido pela `FabricaDeCarros` - alguma coisa que saiba como montar todas as partes
juntas. No entanto, o propósito deste guia isso deve ser claro, que um carro não pode 
ligar-se sozinho, deve haver um impulso externo para ligá-lo. De maneira semelhante, 
um modelo não pode salvar-se sem um impulso externo, portanto, o seguinte pedaço de 
código viola o DDD (Domain Driven Design) e vai ser problemático para reprojetá-lo de 
forma limpa e testável.

.. code-block:: php

   $post->save();

Hence, Doctrine2 is not your typical `Active Record`_ implementation anymore.
Instead Doctrine2 uses a different set of patterns, most importantly the
`Data Mapper`_ and `Unit Of Work`_ patterns. So in Doctrine2 you would do
the following:

Assim, o Doctrine2 não é mais uma típica implementação `Active Record`_.
Ao invés Doctrine2 usaum diferente conjunto de padrões, sendo `Data Mapper`_ 
e `Unit Of Work`_ os padrões mais importantes. Então no Doctrine2 você pode
fazer o seguinte:

.. code-block:: php

   $manager = //... get object manager instance

   $manager->persist($post);
   $manager->flush();


.. code-block:: php

   $manager = //... pega uma instância do object manager

   $manager->persist($post);
   $manager->flush();


The "object manager" is a central object provided by Doctrine whose job
is to persist objects. You'll soon learn much more about this service.
This paradigm shift lets us get rid of any base classes (e.g. the ``Post``
doesn't need to extend any base class) and static dependencies. Any object
can be saved into a database for later retrieval. More than that, once persisted,
an object is managed by the object manager, until the manager gets explicitly
cleared. That means, that all object interactions happen in memory
without ever going to the database until the ``$manager->flush()`` is
called. Needless to say, that this kind of approach lets you worry about
database and query optimizations even less, as all queries are as lazy
as possible (i.e. their execution is deferred until the latest possible
moment).

O "object manager" é um objeto central fornecido pelo Doctrine cujo papel
é persistir objetos. Você vai em breve aprender muito mais sobre este serviço.
Esta mudança de paradigma permite nos livrarmos de quaisquer classes de banco
(ex. o ``Post`` não precisa estender classe de banco sequer) e dependências 
estáticas. Qualquer objeto pode ser salvo num banco de dados para recuperação
futura. Mais que isso, uma vez persistido, um objeto é gerenciado pelo 
object manager, até que o manager seja limpo explicitamente. Isso significa, todos
as interações de objetos acontecem na memória sem nunca ir para o banco de dados
até que ``$manager->flush()`` seja chamado. Desnecessário dizer, que este tipo de
abordagem permite que você se preocupe menos ainda com banco de dados e 
otimização de consultas, como todas as consultas são tão preguiçosas 
quanto é possível (ou seja, a execução delas é atrasada até o momento 
mais tardio possível).

A very important aspect of ActiveRecord is performance, or rather the difficulty
in building a performant system. By using transactions and in-memory object
change tracking, Doctrine2 minimizes the communication with the database,
saving not only database execution time, but also expensive network communication.

Um aspecto muito importante do ActiveRecord é o desempenho, ou melhor, a dificuldade
de construir um sistema de alto desempenho. Usando transações e controle de 
transações de objeto na memória, o Doctrine2 diminui a comunicação com o banco de dados, 
economizando não somente no tempo de execução do banco de dados, mas também 
o grande tráfego na rede.

Conclusion 
----------------
Conclusão
----------------

Thanks to Doctrine2, The Model is now probably the simplest concept in
Symfony2: it is in your complete control and not limited by persistence
specifics.

Graças ao Doctrine2, o "Model" é agora provavelmente o conceito mais simples
do Symfony2: está completamente sob seu controle e não limitado por especifidades da 
persistência.

By teaming up with Doctrine2 to keep your code relieved of persistence
details, Symfony2 makes building database-aware applications even
simpler. Application code stays clean, which will decrease development
time and improve understandability of the code.

Ao associar-se ao Doctrine2 para manter o seu código aliviado dos detalhes de 
persistência, o Symfony2 torna mais simples a construção de aplicações 
"database-aware". O código do aplicativo fica limpo, o que diminuirá o tempo de 
desenvolvimento e melhorará a legibilidade do código.

.. _Doctrine: http://www.doctrine-project.org/
.. _Propel: http://www.propelorm.org/
.. _Doctrine2 DBAL: http://www.doctrine-project.org/projects/dbal
.. _Doctrine2 ORM: http://www.doctrine-project.org/projects/orm
.. _MongoDB ODM: http://www.doctrine-project.org/projects/mongodb_odm
.. _MongoDB: http://www.mongodb.org
.. _Domain Driven Design: http://domaindrivendesign.org/
.. _Active Record: http://martinfowler.com/eaaCatalog/activeRecord.html
.. _Data Mapper: http://martinfowler.com/eaaCatalog/dataMapper.html
.. _Unit Of Work: http://martinfowler.com/eaaCatalog/unitOfWork.html
.. _CRUD: http://en.wikipedia.org/wiki/Create,_read,_update_and_delete
.. _MVC: http://en.wikipedia.org/wiki/Model-View-Controller