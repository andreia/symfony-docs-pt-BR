.. index::
   single: Testes

Testes
======

Sempre que você escrever uma nova linha de código, você também adiciona
potenciais novos bugs. Para construir aplicações melhores e mais confiáveis,
você deve testar seu código usando testes funcionais e unitários.

O Framework de testes PHPUnit
-----------------------------

O Symfony2 se integra com uma biblioteca independente - chamada PHPUnit - 
para dar a você um rico framework de testes. Esse capitulo não vai abranger
o PHPUnit propriamente dito, mas ele tem a sua excelente documentação `documentation`_.

.. note::

    O Symfony2 funciona com o PHPUnit 3.5.11 ou posterior, embora a versão 3.6.4 é
    necessária para testar o código do núcleo do Symfony.

Cada teste - quer seja teste unitário ou teste funcional - é uma classe PHP 
que deve residir no sub-diretório  `Tests/` de seus bundles. Se você seguir 
essa regra, você pode executar todos os testes da sua aplicação com o 
seguinte comando:

.. code-block:: bash

    # espefifique o diretório de configuração na linha de comando
    $ phpunit -c app/

A opção ``-c`` diz para o PHPUnit procurar no diretório ``app/`` por um 
arquivo de configuração. Se você está curioso sobre as opções do PHPUnit,
dê uma olhada no arquivo ``app/phpunit.xml.dist``.

.. tip::

    O Code coverage pode ser gerado com a opção ``--coverage-html``.

.. index::
   single: Testes; Testes Unitários

Testes Unitários
----------------

Um teste unitário é geralmente um teste de uma classe PHP especifica. Se
você quer testar o comportamento global da sua aplicação, veja a seção sobre 
`Testes Funcionais`_.

Escrever testes unitários no Symfony2 não é nada diferente do que escrever um
teste unitário padrão do PHPUnit. Vamos supor que, por exemplo, você tem uma 
classe *incrivelmente* simples chamada ``Calculator`` no diretório ``Utility/``
do seu bundle::

    // src/Acme/DemoBundle/Utility/Calculator.php
    namespace Acme\DemoBundle\Utility;
    
    class Calculator
    {
        public function add($a, $b)
        {
            return $a + $b;
        }
    }

Para testar isso, crie um arquivo chamado ``CalculatorTest`` no diretório ``Tests/Utility``
do seu bundle::

    // src/Acme/DemoBundle/Tests/Utility/CalculatorTest.php
    namespace Acme\DemoBundle\Tests\Utility;

    use Acme\DemoBundle\Utility\Calculator;

    class CalculatorTest extends \PHPUnit_Framework_TestCase
    {
        public function testAdd()
        {
            $calc = new Calculator();
            $result = $calc->add(30, 12);

            // assert that our calculator added the numbers correctly!
            $this->assertEquals(42, $result);
        }
    }

.. note::

    Por convenção, o sub-diretório ``Tests/`` deve replicar o diretório do seu bundle.
    Então, se você estiver testando uma classe no diretório ``Utility/`` do seu bundle,
    coloque o teste no diretório ``Tests/Utility/``.

Assim como na rua aplicação verdadeira - o autoloading é automaticamente habilitado
via o arquivo ``bootstrap.php.cache`` (como configurado por padrão no arquivo 
``phpunit.xml.dist``).

Executar os testes para um determinado arquivo ou diretório também é muito fácil:

.. code-block:: bash

    # executa todos os testes no diretório Utility 
    $ phpunit -c app src/Acme/DemoBundle/Tests/Utility/

    # executa os testes para a classe Article
    $ phpunit -c app src/Acme/DemoBundle/Tests/Utility/CalculatorTest.php

    # executa todos os testes para todo o Bundle
    $ phpunit -c app src/Acme/DemoBundle/

.. index::
   single: Testes; Testes Funcionais

Testes Funcionais
-----------------

Testes funcionais verificam a integração das diferentes camadas de uma aplicação
(do roteamento as views). Eles não são diferentes dos testes unitários levando
em consideração o PHPUnit, mas eles tem um fluxo bem especifico:

* Fazer uma requisição;
* Testar a resposta;
* Clicar em um link ou submeter um formulário;
* Testar a resposta;
* Repetir a operação.

