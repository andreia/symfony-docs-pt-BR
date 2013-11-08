.. index::
single: Bastidores

Bastidores
=========

Parece que você quer entender como funciona o Symfony2 e como extendê-lo.
Isso me deixa muito feliz! Esta seção explica detalhadamente os bastidores
do Symfony2.

.. note::

    Você precisa ler esta seção apenas se quer entender como o Symfony2 funciona
    seu miolo, ou se quer extendê-lo.

Visão Geral
--------

O código do Symfony2 é feito de várias camadas independentes. Cada camada é
construída em cima da anterior.

.. tip::

    O Autoloading não é gerenciado diretamente pelo framework; isso é feito usando
    o autoloader do Composer (``vendor/autoload.php``), que é incluído no arquivo
    ``app/autoload.php``.

Componente ``HttpFoundation``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O nível mais profundo é o componente :namespace:`Symfony\\Component\\HttpFoundation`.
HttpFoundation fornece os principais objetos necessários para lidar com HTTP.
É um objeto que abstrai algumas funções nativas do PHP e variáveis:

* A classe :class:`Symfony\\Component\\HttpFoundation\\Request` abstrai as principais
  variáveis globais do PHP como: ``$_GET``, ``$_POST``, ``$_COOKIE``,
  ``$_FILES``, e ``$_SERVER``;

* A classe :class:`Symfony\\Component\\HttpFoundation\\Response` abstrai algumas
  funções do PHP como: ``header()``, ``setcookie()``, e ``echo``;

* A classe :class:`Symfony\\Component\\HttpFoundation\\Session` e a interface
  :class:`Symfony\\Component\\HttpFoundation\\SessionStorage\\SessionStorageInterface`
  abstraem o gerenciamento da sessão, com uso das funções ``session_*()``.

.. note::

    Leia mais sobre o :doc:`Componente HttpFoundation </components/http_foundation/introduction>`.

Componente ``HttpKernel``
~~~~~~~~~~~~~~~~~~~~~~~~

No topo do HttpFoundation está o componente :namespace:`Symfony\\Component\\HttpKernel`.
HttpKernel manuseia a parte dinâmica do HTTP; é um sutíl que envolve as classes
Request e Response para padronizar as formas como as requisições são manipuladas. Ele
também oferece pontos de extensão e ferramentas ideais para criar um framework Web
sem muito trabalho.

Também, opcionalmente, adiciona conigurabilidade e extensibilidade, obrigado ao
componente de Injeção de Dependência e um poderoso sistema de plugins (bundles).

.. seealso::

    Leia mais sobre o :doc:`Componente HttpKernel </components/http_kernel/introduction>`,
    :doc:`Dependency Injection </book/service_container>` e
    :doc:`Bundles </cookbook/bundles/best_practices>`.

O Bundle ``FrameworkBundle``
~~~~~~~~~~~~~~~~~~~~~~~~~~

O bundle :namespace:`Symfony\\Bundle\\FrameworkBundle` é um pacote que amarra os
principais componentes e bibliotecas para fazer um leve e rápido framework MVC. Ele
vem com uma configuração padrão e convenções para facilitar a curva de aprendizado.

.. index::
single: Bastidores; Kernel

Kernel
------

A classe :class:`Symfony\\Component\\HttpKernel\\HttpKernel` é a classe central
do Symfony2 e é responsável por tratar os peedidos do cliente. Seu principal objetivo
é "converter" um objeto :class:`Symfony\\Component\\HttpFoundation\\Request` para um
objeto :class:`Symfony\\Component\\HttpFoundation\\Response`.

Cada Kernel do Symfony2 implementa
:class:`Symfony\\Component\\HttpKernel\\HttpKernelInterface`::

    function handle(Request $request, $type = self::MASTER_REQUEST, $catch = true)

.. index::
single: Bastidores; Resolvedor do Controlador

Controladores
~~~~~~~~~~~

Para converter um Request para um Response, o Kernel se baseia em um "Controlador". Um
Controlador pode ser qualquer código PHP válido.

O Controlador deve ser uma implementação de
:class:`Symfony\\Component\\HttpKernel\\Controller\\ControllerResolverInterface`::

    public function getController(Request $request);

    public function getArguments(Request $request, $controller);

e o Kernel poderá executá-lo.

O método
:method:`Symfony\\Component\\HttpKernel\\Controller\\ControllerResolverInterface::getController`
retorna o Controlador (um código PHP) associado com o dado Request. A implementação padrão
(:class:`Symfony\\Component\\HttpKernel\\Controller\\ControllerResolver`) procura
pelo atributo ``_controller`` do pedido que representa o nome do controlador
(uma string "classs::método", como ``Bundle\BlogBundle\PostController:indexAction``).

