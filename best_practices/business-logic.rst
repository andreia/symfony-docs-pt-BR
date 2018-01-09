Organizando sua Lógica de Negócio
=================================

Em software de computador, **lógica de negócio** ou lógica de domínio é "a parte do
programa que codifica as regras de negócio do mundo real que determinam como os dados podem
ser criados, exibidos, armazenados e alterados" (leia a `definição completa`_).

Nas aplicações Symfony, a lógica de negócio é todo o código personalizado que você escreve para
sua aplicação que não é específico do framework (por exemplo, roteamento e controladores).
Classes de domínio, entidades do Doctrine e classes PHP regulares que são usadas como
serviços são bons exemplos de lógica de negócio.

Para a maioria dos projetos, você deve manter tudo dentro do AppBundle.
Lá você pode criar quaisquer diretórios que você queira para organizar as coisas:

.. code-block:: text

    projeto-symfony/
    ├─ app/
    ├─ src/
    │  └─ AppBundle/
    │     └─ Utils/
    │        └─ MyClass.php
    ├─ tests/
    ├─ var/
    ├─ vendor/
    └─ web/

Armazenar Classes Fora do Bundle?
---------------------------------

Mas não há nenhuma razão técnica para colocar a lógica de negócio dentro de um bundle.
Se você preferir, pode criar seu próprio namespace dentro do diretório ``src/``
e adicionar as coisas lá:

.. code-block:: text

    projeto-symfony/
    ├─ app/
    ├─ src/
    │  ├─ Acme/
    │  │   └─ Utils/
    │  │      └─ MyClass.php
    │  └─ AppBundle/
    ├─ tests/
    ├─ var/
    ├─ vendor/
    └─ web/

.. tip::

    A abordagem recomendada de usar o diretório ``AppBundle/`` é por
    simplicidade. Se você está avançado o suficiente para saber o que precisa estar em
    um bundle e o que pode ficar fora dele, então, sinta-se livre para fazê-lo.

Serviços: Nomenclatura e Formato
--------------------------------

A aplicação do blog precisa de um utilitário que pode transformar um título do post (por exemplo,
"Olá, Mundo") em um slug (por exemplo, "ola-mundo"). O slug será utilizado como
parte do URL do post.

Vamos criar uma nova classe ``Slugger`` dentro de ``src/AppBundle/Utils/`` e
adicionar o seguinte método ``slugify()``:

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
        # ...

        # use o nome de classe totalmente qualificado como o id do serviço
        AppBundle\Utils\Slugger:
            public: false

.. note::

    Se você está usando a :ref:`configuração services.yml padrão <service-container-services-load-example>`,
    a classe é auto-registrada como um serviço.

Tradicionalmente, a convenção de nomenclatura para um serviço era uma chave curta, mas única
no formato snake case - por exemplo, ``app.utils.slugger``. Mas para a maioria dos serviços, você agora deve
usar o nome da classe.

.. best-practice::

    O id dos serviços da sua aplicação deve ser igual aos seus nomes de classe,
    exceto quando você tem vários serviços configurados para a mesma classe (neste
    caso, use um id no formato snake case).

Agora você pode usar o slugger personalizado em qualquer classe de controlador, como, por exemplo, o
``AdminController``:

.. code-block:: php

    use AppBundle\Utils\Slugger;

    public function createAction(Request $request, Slugger $slugger)
    {
        // ...

        // você também pode buscar um serviço público da seguinte maneira
        // mas buscar serviços desta maneira não é considerado uma boa prática
        // $slugger = $this->get('app.slugger');

        if ($form->isSubmitted() && $form->isValid()) {
            $slug = $slugger->slugify($post->getTitle());
            $post->setSlug($slug);

            // ...
        }
    }

Serviços também podem ser :ref:`públicos ou privados <container-public>`. Se você usar a
:ref:`configuração services.yml padrão <service-container-services-load-example>`,
todos os serviços são privados por padrão.

.. best-practice::

    Serviços devem ser ``private`` sempre que possível. Isso impedirá que você
    acesse esse serviço por meio de ``$container->get()``. Em vez disso, você precisará usar
    injeção de dependência.

Formato de Serviço: YAML
------------------------

Na seção anterior, o YAML foi usado para definir o serviço.

.. best-practice::

    Use o formato YAML para definir os seus próprios serviços.

Isso é controverso, e em nossa experiência, o uso de YAML e XML é uniformemente
distribuído entre os desenvolvedores, com uma ligeira preferência pelo YAML.
Ambos os formatos têm o mesmo desempenho, por isso essa é, no fim de contas, uma questão
de gosto pessoal.

Recomendamos o YAML porque é conciso e amigável para os novatos. Você pode,
naturalmente, usar qualquer formato que preferir.

