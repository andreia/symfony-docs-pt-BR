.. index::
   single: Roteamento; Permitir / no parâmetro de rota

Como permitir um caractere "/" em um parâmetro de rota
======================================================

Às vezes, você precisa compor URLs com parâmetros que podem conter uma barra
``/``. Por exemplo, considere a rota clássica ``/hello/{name}``. Por padrão,
``/hello/Fabien`` irá corresponder a esta rota, mas ``/hello/Fabien/Kris`` não irá.
Isso ocorre porque o Symfony usa esse caractere como separador entre as partes da rota.

Este guia aborda como você pode modificar a rota para que ``/hello/Fabien/Kris``
corresponda à rota ``/hello/{name}``, onde ``{name}`` iguala ``Fabien/Kris``.

Configure a Rota
----------------

Por padrão, o componente de roteamento do Symfony requer que os parâmetros correspondam
com o seguinte padrão de expressão regular: ``[^/]+``. Isso significa que todos os caracteres
são permitidos, exceto ``/``.

Você deve explicitamente permitir que a ``/`` faça parte de seu parâmetro, especificando
um padrão regex mais permissivo.

.. configuration-block::

    .. code-block:: yaml

        _hello:
            pattern: /hello/{name}
            defaults: { _controller: AcmeDemoBundle:Demo:hello }
            requirements:
                name: ".+"

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="_hello" pattern="/hello/{name}">
                <default key="_controller">AcmeDemoBundle:Demo:hello</default>
                <requirement key="name">.+</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('_hello', new Route('/hello/{name}', array(
            '_controller' => 'AcmeDemoBundle:Demo:hello',
        ), array(
            'name' => '.+',
        )));

        return $collection;

    .. code-block:: php-annotations

        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

        class DemoController
        {
            /**
             * @Route("/hello/{name}", name="_hello", requirements={"name" = ".+"})
             */
            public function helloAction($name)
            {
                // ...
            }
        }

É isso! Agora, o parâmetro ``{name}`` pode conter o caractere ``/``.
