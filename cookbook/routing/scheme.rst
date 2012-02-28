.. index::
   single: Roteamento; Exigência do Esquema

Como forçar as rotas a usar sempre HTTPS ou HTTP
================================================

Às vezes, você deseja proteger algumas rotas e ter certeza de que elas serão sempre
acessadas através do protocolo HTTPS. O componente de Roteamento permite que você aplique
o esquema URI através da condição ``_scheme``:

.. configuration-block::

    .. code-block:: yaml

        secure:
            pattern:  /secure
            defaults: { _controller: AcmeDemoBundle:Main:secure }
            requirements:
                _scheme:  https

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="secure" pattern="/secure">
                <default key="_controller">AcmeDemoBundle:Main:secure</default>
                <requirement key="_scheme">https</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('secure', new Route('/secure', array(
            '_controller' => 'AcmeDemoBundle:Main:secure',
        ), array(
            '_scheme' => 'https',
        )));

        return $collection;

A configuração acima força a rota ``secure`` à sempre usar HTTPS.

Ao gerar a URL ``secure``, e se o schema atual for HTTP, o Symfony
irá gerar automaticamente uma URL absoluta com HTTPS como esquema:

.. code-block:: text

    # If the current scheme is HTTPS
    {{ path('secure') }}
    # generates /secure

    # If the current scheme is HTTP
    {{ path('secure') }}
    # generates https://example.com/secure

A condição também é aplicada para as solicitações de entrada. Se você tentar acessar
o caminho ``/secure`` com HTTP, você será automaticamente redirecionado para a
mesma URL, mas com o esquema HTTPS.

O exemplo acima utiliza ``https`` para o ``_scheme``, mas você também pode forçar uma
URL à sempre utilizar ``http``.

.. note::

    O componente Security fornece outra forma de aplicar HTTP ou HTTPs através
    da configuração ``requires_channel``. Este método alternativo é mais adequado
    para proteger uma "área" do seu site (todas as URLs sob o ``/admin``) ou quando
    você quiser proteger URLs definidas em um bundle de terceiros.