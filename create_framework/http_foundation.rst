O Componente HttpFoundation
===========================

Antes de mergulhar no processo de criação do framework, primeiro vamos voltar um passo e 
verificar porque você gostaria de usar um framework em vez de manter as suas
aplicações no bom e velho PHP. Por que usar um framework é realmente uma boa
idéia, até mesmo para o mais simples trecho de código e porque criar o seu framework
utilizando os componentes do Symfony é melhor do que criar um framework do
zero.

.. note::

    Não vamos falar sobre os benefícios óbvios e tradicionais de usar um
    framework ao trabalhar em grandes aplicações com mais do que alguns
    desenvolvedores; a Internet já possui bons e abundantes recursos sobre
    esse tópico.

Mesmo sendo bastante simples a "aplicação" que nós escrevemos no capítulo anterior,
ela possui alguns problemas::

    // framework/index.php

    $input = $_GET['name'];

    printf('Hello %s', $input);

Primeiro, se o parâmetro de consulta ``name`` não for fornecido na query string da URL,
você receberá um aviso PHP, por isso, vamos corrigí-lo::

    // framework/index.php

    $input = isset($_GET['name']) ? $_GET['name'] : 'World';

    printf('Hello %s', $input);

Então, essa *aplicação não é segura*. Dá pra acreditar? Mesmo esse simples
trecho de código PHP é vulnerável a um dos problemas de segurança mais difundidos
na Internet, o XSS (Cross-Site Scripting). Aqui está uma versão mais segura::

    $input = isset($_GET['name']) ? $_GET['name'] : 'World';

    header('Content-Type: text/html; charset=utf-8');

    printf('Hello %s', htmlspecialchars($input, ENT_QUOTES, 'UTF-8'));

.. note::

    Como você deve ter notado, proteger o seu código com o ``htmlspecialchars`` é algo
    tedioso e propenso a erros. Essa é uma das razões pelas quais usar uma template
    engine como o `Twig`_, onde o auto-escaping é ativado por padrão, pode ser uma
    boa idéia (e escapar explicitamente é também menos penoso com o uso de um
    filtro simples ``e``).

Como você pode verificar por si mesmo, o código simples que tínhamos escrito anteriormente não é
mais simples, se quisermos evitar os avisos (warnings/notices) do PHP e tornar o código
mais seguro.

Além da segurança, esse código não é facilmente testável. Mesmo não havendo
muito para testar, parece-me que, escrever testes unitários para o mais simples possível
trecho de código PHP não é algo natural e parece desagradável. Aqui está uma tentativa de
teste unitário no PHPUnit para o código acima::

    // framework/test.php

    class IndexTest extends \PHPUnit_Framework_TestCase
    {
        public function testHello()
        {
            $_GET['name'] = 'Fabien';

            ob_start();
            include 'index.php';
            $content = ob_get_clean();

            $this->assertEquals('Hello Fabien', $content);
        }
    }

.. note::

    Se nossa aplicação for apenas um pouco maior, poderíamos 
    encontrar ainda mais problemas. Se você está curioso sobre eles, leia o capítulo `Symfony 
    versus o PHP puro`_ na documentação do Symfony.

Neste ponto, se você ainda não está convencido de que a segurança e os testes são de fato
duas boas razões para parar de escrever código na forma antiga e adotar um framework
no lugar (independentemente do que signifique adotar um framework neste contexto), você pode parar
de ler esse livro agora e voltar para qualquer código que você estava trabalhando antes.

.. note::

    É claro, usar um framework deverá fornecer mais do que apenas segurança e
    testabilidade, mas o mais importante para ter em mente é que o
    framework que você escolher deve permitir que você escreva código melhor e mais rápido.

Iniciando OOP com o Componente HttpFoundation
---------------------------------------------

Escrever código web significa interagir com o HTTP. Então, os princípios fundamentais
de nosso framework devem ser centrados em torno da `especificação
HTTP`_.

A especificação HTTP descreve como um cliente (um navegador, por exemplo)
interage com um servidor (a nossa aplicação através de um servidor web). O diálogo entre
o cliente e o servidor é especificado por *mensagens* bem definidas, pedidos e
e respostas: *o cliente envia um pedido para o servidor e, com base nesse
pedido, o servidor retorna uma resposta*.

