Organizando sua Lógica de Negócio
=================================

No software de computador, **lógica de negócio** ou a lógica de domínio é "a parte do
programa que codifica as regras de negócio do mundo real que determinam como os dados podem
ser criados, exibidos, armazenados e transformados "(leia a `definição completa`_).

Nas aplicações Symfony, a lógica de negócio é todo o código personalizado que você escreve para
sua aplicação que não é específico do framework (por exemplo, roteamento e controladores).
Classes de domínio, entidades do Doctrine e classes regulares PHP que são usadas ​​como
serviços são bons exemplos de lógica de negócio.

Para a maioria dos projetos, você deve guardar tudo dentro do ``AppBundle``.
Ali dentro, você pode criar quaisquer diretórios para organizar as coisas:

.. code-block:: text

    symfoy2-project/
    ├─ app/
    ├─ src/
    │  └─ AppBundle/
    │     └─ Utils/
    │        └─ MyClass.php
    ├─ vendor/
    └─ web/

Armazenar Classes fora do Bundle?
---------------------------------

Mas não há nenhuma razão técnica para colocar a lógica de negócio dentro de um bundle.
Se você preferir, pode criar seu próprio namespace dentro do diretório ``src/``
e adicionar as coisas lá:

.. code-block:: text

    symfoy2-project/
    ├─ app/
    ├─ src/
    │  ├─ Acme/
    │  │   └─ Utils/
    │  │      └─ MyClass.php
    │  └─ AppBundle/
    ├─ vendor/
    └─ web/

.. tip::

    A abordagem recomendada de usar o diretório ``AppBundle`` é por
    simplicidade. Se você está avançado o suficiente para saber o que precisa residir em
    um bundle e o que pode ficar fora dele, então, sinta-se livre para fazê-lo.

Serviços: Nomeando e Formatando
-------------------------------

A aplicação de blog precisa de um utilitário que pode transformar um título do post (por exemplo,
"Hello World") em um slug (por exemplo, "hello-world"). O slug será utilizado como
parte da URL do post.

Vamos, crie uma nova classe ``Slugger`` dentro de ``src/AppBundle/Utils/`` e
adicione o seguinte método ``slugify()``:

.. code-block:: php

    // src/AppBundle/Utils/Slugger.php
    namespace AppBundle\Utils;

    class Slugger
    {
        public function slugify($string)
        {
            return preg_replace(
                '/[^a-z0-9]/', '-', strtolower(trim(strip_tags($string)))
            );
        }
    }

Em seguida, defina um novo serviço para essa classe.

.. code-block:: yaml

    # app/config/services.yml
    services:
        # keep your service names short
        slugger:
            class: AppBundle\Utils\Slugger

Tradicionalmente, a convenção de nomenclatura para um serviço envolvia seguir
o nome da classe e localização para evitar colisões de nomes. Desse modo, o serviço
*deveria ser* chamado ``app.utils.slugger``. Mas, usando nomes de serviços curtos,
seu código será mais fácil de ler e usar.

.. best-practice::

    O nome dos serviços de sua aplicação deve ser o mais curto possível,
    idealmente apenas uma palavra simples.

Agora, você pode usar o slugger personalizado em qualquer classe de controlador, assim como o
``AdminController``:

.. code-block:: php

    public function createAction(Request $request)
    {
        // ...

        if ($form->isSubmitted() && $form->isValid()) {
            $slug = $this->get('slugger')->slugify($post->getTitle());
            $post->setSlug($slug);

            // ...
        }
    }

Formato de Serviço: YAML
------------------------

Na seção anterior, o YAML foi utilizado para definir o serviço.

.. best-practice::

    Use o formato YAML para definir os seus próprios serviços.

Isso é controverso, e em nossa experiência, o uso de YAML e XML é uniformemente
distribuído entre os desenvolvedores, com uma ligeira preferência pelo YAML.
Ambos os formatos têm o mesmo desempenho, por isso essa é, no fim de contas, uma questão
de gosto pessoal.

Recomendamos YAML porque é amigável para os novatos e conciso. Você,
naturalmente, pode usar qualquer formato que preferir.

Serviço: Sem Parâmetro de Classe
--------------------------------

Você deve ter notado que a definição de serviço anterior não configura
o namespace da classe como um parâmetro:

.. code-block:: yaml

    # app/config/services.yml

    # service definition with class namespace as parameter
    parameters:
        slugger.class: AppBundle\Utils\Slugger

    services:
        slugger:
            class: "%slugger.class%"

Esta prática é complicada e completamente desnecessária para os seus próprios serviços:

.. best-practice::

    Não defina parâmetros para as classes de seus serviços.

Essa prática foi erroneamente adotada a partir de bundles de terceiros. Quando o Symfony
introduziu o seu container de serviço, alguns desenvolvedores usaram essa técnica para permitir
facilmente a sobrescrita dos serviços. No entanto, sobrescrever um serviço apenas mudando o seu
nome da classe é um caso de uso muito raro porque, com frequência, o novo serviço possui
argumentos diferentes no construtor.

Usando uma Camada de Persistência
---------------------------------

