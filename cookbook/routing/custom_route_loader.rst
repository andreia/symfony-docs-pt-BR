.. index::
   single: Roteamento; Loader de Rota Personalizado

Como criar um Loader de Rota personalizado
==========================================

Um loader de rota personalizado permite que você adicione rotas a uma aplicação sem
incluí-las, por exemplo, num arquivo yaml. Isto é útil quando
você tem um bundle, mas não quer adicionar manualmente as rotas ao bundle
em ``app/config/routing.yml``. Isso pode ser especialmente importante quando você quer
tornar o bundle reutilizável, ou quando o disponibilizar como código aberto, porque iria
retardar o processo de instalação e torná-lo suscetível a erros.

Alternativamente, você também pode usar um loader de rota personalizado quando quiser que
as suas rotas sejam geradas ou localizadas automaticamente com base em alguma convenção
ou padrão. Um exemplo é o `FOSRestBundle`_ onde o roteamento é gerado com base
nos nomes dos métodos de ação em um controlador.

.. note ::

    Há muitos bundles que usam seus próprios loaders de rotas para
    realizar casos como os descritos acima, por exemplo
    `FOSRestBundle`_, `KnpRadBundle`_ e `SonataAdminBundle`_.

Carregando Rotas
----------------

As rotas em uma aplicação Symfony são carregadas pelo
:class:`Symfony\\Bundle\\FrameworkBundle\\Routing\\DelegatingLoader`.
Este loader usa vários outros loaders (delegados) para carregar recursos de
diferentes tipos, por exemplo, arquivos YAML ou anotações ``@Route`` e ``@Method``
em arquivos de controlador. Os loaders especializados implementam a
:class:`Symfony\\Component\\Config\\Loader\\LoaderInterface`
e, portanto, tem dois métodos importantes:
:method:`Symfony\\Component\\Config\\Loader\\LoaderInterface::supports`
e :method:`Symfony\\Component\\Config\\Loader\\LoaderInterface::load`.

Considere essas linhas do ``routing.yml``:

.. code-block:: yaml

    _demo:
        resource: "@AcmeDemoBundle/Controller/DemoController.php"
        type:     annotation
        prefix:   /demo

Quando o loader principal realiza o parse, ele tenta todos os loaders delegados e chama
seu método :method:`Symfony\\Component\\Config\\Loader\\LoaderInterface::supports`
com o determinado recurso (``@AcmeDemoBundle/Controller/DemoController.php``)
e tipo (``annotation``) como argumentos. Quando um dos loaders retorna ``true``,
seu método :method:`Symfony\\Component\\Config\\Loader\\LoaderInterface::load`
será chamado, que deve retornar um :class:`Symfony\\Component\\Routing\\RouteCollection`
contendo objetos :class:`Symfony\\Component\\Routing\\Route`.

Criando um Loader Personalizado
-------------------------------

Para carregar rotas de alguma fonte personalizada (ou seja, de algo diferente de anotações,
arquivos YAML ou XML), você precisa criar um loader de rota personalizado. Este loader
deve implementar a :class:`Symfony\\Component\\Config\\Loader\\LoaderInterface`.

O loader de exemplo abaixo, suporta o carregamento de recursos de roteamento com um tipo
``extra``. O tipo ``extra`` não é importante - você pode simplesmente inventar qualquer tipo
de recurso que desejar. O próprio nome do recurso não é realmente usado no exemplo::

    namespace Acme\DemoBundle\Routing;

    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\Config\Loader\LoaderResolver;
    use Symfony\Component\Routing\Route;
    use Symfony\Component\Routing\RouteCollection;

    class ExtraLoader implements LoaderInterface
    {
        private $loaded = false;

        public function load($resource, $type = null)
        {
            if (true === $this->loaded) {
                throw new \RuntimeException('Do not add the "extra" loader twice');
            }

            $routes = new RouteCollection();

            // prepare a new route
            $pattern = '/extra/{parameter}';
            $defaults = array(
                '_controller' => 'AcmeDemoBundle:Demo:extra',
            );
            $requirements = array(
                'parameter' => '\d+',
            );
            $route = new Route($pattern, $defaults, $requirements);

            // add the new route to the route collection:
            $routeName = 'extraRoute';
            $routes->add($routeName, $route);

            return $routes;
        }

        public function supports($resource, $type = null)
        {
            return 'extra' === $type;
        }

        public function getResolver()
        {
            // needed, but can be blank, unless you want to load other resources
            // and if you do, using the Loader base class is easier (see below)
        }

        public function setResolver(LoaderResolver $resolver)
        {
            // same as above
        }
    }

