Configuração
=============

A configuração geralmente envolve diferentes partes da aplicação (como infraestrutura
e credenciais de segurança) e diferentes ambientes (desenvolvimento, produção).
É por isso que o Symfony recomenda dividir a configuração da aplicação em
três partes.

.. _config-parameters.yml:

Configuração Relacionada à Infraestrutura
-----------------------------------------

Estas são as opções que mudam de uma máquina para outra (por exemplo, da sua
máquina de desenvolvimento para o servidor de produção), mas que não alteram
o comportamento da aplicação.

.. best-practice::

    Defina as opções de configuração relacionadas à infraestrutura como
    :doc:`variáveis de ambiente </configuration/external_parameters>`. Durante o
    desenvolvimento, use os arquivos ``.env`` e ``.env.local`` localizados no diretório
    raiz do seu projeto para setá-las.

Por padrão, o Symfony adiciona esses tipos de opções no arquivo ``.env`` ao
instalar novas dependências na aplicação:

.. code-block:: bash

    # .env
    ###> doctrine/doctrine-bundle ###
    DATABASE_URL=sqlite:///%kernel.project_dir%/var/data/blog.sqlite
    ###< doctrine/doctrine-bundle ###

    ###> symfony/swiftmailer-bundle ###
    MAILER_URL=smtp://localhost?encryption=ssl&auth_mode=login&username=&password=
    ###< symfony/swiftmailer-bundle ###

    # ...

Essas opções não estão definidas dentro do arquivo ``config/services.yaml`` porque
elas não têm nada a ver com o comportamento da aplicação. Em outras palavras, sua
aplicação não se preocupa com a localização do seu banco de dados ou as credenciais
para acessá-lo, desde que o banco de dados esteja configurado corretamente.

Para sobrescrever essas variáveis ​​com valores específicos da máquina ou valores sensíveis, crie
um arquivo ``env.local``. Esse arquivo não deve ser adicionado ao controle de versão.

.. caution::

    Cuidado com o dump do conteúdo das variáveis ​​``$ _SERVER`` e ``$ _ENV``
    ou a saída do conteúdo do ``phpinfo()`` irá mostrar os valores do
    variáveis ​​de ambiente, expondo informações sensíveis como as credenciais
    do banco de dados.

.. _best-practices-canonical-parameters:

Parâmetros Canônicos
~~~~~~~~~~~~~~~~~~~~

.. best-practice::

    Defina todas as variáveis env da sua aplicação no arquivo ``.env``.

O Symfony inclui um arquivo de configuração chamado ``.env`` na raiz do projeto, que
armazena a lista canônica de variáveis ​​de ambiente para o aplicação. Este
arquivo deve ser armazenado no controle de versão e, portanto, deve conter apenas
valores padrão não-sensíveis.

.. caution::

    As aplicações criadas antes de novembro de 2018 tinham um sistema ligeiramente diferente,
    envolvendo um arquivo ``.env.dist``. Para obter informações sobre atualização, consulte:
    :doc:`/configuration/dot-env-changes`.

Configuração Relacionada à Aplicação
------------------------------------

.. best-practice::

    Defina as opções de configuração relacionadas ao comportamento da aplicação no
    arquivo ``config/services.yaml``.

O arquivo ``services.yaml`` contém as opções usadas pela aplicação para
modificar seu comportamento, como o remetente de notificações por email ou o
`feature toggles`_ habilitado. Definir esses valores no arquivo ``.env`` adicionaria
uma camada extra de configuração que não é necessária porque você não precisa ou quer
que esses valores de configuração alterem em cada servidor.

As opções de configuração definidas no ``services.yaml`` podem variar de um
:doc:`ambiente </configuration/environments>` para outro. É por isso que o Symfony
suporta definir arquivos ``config/services_dev.yaml`` e ``config/services_prod.yaml``
para que você possa sobrescrever valores específicos para cada ambiente.

Constantes vs Opções de Configuração
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Um dos erros mais comuns ao definir a configuração da aplicação é
criar novas opções para valores que nunca mudam, como o número de itens para
resultados paginados.

.. best-practice::

    Use constantes para definir opções de configuração que raramente mudam.

A abordagem tradicional para definir as opções de configuração fez com que muitas
aplicações Symfony incluíssem uma opção como a seguinte, que seria usada
para controlar o número de postagens a serem exibidas na página inicial do blog:

.. code-block:: yaml

    # config/services.yaml
    parameters:
        homepage.number_of_items: 10

Se você já fez algo assim no passado, é provável que você realmente
*nunca* precisou mudar esse valor. Criar uma opção de configuração para
um valor que você nunca vai configurar simplesmente não é necessário.
Nossa recomendação é definir esses valores como constantes em sua aplicação.
Você poderia, por exemplo, definir uma constante ``NUMBER_OF_ITEMS`` na entidade ``Post``::

    // src/Entity/Post.php
    namespace App\Entity;

    class Post
    {
        const NUMBER_OF_ITEMS = 10;

        // ...
    }

A principal vantagem de definir constantes é que você pode usar seus valores
em qualquer lugar na aplicação. Ao usar parâmetros, eles estão disponíveis apenas
de lugares com acesso ao contêiner do Symfony.

Constantes podem ser usadas por exemplo em seus templates Twig graças a
`função constant()`_:

.. code-block:: html+twig

    <p>
        Displaying the {{ constant('NUMBER_OF_ITEMS', post) }} most recent results.
    </p>

As entidades e repositórios do Doctrine agora podem acessar facilmente esses valores,
enquanto eles não podem acessar os parâmetros do contêiner::

    namespace App\Repository;

    use App\Entity\Post;
    use Doctrine\ORM\EntityRepository;

    class PostRepository extends EntityRepository
    {
        public function findLatest($limit = Post::NUMBER_OF_ITEMS)
        {
            // ...
        }
    }

A única desvantagem notável de usar constantes para esse tipo de valores de
configuração é que você não pode redefini-los facilmente em seus testes.

Nomeação de Parâmetros
----------------------

.. best-practice::

    O nome dos seus parâmetros de configuração deve ser o mais curto possível e
    deve incluir um prefixo comum para a aplicação inteira.

Usar o ``app`` como prefixo dos seus parâmetros é uma prática comum para evitar
colisões com os parâmetros de bibliotecas/bundles do Symfony e de terceiros. Então, use
apenas uma ou duas palavras para descrever o objetivo do parâmetro:

.. code-block:: yaml

    # config/services.yaml
    parameters:
        # don't do this: 'dir' is too generic and it doesn't convey any meaning
        app.dir: '...'
        # do this: short but easy to understand names
        app.contents_dir: '...'
        # it's OK to use dots, underscores, dashes or nothing, but always
        # be consistent and use the same format for all the parameters
        app.dir.contents: '...'
        app.contents-dir: '...'

----

Próximo: :doc:`/best_practices/business-logic`

.. _`feature toggles`: https://en.wikipedia.org/wiki/Feature_toggle
.. _`função constant()`: https://twig.symfony.com/doc/2.x/functions/constant.html
