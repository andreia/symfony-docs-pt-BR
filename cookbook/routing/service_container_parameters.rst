.. index::
   single: Roteamento; Parâmetros do Container de Serviço

Como usar os Parâmetros do Container de Serviço em suas Rotas
=============================================================

.. versionadded:: 2.1
    A possibilidade de usar parâmetros em suas rotas foi adicionada no Symfony 2.1.

Às vezes pode ser útil tornar algumas partes de suas rotas
configuráveis globalmente. Por exemplo, se você construir um site internacionalizado
, você provavelmente vai começar com uma ou duas localidades. Certamente você irá
adicionar um requisito em suas rotas para evitar que um usuário utilize uma localidade
além das suportadas.

Você *pode* codificar manualmente seu requisito ``_locale`` em todas as suas rotas. Mas
uma solução melhor é usar um parâmetro configurável do container de serviço 
dentro de sua configuração de roteamento:

.. configuration-block::

    .. code-block:: yaml

        contact:
            pattern:  /{_locale}/contact
            defaults: { _controller: AcmeDemoBundle:Main:contact }
            requirements:
                _locale: %acme_demo.locales%

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" pattern="/{_locale}/contact">
                <default key="_controller">AcmeDemoBundle:Main:contact</default>
                <requirement key="_locale">%acme_demo.locales%</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('contact', new Route('/{_locale}/contact', array(
            '_controller' => 'AcmeDemoBundle:Main:contact',
        ), array(
            '_locale' => '%acme_demo.locales%',
        )));

        return $collection;

Agora você pode controlar e definir o parâmetro ``acme_demo.locales`` em algum lugar
no seu container:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            acme_demo.locales: en|es

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="acme_demo.locales">en|es</parameter>
        </parameters>

    .. code-block:: php

        # app/config/config.php
        $container->setParameter('acme_demo.locales', 'en|es');

Você também pode usar um parâmetro para definir o seu pattern da rota (ou parte do seu
pattern):

.. configuration-block::

    .. code-block:: yaml

        some_route:
            pattern:  /%acme_demo.route_prefix%/contact
            defaults: { _controller: AcmeDemoBundle:Main:contact }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="some_route" pattern="/%acme_demo.route_prefix%/contact">
                <default key="_controller">AcmeDemoBundle:Main:contact</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('some_route', new Route('/%acme_demo.route_prefix%/contact', array(
            '_controller' => 'AcmeDemoBundle:Main:contact',
        )));

        return $collection;

.. note::

    Assim como em arquivos normais de configuração do container de serviço, se você precisar
    de um ``%`` na sua rota, você pode escapar o sinal de porcentagem duplicando ele
    , por exemplo, ``/score-50%%``, irá converter para ``/score-50%``.
