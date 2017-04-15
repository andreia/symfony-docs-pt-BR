.. index::
    single: Tradução; Localidade

Como Trabalhar com a Localidade do Usuário
==========================================

A localidade do usuário atual é armazenada na requisição e está acessível
através do objeto ``Request``::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $locale = $request->getLocale();
    }

Para definir a localidade do usuário, você pode desejar criar um ouvinte de evento personalizado
de modo que ela será definida antes que quaisquer outras partes do sistema (ou seja, o tradutor)
necessitem dela::

        public function onKernelRequest(GetResponseEvent $event)
        {
            $request = $event->getRequest();

            // some logic to determine the $locale
            $request->setLocale($locale);
        }

Leia :doc:`/session/locale_sticky_session` para mais informações sobre como tornar a
localidade do usuário disponível durante toda a sua sessão.

.. note::

    Definir a localidade usando ``$request->setLocale()`` no controlador é
    tarde demais para afetar o tradutor. Defina a localidade através de um ouvinte
    (como acima), da URL (ver abaixo) ou chamando ``setLocale()`` diretamente no
    serviço ``translator``.

Veja a seção :ref:`translation-locale-url` abaixo sobre a definição da
localidade via roteamento.

.. _translation-locale-url:

A Localidade e a URL
--------------------

Uma vez que você pode armazenar a localidade do usuário na sessão, pode ser tentador
usar a mesma URL para exibir um recurso em diferentes idiomas com base na
localidade do usuário. Por exemplo, ``http://www.example.com/contact`` poderia mostrar
conteúdo em Inglês para um usuário e Francês para outro usuário. Infelizmente,
isso viola uma regra fundamental da Web: que um determinada URL retorna o mesmo
recurso, independentemente do usuário. Para dificultar ainda mais o problema, que
versão do conteúdo seria indexada pelas ferramentas de busca?

A melhor política é incluir a localidade na URL. Isso é totalmente suportado
pelo sistema de roteamento usando o parâmetro especial ``_locale``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        contact:
            path:     /{_locale}/contact
            defaults: { _controller: AppBundle:Contact:index }
            requirements:
                _locale: en|fr|de

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" path="/{_locale}/contact">
                <default key="_controller">AppBundle:Contact:index</default>
                <requirement key="_locale">en|fr|de</requirement>
            </route>
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('contact', new Route(
            '/{_locale}/contact',
            array(
                '_controller' => 'AppBundle:Contact:index',
            ),
            array(
                '_locale' => 'en|fr|de',
            )
        ));

        return $collection;

Ao usar o parâmetro especial ``_locale`` em uma rota, a localidade correspondida
é *automaticamente definida no Request* e pode ser recuperada através do método
:method:`Symfony\\Component\\HttpFoundation\\Request::getLocale`. Em
outras palavras, se um usuário visita a URI ``/fr/contact``, a localidade ``fr`` vai
ser automaticamente definida como a localidade da requisição atual.

Agora você pode usar o local para criar rotas para outras páginas traduzidas em seu
Symfony.

.. tip::

    Leia :doc:`/routing/service_container_parameters` para aprender como evitar
    codificar o ``_locale`` em todas as suas rotas.

.. index::
    single: Traduções; Localização padrão e fallback

.. _translation-default-locale:

Definindo uma localidade padrão
-------------------------------

E se a localidade do usuário não foi determinada? Você pode garantir que uma localidade
será definida em cada requisição do usuário através da definição de um ``default_locale`` para
o framework:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            default_locale: en

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config default-locale="en" />
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'default_locale' => 'en',
        ));