No PHP, o pedido é representado por variáveis ​​globais (``$_GET``, ``$_POST``,
``$_FILE``, ``$_COOKIE``, ``$_SESSION``, ...) e a resposta é gerada por
funções (``echo``, ``header``, ``setcookie``, ...).

O primeiro passo para um código melhor é, provavelmente, usar uma abordagem Orientada a 
Objeto, que é o objetivo principal do componente HttpFoundation do Symfony:
substituindo as variáveis ​​globais e funções padrão do PHP por uma camada Orientada a 
Objeto.

Para usar esse componente, adicione-o como uma dependência do projeto:

.. code-block:: bash

    $ composer require symfony/http-foundation

Executar esse comando também irá baixar automaticamente o componente HttpFoundation
do Symfony e instalá-lo no sob o diretório ``vendor/``.

.. sidebar:: Autoloading de Classe

    Ao instalar uma nova dependência, o Composer também gera um arquivo
    ``vendor/autoload.php`` que permite facilmente o `autoload`_ de 
    qualquer classe. Sem o autoloading, seria necessário fazer o require do arquivo
    onde a classe é definida antes de poder usá-la. Mas graças ao
    `PSR-0`_, podemos deixar o Composer e o PHP fazer o trabalho braçal por nós.

Agora, vamos reescrever nossa aplicação usando as classes ``Request`` e
``Response``::

    // framework/index.php

    require_once __DIR__.'/vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    $request = Request::createFromGlobals();

    $input = $request->get('name', 'World');

    $response = new Response(sprintf('Hello %s', htmlspecialchars($input, ENT_QUOTES, 'UTF-8')));

    $response->send();

O método``createFromGlobals()`` cria um objeto ``Request`` com base nas
variáveis ​​globais atuais do PHP.

O método ``send()`` envia o objeto ``Response`` de volta para o cliente (que
primeiro exibe os cabeçalhos HTTP seguidos do conteúdo).

.. tip::

    Antes de chamar ``send()``, devemos acrescentar uma chamada para o
    método ``prepare()`` (``$response->prepare($request);``) para garantir que
    nossa Resposta é compatível com a especificação HTTP. Por exemplo, se
    chamarmos a página com o método ``HEAD``, ele teria removido
    o conteúdo da Resposta.

A principal diferença do código anterior é que você tem controle total das
mensagens HTTP. Você pode criar qualquer pedido que desejar e você é
responsável por enviar a resposta, sempre que considerar oportuno.

.. note::

    Não vamos definir explicitamente o cabeçalho ``Content-Type`` no código
    reescrito pois o charset padrão do objeto ``Response`` é ``UTF-8``.

Com a classe ``Request``, você tem todas as informações do pedido ao
seu alcance, graças a uma API simples e atraente::

    // A URI que está sendo solicitada (ex.: /about) menos quaisquer parâmetros de consulta (query)
    $request->getPathInfo();

    // recuperar as variáveis ​​GET e POST, respectivamente
    $request->query->get('foo');
    $request->request->get('bar', 'default value if bar does not exist');

    // recuperar as variáveis ​​SERVER
    $request->server->get('HTTP_HOST');

    // recuperar uma instância de UploadedFile identificado por foo
    $request->files->get('foo');

    // recuperar um valor de COOKIE
    $request->cookies->get('PHPSESSID');

    // recuperar um cabeçalho de pedido HTTP, com chaves normalizadas e minúsculas
    $request->headers->get('host');
    $request->headers->get('content_type');

    $request->getMethod();    // GET, POST, PUT, DELETE, HEAD
    $request->getLanguages(); // um array dos idiomas que o cliente aceita

Você também pode simular um pedido::

    $request = Request::create('/index.php?name=Fabien');

Com a classe ``Response``, você pode facilmente ajustar a resposta::

    $response = new Response();

    $response->setContent('Hello world!');
    $response->setStatusCode(200);
    $response->headers->set('Content-Type', 'text/html');

    // configure os cabeçalhos HTTP de cache
    $response->setMaxAge(10);

.. tip::

    Para debugar uma Resposta, altere o seu tipo para string, ela irá retornar a representação HTTP
    da resposta (cabeçalhos e conteúdo).

Por último, mas não menos importante, essas classes, como qualquer outra classe no código do 
Symfony, foram `auditadas`_ para verificação de problemas de segurança por uma empresa independente. E
sendo um projeto Open-Source também significa que muitos desenvolvedores, ao redor do
mundo, leram o código e já corrigiram potenciais problemas de segurança.
Quando foi a última vez que você encomendou uma auditoria de segurança profissional para o
seu framework caseiro?

