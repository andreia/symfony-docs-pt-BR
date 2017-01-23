O Componente HttpKernel: A Classe HttpKernel
============================================

Se você fosse usar o nosso framework agora, provavelmente teria que adicionar suporte 
para mensagens de erro personalizadas. Nesse momento, temos suporte aos erros 404 e 500,
mas, as respostas estão fixas no próprio framework. Torná-las personalizáveis
é bastante fácil: basta *despachar* um novo evento e ouvi-lo.
Fazê-lo corretamente significa que o *listener* precisa chamar um controlador regular.
Entretanto, e se o controlador de erro gerar uma exceção? Você vai acabar em um
*loop* infinito. Deve haver uma maneira mais fácil, certo?

Conheça a classe ``HttpKernel``. Em vez de resolver o mesmo problema 
mais uma vez e, em vez de reinventar a roda toda vez, a classe
``HttpKernel`` é uma implementação genérica, extensível e flexível da
``HttpKernelInterface``.

Essa classe é muito semelhante a classe framework que escrevemos até agora: ela
*despacha* eventos em alguns pontos estratégicos durante o manuseio do pedido,
ela usa um resolvedor de controlador para escolher o controlador para o qual irá 
*despachar* o pedido, e, como bônus, ela cuida de *edge cases* e fornece um ótimo 
*feedback* quando surge um problema.

Aqui está o novo código do framework::

    // example.com/src/Simplex/Framework.php

    namespace Simplex;

    use Symfony\Component\HttpKernel\HttpKernel;

    class Framework extends HttpKernel
    {
    }

E o novo *front controller*::

    // example.com/web/front.php

    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing;
    use Symfony\Component\HttpKernel;
    use Symfony\Component\EventDispatcher\EventDispatcher;

    $request = Request::createFromGlobals();
    $routes = include __DIR__.'/../src/app.php';

    $context = new Routing\RequestContext();
    $matcher = new Routing\Matcher\UrlMatcher($routes, $context);
    $resolver = new HttpKernel\Controller\ControllerResolver();

    $dispatcher = new EventDispatcher();
    $dispatcher->addSubscriber(new HttpKernel\EventListener\RouterListener($matcher));

    $framework = new Simplex\Framework($dispatcher, $resolver);

    $response = $framework->handle($request);
    $response->send();

O ``RouterListener`` é uma implementação da mesma lógica que tínhamos em nosso
framework: ele verifica o pedido de entrada corresponde e popula os atributos 
do pedido com parâmetros da rota.

Nosso código está agora muito mais conciso e surpreendentemente mais robusto e poderoso
do que nunca. Por exemplo, utilize o ``ExceptionListener`` embutido para tornar
o seu gerenciamento de erros configurável::

    $errorHandler = function (HttpKernel\Exception\FlattenException $exception) {
        $msg = 'Something went wrong! ('.$exception->getMessage().')';

        return new Response($msg, $exception->getStatusCode());
    };
    $dispatcher->addSubscriber(new HttpKernel\EventListener\ExceptionListener($errorHandler));

O ``ExceptionListener`` lhe fornece uma instância ``FlattenException`` ao invés da
instância ``Exception`` gerada para facilitar a manipulação de exceção e a exibição. Ele
pode usar qualquer controlador válido como um manipulador de exceção, assim, você pode 
criar uma classe *ErrorController* em vez de usar uma *Closure*::

    $listener = new HttpKernel\EventListener\ExceptionListener('Calendar\\Controller\\ErrorController::exceptionAction');
    $dispatcher->addSubscriber($listener);

O controlador de erro é o seguinte::

    // example.com/src/Calendar/Controller/ErrorController.php

    namespace Calendar\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\Exception\FlattenException;

    class ErrorController
    {
        public function exceptionAction(FlattenException $exception)
        {
            $msg = 'Something went wrong! ('.$exception->getMessage().')';

            return new Response($msg, $exception->getStatusCode());
        }
    }

