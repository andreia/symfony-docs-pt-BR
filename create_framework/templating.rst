Templates
=========

O leitor astuto percebeu que o nosso framework tem fixa no código a forma como um
"código" específico (os templates) é executado. Para páginas simples, como as que
criamos até agora, isso não é um problema, mas, se você quiser adicionar mais lógica,
seria forçado a adicioná-la no próprio template, o que, provavelmente, não é uma boa
idéia, especialmente se você ainda tem o princípio de separação de responsabilidades
em mente.

Vamos separar o código do template da lógica adicionando uma nova camada: o
controlador: *A missão do controlador é gerar uma Resposta com base na
informação transmitida pelo Pedido do cliente.*

Altere a parte de renderização do template do framework para a seguinte::

    // example.com/web/front.php

    // ...

    try {
        $request->attributes->add($matcher->match($request->getPathInfo()));
        $response = call_user_func('render_template', $request);
    } catch (Routing\Exception\ResourceNotFoundException $e) {
        $response = new Response('Not Found', 404);
    } catch (Exception $e) {
        $response = new Response('An error occurred', 500);
    }

Como a renderização é feita agora por uma função externa (aqui ``render_template()``
), precisamos passar para ela os atributos extraídos da URL. Poderíamos
passá-los como um argumento adicional para o ``render_template()``, mas
em vez disso, vamos usar uma outra funcionalidade da classe ``Request`` chamada
*attributes*: os atributos do ``Request`` permitem anexar informações adicionais sobre
o Pedido que não estão diretamente relacionadas aos dados da requisição HTTP.

Agora você pode criar a função ``render_template()``, um controlador genérico 
que renderiza um template quando não existe uma lógica específica. Para manter o mesmo
template de antes, os atributos do ``Request`` são extraídos antes do template ser
renderizado::

    function render_template($request)
    {
        extract($request->attributes->all(), EXTR_SKIP);
        ob_start();
        include sprintf(__DIR__.'/../src/pages/%s.php', $_route);

        return new Response(ob_get_clean());
    }

Sendo a ``render_template`` usada como um argumento para a função PHP ``call_user_func()``, 
podemos substituí-la por quaisquer `callbacks`_ PHP válidas. Isso nos permite
usar uma função, uma função anônima, ou um método de uma classe como um
controlador... a escolha é sua.

Como uma convenção, para cada rota, o controlador associado é configurado através 
do atributo de rota ``_controller``::

    $routes->add('hello', new Routing\Route('/hello/{name}', array(
        'name' => 'World',
        '_controller' => 'render_template',
    )));

    try {
        $request->attributes->add($matcher->match($request->getPathInfo()));
        $response = call_user_func($request->attributes->get('_controller'), $request);
    } catch (Routing\Exception\ResourceNotFoundException $e) {
        $response = new Response('Not Found', 404);
    } catch (Exception $e) {
        $response = new Response('An error occurred', 500);
    }

Uma rota pode ser associada agora a qualquer controlador e, claro, dentro de um
controlador, você ainda pode usar o ``render_template()`` para renderizar um template::

    $routes->add('hello', new Routing\Route('/hello/{name}', array(
        'name' => 'World',
        '_controller' => function ($request) {
            return render_template($request);
        }
    )));

Isto é bastante flexível pois, você pode alterar o objeto ``Response`` posteriormente, e,
até mesmo passar argumentos adicionais para o template::

    $routes->add('hello', new Routing\Route('/hello/{name}', array(
        'name' => 'World',
        '_controller' => function ($request) {
            // $foo will be available in the template
            $request->attributes->set('foo', 'bar');

            $response = render_template($request);

            // change some header
            $response->headers->set('Content-Type', 'text/plain');

            return $response;
        }
    )));

Aqui está a versão atualizada e melhorada do nosso framework::

    // example.com/web/front.php

    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing;

    function render_template($request)
    {
        extract($request->attributes->all(), EXTR_SKIP);
        ob_start();
        include sprintf(__DIR__.'/../src/pages/%s.php', $_route);

        return new Response(ob_get_clean());
    }

    $request = Request::createFromGlobals();
    $routes = include __DIR__.'/../src/app.php';

    $context = new Routing\RequestContext();
    $context->fromRequest($request);
    $matcher = new Routing\Matcher\UrlMatcher($routes, $context);

    try {
        $request->attributes->add($matcher->match($request->getPathInfo()));
        $response = call_user_func($request->attributes->get('_controller'), $request);
    } catch (Routing\Exception\ResourceNotFoundException $e) {
        $response = new Response('Not Found', 404);
    } catch (Exception $e) {
        $response = new Response('An error occurred', 500);
    }

    $response->send();

Para celebrar o nascimento do nosso novo framework, vamos criar uma nova aplicação
que precisa de alguma lógica simples. Nossa aplicação possui uma página que
diz se um determinado ano é bissexto ou não. Ao chamar
``/is_leap_year``, você tem a resposta para o ano corrente, mas, você também 
pode especificar um ano, como em ``/is_leap_year/2009``. Sendo genérico, o
framework não precisa ser modificado de qualquer forma, basta criar um novo
arquivo ``app.php``::

    // example.com/src/app.php

    use Symfony\Component\Routing;
    use Symfony\Component\HttpFoundation\Response;

    function is_leap_year($year = null) {
        if (null === $year) {
            $year = date('Y');
        }

        return 0 == $year % 400 || (0 == $year % 4 && 0 != $year % 100);
    }

    $routes = new Routing\RouteCollection();
    $routes->add('leap_year', new Routing\Route('/is_leap_year/{year}', array(
        'year' => null,
        '_controller' => function ($request) {
            if (is_leap_year($request->attributes->get('year'))) {
                return new Response('Yep, this is a leap year!');
            }

            return new Response('Nope, this is not a leap year.');
        }
    )));

    return $routes;

A função ``is_leap_year()`` retorna ``true`` quando o ano é bissexto, caso
contrário, retorna ``false``. Se o ano for nulo, o ano corrente é testado.
O controlador é simples: ele obtêm o ano a partir dos atributos do pedido,
passa eles para a função ``is_leap_year()``, e, de acordo com o valor retornado
é criado um novo objeto ``Response``.

Como sempre, você pode decidir parar aqui e usar o framework como ele está; é
provavelmente tudo o que você precisa para criar sites simples, como os `websites`_ 
fantasia de uma única página, e, esperamos que alguns outros.

.. _`callbacks`: http://php.net/callback#language.types.callback
.. _`websites`:  http://kottke.org/08/02/single-serving-sites
