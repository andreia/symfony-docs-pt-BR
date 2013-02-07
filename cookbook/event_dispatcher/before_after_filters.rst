.. index::
   single: Dispatcher de Evento

Como configurar Filtros aplicados antes e após
==============================================

É bastante comum, no desenvolvimento de aplicações web, precisar que alguma 
lógica seja executada antes ou após as ações de seu controlador, atuando como filtros
ou hooks.

No symfony1, isto era feito através dos métodos PreExecute e postExecute.
A maioria dos principais frameworks possuem métodos semelhantes, mas isso não existe no Symfony2.
A boa nova é que há uma forma muito melhor para interferir no processo 
Pedido -> Resposta usando o :doc:`componente EventDispatcher</components/event_dispatcher/introduction>`.

Exemplo de validação de token
-----------------------------

Imagine que você precisa desenvolver uma API onde alguns controladores são públicos
mas outros são restritos a um ou alguns clientes. Para estas funcionalidades privadas,
você pode fornecer um token para os clientes identificarem-se.

Então, antes de executar a ação do controlador, você precisa verificar se a ação
é restrita ou não. Se for restrita, você precisa validar o token
informado.

.. note::

    Por favor, note que, por simplicidade, nesta receita os tokens serão definidos na configuração
    e não será usada configuração de banco de dados nem autenticação através do componente de
    Segurança.

Filtros aplicados Antes com o evento ``kernel.controller``
----------------------------------------------------------

Primeiro, armazene algumas configurações básicas do token usando o ``config.yml`` e a
chave ``parameters``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            tokens:
                client1: pass1
                client2: pass2

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="tokens" type="collection">
                <parameter key="client1">pass1</parameter>
                <parameter key="client2">pass2</parameter>
            </parameter>
        </parameters>

    .. code-block:: php

        // app/config/config.php
        $container->setParameter('tokens', array(
            'client1' => 'pass1',
            'client2' => 'pass2',
        ));

Tags de Controladores a serem verificadas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Um ouvinte ``kernel.controller`` é notificado em *todos* os pedidos, mesmo antes do
controlador ser executado. Então, primeiro, você precisa de alguma forma para identificar
se o controlador, que coincide com o pedido, precisa de validação do token.

Uma maneira fácil e limpa é criar uma interface vazia e fazer os controladores
implementá-la::

    namespace Acme\DemoBundle\Controller;

    interface TokenAuthenticatedController
    {
        // ...
    }

Um controlador que implementa essa interface ficaria assim::

    namespace Acme\DemoBundle\Controller;

    use Acme\DemoBundle\Controller\TokenAuthenticatedController;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class FooController extends Controller implements TokenAuthenticatedController
    {
        // An action that needs authentication
        public function barAction()
        {
            // ...
        }
    }

Criando um Ouvinte de Evento
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Em seguida, você precisa criar um ouvinte de evento, que irá conter a lógica que
você deseja executar antes de seus controladores. Se você não está familiarizado com
ouvintes de eventos, você pode aprender mais sobre eles em :doc:`/cookbook/service_container/event_listener`::

    // src/Acme/DemoBundle/EventListener/TokenListener.php
    namespace Acme\DemoBundle\EventListener;

    use Acme\DemoBundle\Controller\TokenAuthenticatedController;
    use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;
    use Symfony\Component\HttpKernel\Event\FilterControllerEvent;

    class TokenListener
    {
        private $tokens;

        public function __construct($tokens)
        {
            $this->tokens = $tokens;
        }

        public function onKernelController(FilterControllerEvent $event)
        {
            $controller = $event->getController();

            /*
             * $controller passed can be either a class or a Closure. This is not usual in Symfony2 but it may happen.
             * If it is a class, it comes in array format
             */
            if (!is_array($controller)) {
                return;
            }

            if ($controller[0] instanceof TokenAuthenticatedController) {
                $token = $event->getRequest()->query->get('token');
                if (!in_array($token, $this->tokens)) {
                    throw new AccessDeniedHttpException('This action needs a valid token!');
                }
            }
        }
    }

Registrando o Ouvinte
~~~~~~~~~~~~~~~~~~~~~

Finalmente, registre seu ouvinte como um serviço e adicione a ele uma tag de ouvinte de evento.
Ao ouvir o ``kernel.controller``, você está dizendo ao Symfony que deseja
que seu ouvinte seja chamado antes que qualquer controlador seja executado.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml (or inside your services.yml)
        services:
            demo.tokens.action_listener:
                class: Acme\DemoBundle\EventListener\TokenListener
                arguments: [ %tokens% ]
                tags:
                    - { name: kernel.event_listener, event: kernel.controller, method: onKernelController }

    .. code-block:: xml

        <!-- app/config/config.xml (or inside your services.xml) -->
        <service id="demo.tokens.action_listener" class="Acme\DemoBundle\EventListener\TokenListener">
            <argument>%tokens%</argument>
            <tag name="kernel.event_listener" event="kernel.controller" method="onKernelController" />
        </service>

    .. code-block:: php

        // app/config/config.php (or inside your services.php)
        use Symfony\Component\DependencyInjection\Definition;

        $listener = new Definition('Acme\DemoBundle\EventListener\TokenListener', array('%tokens%'));
        $listener->addTag('kernel.event_listener', array('event' => 'kernel.controller', 'method' => 'onKernelController'));
        $container->setDefinition('demo.tokens.action_listener', $listener);