Seu Primeiro Teste Funcional
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Testes funcionais são arquivos PHP simples que estão tipicamente no diretório 
``Tests/Controller`` do seu bundle. Se você quer testar as páginas controladas 
pela  sua classe ``DemoController``, inicie criando um novo arquivo ``DemoControllerTest.php``
que extende a classe especial ``WebTestCase``.

Por exemplo, o Symfony2 Standard Edition fornece um teste funcional simples para
o ``DemoController`` (`DemoControllerTest`_) descrito assim::

    // src/Acme/DemoBundle/Tests/Controller/DemoControllerTest.php
    namespace Acme\DemoBundle\Tests\Controller;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class DemoControllerTest extends WebTestCase
    {
        public function testIndex()
        {
            $client = static::createClient();

            $crawler = $client->request('GET', '/demo/hello/Fabien');

            $this->assertTrue($crawler->filter('html:contains("Hello Fabien")')->count() > 0);
        }
    }

.. tip::

    Para executar seus testes funcionais, a classe ``WebTestCase`` class 
    inicializa o kernel da sua aplicação. Na maioria dos casos, isso 
    acontece automaticamente. Entretando, se o seu kernel está em um diretório
    diferente do padrão, você vai precisar modificar seu arquivo ``phpunit.xml.dist``
    para alterar a variável de ambiente ``KERNEL_DIR`` para o diretório do 
    seu kernel::

        <phpunit
            <!-- ... -->
            <php>
                <server name="KERNEL_DIR" value="/path/to/your/app/" />
            </php>
            <!-- ... -->
        </phpunit>

O método ``createClient()`` retorna um cliente, que é como um navegador que você
vai usar para navegar no seu site::

    $crawler = $client->request('GET', '/demo/hello/Fabien');

O método ``request()`` (veja :ref:`mais sobre o método request<book-testing-request-method-sidebar>`)
retorna um objeto :class:`Symfony\\Component\\DomCrawler\\Crawler` que pode ser
usado para selecionar um elemento na Response, clicar em links, e submeter formulários.

.. tip::

    O Crawler só funciona se a resposta é um documento XML ou HTML. 
    Para pegar a resposta bruta, use ``$client->getResponse()->getContent()``.

Clique em um link primeiramente selecionando-o com o Crawler usando uma expressão
XPath ou um seletor CSS, então use o Client para clicar nele. Por exemplo, o 
segunte código acha todos os links com o texto ``Greet``, então seleciona o 
segundo, e então clica nele::

    $link = $crawler->filter('a:contains("Greet")')->eq(1)->link();

    $crawler = $client->click($link);

Submeter um formulário é muito parecido, selecione um botão do formulário, 
opcionalmente sobrescreva alguns valores do formulário, então submeta-o::

    $form = $crawler->selectButton('submit')->form();

    // pega alguns valores
    $form['name'] = 'Lucas';
    $form['form_name[subject]'] = 'Hey there!';

    // submete o formulário
    $crawler = $client->submit($form);

.. tip::

    O formulário também pode manipular uploads e tem métodos para preencher diferentes
    tipos de campos (ex. ``select()`` e ``tick()``). Para mais detalhers, veja a seção
    `Forms`_ abaixo.

Agora que você pode facilmente navegar pela sua aplicação, use as afirmações para
testar que ela realmente faz o que você espera que ela faça. Use o Crawler para 
fazer afirmações no DOM::

    // Afirma que a resposta casa com um seletor informado
    $this->assertTrue($crawler->filter('h1')->count() > 0);

Ou, teste contra o conteúdo do Response diretamente se você só quer afirmar que
o conteudo contém algum texto ou se o Response não é um documento XML/HTML::

    $this->assertRegExp('/Hello Fabien/', $client->getResponse()->getContent());

.. _book-testing-request-method-sidebar:

.. sidebar:: Mais sobre o método ``request()``:

    A assinatura completa do método ``request()`` é::

        request(
            $method,
            $uri, 
            array $parameters = array(), 
            array $files = array(), 
            array $server = array(), 
            $content = null, 
            $changeHistory = true
        )

    O array ``server`` são valores brutos que você espera encontrar normalmente na
    variável superglobal do PHP `$_SERVER`_. Por exemplo, para setar os cabeçalhos 
    HTTP `Content-Type` e `Referer`, você passará o seguinte::

        $client->request(
            'GET',
            '/demo/hello/Fabien',
            array(),
            array(),
            array(
                'CONTENT_TYPE' => 'application/json',
                'HTTP_REFERER' => '/foo/bar',
            )
        );

