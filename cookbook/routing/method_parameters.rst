.. index::
   single: Roteamento; _method

Como usar métodos HTTP além do GET e POST em Rotas
==================================================

O método de um pedido HTTP é um dos requisitos que pode ser verificado
ao ver se ele corresponde a uma rota. Isto é introduzido no capítulo de roteamento
do livro ":doc:`/book/routing`" com exemplos usando GET e POST. Você também pode
usar outros verbos HTTP desta forma. Por exemplo, se você tem um post de blog,
então, você pode usar o mesmo padrão de URL para mostrá-lo, fazer alterações
e removê-lo pela correspondência nos métodos GET, PUT e DELETE.

.. configuration-block::

    .. code-block:: yaml

        blog_show:
            pattern:  /blog/{slug}
            defaults: { _controller: AcmeDemoBundle:Blog:show }
            requirements:
                _method:  GET

        blog_update:
            pattern:  /blog/{slug}
            defaults: { _controller: AcmeDemoBundle:Blog:update }
            requirements:
                _method:  PUT

        blog_delete:
            pattern:  /blog/{slug}
            defaults: { _controller: AcmeDemoBundle:Blog:delete }
            requirements:
                _method:  DELETE

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_show" pattern="/blog/{slug}">
                <default key="_controller">AcmeDemoBundle:Blog:show</default>
                <requirement key="_method">GET</requirement>
            </route>

            <route id="blog_update" pattern="/blog/{slug}">
                <default key="_controller">AcmeDemoBundle:Blog:update</default>
                <requirement key="_method">PUT</requirement>
            </route>

            <route id="blog_delete" pattern="/blog/{slug}">
                <default key="_controller">AcmeDemoBundle:Blog:delete</default>
                <requirement key="_method">DELETE</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('blog_show', new Route('/blog/{slug}', array(
            '_controller' => 'AcmeDemoBundle:Blog:show',
        ), array(
            '_method' => 'GET',
        )));

        $collection->add('blog_update', new Route('/blog/{slug}', array(
            '_controller' => 'AcmeDemoBundle:Blog:update',
        ), array(
            '_method' => 'PUT',
        )));

        $collection->add('blog_delete', new Route('/blog/{slug}', array(
            '_controller' => 'AcmeDemoBundle:Blog:delete',
        ), array(
            '_method' => 'DELETE',
        )));

        return $collection;

Infelizmente, a vida não é tão simples assim, já que a maioria dos navegadores não
suporta o envio de solicitações PUT e DELETE. Felizmente o Symfony2 fornece
uma maneira simples de trabalhar com esta limitação. Ao incluir um parâmetro
``_method`` na query string ou nos parâmetros de um pedido HTTP, o Symfony2 irá
usá-lo como o método ao fazer a correspondência de rotas. Isto pode ser feito facilmente em
formulários com um campo oculto. Suponha que você tenha um formulário para editar um post no blog:

.. code-block:: html+jinja

    <form action="{{ path('blog_update', {'slug': blog.slug}) }}" method="post">
        <input type="hidden" name="_method" value="PUT" />
        {{ form_widget(form) }}
        <input type="submit" value="Update" />
    </form>

O pedido submetido agora vai corresponder à rota ``blog_update`` e a ``updateAction``
será utilizada para processar o formulário.

Do mesmo modo, o formulário de exclusão pode ser alterado para parecer com o seguinte:

.. code-block:: html+jinja

    <form action="{{ path('blog_delete', {'slug': blog.slug}) }}" method="post">
        <input type="hidden" name="_method" value="DELETE" />
        {{ form_widget(delete_form) }}
        <input type="submit" value="Delete" />
    </form>

Ele irá então corresponder à rota ``blog_delete``.
