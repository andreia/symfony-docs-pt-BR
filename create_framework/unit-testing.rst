Testes Unitários
================

Você deve ter notado alguns bugs sutis mas ainda assim importantes
no framework que construímos ontem. Ao criar um framework, você deve ter certeza
de que ele funciona como anunciado. Se não, todas as aplicações que o utilizam como base
apresentarão os mesmos bugs. A boa notícia é que, sempre que você corrigir um bug,
estará corrigindo várias aplicações também.

A missão de hoje é escrever testes unitários para o framework que criamos
usando o `PHPUnit`_. Crie um arquivo de configuração PHPUnit em
``example.com/phpunit.xml.dist``:

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8"?>

    <phpunit backupGlobals="false"
             backupStaticAttributes="false"
             colors="true"
             convertErrorsToExceptions="true"
             convertNoticesToExceptions="true"
             convertWarningsToExceptions="true"
             processIsolation="false"
             stopOnFailure="false"
             syntaxCheck="false"
             bootstrap="vendor/autoload.php"
    >
        <testsuites>
            <testsuite name="Test Suite">
                <directory>./tests</directory>
            </testsuite>
        </testsuites>
    </phpunit>

Essa configuração define padrões sensíveis para a maioria das configurações do PHPUnit; mais
interessante, o ``autoloader`` é usado para inicializar os testes, e os testes serão
armazenados no diretório ``example.com/tests/``.

Agora, vamos escrever um teste para recursos "não encontrados". Para evitar a criação de
todas as dependências ao escrever os testes e para apenas realizar testes unitários do que
realmente queremos, vamos usar o `test doubles`_. *Test doubles* são mais fáceis de
criar quando contamos com *interfaces* ao invés de classes concretas. Felizmente, o Symfony2
fornece essas *interfaces* para os objetos *core* como *URL matcher* e o resolvedor do
controlador. Modifique o framework para usá-las::

    // example.com/src/Simplex/Framework.php

    namespace Simplex;

    // ...

    use Symfony\Component\Routing\Matcher\UrlMatcherInterface;
    use Symfony\Component\HttpKernel\Controller\ControllerResolverInterface;

    class Framework
    {
        protected $matcher;
        protected $resolver;

        public function __construct(UrlMatcherInterface $matcher, ControllerResolverInterface $resolver)
        {
            $this->matcher = $matcher;
            $this->resolver = $resolver;
        }

        // ...
    }

Agora, estamos prontos para escrever nosso primeiro teste::

    // example.com/tests/Simplex/Tests/FrameworkTest.php

    namespace Simplex\Tests;

    use Simplex\Framework;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Routing\Exception\ResourceNotFoundException;

    class FrameworkTest extends \PHPUnit_Framework_TestCase
    {
        public function testNotFoundHandling()
        {
            $framework = $this->getFrameworkForException(new ResourceNotFoundException());

            $response = $framework->handle(new Request());

            $this->assertEquals(404, $response->getStatusCode());
        }

        protected function getFrameworkForException($exception)
        {
            $matcher = $this->getMock('Symfony\Component\Routing\Matcher\UrlMatcherInterface');
            $matcher
                ->expects($this->once())
                ->method('match')
                ->will($this->throwException($exception))
            ;
            $matcher
                ->expects($this->once())
                ->method('getContext')
                ->will($this->returnValue($this->getMock('Symfony\Component\Routing\RequestContext')))
            ;
            $resolver = $this->getMock('Symfony\Component\HttpKernel\Controller\ControllerResolverInterface');

            return new Framework($matcher, $resolver);
        }
    }

Esse teste simula um pedido que não corresponde a nenhuma rota. Como tal, o
método ``match()`` retorna uma exceção ``ResourceNotFoundException`` e estamos
testando se o nosso framework converte essa exceção para uma resposta 404.

Executar esse teste é tão simples quanto executar ``phpunit`` no
diretório ``example.com``:

.. code-block:: bash

    $ phpunit

.. note::

    Caso você não entendeu entender o que está acontecendo no código,
    leia a documentação do PHPUnit em `test doubles`_.

Após o teste executar, você deverá ver uma barra verde. Cas contrário, você tem um bug
no teste ou no código do framework!

Adicionar um teste unitário para qualquer exceção lançada em um controlador é bem fácil::

    public function testErrorHandling()
    {
        $framework = $this->getFrameworkForException(new \RuntimeException());

        $response = $framework->handle(new Request());

        $this->assertEquals(500, $response->getStatusCode());
    }

Por último, mas não menos importante, vamos escrever um teste para quando realmente tivermos uma Resposta
adequada::

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\Controller\ControllerResolver;

    public function testControllerResponse()
    {
        $matcher = $this->getMock('Symfony\Component\Routing\Matcher\UrlMatcherInterface');
        $matcher
            ->expects($this->once())
            ->method('match')
            ->will($this->returnValue(array(
                '_route' => 'foo',
                'name' => 'Fabien',
                '_controller' => function ($name) {
                    return new Response('Hello '.$name);
                }
            )))
        ;
        $resolver = new ControllerResolver();

        $framework = new Framework($matcher, $resolver);

        $response = $framework->handle(new Request());

        $this->assertEquals(200, $response->getStatusCode());
        $this->assertContains('Hello Fabien', $response->getContent());
    }

Nesse teste, simulamos uma rota que corresponde e retorna um controlador
simples. Verificamos que o status da resposta é 200 e que seu conteúdo é
o que nós definidos no controlador.

Para verificar se nós cobrimos todos os casos de uso possíveis, execute a funcionalidade 
*test coverage* do PHPUnit (você precisa habilitar o `XDebug`_ primeiro):

.. code-block:: bash

    $ phpunit --coverage-html=cov/

Abra ``example.com/cov/src_Simplex_Framework.php.html`` em um navegador e verifique
se todas as linhas para a classe Framework estão verdes (isso significa que elas foram
visitadas quando os testes foram executados).

Graças ao código orientado a objetos simples que escrevemos até agora, 
podemos escrever testes unitários para cobrir todos os casos de uso possíveis do nosso
framework; o *test doubles* garante que estamos realmente testando o nosso código e não
o código do Symfony.

Agora que estamos confiantes (novamente) sobre o código que escrevemos, podemos, com
segurança, pensar no próximo conjunto de funcionalidades que queremos adicionar ao nosso framework.

.. _`PHPUnit`:      http://www.phpunit.de/manual/current/en/index.html
.. _`test doubles`: http://www.phpunit.de/manual/current/en/test-doubles.html
.. _`XDebug`:       http://xdebug.org/