Serviço: Sem Parâmetro de Classe
--------------------------------

Você deve ter notado que a definição de serviço anterior não configura
o namespace da classe como um parâmetro:

.. code-block:: yaml

    # app/config/services.yml

    # definição de serviço com namespace da classe como parâmetro
    parameters:
        slugger.class: AppBundle\Utils\Slugger

    services:
        app.slugger:
            class: '%slugger.class%'

Esta prática é complicada e completamente desnecessária para os seus próprios serviços:

.. best-practice::

    Não defina parâmetros para as classes dos seus serviços.

Esta prática foi erroneamente adotada a partir dos bundles de terceiros. Quando o Symfony
introduziu o seu container de serviço, alguns desenvolvedores usaram essa técnica para permitir
facilmente a sobrescrita dos serviços. No entanto, sobrescrever um serviço apenas mudando o seu
nome da classe é um caso de uso muito raro porque, freqüentemente, o novo serviço possui
argumentos diferentes no construtor.

Usando uma Camada de Persistência
---------------------------------

O Symfony é um framework HTTP que só se preocupa com a geração de uma resposta HTTP
para cada requisição HTTP. É por isso que o Symfony não fornece uma forma para falar com
a camada de persistência (por exemplo, banco de dados, API externa). Você pode escolher qualquer
biblioteca ou estratégia que desejar para isso.

Na prática, muitas aplicações Symfony confiam no `projeto independente Doctrine`_
para definir o seu modelo usando entidades e repositórios.
Assim como com a lógica de negócio, recomendamos armazenar as entidades do Doctrine
no AppBundle.

As três entidades definidas pela nossa aplicação de exemplo do blog são um bom exemplo:

.. code-block:: text

    projeto-symfony/
    ├─ ...
    └─ src/
       └─ AppBundle/
          └─ Entity/
             ├─ Comment.php
             ├─ Post.php
             └─ User.php

.. tip::

    Se você está mais avançado, você pode, naturalmente, armazená-las no seu próprio
    namespace em ``src/``.

Informação de Mapeamento do Doctrine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As Entidades do Doctrine são objetos PHP simples que você armazena em algum "banco de dados".
O Doctrine só sabe sobre as suas entidades através dos metadados de mapeamento configurados
para as suas classes de modelo. O Doctrine suporta quatro formatos de metadados: YAML, XML,
PHP e anotações.

.. best-practice::

    Use anotações para definir as informações de mapeamento das entidades do Doctrine.

As anotações são, de longe, a forma mais conveniente e ágil de configurar e
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
         * @ORM\OrderBy({"publishedAt"="ASC"})
         */
        private $comments;

        public function __construct()
        {
            $this->publishedAt = new \DateTime();
            $this->comments = new ArrayCollection();
        }

        // getters e setters ...
    }

Todos os formatos têm o mesmo desempenho, por isso essa é, mais uma vez, uma
questão de gosto.

Fixtures de Dados
~~~~~~~~~~~~~~~~~

Como o suporte a fixtures não está habilitado por padrão no Symfony, você deve executar
o seguinte comando para instalar o bundle de fixtures do Doctrine:

.. code-block:: terminal

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
                $bundles[] = new Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle();
            }

            return $bundles;
        }

        // ...
    }

Recomendamos criar apenas *uma* `classe fixture`_ por simplicidade, embora
você possa ter mais no caso dessa classe ficar muito grande.

Supondo que você tenha pelo menos uma classe fixture e que o acesso ao banco de dados
está configurado corretamente, você pode carregar suas fixtures executando o seguinte
comando:

.. code-block:: terminal

    $ php bin/console doctrine:fixtures:load

    Careful, database will be purged. Do you want to continue Y/N ? Y
      > purging database
      > loading AppBundle\DataFixtures\ORM\LoadFixtures

Padrões de Codificação
----------------------

O código fonte do Symfony segue os padrões de codificação `PSR-1`_ e `PSR-2`_ que
foram definidos pela comunidade PHP. Você pode aprender mais sobre
:doc:`os Padrões de Codificação do Symfony</contributing/code/standards>` e até mesmo
usar o `PHP-CS-Fixer`_, que é um utilitário de linha de comando que pode corrigir os
padrões de codificação de uma base de código inteira em questão de segundos.

----

Next: :doc:`/best_practices/controllers`

.. _`definição completa`: https://en.wikipedia.org/wiki/Business_logic
.. _`projeto independente Doctrine`: http://www.doctrine-project.org/
.. _`classe fixture`: https://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html#writing-simple-fixtures
.. _`PSR-1`: http://www.php-fig.org/psr/psr-1/
.. _`PSR-2`: http://www.php-fig.org/psr/psr-2/
.. _`PHP-CS-Fixer`: https://github.com/FriendsOfPHP/PHP-CS-Fixer