.. tip::

    A implementação padrão utiliza o
    :class:`Symfony\\Bundle\\FrameworkBundle\\EventListener\\RouterListener`
    para definir o atributo ``_controller`` do Request (veja :ref:`kernel-core-request`).

O método
:method:`Symfony\\Component\\HttpKernel\\Controller\\ControllerResolverInterface::getArguments`
retorna um arrau de argumentos para passar para o Controlador. A implementação padrão
automaticamente resolve os argumentos dos métodos, baseado nos atributos do Request.

.. sidebar:: Correspondente os argumentos dos métodos do Controlador a partir dos
atributos do Request

    Para cada argumento, o Symfony2 tenta pegar o valor do atributo do Request com o
    mesmo nome. Se este não estiver definido, o valor do argumento padrão é usado se
    definido::

        // Symfony2 vai olhar para um atributo 'id' (obrigatório)
        // e um 'admin' (opcional)
        public function showAction($id, $admin = true)
        {
            // ...
        }

.. index::
single: Bastidores; Trantando os Requests

Trantando os Requests
~~~~~~~~~~~~~~~~~

O método :method:`Symfony\\Component\\HttpKernel\\HttpKernel::handle` pega um
``Request`` e *sempre* retorna um ``Response``. Para converter o ``Request``,
o ``handle()`` depende do Resolver e uma cadeia ordenada de notificações de eventos
(veja a próxima seção para mais informações sobre cada Event):

#. Antes de qualquer coisa, o evento ``kernel.request`` é notificado -- se um dos
   listeners retorna um ``Response``, pula diretamente para o passo 8;

#. O Resolver é chamado para determinar qual Controlador executar;

#. Os listeners do evento ``kernel.controller`` podem manipular o Controlador
   chamado como queiram (alterá-lo, envolvê-lo, ...);

#. O Kernel verifica se o Controlador é um código PHP válido;

#. O Resolver é chamado para determinar os argumentos para passar para o Controlador;

#. O Kernel chama o Controlador;

#. Se o Controlador não retorna um ``Response``, os listeners do evento
   ``kernel.view`` podem converter o valor de retorno do Controlador para um ``Response``;

#. Os listeners do evento ``kernel.response`` podem manipular o ``Response``
   (conteúdo e cabeçalho);

#. O Response é retornado.

Se uma Exception é lançada durante o processamento, o ``kernel.exception`` é notificado
e os listeners têm a oportundade de converter a Exception em um Response. Se isso
funcionar, o evento ``kernel.reponse`` é notificado; se não, a Exception é relançada.

Se você não quer ser pego por uma Exception (para pedidos embutido por exemplo),
desabilite o evento ``kernel.exception`` pasando ``false`` como o terceiro argumento
do método ``handle()``.

.. index::
single: Bastidores; Requests Internos

Requests Internos
~~~~~~~~~~~~~~~~~

A qualquer momento durante o tratamento de um pedido (o 'master'), um sub-pedido
pode ser manipulado. Você pode passar o tipo do pedido para o método ``handle()``
(segundo argumento):

* ``HttpKernelInterface::MASTER_REQUEST``;
* ``HttpKernelInterface::SUB_REQUEST``.

O tipo é passado para todos os eventos e os listeners podem agir de acordo (alguns
processamentos só devem ocorrer no pedido principal).

.. index::
pair: Kernel; Evento

Eventos
~~~~~~

Cada evento acionado pelo Kernel é uma subclasse do
:class:`Symfony\\Component\\HttpKernel\\Event\\KernelEvent`. Isto significa que cada
evento tem acesso para a mesma informação básica:

* :method:`Symfony\\Component\\HttpKernel\\Event\\KernelEvent::getRequestType` 
  - retorna o *tipo* do pedido (``HttpKernelInterface::MASTER_REQUEST``
  ou ``HttpKernelInterface::SUB_REQUEST``);

* :method:`Symfony\\Component\\HttpKernel\\Event\\KernelEvent::getKernel` 
  - retorna o Kernel manipulando o pedido;

* :method:`Symfony\\Component\\HttpKernel\\Event\\KernelEvent::getRequest` 
  - retorna o ``Request`` sendo manipulado.

``getRequestType()``
....................