.. index::
   single: Testes; Assertions

.. sidebar: Afirmações Úteis

    Para você começar mais rápido, aqui está uma lista com as afirmações
    mais comuns e úteis::

        // Afirma que tem exatamente uma tag h2 com a classe  "subtitle"
        $this->assertTrue($crawler->filter('h2.subtitle')->count() > 0);

        // Afirma que tem 4 tags h2 na página
        $this->assertEquals(4, $crawler->filter('h2')->count());

        // Afirma que o cabeçalho "Content-Type" é "application/json"
        $this->assertTrue($client->getResponse()->headers->contains('Content-Type', 'application/json'));

        // Afirma que o conteúdo da resposta casa com  a regexp.
        $this->assertRegExp('/foo/', $client->getResponse()->getContent());

        // Afirma que o código do status da resposta é 2xx
        $this->assertTrue($client->getResponse()->isSuccessful());
        // Afirma que o código do status da resposta é 404
        $this->assertTrue($client->getResponse()->isNotFound());
        // Afirma um especifico código 200
        $this->assertEquals(200, $client->getResponse()->getStatusCode());

        // Afirma que a resposta é um redirecionamento para /demo/contact
        $this->assertTrue($client->getResponse()->isRedirect('/demo/contact'));
        // ou simplesmente checa que a resposta redireciona para qualquer URL
        $this->assertTrue($client->getResponse()->isRedirect());

.. index::
   single: Testes; Client

Trabalhando com o Teste Client
------------------------------

O teste Client simula um cliente HTTP como um navegador e faz requisições na sua
aplicação Symfony2::

    $crawler = $client->request('GET', '/hello/Fabien');

O método ``request()`` pega o método HTTP e a URL como argumentos e retorna uma 
instancia de ``Crawler``.

Utilize o Crawler para encontrar elementos DOM no Response. Esses elementos podem
então ser usados para clicar em links e submeter formulários::

    $link = $crawler->selectLink('Go elsewhere...')->link();
    $crawler = $client->click($link);

    $form = $crawler->selectButton('validate')->form();
    $crawler = $client->submit($form, array('name' => 'Fabien'));

Os métodos ``click()`` e ``submit()`` retornam um objeto ``Crawler``.
Esses métodos são a melhor maneira de navegar na sua aplicação por tomarem
conta de várias coisas para você, como detectar o método HTTP de um formulário
e dar para você uma ótima API para upload de arquivos.

.. tip::

    Você vai aprende rmais sobre os objetos ``Link`` e ``Form`` na seção
    :ref:`Crawler<book-testing-crawler>` abaixo.

O método ``request`` pode também ser usado para simular submissões de formulários
diretamente ou fazer requisições mais complexas::

    // Submeter diretamente um formuário (mas utilizando o Crawler é mais fácil!)
    $client->request('POST', '/submit', array('name' => 'Fabien'));

    // Submissão de formulário com um upload de arquivo
    use Symfony\Component\HttpFoundation\File\UploadedFile;

    $photo = new UploadedFile(
        '/path/to/photo.jpg',
        'photo.jpg',
        'image/jpeg',
        123
    );
    // ou
    $photo = array(
        'tmp_name' => '/path/to/photo.jpg',
        'name' => 'photo.jpg',
        'type' => 'image/jpeg',
        'size' => 123,
        'error' => UPLOAD_ERR_OK
    );
    $client->request(
        'POST',
        '/submit',
        array('name' => 'Fabien'),
        array('photo' => $photo)
    );

    // Executa uma requisição de DELETE e passa os cabeçalhos HTTP
    $client->request(
        'DELETE',
        '/post/12',
        array(),
        array(),
        array('PHP_AUTH_USER' => 'username', 'PHP_AUTH_PW' => 'pa$$word')
    );

Por último mas não menos importante, você pode forçar cara requisição para ser 
executada em seu pŕoprio processo PHP para evitar qualquer efeito colateral quando
estiver trabalhando com vários clientes no mesmo script::

    $client->insulate();

Navegando
~~~~~~~~~

O Cliente suporta muitas operação que podem ser realizadas em um navegador real::

    $client->back();
    $client->forward();
    $client->reload();

    // Limpa todos os cookies e histórico
    $client->restart();