Com esta configuração, seu método ``TokenListener`` ``onKernelController``
será executado em cada pedido. Se o controlador que está prestes a ser executado
implementa ``TokenAuthenticatedController``, o token de autenticação é
aplicado. Isso permite que você tenha um filtro "antes" em qualquer controlador que
desejar.

Filtros aplicados "Após" com o evento ``kernel.response``
---------------------------------------------------------

Além de ter um "hook" que é executado antes de seu controlador, você também pode adicionar
um hook que será executado *após* seu controlador. Para este exemplo,
imagine que você deseja adicionar um hash sha1 (com um salt usando aquele token) para
todas as respostas que passaram este token de autenticação.

Outro evento do núcleo do Symfony - chamado ``kernel.response`` - é notificado em
cada pedido, mas depois que o controlador retorna um objeto de Resposta. Criar
um ouvinte "após" é tão fácil quanto criar uma classe ouvinte e registrá-la
como um serviço neste evento.

Por exemplo, considere o ``TokenListener`` do exemplo anterior e primeiro
grave o token de autenticação dentro dos atributos do pedido. Isto
servirá como uma flag básica de que este pedido foi submetido à autenticação por token::

    public function onKernelController(FilterControllerEvent $event)
    {
        // ...

        if ($controller[0] instanceof TokenAuthenticatedController) {
            $token = $event->getRequest()->query->get('token');
            if (!in_array($token, $this->tokens)) {
                throw new AccessDeniedHttpException('This action needs a valid token!');
            }

            // mark the request as having passed token authentication
            $event->getRequest()->attributes->set('auth_token', $token);
        }
    }

Agora, adicione um outro método nesta classe - ``onKernelResponse`` - que procura
por esta flag no objeto do pedido e define um cabeçalho personalizado na resposta
se ele for encontrado::

    // add the new use statement at the top of your file
    use Symfony\Component\HttpKernel\Event\FilterResponseEvent;

    public function onKernelResponse(FilterResponseEvent $event)
    {
        // check to see if onKernelController marked this as a token "auth'ed" request
        if (!$token = $event->getRequest()->attributes->get('auth_token')) {
            return;
        }

        $response = $event->getResponse();

        // create a hash and set it as a response header
        $hash = sha1($response->getContent().$token);
        $response->headers->set('X-CONTENT-HASH', $hash);
    }

Finalmente, uma segunda "tag" é necessária na definição do serviço para notificar o Symfony
que o evento ``onKernelResponse`` deve ser notificado para o evento 
``kernel.response``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml (or inside your services.yml)
        services:
            demo.tokens.action_listener:
                class: Acme\DemoBundle\EventListener\TokenListener
                arguments: [ %tokens% ]
                tags:
                    - { name: kernel.event_listener, event: kernel.controller, method: onKernelController }
                    - { name: kernel.event_listener, event: kernel.response, method: onKernelResponse }

    .. code-block:: xml

        <!-- app/config/config.xml (or inside your services.xml) -->
        <service id="demo.tokens.action_listener" class="Acme\DemoBundle\EventListener\TokenListener">
            <argument>%tokens%</argument>
            <tag name="kernel.event_listener" event="kernel.controller" method="onKernelController" />
            <tag name="kernel.event_listener" event="kernel.response" method="onKernelResponse" />
        </service>

    .. code-block:: php

        // app/config/config.php (or inside your services.php)
        use Symfony\Component\DependencyInjection\Definition;

        $listener = new Definition('Acme\DemoBundle\EventListener\TokenListener', array('%tokens%'));
        $listener->addTag('kernel.event_listener', array('event' => 'kernel.controller', 'method' => 'onKernelController'));
        $listener->addTag('kernel.event_listener', array('event' => 'kernel.response', 'method' => 'onKernelResponse'));
        $container->setDefinition('demo.tokens.action_listener', $listener);

É isso! O ``TokenListener`` agora é notificado antes de cada controlador ser
executado (``onKernelController``) e depois de cada controlador retornar uma resposta
(``onKernelResponse``). Fazendo com que controladores específicos implementem a interface ``TokenAuthenticatedController``
, o ouvinte saberá em quais controladores ele deve agir.
E armazenando um valor nos "atributos" do pedido, o método ``onKernelResponse``
sabe adicionar o cabeçalho extra. Divirta-se!
