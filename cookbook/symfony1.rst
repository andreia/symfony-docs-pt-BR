Como o Symfony2 difere do symfony1
==================================

O framework Symfony2 incorpora uma evolução significativa quando comparado com
a primeira versão do framework. Felizmente, com a arquitetura MVC
em sua essência, as habilidades usadas para dominar um projeto symfony1 continuam a ser
muito relevantes ao desenvolver com o Symfony2. Claro, o ``app.yml`` não existe mais, mas quanto ao 
roteamento, controladores e templates, todos permanecem.

Neste capítulo, vamos percorrer as diferenças entre o symfony1 e Symfony2.
Como você verá, muitas tarefas são abordadas de uma forma ligeiramente diferente. Você vai
apreciar estas pequenas diferenças pois elas promovem um código estável, previsível,
testável e desacoplado em suas aplicações Symfony2.

Então, sente e relaxe, pois vamos guiá-lo a partir de agora.

Estrutura de Diretórios
-----------------------

Ao olhar um projeto Symfony2 - por exemplo, o `Symfony2 Standard`_ -
você verá uma estrutura de diretórios muito diferente da encontrada no symfony1. 
As diferenças, no entanto, são um tanto superficiais.

O Diretório ``app/``
~~~~~~~~~~~~~~~~~~~~

No symfony1, o seu projeto tem uma ou mais aplicações, e cada uma fica localizada dentro
do diretório ``apps/`` (ex. ``apps/frontend``). Por padrão, no Symfony2,
você tem apenas uma aplicação representada pelo diretório ``app/``. Como 
no symfony1, o diretório ``app/`` contém a configuração específica para determinada
aplicação. Ele também contém os diretórios cache, log e template
específicos para a aplicação, bem como, uma classe ``Kernel`` (``AppKernel``), que é o 
objeto base que representa a aplicação.

Ao contrário do symfony1, quase nenhum código PHP reside no diretório ``app/``. Este
diretório não pretende armazenar os módulos ou arquivos de biblioteca como acontece no symfony1.
Em vez disso, ele é simplesmente o lar da configuração e outros recursos (templates,
arquivos de tradução).

O Diretório ``src/``
~~~~~~~~~~~~~~~~~~~~

Simplificando, o seu código fica armazendo aqui. No Symfony2, todo o código real da aplicação 
reside dentro de um bundle (equivalente a um plugin no symfony1) e, por padrão,
cada bundle reside dentro do diretório ``src``. Dessa forma, o diretório ``src``
é um pouco parecido com o diretório ``plugins`` do symfony1, mas muito mais
flexível. Além disso, enquanto os *seus* bundles vão residir no diretório ``src/``,
os bundles de terceiros podem residir no diretório ``vendor/bundles/``.

Para obter uma imagem melhor do diretório ``src/``, vamos primeiro pensar em uma
aplicação symfony1. Primeiro, parte do seu código provavelmente reside dentro de uma ou
mais aplicações. Mais geralmente estas incluem módulos, mas também podem incluir
quaisquer outras classes PHP que você adicionar na sua aplicação. Você também pode ter criado
um arquivo ``schema.yml`` no diretório ``config`` do seu projeto e construiu
vários arquivos de modelo. Finalmente, para ajudar com alguma funcionalidade comum, você está
usando vários plugins de terceiros que residem no diretório ``plugins/``.
Em outras palavras, o código que compôem a sua aplicação reside em vários locais 
diferentes.

No Symfony2, a vida é muito mais simples porque *todo* o código Symfony2 deve residir em
um bundle. Em nosso projeto symfony1, todo o código *pode* ser movido
em um ou mais plugins (o que, na verdade, é uma prática muito boa). Assumindo
que todos os módulos, classes PHP, esquema, configuração de roteamento, etc, foram movidos
para um plugin, o diretório ``plugins/`` do symfony1 seria muito semelhante
ao diretório ``src/`` do Symfony2.

Simplificando novamente, o diretório ``src/`` é onde o seu código, assets,
templates e mais qualquer outra coisa específica ao seu projeto, vai residir.

O Diretório ``vendor/``
~~~~~~~~~~~~~~~~~~~~~~~

O diretório ``vendor/`` é basicamente equivalente ao diretório
``lib/vendor/`` do symfony1, que foi o diretório convencional para todas as bibliotecas 
vendor e bundles. Por padrão, você vai encontrar os arquivos de biblioteca do Symfony2
nesse diretório, juntamente com várias outras bibliotecas dependentes, como Doctrine2,
Twig e SwiftMailer. Bundles Symfony2 de terceiros geralmente residem no 
``vendor/bundles/``.