O método ``getRequestType()`` permite os listeners para a saber o tipo do pedido.
Por exemple, se um listener só deve estar ativo para um pedidos principais, adicione
o seguinte código no início do seu método listener::

    use Symfony\Component\HttpKernel\HttpKernelInterface;

    if (HttpKernelInterface::MASTER_REQUEST !== $event->getRequestType()) {
        // return immediately
        return;
    }

.. tip::

    Se você ainda não está familiarizado com o Dispatcher de Evento do Symfony2, leia
    primeiro seção
    :doc:`Documentação do Compoenente Dispatcher de Evento </components/event_dispatcher/introduction>`
    .

.. index::
single: Evento; kernel.request

.. _kernel-core-request:

Evento ``kernel.request``
........................

*Classe do Evento*: :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseEvent`

O objetivo deste evento é retornar um objeto ``Response`` ou variáveis de configuração
de um Controlador podem ser chamadas depois do evento. Qualquer listener pode returnar
um objeto ``Reponse`` através do métod ``setResponse()`` no evento. Neste caso, todos
os outros listeners não serão chamados.

Este evento é usado pelo ``FrameworkBundle`` para popular o ``_controller`` do atributo
do ``Request``, através do
:class:`Symfony\\Bundle\\FrameworkBundle\\EventListener\\RouterListener`. RequestListener
usa um objeto  :class:`Symfony\\Component\\Routing\\RouterInterface` para combinar o
``Request`` e determinar o nome do Controlador (armazenado no atributo ``_controller``
do ``Request``).

.. seealso::

    Leia mais em :ref:`evento kernel.request <component-http-kernel-kernel-request>`.

.. index::
single: Evento; kernel.controller

Evento ``kernel.controller``
...........................

*Classe do Evento*: :class:`Symfony\\Component\\HttpKernel\\Event\\FilterControllerEvent`

Este evento não é usado pelo ``FrameworkBundle``, mas pode ser ponto de entrada usado
para modificar o controlador que será executado::

    use Symfony\Component\HttpKernel\Event\FilterControllerEvent;

    public function onKernelController(FilterControllerEvent $event)
    {
        $controller = $event->getController();
        // ...

        // o controlador pode ser trocado por qualquer código PHP válido
        $event->setController($controller);
    }

.. seealso::

    Leio mais em :ref:`evento kernel.controller <component-http-kernel-kernel-controller>`.

.. index::
single: Evento; kernel.view

Evento ``kernel.view``
.....................

*Classe do Evento*: :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForControllerResultEvent`

Este evento não é usado pelo ``FrameworkBundle``, mas pode ser usado para implementar
um sub-sistema de view. Este evento é chamado *apenas* se o Controlador *não* retornar
um objeto ``Response``. A proposta deste evento é permitir que qualquer outro valor possa
ser convertido em um ``Response``.

O valor retornado pelo Controlador é acessível através do método ``getControllerResult``::

    use Symfony\Component\HttpKernel\Event\GetResponseForControllerResultEvent;
    use Symfony\Component\HttpFoundation\Response;

    public function onKernelView(GetResponseForControllerResultEvent $event)
    {
        $val = $event->getControllerResult();
        $response = new Response();

        // ... uma maneria de customizar o Response a partir de um valor de retorno

        $event->setResponse($response);
    }

.. seealso::

    Leio mais no :ref:`evento kernel.view <component-http-kernel-kernel-view>`.

.. index::
single: Evento; kernel.response

Evento ``kernel.response``
.........................

*Classe do Evento*: :class:`Symfony\\Component\\HttpKernel\\Event\\FilterResponseEvent`

A proposta deste evento é permitir outros sistemas de modificar ou substituir o objeto
``Response`` depois de criado::

    public function onKernelResponse(FilterResponseEvent $event)
    {
        $response = $event->getResponse();

        // ... modifique o objeto response
    }

O``FrameworkBundle`` registra vários listeners:

* :class:`Symfony\\Component\\HttpKernel\\EventListener\\ProfilerListener`:
  coleta dados para o pedido atual;

* :class:`Symfony\\Bundle\\WebProfilerBundle\\EventListener\\WebDebugToolbarListener`:
  injeta a Web Debug Toolbar;

* :class:`Symfony\\Component\\HttpKernel\\EventListener\\ResponseListener`: fixes the
  Responde ``Content-Type`` baseado no formato do pedido;

* :class:`Symfony\\Component\\HttpKernel\\EventListener\\EsiListener`: adds a
  Cabeçalho HTTP ``Surrogate-Control`` quando o Response precisa ser transformado para
  tags ESI.

