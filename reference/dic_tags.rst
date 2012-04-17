Tags de injeção de dependência
==============================

Tags:

* ``data_collector``
* ``form.type``
* ``form.type_extension``
* ``form.type_guesser``
* ``kernel.cache_warmer``
* ``kernel.event_listener``
* ``kernel.event_subscriber``
* ``monolog.logger``
* ``monolog.processor``
* ``templating.helper``
* ``routing.loader``
* ``translation.loader``
* ``twig.extension``
* ``validator.initializer``

Habilitando seus próprios PHP Template Helpers
----------------------------------------------

Para habilitar seus próprios template helpers, adicione ele como um serviço normal
na sua configuração, marque ele com ``templating.helper`` e defina um ``alias``
(o helper será acessível através deste nome na template).

.. configuration-block::

    .. code-block:: yaml

        services:
            templating.helper.your_helper_name:
                class: Fully\Qualified\Helper\Class\Name
                tags:
                    - { name: templating.helper, alias: alias_name }

    .. code-block:: xml

        <service id="templating.helper.your_helper_name" class="Fully\Qualified\Helper\Class\Name">
            <tag name="templating.helper" alias="alias_name" />
        </service>

    .. code-block:: php

        $container
            ->register('templating.helper.your_helper_name', 'Fully\Qualified\Helper\Class\Name')
            ->addTag('templating.helper', array('alias' => 'alias_name'))
        ;

.. _reference-dic-tags-twig-extension:

Habilitando suas próprias extensões Twig
----------------------------------------

Para habilitar uma extensão Twig, adicione como um serviço normal a sua configuração
e marque ele com ``twig.extension``:

.. configuration-block::

    .. code-block:: yaml

        services:
            twig.extension.your_extension_name:
                class: Fully\Qualified\Extension\Class\Name
                tags:
                    - { name: twig.extension }

    .. code-block:: xml

        <service id="twig.extension.your_extension_name" class="Fully\Qualified\Extension\Class\Name">
            <tag name="twig.extension" />
        </service>

    .. code-block:: php

        $container
            ->register('twig.extension.your_extension_name', 'Fully\Qualified\Extension\Class\Name')
            ->addTag('twig.extension')
        ;

Para mais informações em como criar a extensão Twig, veja o tópico `Twig's documentation`_ .


.. _dic-tags-kernel-event-listener:

Habilitando seus próprios Listeners
-----------------------------------

Para habilitar um listener, adicione-o como um serviço na sua configuração e
marque-o com ``kernel.event_listener``. Você deverá informar qual o nome do evento
ao qual seu listener estará associado bem como o método que será executado quando
o evento ocorrer.

.. configuration-block::

    .. code-block:: yaml

        services:
            kernel.listener.your_listener_name:
                class: Fully\Qualified\Listener\Class\Name
                tags:
                    - { name: kernel.event_listener, event: xxx, method: onXxx }

    .. code-block:: xml

        <service id="kernel.listener.your_listener_name" class="Fully\Qualified\Listener\Class\Name">
            <tag name="kernel.event_listener" event="xxx" method="onXxx" />
        </service>

    .. code-block:: php

        $container
            ->register('kernel.listener.your_listener_name', 'Fully\Qualified\Listener\Class\Name')
            ->addTag('kernel.event_listener', array('event' => 'xxx', 'method' => 'onXxx'))
        ;

.. note::

    Você também pode especificar a ordem de execuçãoi que um recurso marcado com
    a tag kernel.event_listener será chamado (assim como fez com os atributos
    method e event acima, mas utilizando o atributo priority) definindo um inteiro
    positivo ou negativo. Isto permite, por exemplo, que Você tenha certeza que seu listener
    será sempre chamado antes ou depois de outro listener ligado ao mesmo evento.



.. _dic-tags-kernel-event-subscriber:

Habilitando seus próprios Subscribers
-------------------------------------

.. versionadded:: 2.1

   A possibilidade de adicionar subscribers existe a partir da versão 2.1.

Para habilitar seu subscriber, adicione-o como um serviço em sua configuração
e marque-o com a tag ``kernel.event_subscriber``:

.. configuration-block::

    .. code-block:: yaml

        services:
            kernel.subscriber.your_subscriber_name:
                class: Fully\Qualified\Subscriber\Class\Name
                tags:
                    - { name: kernel.event_subscriber }

    .. code-block:: xml

        <service id="kernel.subscriber.your_subscriber_name" class="Fully\Qualified\Subscriber\Class\Name">
            <tag name="kernel.event_subscriber" />
        </service>

    .. code-block:: php

        $container
            ->register('kernel.subscriber.your_subscriber_name', 'Fully\Qualified\Subscriber\Class\Name')
            ->addTag('kernel.event_subscriber')
        ;

.. note::

    Seu serviço deverá implementar a interface :class:`Symfony\Component\EventDispatcher\EventSubscriberInterface` .

.. note::

    Se o seu serviço é criado por uma factory, Você **deverá** definir
    o parâmetro ``class`` para esta tag funcionar corretamente.

Habilitando seu próprio sistema de templates
--------------------------------------------

Para habilitar um sistema de template, adicione-o como um serviço e marque-o
com a tag ``templating.engine``:

.. configuration-block::

    .. code-block:: yaml

        services:
            templating.engine.your_engine_name:
                class: Fully\Qualified\Engine\Class\Name
                tags:
                    - { name: templating.engine }

    .. code-block:: xml

        <service id="templating.engine.your_engine_name" class="Fully\Qualified\Engine\Class\Name">
            <tag name="templating.engine" />
        </service>

    .. code-block:: php

        $container
            ->register('templating.engine.your_engine_name', 'Fully\Qualified\Engine\Class\Name')
            ->addTag('templating.engine')
        ;

Habilitando seus próprios carregadores de rotas (Routing Loaders)
-----------------------------------------------------------------

Para habilitar seus próprios carregadores, adicione-o como um serviço na
sua configuração e marque-o com a tag ``routing.loader``:

.. configuration-block::

    .. code-block:: yaml

        services:
            routing.loader.your_loader_name:
                class: Fully\Qualified\Loader\Class\Name
                tags:
                    - { name: routing.loader }

    .. code-block:: xml

        <service id="routing.loader.your_loader_name" class="Fully\Qualified\Loader\Class\Name">
            <tag name="routing.loader" />
        </service>

    .. code-block:: php

        $container
            ->register('routing.loader.your_loader_name', 'Fully\Qualified\Loader\Class\Name')
            ->addTag('routing.loader')
        ;

.. _dic_tags-monolog:

Usando um canal próprio para logging com o Monolog
--------------------------------------------------

O Monolog permite que Você compartilhe seus manipuladores entre vários canais
de logging. O serviço logger utiliza o canal ``app``, mas Você pode alterá-lo
quando injetar o serviço em outro serviço.
Monolog allows you to share its handlers between several logging channels.
The logger service uses the channel ``app`` but you can change the
channel when injecting the logger in a service.

.. configuration-block::

    .. code-block:: yaml

        services:
            my_service:
                class: Fully\Qualified\Loader\Class\Name
                arguments: [@logger]
                tags:
                    - { name: monolog.logger, channel: acme }

    .. code-block:: xml

        <service id="my_service" class="Fully\Qualified\Loader\Class\Name">
            <argument type="service" id="logger" />
            <tag name="monolog.logger" channel="acme" />
        </service>

    .. code-block:: php

        $definition = new Definition('Fully\Qualified\Loader\Class\Name', array(new Reference('logger'));
        $definition->addTag('monolog.logger', array('channel' => 'acme'));
        $container->register('my_service', $definition);;

.. note::

    This works only when the logger service is a constructor argument,
    not when it is injected through a setter.

.. _dic_tags-monolog-processor:

Adding a processor for Monolog
------------------------------

Monolog allows you to add processors in the logger or in the handlers to add
extra data in the records. A processor receives the record as an argument and
must return it after adding some extra data in the ``extra`` attribute of
the record.

Let's see how you can use the built-in ``IntrospectionProcessor`` to add
the file, the line, the class and the method where the logger was triggered.

You can add a processor globally:

.. configuration-block::

    .. code-block:: yaml

        services:
            my_service:
                class: Monolog\Processor\IntrospectionProcessor
                tags:
                    - { name: monolog.processor }

    .. code-block:: xml

        <service id="my_service" class="Monolog\Processor\IntrospectionProcessor">
            <tag name="monolog.processor" />
        </service>

    .. code-block:: php

        $definition = new Definition('Monolog\Processor\IntrospectionProcessor');
        $definition->addTag('monolog.processor');
        $container->register('my_service', $definition);

.. tip::

    If your service is not a callable (using ``__invoke``) you can add the
    ``method`` attribute in the tag to use a specific method.

You can add also a processor for a specific handler by using the ``handler``
attribute:

.. configuration-block::

    .. code-block:: yaml

        services:
            my_service:
                class: Monolog\Processor\IntrospectionProcessor
                tags:
                    - { name: monolog.processor, handler: firephp }

    .. code-block:: xml

        <service id="my_service" class="Monolog\Processor\IntrospectionProcessor">
            <tag name="monolog.processor" handler="firephp" />
        </service>

    .. code-block:: php

        $definition = new Definition('Monolog\Processor\IntrospectionProcessor');
        $definition->addTag('monolog.processor', array('handler' => 'firephp');
        $container->register('my_service', $definition);

You can also add a processor for a specific logging channel by using the ``channel``
attribute. This will register the processor only for the ``security`` logging
channel used in the Security component:

.. configuration-block::

    .. code-block:: yaml

        services:
            my_service:
                class: Monolog\Processor\IntrospectionProcessor
                tags:
                    - { name: monolog.processor, channel: security }

    .. code-block:: xml

        <service id="my_service" class="Monolog\Processor\IntrospectionProcessor">
            <tag name="monolog.processor" channel="security" />
        </service>

    .. code-block:: php

        $definition = new Definition('Monolog\Processor\IntrospectionProcessor');
        $definition->addTag('monolog.processor', array('channel' => 'security');
        $container->register('my_service', $definition);

.. note::

    You cannot use both the ``handler`` and ``channel`` attributes for the
    same tag as handlers are shared between all channels.

..  _`Twig's documentation`: http://twig.sensiolabs.org/doc/extensions.html