O Diretório ``web/``
~~~~~~~~~~~~~~~~~~~~

Não mudou muita coisa no diretório ``web/``. A diferença mais notável é a ausência 
dos diretórios ``css/``, ``js/`` e ``images/``. Isto é intencional. Tal como o seu 
código PHP, todos os assets também devem residir dentro de um bundle. Com a ajuda 
de um comando do console, o diretório ``Resources/public/`` de cada pacote é copiado 
ou ligado simbolicamente ao diretório ``web/bundles/``. Isto permite-lhe manter os 
assets organizados dentro do seu bundle, mas, ainda torná-los disponíveis ao público. 
Para certificar-se de que todos os bundles estão disponíveis, execute o seguinte comando::

    php app/console assets:install web

.. note::

   Este comando é equivalente no Symfony2 ao comando ``plugin:publish-assets`` do 
   symfony1.

O Autoloading
-------------

Uma das vantagens dos frameworks modernos é nunca ter que preocupar-se em incluir 
arquivos. Ao fazer uso de um autoloader, você pode fazer referência à qualquer classe
em seu projeto e confiar que ela estará disponível. O autoloading mudou no Symfony2 
para ser mais universal, mais rápido e independente da necessidade de limpar o 
seu cache.

No symfony1, o autoloading é realizado pesquisando em todo o projeto pela presença 
de arquivos de classe PHP e realizando o cache desta informação em um array gigante.
Este array diz ao symfony1 exatamente qual arquivo continha cada classe. No ambiente 
de produção, isto faz com que você precise limpar o cache quando as classes
forem adicionadas ou movidas.

No Symfony2, uma nova classe - ``UniversalClassLoader`` - lida com esse processo.
A idéia por trás do autoloader é simples: o nome da sua classe (incluindo
o namespace) deve coincidir com o caminho para o arquivo que contém essa classe.
Considere, por exemplo, o ``FrameworkExtraBundle`` da Edição Standard do 
Symfony2::

    namespace Sensio\Bundle\FrameworkExtraBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;
    // ...

    class SensioFrameworkExtraBundle extends Bundle
    {
        // ...

O arquivo em si reside em 
``vendor/bundle/Sensio/Bundle/FrameworkExtraBundle/SensioFrameworkExtraBundle.php``.
Como você pode ver, a localização do arquivo segue o namespace da classe.
Especificamente, o namespace, ``Sensio\Bundle\FrameworkExtraBundle``, indica
o diretório que o arquivo deve residir em 
(``vendor/bundle/Sensio/Bundle/FrameworkExtraBundle``). Isto é porque, no
arquivo ``app/autoload.php``, você vai configurar o Symfony para procurar pelo namespace ``Sensio``
no diretório ``vendor/bundle``:

.. code-block:: php

    // app/autoload.php

    // ...
    $loader->registerNamespaces(array(
        // ...
        'Sensio'           => __DIR__.'/../vendor/bundles',
    ));

Se o arquivo *não* residir nesta localização exata, você receberá o seguinte
erro: ``Class "Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle" does not exist.``.
No Symfony2, a mensagem "classe não existe" significa que o namespace suspeito da classe 
e a sua localização física não correspondem. Basicamente, o Symfony2 estará procurando
em um local exato por esta classe, mas esse local não existe (ou contém uma classe 
diferente). A fim de que uma classe seja carregada automaticamente, você
**nunca precisará limpar o cache** no Symfony2.

Como mencionado anteriormente, para o autoloader funcionar, ele precisa saber que o
namespace ``Sensio`` reside no diretório ``vendor/bundles`` e que, por
exemplo, o namespace ``Doctrine`` reside no diretório ``vendor/doctrine/lib/``
. Este mapeamento é controlado inteiramente por você através do
arquivo ``app/autoload.php``.

Se você olhar o ``HelloController`` da Edição Standard do Symfony2 
poderá ver que ele reside no namespace ``Acme\DemoBundle\Controller``. Sim, o
namespace ``Acme`` não é definido no ``app/autoload.php``. Por padrão, você
não precisa configurar explicitamente o local dos bundles que residem no 
diretório ``src/``. O ``UniversalClassLoader`` está configurado para usar como alternativa
o diretório ``src/`` usando o seu método ``registerNamespaceFallbacks``:

.. code-block:: php

    // app/autoload.php

    // ...
    $loader->registerNamespaceFallbacks(array(
        __DIR__.'/../src',
    ));

Usando o Console
-----------------

No symfony1, o console está no diretório raiz do seu projeto e é
chamado ``symfony``:

.. code-block:: text

    php symfony

No Symfony2, o console está agora no sub-diretório app e é chamado
``console``:

.. code-block:: text

    php app/console

Aplicações
----------

Em um projeto symfony1, é comum ter várias aplicações: uma para o
frontend e uma para o backend, por exemplo.

Em um projeto Symfony2, você só precisa criar uma única aplicação (um aplicação
blog, uma aplicação intranet, ...). Na maioria das vezes, se você desejar
criar uma segunda aplicação, você pode, em vez, criar outro projeto e 
compartilhar alguns bundles entre elas.

E, se você precisa separar as funcionalidades do frontend e backend de alguns
bundles, você pode criar sub-namespaces para os controladores, sub-diretórios para
templates, diferentes configurações semânticas, configurações separadas de roteamento,
e assim por diante.

Claro, não há nada de errado em ter várias aplicações em seu projeto, isto depende
inteiramente de você. Uma segunda aplicação significaria um novo diretório, 
por exemplo: ``my_app/``, com a mesma configuração básica do diretório ``app/``.

.. tip::

    Leia a definição de :term:`Projeto`, :term:`Aplicação` e 
    :term:`Bundle` no glossário.

Bundles e Plugins
-----------------

Em um projeto symfony1, um plugin pode conter configuração, módulos, bibliotecas PHP,
assets e qualquer outra coisa relacionada ao seu projeto. No Symfony2, 
a idéia de um plugin é substituída pelo "bundle". Um bundle é ainda mais poderoso
do que um plugin porque o núcleo do framework Symfony2 é fornecido através de uma série
de bundles. No Symfony2, os bundles são cidadãos de primeira classe que são tão flexíveis
que mesmo o código do núcleo em si é um bundle.

No symfony1, um plugin deve ser ativado dentro da classe 
``ProjectConfiguration``::

    // config/ProjectConfiguration.class.php
    public function setup()
    {
        $this->enableAllPluginsExcept(array(/* some plugins here */));
    }

No Symfony2, os bundles são ativados dentro do kernel da aplicação::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            // ...
            new Acme\DemoBundle\AcmeDemoBundle(),
        );

        return $bundles;
    }

