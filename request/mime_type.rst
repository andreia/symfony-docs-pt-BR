.. index::
   single: Request; Add a request format and mime type

Como registrar um novo Formato de Requisição e de Mime Type
===========================================================

Todo ``Request`` possui um "formato" (e.g. ``html``, ``json``), que é usado
para determinar o tipo de conteúdo a ser retornado pelo ``Response``. Na
verdade, o formato de requisição, acessível pelo método
:method:`Symfony\\Component\\HttpFoundation\\Request::getRequestFormat`,
é usado para definir o MIME type do cabeçalho ``Content-Type`` no objeto
``Response``. Internamente, Symfony contém um mapa dos formatos mais comuns
(e.g. ``html``, ``json``) e seus MIME types associados (e.g. ``text/html``,
``application/json``). Naturalmente, formatos adicionais de MIME type de
entrada podem ser facilmente adicionados. Este documento irá mostrar como
você pode adicionar o formato ``jsonp`` e seu MIME type correspondente.

Criando um ``kernel.request`` Listener
--------------------------------------

A chave para definir um novo MIME type é criar uma classe que irá "ouvir" o
evento ``kernel.request`` enviado pelo kernel do Symfony. O evento
``kernel.request`` é enviado no início no processo de manipulação da
requisição Symfony e permite que você modifique o objeto da requisição.


Crie a seguinte classe, substituindo o caminho com um caminho para um pacote
em seu projeto::

    // src/Acme/DemoBundle/RequestListener.php
    namespace Acme\DemoBundle;

    use Symfony\Component\HttpKernel\HttpKernelInterface;
    use Symfony\Component\HttpKernel\Event\GetResponseEvent;

    class RequestListener
    {
        public function onKernelRequest(GetResponseEvent $event)
        {
            $event->getRequest()->setFormat('jsonp', 'application/javascript');
        }
    }

Registrando seu Listener
------------------------

Como para qualquer outro listener, você precisa adicioná-lo em um arquivo de
configuração e registrá-lo como um listerner adicionando a tag
``kernel.event_listener``:

.. configuration-block::

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" ?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <service id="acme.demobundle.listener.request" class="Acme\DemoBundle\RequestListener">
                <tag name="kernel.event_listener" event="kernel.request" method="onKernelRequest" />
            </service>

        </container>

    .. code-block:: yaml

        # app/config/config.yml
        services:
            acme.demobundle.listener.request:
                class: Acme\DemoBundle\RequestListener
                tags:
                    - { name: kernel.event_listener, event: kernel.request, method: onKernelRequest }

    .. code-block:: php

        # app/config/config.php
        $definition = new Definition('Acme\DemoBundle\RequestListener');
        $definition->addTag('kernel.event_listener', array('event' => 'kernel.request', 'method' => 'onKernelRequest'));
        $container->setDefinition('acme.demobundle.listener.request', $definition);

Neste ponto, o serviço ``acme.demobundle.listener.request`` foi configurado e
será notificado quando o Symfony kernel enviar um evento ``kernel.request``.

.. tip::

    Você também pode registrar o ouvinte em uma classe de extensão de
    configuração (see :ref:`service-container-extension-configuration`
    para mais informações).