Acessando Objetos Internos
~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você usa o cliente para testar sua aplicação, você pode querer acessar os
objetos internos do cliente::

    $history   = $client->getHistory();
    $cookieJar = $client->getCookieJar();

Você também pode pegar os objetos relacionados a requisição mais recente::

    $request  = $client->getRequest();
    $response = $client->getResponse();
    $crawler  = $client->getCrawler();

Se as suas requisição não são isoladas, você pode também acessar o ``Container``
e o ``Kernel``::

    $container = $client->getContainer();
    $kernel    = $client->getKernel();

Acessando o Container
~~~~~~~~~~~~~~~~~~~~~

É altamente recomendado que um teste funcional teste somente o Response. Mas
em circunstancias extremamente raras, você pode querer acessar algum objeto 
interno para escrever afirmações. Nestes casos, você pode acessar o dependency
injection container::

    $container = $client->getContainer();

Esteja ciente que isso não funciona se você isolar o cliente ou se você usar
uma camada HTTP. Para ver a lista de serviços disponíves na sua aplicação, utilize
a task ``container:debug``.

.. tip::

    Se a informação que você precisa verificar está disponível no profiler, uso-o então

Acessando dados do Profiler
~~~~~~~~~~~~~~~~~~~~~~~~~~~

En cada requisição, o profiler do Symfony coleta e guarda uma grande quantidade de
dados sobre a manipulação interna de cada request. Por exemplo, o profiler pode ser
usado para verificar se uma determinada página executa menos consultas no banco 
quando estiver carregando.

Para acessar o Profiler da última requisição, faço o seguinte::

    $profile = $client->getProfile();

Para detalhes especificos de como usar o profiler dentro de um teste, seja o artigo
:doc:`/cookbook/testing/profiling` do cookbook.

Redirecionamento
~~~~~~~~~~~~~~~~

Quando uma requisição retornar uma redirecionamento como resposta, o cliente automaticamente
segue o redirecionamento. Se você quer examinar o Response antes do redirecionamento use o 
método ``followRedirects()``::

    $client->followRedirects(false);

Quando o cliente não segue os redirecionamentos, você pode forçar o redirecionamento com
o método ``followRedirect()``::

    $crawler = $client->followRedirect();

.. index::
   single: Testes; Crawler

.. _book-testing-crawler:

O Crawler
---------

Uma instancia do Crawler é retornada cada vez que você faz uma requisição com o Client.
Ele permite que você examinar documentos HTML, selecionar nós, encontrar links e 
formulários.

Examinando
~~~~~~~~~~

Como o jQuery, o Crawler tem metodos para examinar o DOM de um documento HTML/XML.
Por exemplo, isso encontra todos os elementos ``input[type=submit]``, seleciona o 
último da página, e então seleciona o elemento imediatamente acima dele::

    $newCrawler = $crawler->filter('input[type=submit]')
        ->last()
        ->parents()
        ->first()
    ;

Muitos outros métodos também estão disponíveis:

+------------------------+----------------------------------------------------+
| Metodos                | Descrição                                          |
+========================+====================================================+
| ``filter('h1.title')`` | Nós que casam com o seletor CSS                    |
+------------------------+----------------------------------------------------+
| ``filterXpath('h1')``  | Nós que casam com a expressão XPath                |
+------------------------+----------------------------------------------------+
| ``eq(1)``              | Nó para a posição especifica                       |
+------------------------+----------------------------------------------------+
| ``first()``            | Primeiro nó                                        |
+------------------------+----------------------------------------------------+
| ``last()``             | Último nó                                          |
+------------------------+----------------------------------------------------+
| ``siblings()``         | Irmãos                                             |
+------------------------+----------------------------------------------------+
| ``nextAll()``          | Todos os irmãos posteriores                        |
+------------------------+----------------------------------------------------+
| ``previousAll()``      | Todos os irmãos anteriores                         |
+------------------------+----------------------------------------------------+
| ``parents()``          | Nós de um nivel superior                           |
+------------------------+----------------------------------------------------+
| ``children()``         | Filhos                                             |
+------------------------+----------------------------------------------------+
| ``reduce($lambda)``    | Nós que a função não retorne false                 |
+------------------------+----------------------------------------------------+

