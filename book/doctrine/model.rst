.. index::
   single: Model

Introdução ao "Modelo"
======================

Se você queria aprender mais sobre desfiles e top models, então esta seção
não vai ser útil para você. Mas se você está procurando aprender sobre o 
modelo - a camada da sua aplicação que gerencia dados - então continue lendo.
A descrição de Model nesta seção é uma das usadas para falar sobre aplicações 
*Modelo-Visão-Controlador*.

.. note::

   Modelo-Visão-Controlador (MVC) é um padrão de projeto de aplicação, que 
   foi originalmente introduzido por Trygve Reenskaug para a plataforma Smalltalk.
   A idéia principal do MVC é separar a apresentação dos dados e separar o controlador
   da apresentação. Esse tipo de separação faz cada parte da aplicação focar em
   exatamente um ponto. O Controlador foca na mudança dos dados do Modelo, o Modelo
   expõe seus dados para a Visão, e a Visão foca em criar a representação do Modelo.
   (Ex. uma página HTML mostrando "Postagens do Blog".

Por exemplo, quando um usuário vai para a homepage do seu blog, o navegador do usuário
envia uma requisição, que é passada ao Controlador responsável por renderizar postagens.
O Controlador calcula quais postagens devem ser exibidas, recupera os Modelos de ``Post`` da
base de dados e passa o array para a Visão. A Visão renderiza o HTML que é interpretado pelo
navegador.

O que é um Model afinal?
--------------------------------------

Um *Modelo* é o significado do "M" no "MVC_". É um dos três pilares de 
uma aplicação MVC. Um modelo é responsável por mudar seu estado interno 
baseado em requisições vindas do :doc:`controlador
</quick_tour/the_controller>` e fornecer informações sobre seu estado atual
para a :doc:`visão </book/templating>`. Ele [o modelo] é o principal detentor da
lógica da aplicação.

Por exemplo, se você está construindo um blog, então você vai ter um modelo 
``Post``. Se você está contruindo um sistema de gerenciamento de conteúdo (CMS), 
então você vai precisar de um modelo ``Página``.

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
    
É óbvio que a classe acima é bem simples e testável, no entanto está 
quase completa e vai satisfazer todas as necessidades de um simples
gerenciador de blogs. 

É isso aí! Agora você sabe o que é um Modelo no Symfony2: é alguma
classe que você quer salvar em algum tipo de sistema de armazenamento e 
recuperar depois. O restante do capítulo é dedicado a explicar como interagir 
com o banco de dados.

Bancos de dados e o Symfony2
----------------------

É uma pena observar que o Symfony2 não vem com seu próprio ORM ou biblioteca
de abstração de banco de dados, isso apenas não cabe ao Symfony2 resolver. 
De qualquer maneira, ele fornece profunda integração com bibiliotecas como 
Doctrine_ and Propel_, deixando que você escolha utilizar a qual preferir.

.. note::

   O acrônimo "ORM" significa "Object Relational Mapping" ou 
   "Mapeamento Objeto-Relacional" e representa uma
   técnica de programação de converter dados entre sistemas de tipos 
   incompatíveis. Dizer que temos um ``Post``, qual é armazenada como
   um conjunto de colunas em um banco de dados, mas representado pela 
   instância da classe ``Post`` na sua aplicação. O processo de transformar
   uma tabela de banco de dados em um objeto é chamado *object relation mapping* 
   ou *mapeamento de objeto-relação*. Veremos também que esse termo é um pouco 
   desatualizado pois ele é usado para lidar com sistemas gerenciadores de 
   bancos de dados relacionais. Hoje em dia existem toneladas de mecanismos de 
   armazenamento de dados não relacionais disponíveis. Um desses mecanismos é 
   o *document oriented database* ou *banco de dados orientado a documentos* 
   (ex. MongoDB), para qual nós inventamos um novo termo "ODM" or 
   "Object Document Mapping" ou em nossa língua "Mapeamento Objeto-Documento".
   

Indo adiante, você vai aprender sobre o `Doctrine2 ORM`_ and Doctrine2
`MongoDB ODM`_ (qual serve como um ODM para MongoDB_ - um popular armazenador 
de documentos, visto que ambos possuem profunda integração com o Symfony2 até o
momento dessa escrita.

Um Model não é uma tabela
------------------------------------------

A percepção de um modelo de classe como uma tabela de banco de dados,
e cada instância individual como uma tupla foi popularizada pelo
framework Ruby on Rails. Essa é uma boa forma de pensar sobre o
modelo primeiro e isso levará você longe o bastante, se você está
expondo uma simples interface `CRUD`_ (criar, recuperar, atualizar, deletar)
na sua aplicação para modificar os dados de um modelo.

Esta abordagem pode atualmente causar problemas quando você está além
da parte CRUD da sua aplicação e está tentando adicionar mais regra de negócio. 
Estas são as limitações comuns da abordagem acima descrita:

* Projetar o esquema antes do software que irá utilizá-lo é como cavar 
  um buraco antes de saber o que você irá precisar enterrar nele.

* Bancos de dados precisam ser adaptados para atender as necessidades 
  da sua aplicação, não o contrário.

* Alguns mecanismos de armazenamento de dados não têm uma noção de 
  tabelas, linhas ou até mesmo de esquema, o que torna difícil usá-los se a 
  sua percepção de um modelo é que ele representa uma tabela.

* Manter o esquema de banco de dados na sua cabeça enquanto
  planeja o domínio da sua aplicação é problemático, e seguindo a regra
  do menor denominador comum vai lhe trazer o pior dos dois mundos.

O `Doctrine2 ORM`_ é concebido para remover a necessidade de manter
a estrutura de banco de dados em mente e deixar você concentrar-se em 
escrever os modelos mais simples possíveis e que satisfarão as 
necessidades do seu negócio. Ele deixa você projetar suas classes e as 
interações delas, possibilitando que você adie decisões sobre a 
persistência até que você esteja pronto para isso.

Mudança de Paradigma
--------------------------------------

Com a introdução do Doctrine2, muitos dos paradigmas fundamentais foram alterados.
`Domain Driven Design`_ nos ensina que objetos são melhores modelados quando
modelados após seus protótipos do mundo real. Por exemplo um objeto `Carro` é
melhor modelado contendo `Motor`, quatro instâncias de `Pneu`, etc. e deve ser
produzido pela `FabricaDeCarros` - alguma coisa que saiba como montar todas as partes
juntas. No entanto, o propósito deste guia deve ser claro, que um carro não pode 
ligar-se sozinho, deve haver um impulso externo para ligá-lo. De maneira semelhante, 
um modelo não pode salvar-se sem um impulso externo, portanto, o seguinte pedaço de 
código viola o DDD (Domain Driven Design) e vai ser problemático para reprojetá-lo de 
forma limpa e testável.

.. code-block:: php

   $post->save();

Assim, o Doctrine2 não é mais uma típica implementação `Active Record`_.
Ao invés Doctrine2 usa um diferente conjunto de padrões, sendo `Data Mapper`_ 
e `Unit Of Work`_ os padrões mais importantes. Então no Doctrine2 você pode
fazer o seguinte:

.. code-block:: php

   $manager = //... pega uma instância do "object manager"

   $manager->persist($post);
   $manager->flush();

O "object manager" é um objeto central fornecido pelo Doctrine cujo papel
é persistir objetos. Você vai em breve aprender muito mais sobre este serviço.
Essa mudança de paradigma permite nos livrarmos de quaisquer classes de banco
(ex. o ``Post`` não precisa estender classe de banco sequer) e dependências 
estáticas. Qualquer objeto pode ser salvo num banco de dados para recuperação
futura. Mais que isso, uma vez persistido, um objeto é gerenciado pelo 
object manager, até que o manager seja limpo explicitamente. Isso significa, todas
as interações de objetos acontecem na memória sem nunca ir para o banco de dados
até que ``$manager->flush()`` seja chamado. Desnecessário dizer, que este tipo de
abordagem permite que você se preocupe menos ainda com banco de dados e 
otimização de consultas, como todas as consultas são tão preguiçosas 
quanto é possível (ou seja, a execução delas é atrasada até o momento 
mais tardio possível).

Um aspecto muito importante do ActiveRecord é o desempenho, ou melhor, a dificuldade
de construir um sistema de alto desempenho. Usando transações e controle de 
transações de objeto na memória, o Doctrine2 diminui a comunicação com o banco de dados, 
economizando não somente no tempo de execução do banco de dados, mas também 
o grande tráfego na rede.

Conclusão
----------------

Graças ao Doctrine2, o Modelo é agora provavelmente o conceito mais simples
do Symfony2: está completamente sob seu controle e não limitado por especifidades da 
persistência.

Ao associar-se ao Doctrine2 para manter o seu código aliviado dos detalhes de 
persistência, o Symfony2 torna mais simples a construção de aplicações 
do tipo "database-aware". O código do aplicativo fica limpo, o que diminuirá 
o tempo de desenvolvimento e melhorará a legibilidade do código.

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