.. seealso::

    Leia mais no :ref:`evento kernel.response <component-http-kernel-kernel-response>`.

.. index::
single: Evento; kernel.terminate

Evento ``kernel.terminate``
..........................

O objetivo deste evento é relizar tarefas mais "pesadas" depois que a resposta foi
entregue ao cliente.

.. seealso::

    Leia mais em :ref:`evento kernel.terminate <component-http-kernel-kernel-terminate>`.

.. index::
single: Evento; kernel.exception

.. _kernel-kernel.exception:

Evento ``kernel.exception``
..........................

*Classe de Evento*: :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForExceptionEvent`

``FrameworkBundle`` registra um
:class:`Symfony\\Component\\HttpKernel\\EventListener\\ExceptionListener` que encaminha
o ``Request`` para um dado Controlador (o valor do parâmetro
``exception_listener.controller`` -- deve ser uma notação ``class::method``).

Um listener deste evento pode criar e definir um objeto ``Response``, criar e definir
um novo objeto ``Exception``, ou fazer nada::

    use Symfony\Component\HttpKernel\Event\GetResponseForExceptionEvent;
    use Symfony\Component\HttpFoundation\Response;

    public function onKernelException(GetResponseForExceptionEvent $event)
    {
        $exception = $event->getException();
        $response = new Response();
        // configura o objeto Response baseado na exception capturada
        $event->setResponse($response);

        // você também pode definir uma nova Exception
        // $exception = new \Exception('Some special exception');
        // $event->setException($exception);
    }

.. note::

    Como o Symfony garante que o código do status do Response é como o mais adequado
    dependendo da exceção, não vai funciona se definir o status na resposta. Se você
    quiser substituir o código do status (você não deve fazer sem uma boa razão), defina
    o ``X-Status-Code`` no cabeçalho::

        return new Response(
            'Error',
            404 // ignored,
            array('X-Status-Code' => 200)
        );

.. index::
single: Dispatcher de Evento

O Dispatcher de Evento
--------------------

O dispatcher de evento é um componente autônomo que é responsável por grande parte da lógica
subjacente e do fluxo por trás de um pedido Symfony. Para mais informações, veja a
 :doc:`Documentação do Componente Dispatcher de Evento </components/event_dispatcher/introduction>`.

.. seealso::

    Leia mais no :ref:`evento kernel.exception <component-http-kernel-kernel-exception>`.

.. index::
single: Profiler

.. _internals-profiler:

Profiler
--------

Quando ativado, o profiler do Symfony2 coleta informações úteis sobre cada pedido feito
à sua aplicação e armazenadas para uma análise posterior. Utilize o profiler no
ambiente de desenvolvimento para ajudar a depurar seu código e melhorar o desempenho;
utilize-o em produção para problemas quando acontecerem.

Você raramente precisa lidar com o profiler diretamente, como o Symfony2 oferece ferramentas
como Web Debug Toolbar e o Web Profiler. Se você utilizar o Symfony2 Standard Edition,
o profiler, o web debug toolbar, e o web profiler estão configurados com configurações
razoáveis.

.. note::

    O profiler coleta informações sobre todo os pedidos (pedidos simples, redirecionamentos,
    exeçõec, pedidos Ajax, pedidos ESI; e para todos os métodos HTTP e todos formatos).
    Isso significa que para uma única URL, você pode ter vários dados de perfis associados
    (por pedido externo/resposta)

.. index::
single: Profiler; Visualizando

Visualizando os dados do Profiler
~~~~~~~~~~~~~~~~~~~~~~~~~~

Usando a Web Debug Toolbar
...........................

No ambiente de desenvolvimento, a web debug toolbar está dispinível na parte de inferior
de todas as páginas. Ela mostra um bom resumo do dados coletados e dá acesso instantâneo
a uma grande quantidade de informações úteis quando algo não funciona como o esperado.

Se o resumo oferecido pela Web Debug Toolbar não é suficiente, clique no link do token
(uma string feita de 13 caractéres randômicos) para acessar o Web Profiler.

.. note::

    Se o token não está clicável, isso significa que a rota do profiler não está
    registrada (veja abaixo para obter informações de configuração)

Analisando os dados com o Web Profiler
..............................................

O Web Profiler é uma ferramenta de visualização de dados que você pode usar no
desenvolvimento para depurar seu código e melhor o desempenho; mas pode também ser
usado para explorar problemas que ocorrem em produção. Ele expõe toda informação
coletada pelo profiler na interface web.

.. index::
single: Profiler; Usando o serviço do profiler

Acessando a informação coletada
...................................

Você não precisa usar o visualizador padrão para acessar as informações coletadas. Mas
como você pode recuperar informações para um pedido específico? Quando o profiler armazena
os dados de um Request, também associa um token para ele; este token está disponível no
cabeçalho HTTP ``X-Debug-Token`` do Response::

    $profile = $container->get('profiler')->loadProfileFromResponse($response);

    $profile = $container->get('profiler')->loadProfile($token);

.. tip::

    Quando o profiler está ativado man não a web debug toolbar, ou quando você precisa
    pegar o token para um pedido Aja, utilize uma ferramenta como o Firebug para pegar
    o valor do cabeçalho HTTP ``X-Debug-Token``.

Utilize o método :method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::find`
para acessar os tokens baseados em alguns critérios::

    // pega os 10 últimos tokens
    $tokens = $container->get('profiler')->find('', '', 10);

    // pegue os 10 últimos tokens para todas URLs que contenham /admin/
    $tokens = $container->get('profiler')->find('', '/admin/', 10);

    // pegue os 10 últimos tokens para pedidos locais
    $tokens = $container->get('profiler')->find('127.0.0.1', '', 10);

