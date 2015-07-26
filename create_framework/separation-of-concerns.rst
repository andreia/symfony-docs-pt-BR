Separação das Responsabilidades (Separation of Concerns)
========================================================

Um aspecto negativo do nosso framework atual é que, precisamos copiar e colar o
código em ``front.php`` toda vez que criamos um novo site. 40 linhas de código não é
muito, mas seria bom se pudéssemos envolver esse código em uma classe adequada.
Para citar apenas alguns benefícios, isso nos traria melhor *reusabilidade* e testes
mais fáceis.

Se você olhar atentamente o código, o ``front.php`` tem uma entrada, o
Pedido (``Request``), e uma saída, a Resposta (``Response``). Nossa classe do framework
seguirá esse princípio simples: a lógica será sobre a criação do ``Response`` associado a um
``Request``.

Vamos criar nosso próprio namespace para nosso framework: ``Simplex``. Mova a
lógica da manipulação do pedido para sua própria classe ``Simplex\\Framework``::

    // example.com/src/Simplex/Framework.php

    namespace Simplex;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing\Matcher\UrlMatcher;
    use Symfony\Component\Routing\Exception\ResourceNotFoundException;
    use Symfony\Component\HttpKernel\Controller\ControllerResolver;

    class Framework
    {
        protected $matcher;
        protected $resolver;

        public function __construct(UrlMatcher $matcher, ControllerResolver $resolver)
        {
            $this->matcher = $matcher;
            $this->resolver = $resolver;
        }

        public function handle(Request $request)
        {
            $this->matcher->getContext()->fromRequest($request);

            try {
                $request->attributes->add($this->matcher->match($request->getPathInfo()));

                $controller = $this->resolver->getController($request);
                $arguments = $this->resolver->getArguments($request, $controller);

                return call_user_func_array($controller, $arguments);
            } catch (ResourceNotFoundException $e) {
                return new Response('Not Found', 404);
            } catch (\Exception $e) {
                return new Response('An error occurred', 500);
            }
        }
    }

E, atualize ``example.com/web/front.php`` de acordo::

    // example.com/web/front.php

    // ...

    $request = Request::createFromGlobals();
    $routes = include __DIR__.'/../src/app.php';

    $context = new Routing\RequestContext();
    $matcher = new Routing\Matcher\UrlMatcher($routes, $context);
    $resolver = new HttpKernel\Controller\ControllerResolver();

    $framework = new Simplex\Framework($matcher, $resolver);
    $response = $framework->handle($request);

    $response->send();

Para concluir o refatoramento, vamos mover tudo, exceto a definição das rotas,
de ``example.com/src/app.php`` para outro namespace: ``Calendar``.

Para as classes definidas sob os namespaces ``Simplex`` e ``Calendar`` serem
carregadas automaticamente, atualize o arquivo ``composer.json``:

.. code-block:: javascript

    {
        "require": {
            "symfony/class-loader": "2.1.*",
            "symfony/http-foundation": "2.1.*",
            "symfony/routing": "2.1.*",
            "symfony/http-kernel": "2.1.*"
        },
        "autoload": {
            "psr-0": { "Simplex": "src/", "Calendar": "src/" }
        }
    }

.. note::

    Para o ``autoloader`` do Composer ser atualizado, execute ``composer update``.

Mova o controlador para ``Calendar\\Controller\\LeapYearController``::

    // example.com/src/Calendar/Controller/LeapYearController.php

    namespace Calendar\Controller;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Calendar\Model\LeapYear;

    class LeapYearController
    {
        public function indexAction(Request $request, $year)
        {
            $leapyear = new LeapYear();
            if ($leapyear->isLeapYear($year)) {
                return new Response('Yep, this is a leap year!');
            }

            return new Response('Nope, this is not a leap year.');
        }
    }

E, mova a função ``is_leap_year()`` para sua própria classe também::

    // example.com/src/Calendar/Model/LeapYear.php

    namespace Calendar\Model;

    class LeapYear
    {
        public function isLeapYear($year = null)
        {
            if (null === $year) {
                $year = date('Y');
            }

            return 0 == $year % 400 || (0 == $year % 4 && 0 != $year % 100);
        }
    }

Não se esqueça de atualizar o arquivo ``example.com/src/app.php`` de acordo::

    $routes->add('leap_year', new Routing\Route('/is_leap_year/{year}', array(
        'year' => null,
        '_controller' => 'Calendar\\Controller\\LeapYearController::indexAction',
    )));

Para resumir, aqui está o novo layout dos arquivos:

.. code-block:: text

    example.com
    ├── composer.json
    ├── composer.lock    
    ├── src
    │   ├── app.php
    │   └── Simplex
    │       └── Framework.php
    │   └── Calendar
    │       └── Controller
    │       │   └── LeapYearController.php
    │       └── Model
    │           └── LeapYear.php
    ├── vendor
    │   └── autoload.php
    └── web
        └── front.php

É isso! Nossa aplicação agora possui quatro camadas diferentes e cada uma delas tem
um objetivo bem definido:

* ``web/front.php``: O ``front controller``; o único código PHP exposto que
  faz a interface com o cliente (ele recebe o Pedido e envia a Resposta) e
  fornece o código padrão para inicializar o framework e
  a nossa aplicação;

* ``src/Simplex``: O código reutilizável do framework que abstrai a manipulação de
  dos Pedidos de entrada (a propósito, torna os seus controladores/templates facilmente
  testáveis - veremos sobre isso mais tarde);

* ``src/Calendar``: Nosso código específico da aplicação (os controladores e o
  modelo);

* ``src/app.php``: A configuração da aplicação/customização do framework.
