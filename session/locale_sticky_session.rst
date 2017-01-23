.. index::
    single: Sessões, salvar a localidade

Tornando a Localidade Persistente durante a Sessão de um Usuário
================================================================

O Symfony armazena a configuração de localidade na requisição (Request), o que significa que essa
configuração não está disponível nas requisições subsequentes. Neste artigo, você vai aprender a
armazenar a localidade na sessão, para que ela seja a mesma para cada requisição
subsequente.

Criando um LocaleListener
-------------------------

Para simular que a localidade está armazenada numa sessão, você precisa criar e
registrar um :doc:`novo event listener </cookbook/event_dispatcher/event_listener>`.
O listener será parecido com o seguinte. Tipicamente, ``_locale`` é usado
como um parâmetro de roteamento para indicar a localidade, embora, realmente não faz
diferença como você determina a localidade desejada a partir da requisição::

    // src/AppBundle/EventListener/LocaleListener.php
    namespace AppBundle\EventListener;

    use Symfony\Component\HttpKernel\Event\GetResponseEvent;
    use Symfony\Component\HttpKernel\KernelEvents;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;

    class LocaleListener implements EventSubscriberInterface
    {
        private $defaultLocale;

        public function __construct($defaultLocale = 'en')
        {
            $this->defaultLocale = $defaultLocale;
        }

        public function onKernelRequest(GetResponseEvent $event)
        {
            $request = $event->getRequest();
            if (!$request->hasPreviousSession()) {
                return;
            }

            // try to see if the locale has been set as a _locale routing parameter
            if ($locale = $request->attributes->get('_locale')) {
                $request->getSession()->set('_locale', $locale);
            } else {
                // if no explicit locale has been set on this request, use one from the session
                $request->setLocale($request->getSession()->get('_locale', $this->defaultLocale));
            }
        }

        public static function getSubscribedEvents()
        {
            return array(
                // must be registered after the default Locale listener
                KernelEvents::REQUEST => array(array('onKernelRequest', 15)),
            );
        }
    }

Em seguida, registre o listener:

.. configuration-block::

    .. code-block:: yaml

        services:
            app.locale_listener:
                class: AppBundle\EventListener\LocaleListener
                arguments: ['%kernel.default_locale%']
                tags:
                    - { name: kernel.event_subscriber }

    .. code-block:: xml

        <service id="app.locale_listener"
            class="AppBundle\EventListener\LocaleListener">
            <argument>%kernel.default_locale%</argument>

            <tag name="kernel.event_subscriber" />
        </service>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition;

        $container
            ->setDefinition('app.locale_listener', new Definition(
                'AppBundle\EventListener\LocaleListener',
                array('%kernel.default_locale%')
            ))
            ->addTag('kernel.event_subscriber')
        ;

É isso! Agora celebre alterando a localidade do usuário e vendo que ela está
persistindo em toda a requisição. Lembre-se, para obter a localidade do usuário,
use sempre o método
:method:`Request::getLocale <Symfony\\Component\\HttpFoundation\\Request::getLocale>`::

    // from a controller...
    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $locale = $request->getLocale();
    }

Definindo a Localidade com base nas Preferências do Usuário
-----------------------------------------------------------

Você pode querer melhorar essa técnica ainda mais e definir a localidade com base na
entidade de usuário do usuário logado. No entanto, uma vez que o ``LocaleListener`` é chamado
antes do ``FirewallListener``, que é responsável pelo tratamento de autenticação e
definição do token do usuário no ``TokenStorage``, você não tem acesso ao usuário
que está logado.

Suponha que você tenha definido uma propriedade ``locale`` em sua entidade ``User``
e quer usá-la como localidade para o usuário. Para conseguir isso,
você pode alterar durante o processo de login e atualizar a sessão do usuário
com esse valor de localidade antes de serem redirecionadas para sua primeira página.

Para fazer isso, você precisa de um event listener para o evento
``security.interactive_login``:

.. code-block:: php

    // src/AppBundle/EventListener/UserLocaleListener.php
    namespace AppBundle\EventListener;

    use Symfony\Component\HttpFoundation\Session\Session;
    use Symfony\Component\Security\Http\Event\InteractiveLoginEvent;

    /**
     * Stores the locale of the user in the session after the
     * login. This can be used by the LocaleListener afterwards.
     */
    class UserLocaleListener
    {
        /**
         * @var Session
         */
        private $session;

        public function __construct(Session $session)
        {
            $this->session = $session;
        }

        /**
         * @param InteractiveLoginEvent $event
         */
        public function onInteractiveLogin(InteractiveLoginEvent $event)
        {
            $user = $event->getAuthenticationToken()->getUser();

            if (null !== $user->getLocale()) {
                $this->session->set('_locale', $user->getLocale());
            }
        }
    }

Em seguida, registre o listener:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.user_locale_listener:
                class: AppBundle\EventListener\UserLocaleListener
                arguments: ['@session']
                tags:
                    - { name: kernel.event_listener, event: security.interactive_login, method: onInteractiveLogin }

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="app.user_locale_listener"
                    class="AppBundle\EventListener\UserLocaleListener">

                    <argument type="service" id="session"/>

                    <tag name="kernel.event_listener"
                        event="security.interactive_login"
                        method="onInteractiveLogin" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/services.php
        $container
            ->register('app.user_locale_listener', 'AppBundle\EventListener\UserLocaleListener')
            ->addArgument('session')
            ->addTag(
                'kernel.event_listener',
                array('event' => 'security.interactive_login', 'method' => 'onInteractiveLogin'
            );

.. caution::

    Para atualizar o idioma imediatamente após um usuário alterar as
    suas preferências de idioma, você precisa atualizar a sessão após uma
    atualização na entidade ``User``.