Mesmo algo tão simples, como obter o endereço IP do cliente, pode ser inseguro::

    if ($myIp == $_SERVER['REMOTE_ADDR']) {
        // o cliente é conhecido, então, concede-se mais algum privilégio
    }

Ele funciona perfeitamente bem até você adicionar um proxy reverso na frente dos
servidores de produção; neste ponto, você terá que alterar o seu código para fazer
ele funcionar tanto na máquina de desenvolvimento (onde você não tem um proxy) quanto
nos seus servidores::

    if ($myIp == $_SERVER['HTTP_X_FORWARDED_FOR'] || $myIp == $_SERVER['REMOTE_ADDR']) {
        // o cliente é conhecido, então, concede-se mais algum privilégio
    }

Usando o método ``Request::getClientIp()`` lhe fornece o funcionamento 
correto desde o primeiro dia (e teria coberto também o caso onde você tem
proxies encadeados)::

    $request = Request::createFromGlobals();

    if ($myIp == $request->getClientIp()) {
        // o cliente é conhecido, então, concede-se mais algum privilégio
    }

E há um benefício adicional: é *seguro* por padrão. O que isso significa?
Não se pode confiar no valor de ``$_SERVER['HTTP_X_FORWARDED_FOR']``, uma vez que,
ele pode ser manipulado pelo usuário final, quando não há um proxy. Então, se você
estiver usando esse código em produção, sem um proxy, torna-se bem fácil 
abusar do seu sistema. Isso não é o caso do método ``getClientIp()`` pois
você deve confiar explicitamente nos seus proxies reversos chamando ``trustProxyData()``::

    Request::setTrustedProxies(array('10.0.0.1'));

    if ($myIp == $request->getClientIp(true)) {
        // the client is a known one, so give it some more privilege
    }

Assim, o método ``getClientIp()`` funciona com segurança em todas as circunstâncias. Você pode
usá-lo em todos os seus projetos, seja qual for a sua configuração, ele irá funcionar 
corretamente e com segurança. Esse é um dos objetivos do uso de um framework. Se você fosse
escrever um framework a partir do zero, teria que pensar em todos esses
casos por si mesmo. Porque então não usar uma tecnologia que já funciona?

.. note::

    Se você quiser saber mais sobre o componente HttpFoundation, pode olhar a
    API :namespace:`Symfony\\Component\\HttpFoundation` ou ler a
    :doc:`documentação </components/http_foundation/index>` dedicada.

Acredite ou não, mas temos o nosso primeiro framework. Você pode parar agora se quiser.
Apenas usando o componente HttpFoundation do Symfony já é possível escrever
código melhor e mais testável. Ele também permite que você escreva código mais rápido pois, muitos
dos problemas do dia-a-dia, já foram resolvidos para você.

De fato, projetos como o Drupal adotou o componente HttpFoundation;
se funciona para eles, provavelmente funcionará para você também.
Não reinvente a roda.

Quase me esqueci de falar sobre um benefício adicional: o uso do componente HttpFoundation
é o início de uma melhor interoperabilidade entre todos os frameworks e
aplicações que o utilizam (como ``Symfony`_, `Drupal 8`_, `phpBB 4`_, `ezPublish
5`_, `Laravel`_, `Silex`_, e `mais`_).

.. _`Twig`: http://twig.sensiolabs.com/
.. _`Symfony versus o PHP puro`: http://symfony.com/doc/current/book/from_flat_php_to_symfony2.html
.. _`HTTP specification`: http://tools.ietf.org/wg/httpbis/
.. _`audited`: http://symfony.com/blog/symfony2-security-audit
.. _`Symfony`: http://symfony.com/
.. _`Drupal 8`: http://drupal.org/
.. _`phpBB 4`: http://www.phpbb.com/
.. _`ezPublish 5`: http://ez.no/
.. _`Laravel`: http://laravel.com/
.. _`Silex`: http://silex.sensiolabs.org/
.. _`Midgard CMS`: http://www.midgard-project.org/
.. _`Zikula`: http://zikula.org/
.. _`autoloaded`: http://php.net/autoload
.. _`PSR-0`: https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-0.md
.. _`mais`: http://symfony.com/components/HttpFoundation
