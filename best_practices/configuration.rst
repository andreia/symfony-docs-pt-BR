Configuração
============

Configuração geralmente envolve diferentes partes da aplicação (tais como infraestrutura
e credenciais de segurança) e diferentes ambientes (desenvolvimento, produção).
É por isso que o Symfony recomenda que você divida a configuração da aplicação em
três partes.

Configuração Relacionada à Infraestrutura
-----------------------------------------

.. best-practice::

    Defina as opções de configuração de infraestrutura no
    arquivo ``app/config/parameters.yml``.

O arquivo default ``parameters.yml`` segue essa recomendação e define as
opções relacionadas à infraestrutura do banco de dados e servidor de e-mail:

.. code-block:: yaml

    # app/config/parameters.yml
    parameters:
        database_driver:   pdo_mysql
        database_host:     127.0.0.1
        database_port:     ~
        database_name:     symfony
        database_user:     root
        database_password: ~

        mailer_transport:  smtp
        mailer_host:       127.0.0.1
        mailer_user:       ~
        mailer_password:   ~

        # ...

Essas opções não são definidas no arquivo ``app/config/config.yml`` porque
elas não têm nada a ver com o comportamento da aplicação. Em outras palavras, a sua
aplicação não se preocupa com a localização do seu banco de dados ou com as credenciais
para acessá-lo, contanto que o banco de dados está configurado corretamente.

Parâmetros Canônicos
~~~~~~~~~~~~~~~~~~~~

.. best-practice::

    Defina todos os parâmetros de sua aplicação no
    arquivo ``app/config/parameters.yml.dist``.

Desde a versão 2.3, o Symfony inclui um arquivo de configuração chamado ``parameters.yml.dist``,
que armazena a lista canônica dos parâmetros de configuração da aplicação.

Sempre que um novo parâmetro de configuração é definido para a aplicação, você
também deve adicioná-lo nesse arquivo e enviar as alterações para o seu sistema de controle de
versão. Então, sempre que um desenvolvedor atualiza ou implanta o projeto em um servidor,
o Symfony irá verificar se há alguma diferença entre o arquivo canônico
``parameters.yml.dist`` e o seu arquivo local ``parameters.yml``. Caso houver
, o Symfony irá pedir-lhe para fornecer um valor para o novo parâmetro
e vai adicioná-lo ao seu arquivo local ``parameters.yml``.

Configuração Relacionada à Aplicação
------------------------------------

.. best-practice::

    Defina as opções de configuração relacionadas ao comportamento da aplicação no
    arquivo ``app/config/config.yml``.

O arquivo ``config.yml`` contém as opções usadas pela aplicação para modificar
seu comportamento, como o remetente de notificações de e-mail, ou `feature toggles`_
habilitado. Definir esses valores no arquivo `` parameters.yml``
adicionaria uma camada extra de configuração que não é necessária, porque você não precisa
ou quer que esses valores de configuração mudem em cada servidor.

As opções de configuração definidas no arquivo ``config.yml`` geralmente variam de
um `ambiente de execução`_ para outro. É por isso que o Symfony já inclui
os arquivos ``app/config/config_dev.yml`` e ``app/config/config_prod.yml``
onde você pode substituir valores específicos para cada ambiente.

Constantes vs Opções de Configuração
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Um dos erros mais comuns na definição de configuração da aplicação é
criar novas opções para valores que nunca mudam, como o número de itens para
resultados paginados.

.. best-practice::

    Use constantes para definir opções de configuração que raramente mudam.

A abordagem tradicional para a definição de opções de configuração fez muitas
aplicações Symfony adicionarem uma opção como a seguinte, que seria usada
para controlar o número de posts que serão exibidos na página inicial do blog:

.. code-block:: yaml

    # app/config/config.yml
    parameters:
        homepage.num_items: 10

Se você perguntar a si mesmo quando foi a última vez que alterou o valor de
*qualquer opção* como essa, as chances são de que você *nunca* tenha alterado. Criar uma opção de configuração
para um valor que você nunca irá configurar simplesmente não é necessário.
Nossa recomendação é definir esses valores como constantes na sua aplicação.
Você pode, por exemplo, definir uma constante ``NUM_ITEMS`` na entidade ``Post``:

.. code-block:: php

    // src/AppBundle/Entity/Post.php
    namespace AppBundle\Entity;

    class Post
    {
        const NUM_ITEMS = 10;

        // ...
    }

A principal vantagem da definição de constantes é que você pode usar seus valores
em qualquer local da sua aplicação. Ao utilizar parâmetros, eles somente estão disponíveis
em locais com acesso ao container do Symfony.

Constantes podem ser usadas, por exemplo, em seus templates Twig graças a
função ``constant()``:

.. code-block:: html+jinja

    <p>
        Displaying the {{ constant('NUM_ITEMS', post) }} most recent results.
    </p>

E entidades e repositórios do Doctrine agora podem acessar facilmente esses valores,
ao passo que, eles não podem acessar os parâmetros do container:

.. code-block:: php

    namespace AppBundle\Repository;

    use Doctrine\ORM\EntityRepository;
    use AppBundle\Entity\Post;

    class PostRepository extends EntityRepository
    {
        public function findLatest($limit = Post::NUM_ITEMS)
        {
            // ...
        }
    }

A única desvantagem notável do uso de constantes para esses tipos de valores de configuração
é que você não pode redefini-los facilmente em seus testes.

Configuração Semântica: Não faça isso
-------------------------------------

.. best-practice::

    Não defina uma configuração de injeção de dependência semântica para seus bundles.

Conforme explicado no artigo `Como expor uma configuração semântica para um Bundle`_,
os bundles do Symfony têm duas opções para lidar com a configuração: configuração normal do serviço
através do arquivo ``services.yml`` e configuração semântica
através de uma classe especial ``*Extension``.

Embora a configuração semântica seja muito mais poderosa e ofereça bons recursos
tais como validação de configuração, a quantidade de trabalho necessário para definir essa
configuração não vale a pena para bundles que não são destinados a serem compartilhados como
bundles de terceiros.

Movendo Opções Sensíveis Totalmente fora do Symfony
---------------------------------------------------

Quando se tratar de opções sensíveis, como as credenciais de banco de dados, também recomendamos
que você armazene elas fora do projeto Symfony e torne-as disponíveis
através de variáveis ​​de ambiente. Saiba como fazer no seguinte artigo:
`Como definir Parâmetros externos no Container de Serviço`_

.. _`feature toggles`: http://en.wikipedia.org/wiki/Feature_toggle
.. _`ambiente de execução`: http://symfony.com/doc/current/cookbook/configuration/environments.html
.. _`função constant()`: http://twig.sensiolabs.org/doc/functions/constant.html
.. _`Como expor uma configuração semântica para um Bundle`: http://symfony.com/doc/current/cookbook/bundles/extension.html
.. _`Como definir Parâmetros externos no Container de Serviço`: http://symfony.com/doc/current/cookbook/configuration/external_parameters.html
