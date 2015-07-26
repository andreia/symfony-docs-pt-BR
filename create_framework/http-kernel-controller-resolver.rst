O Componente HttpKernel: o Resolvedor do Controlador
====================================================

Você pode pensar que o nosso framework já está bastante sólido e, provavelmente, 
está certo. Mas, vamos ver como podemos melhorá-lo mesmo assim.

Até agora, todos os nossos exemplos usaram código procedural, mas, lembre-se que os controladores
podem ser qualquer callback PHP válido. Vamos converter nosso controlador para uma classe 
adequada::

    class LeapYearController
    {
        public function indexAction($request)
        {
            if (is_leap_year($request->attributes->get('year'))) {
                return new Response('Yep, this is a leap year!');
            }

            return new Response('Nope, this is not a leap year.');
        }
    }

Atualize a definição de rota, de acordo::

    $routes->add('leap_year', new Routing\Route('/is_leap_year/{year}', array(
        'year' => null,
        '_controller' => array(new LeapYearController(), 'indexAction'),
    )));

A modificação é bastante simples e faz muito sentido, logo que você
criar mais páginas, mas, você deve ter notado um ​​efeito colateral não-desejável...
A classe ``LeapYearController`` é *sempre* instanciada, mesmo se a
URL solicitada não corresponder a rota ``leap_year``. Isso é ruim, devido a um motivo 
principal: desempenho, todos os controladores para todas as rotas devem ser agora
instanciados para cada solicitação. Seria melhor se os controladores fossem
*lazy-loaded* de modo que, somente o controlador associado com a rota correspondente é
instanciado.

Para resolver esse problema e vários outros, vamos instalar e usar o componente 
HttpKernel::

.. code-block:: bash

    $ composer require symfony/http-kernel

O componente HttpKernel possui muitas funcionalidades interessantes, mas, a que precisamos
agora é o *resolvedor de controlador*. Um *resolvedor de controlador* sabe como determinar 
o controlador que deve ser executado e os argumentos que devem ser passados a ele, com base em
um objeto ``Request``. Todos os *resolvedores de controlador* implementam a seguinte interface::

    namespace Symfony\Component\HttpKernel\Controller;

    interface ControllerResolverInterface
    {
        function getController(Request $request);

        function getArguments(Request $request, $controller);
    }

O método ``getController()`` baseia-se na mesma convenção que definimos 
anteriormente: o atributo do pedido ``_controller`` deve conter o controlador 
associado ao ``Request``. Além dos callbacks PHP integrados, o ``getController()`` 
também suporta strings compostas por um nome de classe seguido de dois dois-pontos 
e um nome de método, como um callback válido, por exemplo 'class::method'::

    $routes->add('leap_year', new Routing\Route('/is_leap_year/{year}', array(
        'year' => null,
        '_controller' => 'LeapYearController::indexAction',
    )));

Para fazer este código funcionar, modifique o código do framework para usar o 
*resolvedor de controlador* a partir do HttpKernel::

    use Symfony\Component\HttpKernel;

    $resolver = new HttpKernel\Controller\ControllerResolver();

    $controller = $resolver->getController($request);
    $arguments = $resolver->getArguments($request, $controller);

    $response = call_user_func_array($controller, $arguments);

.. note::

    Como um bônus adicional, o *resolvedor de controlador* manipula apropriadamente 
    o gerenciamento de erros para você: por exemplo, quando você esquecer de definir 
    um atributo ``_controller`` para uma rota.

Agora, vamos ver como os argumentos do controlador são adivinhados. O ``getArguments()``
examinará a assinatura do controlador para determinar quais argumentos deve passar para ele
usando o `reflection`_ nativo do PHP.

O método ``indexAction()`` precisa do objeto ``Request`` como um argumento.
O ``getArguments()`` sabe quando injetá-lo adequadamente se sua indução de tipo
estiver correta::

    public function indexAction(Request $request)

    // não funciona
    public function indexAction($request)

Mais interessante, o ``getArguments()`` também é capaz de injetar qualquer atributo 
do ``Request``; o argumento só precisa ter o mesmo nome do atributo
correspondente::

    public function indexAction($year)

Você também pode injetar o ``Request`` e alguns atributos ao mesmo tempo (como a
correspondência é feita utilizando o nome do argumento ou uma indução de tipo, a ordem dos argumentos
não importa)::

    public function indexAction(Request $request, $year)

    public function indexAction($year, Request $request)

Finalmente, você também pode definir valores padrão para qualquer argumento que corresponda a um
atributo opcional do ``Request``::

    public function indexAction($year = 2012)

Vamos apenas injetar o atributo ``$year`` do pedido para o nosso controlador::

    class LeapYearController
    {
        public function indexAction($year)
        {
            if (is_leap_year($year)) {
                return new Response('Yep, this is a leap year!');
            }

            return new Response('Nope, this is not a leap year.');
        }
    }

O *resolvedor de controlador* também se encarrega de validar o ``callable`` do controlador 
e seus argumentos. No caso de um problema, ele gera uma exceção com uma agradável
mensagem explicando o problema (a classe do controlador não existe, o
método não está definido, um argumento não possui um atributo correspondente, ...).

.. note::

    Com a grande flexibilidade do *resolvedor de controlador* padrão, você pode
    perguntar por que alguém iria desejar criar outro (por que haveria uma
    interface?). Dois exemplos: no Symfony2, o ``getController()`` é
    aprimorado para suportar `controladores como serviços`_; e no
    `FrameworkExtraBundle`_, o ``getArguments()`` é aprimorado para suportar
    conversores de parâmetros, onde os atributos do pedido são convertidos
    automaticamente em objetos.

Vamos concluir com a nova versão do nosso framework::

    // example.com/web/front.php

    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing;
    use Symfony\Component\HttpKernel;

    function render_template(Request $request)
    {
        extract($request->attributes->all());
        ob_start();
        include sprintf(__DIR__.'/../src/pages/%s.php', $_route);

        return new Response(ob_get_clean());
    }

    $request = Request::createFromGlobals();
    $routes = include __DIR__.'/../src/app.php';

    $context = new Routing\RequestContext();
    $context->fromRequest($request);
    $matcher = new Routing\Matcher\UrlMatcher($routes, $context);
    $resolver = new HttpKernel\Controller\ControllerResolver();

    try {
        $request->attributes->add($matcher->match($request->getPathInfo()));

        $controller = $resolver->getController($request);
        $arguments = $resolver->getArguments($request, $controller);

        $response = call_user_func_array($controller, $arguments);
    } catch (Routing\Exception\ResourceNotFoundException $e) {
        $response = new Response('Not Found', 404);
    } catch (Exception $e) {
        $response = new Response('An error occurred', 500);
    }

    $response->send();

Pense nisso mais uma vez: o nosso framework é mais robusto e mais flexível do que
nunca e ele ainda tem menos de 40 linhas de código.

.. _`reflection`:              http://php.net/reflection
.. _`FrameworkExtraBundle`:    http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`controladores como serviços`: http://symfony.com/doc/current/cookbook/controller/service.html