Voilà! Gerenciamento de erro limpo e personalizável, sem esforços. E, é claro, no 
caso do seu controlador gerar uma exceção, o HttpKernel vai lidar com isso muito bem.

No capítulo dois, falamos sobre o método ``Response::prepare()``, que
garante que a Resposta é compatível com a especificação HTTP. É
provavelmente uma boa idéia sempre chamá-lo antes de enviar a resposta
ao cliente, e é exatamente isso o que o ``ResponseListener`` faz::

    $dispatcher->addSubscriber(new HttpKernel\EventListener\ResponseListener('UTF-8'));

Isto foi fácil demais! Vamos para outra: você quer suporte pronto para 
uso para stream das respostas? Basta fazer o *subscribe* no
``StreamedResponseListener``::

    $dispatcher->addSubscriber(new HttpKernel\EventListener\StreamedResponseListener());

E, em seu controlador, retornar uma instância ``StreamedResponse`` em vez de uma
instância ``Response``.

.. tip::

    Leia o capítulo `Internals`_ na documentação do Symfony para saber mais
    sobre os eventos *despachados* pelo HttpKernel e como eles permitem que você altere
    o fluxo de um pedido.

Agora, vamos criar um *listener*, que permite a um controlador retornar uma *string*
ao invés de um objeto ``Response`` completo::

    class LeapYearController
    {
        public function indexAction(Request $request, $year)
        {
            $leapyear = new LeapYear();
            if ($leapyear->isLeapYear($year)) {
                return 'Yep, this is a leap year! ';
            }

            return 'Nope, this is not a leap year.';
        }
    }

Para implementar esse recurso, vamos *ouvir* o evento ``kernel.view``, que é
acionado imediatamente após o controlador ter sido chamado. Seu objetivo é converter
o valor de retorno do controlador para a instância ``Response`` apropriada, mas,
apenas se necessário::

    // example.com/src/Simplex/StringResponseListener.php

    namespace Simplex;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\HttpKernel\Event\GetResponseForControllerResultEvent;
    use Symfony\Component\HttpFoundation\Response;

    class StringResponseListener implements EventSubscriberInterface
    {
        public function onView(GetResponseForControllerResultEvent $event)
        {
            $response = $event->getControllerResult();

            if (is_string($response)) {
                $event->setResponse(new Response($response));
            }
        }

        public static function getSubscribedEvents()
        {
            return array('kernel.view' => 'onView');
        }
    }

O código é simples, pois o evento ``kernel.view`` só é acionado quando o valor
de retorno do controlador não é ``Response`` e porque configurar a resposta
no evento pára a propagação do evento (o nosso *listener* não pode interferir com
outros *view listeners*).

Não se esqueça de registrá-lo no *front controller*::

    $dispatcher->addSubscriber(new Simplex\StringResponseListener());

.. note::

    Se você esquecer de registrar o *subscriber*, o HttpKernel irá lançar uma
    exceção, com uma bela mensagem: ``The controller must return a response
    (Nope, this is not a leap year. given).``.

Nesse ponto, todo o código do nosso framework é o mais compacto possível e é
composto principalmente da montagem de bibliotecas existentes. Estender é apenas
uma questão de registrar *listeners/subscribers* de eventos.

Esperamos que agora você tenha uma melhor compreensão do por que um simples olhar na
``HttpKernelInterface`` é tão poderoso. Sua implementação padrão, ``HttpKernel``, 
fornece acesso a uma série de recursos interessantes, prontos para serem usados,
sem esforços. E, devido ao HttpKernel ser, na verdade, o código que alimenta os 
frameworks Symfony e Silex, você tem o melhor de ambos os mundos: um framework personalizado, 
adaptado às suas necessidades, mas com base em uma arquitetura de baixo nível sólida
e com boa manutenção que provou funcionar em muitos sites; um código 
que foi auditado em questões relativas à segurança e que provou
ser bem escalável. 

.. _`Internals`: http://symfony.com/doc/current/book/internals.html#events