Roteamento (``routing.yml``) e configuração (``config.yml``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No symfony1, os arquivos de configuração ``routing.yml`` e ``app.yml`` são automaticamente 
carregados dentro de qualquer plugin. No Symfony2, o roteamento e a configuração da 
aplicação dentro de um bundle devem ser incluídos manualmente. Por exemplo, para
incluir um recurso de roteamento de um bundle chamado ``AcmeDemoBundle``, você pode
fazer o seguinte::

    # app/config/routing.yml
    _hello:
        resource: "@AcmeDemoBundle/Resources/config/routing.yml"

Isto irá carregar as rotas encontradas no arquivo ``Resources/config/routing.yml`` do 
``AcmeDemoBundle``. O especial ``@AcmeDemoBundle`` é uma sintaxe de atalho
que, internamente, resolve o caminho completo para esse bundle.

Você pode usar essa mesma estratégia para trazer a configuração de um bundle:

.. code-block:: yaml

    # app/config/config.yml
    imports:
        - { resource: "@AcmeDemoBundle/Resources/config/config.yml" }

No Symfony2, a configuração é um pouco semelhante ao ``app.yml`` do symfony1, exceto que é muito
mais sistemática. Com o ``app.yml``, você poderia simplesmente criar as chaves que desejava.
Por padrão, as entradas eram sem significado e dependia inteiramente de como você
utilizava em sua aplicação:

.. code-block:: yaml

    # some app.yml file from symfony1
    all:
      email:
        from_address:  foo.bar@example.com

No Symfony2, você também pode criar entradas arbitrárias sob a chave ``parameters``
de sua configuração:

.. code-block:: yaml

    parameters:
        email.from_address: foo.bar@example.com

Você pode agora acessar ele a partir de um controlador, por exemplo::

    public function helloAction($name)
    {
        $fromAddress = $this->container->getParameter('email.from_address');
    }

Na realidade, a configuração no Symfony2 é muito mais potente e é usada
principalmente para configurar objetos que você pode usar. Para maiores informações, 
visite o capítulo intitulado ":doc:`/book/service_container`".

.. _`Symfony2 Standard`: https://github.com/symfony/symfony-standard