Como cada um desses métodos retorna uma nova instância de ``Crawler``, você pode
restringir os nós selecionados encadeando a chamada de métodos::

    $crawler
        ->filter('h1')
        ->reduce(function ($node, $i)
        {
            if (!$node->getAttribute('class')) {
                return false;
            }
        })
        ->first();

.. tip::

    Utilize a função ``count()`` para pegar o número de nós armazenados no Crawler:
    ``count($crawler)``

Extraindo Informações
~~~~~~~~~~~~~~~~~~~~~

O Crawler pode extrair informações dos nós::

    // Retornar o valor do atributo para o primeiro nó
    $crawler->attr('class');

    // Retorna o valor do nó para o primeiro nó
    $crawler->text();

    // Extrai um array de atributos para todos os nós (_text retorna o valor do nó)
    // retorna um array para cara elemento no crawler, cara um com o valor e href
    $info = $crawler->extract(array('_text', 'href'));

    // Executa a lambda para cada nó e retorna um array de resultados
    $data = $crawler->each(function ($node, $i)
    {
        return $node->attr('href');
    });

Links
~~~~~

Para selecionar links, você pode usar os métodos acima ou o conveniente atalho
``selectLink()``::

    $crawler->selectLink('Click here');

Isso seleciona todos os links que contém o texto, ou imagens que o atributo ``alt``
contém o determinado texto. Como outros métodos de filtragem, esse retorna outro 
objeto ``Crawler``.

Uma vez selecionado um link, você pode ter acesso a um objeto especial ``Link``,
que tem métodos especificos muito úties para links (como ``getMethod()`` e 
``getUri()``). Para clicar no link, use o método do Client ``click()`` e passe
um objeto do tipo ``Link``::
    
    $link = $crawler->selectLink('Click here')->link();

    $client->click($link);

Formulários
~~~~~~~~~~~

Assim como nos links, você seleciona o form com o método ``selectButton()``::

    $buttonCrawlerNode = $crawler->selectButton('submit');

.. note::

    Note que selecionamos os botões do formulário e não os forms, pois o form pode
    ter vários botões; se você usar a API para examinar, tenha em mente que você deve
    procurar por um botão.

O método ``selectButton()`` pode selecionar tags ``button`` e submit tags ``input``.
Ele usa diversas partes diferentes do botão para encontrá-los:

* O atributo ``value``;

* O atributo ``id`` ou ``alt`` para imagens;

* O valor do atributo ``id`` ou ``name`` para tags ``button``.

Uma vez que você tenha o Crawler representanto um botão, chame o método ``form()`` 
para pegar a instancia de ``Form`` do form que envolve o nó do botão::

    $form = $buttonCrawlerNode->form();

Quando chamar o método ``form()``, você pode também passar uma array com valores
dos campos para sobreescrever os valores padrões::

    $form = $buttonCrawlerNode->form(array(
        'name'              => 'Fabien',
        'my_form[subject]'  => 'Symfony rocks!',
    ));

E se você quiser simular algum método HTTP especifico para o form, passe-o como um
segundo argumento::

    $form = $crawler->form(array(), 'DELETE');

O Client pode submeter instancias de ``Form``::

    $client->submit($form);

Os valores dos campos também posem ser passsados como um segundo argumento do 
método ``submit()``::

    $client->submit($form, array(
        'name'              => 'Fabien',
        'my_form[subject]'  => 'Symfony rocks!',
    ));

Para situações mais complexas, use a instancia de ``Form`` como um array para 
setar o valor de cada campo individualmente::

    // Muda o valor do campo
    $form['name'] = 'Fabien';
    $form['my_form[subject]'] = 'Symfony rocks!';

Também existe uma API para manipular os valores do campo de acordo com o seu tipo::

    // Seleciona um option ou um radio
    $form['country']->select('France');

    // Marca um checkbox
    $form['like_symfony']->tick();

    // Faz o upload de um arquivo
    $form['photo']->upload('/path/to/lucas.jpg');

.. tip::

    Você pode pegar os valores que serão submetidos chamando o método ``getValues()``
    no objeto ``Form``. Os arquivos do upload estão disponiveis em um array
    separado retornado por ``getFiles()``. Os métodos ``getPhpValues()`` e
    ``getPhpFiles()`` também retorna valores submetidos, mas no formato
    PHP (ele converte as chaves para a notação de colchetes - ex. 
    ``my_form[subject]`` - para PHP arrays).