.. note::

    Certifique-se que o controlador que você especificou realmente existe.

Agora defina um serviço para o ``ExtraLoader``:

.. configuration-block::

    .. code-block:: yaml

        services:
            acme_demo.routing_loader:
                class: Acme\DemoBundle\Routing\ExtraLoader
                tags:
                    - { name: routing.loader }

    .. code-block:: xml

        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="acme_demo.routing_loader" class="Acme\DemoBundle\Routing\ExtraLoader">
                    <tag name="routing.loader" />
                </service>
            </services>
        </container>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition;

        $container
            ->setDefinition(
                'acme_demo.routing_loader',
                new Definition('Acme\DemoBundle\Routing\ExtraLoader')
            )
            ->addTag('routing.loader')
        ;

Observe a tag ``routing.loader``. Todos os serviços com esta tag serão marcados
como potenciais loaders de rota e adicionados como roteadores especializados para o
:class:`Symfony\\Bundle\\FrameworkBundle\\Routing\\DelegatingLoader`.

Usando o Loader Personalizado
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você não fez nada mais, seu loader de roteamento personalizado *não* será chamado.
Em vez disso, você só precisa adicionar algumas linhas extras para a configuração de roteamento:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        AcmeDemoBundle_Extra:
            resource: .
            type: extra

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="." type="extra" />
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->addCollection($loader->import('.', 'extra'));

        return $collection;

A parte importante aqui é a chave ``type``. Seu valor deve ser "extra".
Este é o tipo que nosso ``ExtraLoader`` suporta e que irá certificar-se
que seu método ``load()`` é chamado. A chave ``resource`` é insignificante
para o ``ExtraLoader``, então defina ela como ".".

.. note ::

    O cache das rotas definidas usando loaders personalizados será feita automaticamente pelo
    framework. Assim, sempre que você mudar alguma coisa na classe do
    loader, não se esqueça de limpar o cache.

Loaders mais Avançados
----------------------

Na maioria dos casos é melhor não implementar a
:class:`Symfony\\Component\\Config\\Loader\\LoaderInterface`
você mesmo, mas estender do :class:`Symfony\\Component\\Config\\Loader\\Loader`.
Esta classe sabe como usar um :class:`Symfony\\Component\\Config\\Loader\\LoaderResolver`
para carregar os recursos de roteamento secundários.

Claro que você ainda precisa implementar
:method:`Symfony\\Component\\Config\\Loader\\LoaderInterface::supports`
e :method:`Symfony\\Component\\Config\\Loader\\LoaderInterface::load`.
Sempre que quiser carregar outro recurso - por exemplo, um arquivo de configuração 
Yaml - você pode chamar o
:method:`Symfony\\Component\\Config\\Loader\\Loader::import` method::

    namespace Acme\DemoBundle\Routing;

    use Symfony\Component\Config\Loader\Loader;
    use Symfony\Component\Routing\RouteCollection;

    class AdvancedLoader extends Loader
    {
        public function load($resource, $type = null)
        {
            $collection = new RouteCollection();

            $resource = '@AcmeDemoBundle/Resources/config/import_routing.yml';
            $type = 'yaml';

            $importedRoutes = $this->import($resource, $type);

            $collection->addCollection($importedRoutes);

            return $collection;
        }

        public function supports($resource, $type = null)
        {
            return $type === 'advanced_extra';
        }
    }

.. note::

    O nome e o tipo do recurso da configuração de roteamento importada pode
    ser qualquer coisa que é normalmente suportada pelo loader de configuração
    de roteamento (YAML, XML, PHP, anotação, etc.)

.. _`FOSRestBundle`: https://github.com/FriendsOfSymfony/FOSRestBundle
.. _`KnpRadBundle`: https://github.com/KnpLabs/KnpRadBundle
.. _`SonataAdminBundle`: https://github.com/sonata-project/SonataAdminBundle
