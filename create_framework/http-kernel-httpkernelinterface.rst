O Componente HttpKernel: HttpKernelInterface
============================================

Na conclusão da segunda parte deste livro falei sobre um grande benefício
do uso dos componentes do Symfony: a *interoperabilidade* entre todos os
frameworks e aplicações que os utilizam. Vamos dar um grande passo neste
sentido fazendo o nosso framework implementar a ``HttpKernelInterface``::

    namespace Symfony\Component\HttpKernel;

    interface HttpKernelInterface
    {
        /**
         * @return Response A Response instance
         */
        function handle(Request $request, $type = self::MASTER_REQUEST, $catch = true);
    }

``HttpKernelInterface`` é, provavelmente, o trecho de código mais importante no
componente HttpKernel, sem brincadeira. Frameworks e aplicações que implementam
essa interface são completamente interoperáveis. Além disso, vários recursos
incríveis virão com ela, de graça.

Atualize seu framework para que ele implemente essa interface::

    // example.com/src/Framework.php

    // ...

    use Symfony\Component\HttpKernel\HttpKernelInterface;

    class Framework implements HttpKernelInterface
    {
        // ...

        public function handle(Request $request, $type = HttpKernelInterface::MASTER_REQUEST, $catch = true)
        {
            // ...
        }
    }

Mesmo essa mudança parecendo trivial, ela nos traz muito! Vamos falar sobre uma das
funcionalidades mais impressionantes: suporte à `HTTP caching`_ transparente.

A classe ``HttpCache`` implementa um proxy reverso cheio de recursos, escrito em
PHP; ele implementa a ``HttpKernelInterface`` e envolve uma outra
instância ``HttpKernelInterface``::

    // example.com/web/front.php

    $framework = new Simplex\Framework($dispatcher, $matcher, $resolver);
    $framework = new HttpKernel\HttpCache\HttpCache($framework, new HttpKernel\HttpCache\Store(__DIR__.'/../cache'));

    $framework->handle($request)->send();

Isso é tudo que precisa para adicionar suporte a cache HTTP no nosso framework. Não é
surpreendente?

A configuração do cache precisa ser feita através de cabeçalhos HTTP cache. Por exemplo,
para armazenar em cache uma resposta por 10 segundos, use o método ``Response::setTtl()``::

    // example.com/src/Calendar/Controller/LeapYearController.php

    public function indexAction(Request $request, $year)
    {
        $leapyear = new LeapYear();
        if ($leapyear->isLeapYear($year)) {
            $response = new Response('Yep, this is a leap year!');
        } else {
            $response = new Response('Nope, this is not a leap year.');
        }

        $response->setTtl(10);

        return $response;
    }

.. tip::

    Se, como eu, você está executando o framework a partir da linha de comando
    simulando pedidos (``Request::create('/is_leap_year/2012')``), você pode
    depurar facilmente instâncias ``Response`` através do *dump* de sua representação string
    (``echo $response;``) já que, ele exibe todos os cabeçalhos bem como o conteúdo da
    resposta.

Para validar se está funcionando corretamente, adicione um número aleatório ao conteúdo
da resposta e verifique se o número só muda a cada 10 segundos::

     $response = new Response('Yep, this is a leap year! '.rand());

.. note::

    Ao implantar em seu ambiente de produção, continue usando o proxy reverso do 
    Symfony (ótimo para hospedagem compartilhada) ou, ainda melhor, mude para um 
    proxy reverso mais eficiente como o `Varnish`_.

O uso de cabeçalhos HTTP Cache para gerenciar o cache da sua aplicação é muito poderoso e
permite ajustar finamente a sua estratégia de cache, pois, você pode usar tanto o
modelo ``expiration`` quanto o ``validation`` da especificação HTTP. Se não estiver
confortável com esses conceitos, leia o capítulo `HTTP caching`_ na documentação
do Symfony.

A classe ``Response`` contém muitos outros métodos que permitem configurar o
cache HTTP com muita facilidade. Um dos mais poderosos é o ``setCache()`` uma vez que ele
abstrai as estratégias mais utilizadas de cache em um simples array::

    $date = date_create_from_format('Y-m-d H:i:s', '2005-10-15 10:00:00');

    $response->setCache(array(
        'public'        => true,
        'etag'          => 'abcde',
        'last_modified' => $date,
        'max_age'       => 10,
        's_maxage'      => 10,
    ));

    // it is equivalent to the following code
    $response->setPublic();
    $response->setEtag('abcde');
    $response->setLastModified($date);
    $response->setMaxAge(10);
    $response->setSharedMaxAge(10);

Ao usar o modelo de validação, o método ``isNotModified()`` permite que você
facilmente corte o tempo de resposta gerando a mesma o mais cedo
possível::

    $response->setETag('whatever_you_compute_as_an_etag');

    if ($response->isNotModified($request)) {
        return $response;
    }
    $response->setContent('The computed content of the response');

    return $response;

Usar o cache HTTP é ótimo, mas, e se você não pode armazenar em cache a página inteira? E se
você pode armazenar em cache tudo, menos algumas barras laterais que são mais dinâmicas que o
resto do conteúdo? Temos o *Edge Side Includes* (`ESI`_) para nos socorrer! Em vez de
gerar todo o conteúdo de uma só vez, o ESI permite que você marque uma região de uma
página como sendo o conteúdo da chamada de um sub-pedido::

    Este é o conteúdo de sua página

    2012 é um ano bissexto? <esi:include src="/leapyear/2012" />

    Algum outro conteúdo

Para as tags ESI serem suportadas pelo HttpCache, você precisa passar uma instância da
classe ``ESI``. A classe ``ESI`` automaticamente realiza o *parse* das tags ESI e faz
sub-pedidos para convertê-las em seu conteúdo apropriado::

    $framework = new HttpKernel\HttpCache\HttpCache(
        $framework,
        new HttpKernel\HttpCache\Store(__DIR__.'/../cache'),
        new HttpKernel\HttpCache\ESI()
    );

.. note::

    Para o ESI funcionar, você precisa usar um proxy reverso que o suporte, como a
    implementação do Symfony. `Varnish`_ é a melhor alternativa e é
    Open-Source.

Ao utilizar estratégias complexas de cache HTTP e/ou muitas *include tags ESI*, pode 
ser difícil entender por que e quando um recurso deve ser armazenado em cache ou não. Para
depurar facilmente, você pode ativar o modo de depuração::

    $framework = new HttpCache($framework, new Store(__DIR__.'/../cache'), new ESI(), array('debug' => true));

O modo de depuração acrescenta um cabeçalho ``X-Symfony-Cache`` em cada resposta que
descreve o que a camada de cache fez:

.. code-block:: text

    X-Symfony-Cache:  GET /is_leap_year/2012: stale, invalid, store

    X-Symfony-Cache:  GET /is_leap_year/2012: fresh

O HttpCache tem muitas funcionalidades, uma delas é o suporte as
extensões HTTP Cache-Control ``stale-while-revalidate`` e ``stale-if-error``
como definidas no RFC 5861.

Com a adição de uma única interface, o nosso framework agora pode se beneficiar
de muitas funcionalidades incorporadas no componente HttpKernel; sendo o cache HTTP apenas
uma delas, porém importante, pois ele pode fazer a suas aplicações voarem!

.. _`HTTP caching`: http://symfony.com/doc/current/book/http_cache.html
.. _`ESI`:          http://en.wikipedia.org/wiki/Edge_Side_Includes
.. _`Varnish`:      https://www.varnish-cache.org/
