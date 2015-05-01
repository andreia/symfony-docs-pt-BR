.. index::
   single: Templating; Renderização de template sem controlador personalizado

Como renderizar um template sem um Controlador personalizado
============================================================

Normalmente, quando você precisa criar uma página, você cria um controlador
e renderiza um template a partir deste controlador. Mas se você estiver renderizando
um template simples que não precisa de quaisquer dados passados ​​para ele, pode-se evitar totalmente
a criação do controlador, usando o controlador incorporado ``FrameworkBundle:Template:template``
.

Por exemplo, suponha que você queira renderizar um template
``static/privacy.html.twig``, que não requer que quaisquer variáveis ​​sejam passadas ​​a ele. Você
pode fazer isso sem criar um controlador:

.. configuration-block::

    .. code-block:: yaml

        acme_privacy:
            path: /privacy
            defaults:
                _controller: FrameworkBundle:Template:template
                template:    static/privacy.html.twig

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="acme_privacy" path="/privacy">
                <default key="_controller">FrameworkBundle:Template:template</default>
                <default key="template">static/privacy.html.twig</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('acme_privacy', new Route('/privacy', array(
            '_controller'  => 'FrameworkBundle:Template:template',
            'template'     => 'static/privacy.html.twig',
        )));

        return $collection;

O controlador ``FrameworkBundle:Template:template`` irá simplesmente renderizar qualquer
template que você passar como o valor padrão ``template``.

Naturalmente, você também pode usar esse truque ao renderizar controladores embutidos
a partir de um template. Mas, uma vez que a finalidade de renderizar um controlador a
partir de um template é tipicamente para preparar alguns dados num controlador personalizado,
isso provavelmente só é útil se você gostaria de armazenar em cache esta página parcial (veja
:ref:`cookbook-templating-no-controller-caching`).

.. configuration-block::

    .. code-block:: html+jinja

        {{ render(url('acme_privacy')) }}

    .. code-block:: html+php

        <?php echo $view['actions']->render(
            $view['router']->generate('acme_privacy', array(), true)
        ) ?>

.. _cookbook-templating-no-controller-caching:

Armazenar em cache o Template estático
--------------------------------------

Uma vez que os templates renderizados dessa forma são tipicamente estáticos, pode
fazer sentido armazená-los em cache. Felizmente, isso é fácil! Configurando algumas
outras variáveis ​​em sua rota, você pode controlar exatamente como a página é armazenada em cache:

.. configuration-block::

    .. code-block:: yaml

        acme_privacy:
            path: /privacy
            defaults:
                _controller:  FrameworkBundle:Template:template
                template:     'static/privacy.html.twig'
                maxAge:       86400
                sharedAge:    86400

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="acme_privacy" path="/privacy">
                <default key="_controller">FrameworkBundle:Template:template</default>
                <default key="template">static/privacy.html.twig</default>
                <default key="maxAge">86400</default>
                <default key="sharedAge">86400</default>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('acme_privacy', new Route('/privacy', array(
            '_controller'  => 'FrameworkBundle:Template:template',
            'template'     => 'static/privacy.html.twig',
            'maxAge'       => 86400,
            'sharedAge' => 86400,
        )));

        return $collection;

Os valores ``maxAge`` e ``sharedAge`` são usados ​​para modificar o objeto
Response criado no controlador. Para mais informações sobre cache, consulte
:doc:`/book/http_cache`.

Há também uma variável ``private`` (não mostrada aqui). Por padrão, o Response
será tornado público enquanto ``maxAge`` ou ``sharedAge`` são passados.
Se definida como ``true``, o Response será marcado como privado.