Se você precisa manipular os dados coletados em uma máquina diferente da em que os dados
foram gerados, utilize os métodos
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::export` e
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::import`::

    // na máquina de produção
    $profile = $container->get('profiler')->loadProfile($token);
    $data = $profiler->export($profile);

    // na máquina de desenvolvimento
    $profiler->import($data);

.. index::
single: Profiler; Visualizando

Configuração
.............

A configuração padrão do Symfony2 vem com definições razoáveis para o profiler, a web
debug toolbar, e o web profiler. Aqui está um exemplo de configuração para o ambiente
de desenvolvimento:

.. configuration-block::

    .. code-block:: yaml

        # carrega o profiler
        framework:
            profiler: { only_exceptions: false }

        # ativa o web profiler
        web_profiler:
            toolbar: true
            intercept_redirects: true

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:webprofiler="http://symfony.com/schema/dic/webprofiler"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/webprofiler http://symfony.com/schema/dic/webprofiler/webprofiler-1.0.xsd
                                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <!-- load the profiler -->
            <framework:config>
                <framework:profiler only-exceptions="false" />
            </framework:config>

            <!-- enable the web profiler -->
            <webprofiler:config
                toolbar="true"
                intercept-redirects="true"
                verbose="true"
            />
        </container>

    .. code-block:: php

        // carrega o profiler
        $container->loadFromExtension('framework', array(
            'profiler' => array('only-exceptions' => false),
        ));

        // ativa o web profiler
        $container->loadFromExtension('web_profiler', array(
            'toolbar'             => true,
            'intercept-redirects' => true,
        ));

Quando ``only-execptions`` está definida como ``true``, o profiler apenas coleta dados
quando uma exception é lançada pela aplicação.

Quando ``intercept-redirects`` é definido como ``true``, o web profiler intercepta os
redirecionamentos e dá a você a oportunidade de ver os dados coletados antes de seguir
o redirecionamento.

Se você ativa o web profiler, você também precisa montar as rotas do profiler:

.. configuration-block::

    .. code-block:: yaml

        _profiler:
            resource: @WebProfilerBundle/Resources/config/routing/profiler.xml
            prefix:   /_profiler

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import
                resource="@WebProfilerBundle/Resources/config/routing/profiler.xml"
                prefix="/_profiler"
            />
        </routes>

    .. code-block:: php

        $collection->addCollection(
            $loader->import(
                "@WebProfilerBundle/Resources/config/routing/profiler.xml"
            ),
            '/_profiler'
        );

Como o profiler adiciona alguma sobrecarga, você pode querer ativar apenas em algumas
circunstâncias no ambiente de desenvolvimento. O parâmetro ``only-exceptions`` limita
para 500 páginas, mas se quiser obter informações quando um IP do cliente vier de um
endereço específico, ou de uma parte limitada do site? Você pode usar um Profiler
Matcher, aprenda mais sobre ele no ":doc:`/cookbook/profiler/matchers`".

Aprenda mais com o Cookbook
----------------------------

* :doc:`/cookbook/testing/profiling`
* :doc:`/cookbook/profiler/data_collector`
* :doc:`/cookbook/event_dispatcher/class_extension`
* :doc:`/cookbook/event_dispatcher/method_behavior`

.. _`Componete de Injeção de Dependência do Symfony2`: https://github.com/symfony/DependencyInjection
