.. index::
   single: Doctrine; Generating entities from existing database

Como gerar Entidades de uma base de dados existente
===================================================

Quando se começa a trabalhar em um novo projeto que usa banco de dados, duas
diferentes situações são comuns. Na maioria dos casos, o modelo de banco de
dados é projetado e construído do zero. Mas algumas vezes, você começará com
um modelo de banco de dados existente e provavelmente não poderá alterá-lo.
Felizmente, o Doctrine possui muitas ferramentas que podem te ajudar a gerar
as classes model do seu de banco de dados já existente.

.. note::

    Como a `Doctrine tools documentation`_ diz, engenharia reversa é um
    processo único ao iniciar um projeto. Doctrine é capaz de converter
    aproximadamente 70-80% das instruções de mapeamento necessárias baseadas
    em campos, índices e chaves estrangeiras. Doctrine não pode descobrir
    associações inversas, tipos de herança, entidades com chaves estrangeiras
    como chaves primárias ou operações semânticas em associações como cascata
    ou eventos de ciclo de vida. Alguns ajustes adicionais nas entidades
    geradas são necessários posteriormente para adequar as especificidades de
    cada modelo de domínio.

Este tutorial asssume que você está utilizando uma simples aplicação de blog
com as seguintes duas tabelas: ``blog_post`` e ``blog_comment``. Um comentário
gravado é ligado a um post gravado graças a uma chave estrangeira.

.. code-block:: sql

    CREATE TABLE `blog_post` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `title` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
      `content` longtext COLLATE utf8_unicode_ci NOT NULL,
      `created_at` datetime NOT NULL,
      PRIMARY KEY (`id`),
    ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

    CREATE TABLE `blog_comment` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `post_id` bigint(20) NOT NULL,
      `author` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
      `content` longtext COLLATE utf8_unicode_ci NOT NULL,
      `created_at` datetime NOT NULL,
      PRIMARY KEY (`id`),
      KEY `blog_comment_post_id_idx` (`post_id`),
      CONSTRAINT `blog_post_id` FOREIGN KEY (`post_id`) REFERENCES `blog_post` (`id`) ON DELETE CASCADE
    ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

Antes de começar a receita, verifique se seus parâmetros de conexão com banco
de dados estão configurados corretamente no arquivo
``app/config/parameters.ini`` (ou onde quer que sua configuração de banco de
dados é mantida) e que você tenha inicializado um bundle que irá receber sua
futura classe de entidade. NEste tutorial, vamos supor que um
``AcmeBlogBundle`` existe e está localizado na pasta ``src/Acme/BlogBundle``.

O primeiro passo para construir as classes de entidade de uma base da dados
é solicitar que o Doctrine faça a introspecção do banco de dados e gere os
arquivos de metadados. Os arquivos de metadados descrevem a classe de entidade
baseados nos campos da tabela.


.. code-block:: bash

    php app/console doctrine:mapping:convert xml ./src/Acme/BlogBundle/Resources/config/doctrine/metadata/orm --from-database --force

Esta ferramenta de linha de comando pede para o Doctrine fazer a introspecção
do banco de dados e gerar os arquivos de metadados na pasta ``src/Acme/BlogBundle/Resources/config/doctrine/metadata/orm``
do seu bundle.

.. tip::

    Também é possivel gerar a classe de metadados no formato YAML alterando o
    primeiro argumento para `yml`.

O arquivo de metadados gerado ``BlogPost.dcm.xml`` é semelhante a isto:

.. code-block:: xml

    <?xml version="1.0" encoding="utf-8"?>
    <doctrine-mapping>
      <entity name="BlogPost" table="blog_post">
        <change-tracking-policy>DEFERRED_IMPLICIT</change-tracking-policy>
        <id name="id" type="bigint" column="id">
          <generator strategy="IDENTITY"/>
        </id>
        <field name="title" type="string" column="title" length="100"/>
        <field name="content" type="text" column="content"/>
        <field name="isPublished" type="boolean" column="is_published"/>
        <field name="createdAt" type="datetime" column="created_at"/>
        <field name="updatedAt" type="datetime" column="updated_at"/>
        <field name="slug" type="string" column="slug" length="255"/>
        <lifecycle-callbacks/>
      </entity>
    </doctrine-mapping>

.. note::

    Caso possuir relacionamentos ``oneToMany`` entre suas entidades,
    você precisará editar os arquivos ``xml`` ou ``yml`` gerados para adicionar
    uma seção sobre as entidades específicas para ``oneToMany`` definindo as
    partes ``inversedBy`` e ``mappedBy``.

Uma vez que os arquivos de metados foram gerados, você pode pedir para Doctrine
importar o esquema e construir as classes de entidade relacionadas com a
execução dos dois comandos a seguir.

.. code-block:: bash

    php app/console doctrine:mapping:import AcmeBlogBundle annotation
    php app/console doctrine:generate:entities AcmeBlogBundle

O primeiro comando gera as classes de entidade com um mapeamento de anotações,
msa você poderá alterar o argumento ``annotation``para ``xml`` ou ``yml``.
A classe de entidade ``BlogComment`` recém-criada é semelhante a isto:

.. code-block:: php

    <?php

    // src/Acme/BlogBundle/Entity/BlogComment.php
    namespace Acme\BlogBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;

    /**
     * Acme\BlogBundle\Entity\BlogComment
     *
     * @ORM\Table(name="blog_comment")
     * @ORM\Entity
     */
    class BlogComment
    {
        /**
         * @var bigint $id
         *
         * @ORM\Column(name="id", type="bigint", nullable=false)
         * @ORM\Id
         * @ORM\GeneratedValue(strategy="IDENTITY")
         */
        private $id;

        /**
         * @var string $author
         *
         * @ORM\Column(name="author", type="string", length=100, nullable=false)
         */
        private $author;

        /**
         * @var text $content
         *
         * @ORM\Column(name="content", type="text", nullable=false)
         */
        private $content;

        /**
         * @var datetime $createdAt
         *
         * @ORM\Column(name="created_at", type="datetime", nullable=false)
         */
        private $createdAt;

        /**
         * @var BlogPost
         *
         * @ORM\ManyToOne(targetEntity="BlogPost")
         * @ORM\JoinColumn(name="post_id", referencedColumnName="id")
         */
        private $post;
    }

Como você pode ver, Doctrine converte todos os campos da tabela para
propriedades privadas e anotadas da classe. O mais impressionante é que ele
descobre o relacionamento com a classe de entidade ``BlogPost`` baseada na
restrição de chave estrangeira.
Conseqüentemente, você pode encontrar uma propriedade privada ``$post`` mapeada
com por uma entidade ``BlogPost`` na classe de entidade ``BlogComment``.

O último comando gerou todos os getters e setters para todas as propriedades
das duas classes de entidade ``BlogPost`` e ``BlogComment``. As entidades
geradas agora estão prontas para serem usadas. Divirta-se!

.. _`Doctrine tools documentation`: .. _`Doctrine tools documentation`: http://docs.doctrine-project.org/projects/doctrine-orm/en/2.1/reference/tools.html#reverse-engineering
