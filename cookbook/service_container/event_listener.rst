.. index::
   single: Eventos; Criar ouvinte (listener)

Como criar um Ouvinte de Evento (Event Listener)
================================================

O Symfony tem vários eventos e hooks que podem ser utilizados para disparar comportamentos personalizados
em sua aplicação. Esses eventos são lançados pelo componente HttpKernel
e podem ser vistos na classe :class:`Symfony\\Component\\HttpKernel\\KernelEvents`

Para ligar em um evento e adicionar sua própria lógica personalizada, você tem que criar
um serviço que vai agir como um ouvinte de evento nesse evento. Neste artigo,
você vai criar um serviço que irá funcionar como um Exception Listener, permitindo que
você modifique a forma como as exceções são mostradas pela sua aplicação. O evento ``KernelEvents::EXCEPTION``
é apenas um dos eventos de kernel do núcleo::

    // src/Acme/DemoBundle/EventListener/AcmeExceptionListener.php
    namespace Acme\DemoBundle\EventListener;

    use Symfony\Component\HttpKernel\Event\GetResponseForExceptionEvent;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\Exception\HttpExceptionInterface;

    class AcmeExceptionListener
    {
        public function onKernelException(GetResponseForExceptionEvent $event)
        {
            // You get the exception object from the received event
            $exception = $event->getException();
            $message = sprintf(
                'My Error says: %s with code: %s',
                $exception->getMessage(),
                $exception->getCode()
            );

            // Customize your response object to display the exception details
            $response = new Response();
            $response->setContent($message);

            // HttpExceptionInterface is a special type of exception that
            // holds status code and header details
            if ($exception instanceof HttpExceptionInterface) {
                $response->setStatusCode($exception->getStatusCode());
                $response->headers->replace($exception->getHeaders());
            } else {
                $response->setStatusCode(Response::HTTP_INTERNAL_SERVER_ERROR);
            }

            // Send the modified response object to the event
            $event->setResponse($response);
        }
    }

.. versionadded:: 2.4
    O suporte para constantes de códigos de status HTTP foi introduzido no Symfony 2.4.

.. tip::

    Cada evento recebe um tipo ligeiramente diferente do objeto ``$event``. Para o
    evento ``kernel.exception``, é :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForExceptionEvent`.
    Para ver que tipo de objeto cada ouvinte de evento recebe, consulte :class:`Symfony\\Component\\HttpKernel\\KernelEvents`.

Agora que a classe está criada, você só precisa registrá-la como um serviço e
notificar o Symfony que é um "listener" no evento ``kernel.exception``
usando uma "tag" especial:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            kernel.listener.your_listener_name:
                class: Acme\DemoBundle\EventListener\AcmeExceptionListener
                tags:
                    - { name: kernel.event_listener, event: kernel.exception, method: onKernelException }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <service id="kernel.listener.your_listener_name" class="Acme\DemoBundle\EventListener\AcmeExceptionListener">
            <tag name="kernel.event_listener" event="kernel.exception" method="onKernelException" />
        </service>

    .. code-block:: php

        // app/config/config.php
        $container
            ->register('kernel.listener.your_listener_name', 'Acme\DemoBundle\EventListener\AcmeExceptionListener')
            ->addTag('kernel.event_listener', array('event' => 'kernel.exception', 'method' => 'onKernelException'))
        ;

.. note::

    Há uma opção adicional ``priority`` da tag que é opcional e o padrão
    é 0. Esse valor pode ser de -255 a 255, e os ouvintes serão executados
    na ordem de prioridade (do maior para o menor). Isso é útil quando
    você precisa garantir que um ouvinte seja executado antes de outro.

Eventos de Pedido (Request Events), Tipos de Verificação
--------------------------------------------------------

.. versionadded:: 2.4
    O método ``isMasterRequest()`` foi introduzido no Symfony 2.4.
    Antes, o método ``getRequestType()`` deve ser utilizado.

Uma única página pode fazer vários pedidos (um pedido mestre e, em seguida múltiplos
sub-pedidos), é por isso que, quando se trabalha com o evento ``KernelEvents::REQUEST``
, pode ser necessário verificar o tipo do pedido. Isso pode ser feito facilmente
como segue::

    // src/Acme/DemoBundle/EventListener/AcmeRequestListener.php
    namespace Acme\DemoBundle\EventListener;

    use Symfony\Component\HttpKernel\Event\GetResponseEvent;
    use Symfony\Component\HttpKernel\HttpKernel;

    class AcmeRequestListener
    {
        public function onKernelRequest(GetResponseEvent $event)
        {
            if (!$event->isMasterRequest()) {
                // don't do anything if it's not the master request
                return;
            }

            // ...
        }
    }

.. tip::

    Dois tipos de pedidos estão disponíveis na interface :class:`Symfony\\Component\\HttpKernel\\HttpKernelInterface`
    ``HttpKernelInterface::MASTER_REQUEST`` e
    ``HttpKernelInterface::SUB_REQUEST``.