.. index::
   pair: Testes; Configuração

Configuração de Testes
----------------------

O Client usado pelos testes funcionais cria um Kernel que roda em um ambiente
especial chamado ``test``. Uma vez que o Symfony carrega o ``app/config/config_test.yml``
no ambiente ``test``, você pode ajustar qualquer configuração de sua aplicação 
especificamente para testes.

Por exemplo, por padrão, o swiftmailer é configurado para *não* enviar realmente
os e-mails no ambiente ``test``. Você pode ver isso na opção de configuração
``swiftmailer``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_test.yml
        # ...

        swiftmailer:
            disable_delivery: true

    .. code-block:: xml

        <!-- app/config/config_test.xml -->
        <container>
            <!-- ... -->

            <swiftmailer:config disable-delivery="true" />
        </container>

    .. code-block:: php

        // app/config/config_test.php
        // ...

        $container->loadFromExtension('swiftmailer', array(
            'disable_delivery' => true
        ));

Você também pode usar um ambiente completamente diferente, ou sobrescrever
o modo de debug (``true``) passando cada um como uma opção para o método
``createClient()``::

    $client = static::createClient(array(
        'environment' => 'my_test_env',
        'debug'       => false,
    ));

Se a sua aplicação se comporta de acordo com alguns cabeçalhos HTTP, passe eles
como o segundo argumento de ``createClient()``::

    $client = static::createClient(array(), array(
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ));

Você também pode sobrescrever cabeçalhos HTTP numa base por requisições::

    $client->request('GET', '/', array(), array(), array(
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ));

.. tip::

    O cliente de testes está disponível como um serviço no container no ambiente
    ``teste`` (ou em qualquer lugar que a opção :ref:`framework.test<reference-framework-test>`
    esteja habilitada). Isso significa que você pode sobrescrever o serviço inteiramente
    se você precisar.

.. index::
   pair: PHPUnit; Configuração

Configuração do PHPUnit
~~~~~~~~~~~~~~~~~~~~~~~

Cada aplicação tem a sua própria configuração do PHPUnit, armazenada no arquivo
``phpunit.xml.dist``. Você pode editar o arquivo para mudar os valores padrões
ou criar um arquivo ``phpunit.xml``` para ajustar a configuração para sua máquina
local.

.. tip::

    Armazene o arquivo ``phpunit.xml.dist`` no seu repositório de códigos e ignore
    o arquivo ``phpunit.xml``.

Por padrão, somente os testes armazenados nos bundles "standard" são rodados
pelo comando ``phpunit`` (standard sendo os testes nos diretórios ``src/*/Bundle/Tests`` ou
``src/*/Bundle/*Bundle/Tests``) Mas você pode facilmente adicionar mais diretórios.
Por exemplo, a seguinte configuração adiciona os testes de um bundle de terceiros
instalado:

.. code-block:: xml

    <!-- hello/phpunit.xml.dist -->
    <testsuites>
        <testsuite name="Project Test Suite">
            <directory>../src/*/*Bundle/Tests</directory>
            <directory>../src/Acme/Bundle/*Bundle/Tests</directory>
        </testsuite>
    </testsuites>

Para incluir outros diretórios no code coverage, edite também a sessção ``<filter>``:

.. code-block:: xml

    <filter>
        <whitelist>
            <directory>../src</directory>
            <exclude>
                <directory>../src/*/*Bundle/Resources</directory>
                <directory>../src/*/*Bundle/Tests</directory>
                <directory>../src/Acme/Bundle/*Bundle/Resources</directory>
                <directory>../src/Acme/Bundle/*Bundle/Tests</directory>
            </exclude>
        </whitelist>
    </filter>

Aprenda mais
------------

.. toctree::
    :maxdepth: 1
    :glob:

    testing/*

* :doc:`O capítulo sobre testes nas Melhores Práticas do Framework Symfony </best_practices/tests>`
* :doc:`/components/dom_crawler`
* :doc:`/components/css_selector`

.. _`$_SERVER`: http://php.net/manual/en/reserved.variables.server.php
.. _`documentation`: https://phpunit.de/manual/current/en/
.. _`installed as PHAR`: https://phpunit.de/manual/current/en/installation.html#installation.phar