O Symfony é um framework HTTP que só se preocupa com a geração de uma resposta HTTP
para cada solicitação HTTP. É por isso que o Symfony não fornece uma forma para falar com
a camada de persistência (por exemplo, banco de dados, API externa). Você pode escolher qualquer
biblioteca ou estratégia que desejar para isso.

Na prática, muitas aplicações Symfony confiam no `projeto Doctrine`_
independente para definir o seu modelo usando entidades e repositórios.
Assim como com a lógica de negócio, recomendamos armazenar as entidades do Doctrine
no ``AppBundle``

As três entidades definidas pela nossa aplicação exemplo de blog são um bom exemplo:

.. code-block:: text

    symfony2-project/
    ├─ ...
    └─ src/
       └─ AppBundle/
          └─ Entity/
             ├─ Comment.php
             ├─ Post.php
             └─ User.php

.. tip::

    Se você está mais avançado, pode, naturalmente, armazená-las sob o seu próprio
    namespace em ``src/``.

Informação de Mapeamento do Doctrine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As Entidades do Doctrine são objetos PHP simples que você armazena em algum "banco de dados".
O Doctrine só sabe sobre as suas entidades através do mapeamento de metadados configurado
para as suas classes de modelo. O Doctrine suporta quatro formatos de metadados: YAML, XML,
PHP e anotações.

.. best-practice::

    Use anotações para definir as informações de mapeamento das entidades do Doctrine.

As anotações são, de longe, a forma mais prática e ágil de configurar e
procurar informações de mapeamento:

.. code-block:: php

    namespace AppBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Doctrine\Common\Collections\ArrayCollection;

    /**
     * @ORM\Entity
     */
    class Post
    {
        const NUM_ITEMS = 10;

        /**
         * @ORM\Id
         * @ORM\GeneratedValue
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string")
         */
        private $title;

        /**
         * @ORM\Column(type="string")
         */
        private $slug;

        /**
         * @ORM\Column(type="text")
         */
        private $content;

        /**
         * @ORM\Column(type="string")
         */
        private $authorEmail;

        /**
         * @ORM\Column(type="datetime")
         */
        private $publishedAt;

        /**
         * @ORM\OneToMany(
         *      targetEntity="Comment",
         *      mappedBy="post",
         *      orphanRemoval=true
         * )
         * @ORM\OrderBy({"publishedAt" = "ASC"})
         */
        private $comments;

        public function __construct()
        {
            $this->publishedAt = new \DateTime();
            $this->comments = new ArrayCollection();
        }

        // getters and setters ...
    }

Todos os formatos têm o mesmo desempenho, por isso essa é, mais uma vez, uma
questão de gosto.

Fixtures de Dados
~~~~~~~~~~~~~~~~~

Como o suporte a fixtures não está habilitado por padrão no Symfony, você deve executar
o seguinte comando para instalar o bundle de fixtures do Doctrine:

.. code-block:: bash

    $ composer require "doctrine/doctrine-fixtures-bundle"

Em seguida, habilite o bundle em ``AppKernel.php``, mas apenas para os ambientes ``dev``
e ``test``:

.. code-block:: php

    use Symfony\Component\HttpKernel\Kernel;

    class AppKernel extends Kernel
    {
        public function registerBundles()
        {
            $bundles = array(
                // ...
            );

            if (in_array($this->getEnvironment(), array('dev', 'test'))) {
                // ...
                $bundles[] = new Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle(),
            }

            return $bundles;
        }

        // ...
    }

Recomendamos a criação de apenas *uma* `classe fixture`_ para simplicidade, embora
você possa ter mais no caso dessa classe ficar muito grande.

Supondo que você tenha pelo menos uma classe fixture e que o acesso ao banco de dados
está configurado corretamente, você pode carregar suas fixtures, executando o seguinte
comando:

.. code-block:: bash

    $ php app/console doctrine:fixtures:load

    Careful, database will be purged. Do you want to continue Y/N ? Y
      > purging database
      > loading AppBundle\DataFixtures\ORM\LoadFixtures

Padrões de Codificação
----------------------

O código fonte do Symfony segue os padrões de codificação `PSR-1`_ e `PSR-2`_ que
foram definidos pela comunidade PHP. Você pode aprender mais sobre
`padrões de código do Symfony`_ e até mesmo usar o `PHP-CS-Fixer`_, que é
um utilitário de linha de comando que pode corrigir os padrões de codificação de uma base de código inteira
em questão de segundos.

.. _`definição completa`: http://en.wikipedia.org/wiki/Business_logic
.. _`projeto Doctrine`: http://www.doctrine-project.org/
.. _`classe fixture`: http://symfony.com/doc/master/bundles/DoctrineFixturesBundle/index.html#writing-simple-fixtures
.. _`PSR-1`: http://www.php-fig.org/psr/psr-1/
.. _`PSR-2`: http://www.php-fig.org/psr/psr-2/
.. _`padrões de código do Symfony`: http://symfony.com/doc/current/contributing/code/standards.html
.. _`PHP-CS-Fixer`: https://github.com/fabpot/PHP-CS-Fixer
