.. index::
   single: Roteamento; Informações Extras

Como Passar Informação Extra para um Controlador a partir de uma Rota
=====================================================================

Os parâmetros dentro da coleção ``defaults`` não têm necessariamente que
coincidir com um placeholder na rota ``path``. Na verdade, você pode usar o
array ``defaults`` para especificar parâmetros adicionais que serão então acessíveis como
argumentos no controlador:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        blog:
            path:      /blog/{page}
            defaults:
                _controller: AppBundle:Blog:index
                page:        1
                title:       "Hello world!"

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">AppBundle:Blog:index</default>
                <default key="page">1</default>
                <default key="title">Hello world!</default>
            </route>
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'AppBundle:Blog:index',
            'page'        => 1,
            'title'       => 'Hello world!',
        )));

        return $collection;

Agora, você pode acessar esse parâmetro extra em seu controlador::

    public function indexAction($page, $title)
    {
        // ...
    }

Como você pode ver, a variável ``$title`` nunca foi definida dentro do path da rota,
mas ainda é possível acessar seu valor dentro de seu controlador.
