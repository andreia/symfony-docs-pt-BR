.. index::
    single: Roteamento; Redirecionar URLs com uma barra no final

Redirecionar URLs com uma Barra no Final
========================================

O objetivo deste cookbook é demonstrar como redirecionar URLs com uma
barra no final para a mesma URL sem a barra no final
(por exemplo, ``/en/blog/`` para ``/en/blog``).

Crie um controlador que irá corresponder a qualquer URL com uma barra no final, remova
a barra do final (mantendo os parâmetros de consulta, se houver) e redirecione para a
nova URL com um código de status 301 de resposta::

    // src/Acme/DemoBundle/Controller/RedirectingController.php
    namespace Acme\DemoBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Request;

    class RedirectingController extends Controller
    {
        public function removeTrailingSlashAction(Request $request)
        {
            $pathInfo = $request->getPathInfo();
            $requestUri = $request->getRequestUri();

            $url = str_replace($pathInfo, rtrim($pathInfo, ' /'), $requestUri);

            return $this->redirect($url, 301);
        }
    }

Depois disso, crie uma rota para este controlador que corresponde sempre que uma URL
com uma barra no final é solicitada. Não se esqueça de colocar esta rota por último
no seu sistema, como explicado a seguir:

.. configuration-block::

    .. code-block:: yaml

        remove_trailing_slash:
            path: /{url}
            defaults: { _controller: AcmeDemoBundle:Redirecting:removeTrailingSlash }
            requirements:
                url: .*/$
                _method: GET


    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing">
            <route id="remove_trailing_slash" path="/{url}">
                <default key="_controller">AcmeDemoBundle:Redirecting:removeTrailingSlash</default>
                <requirement key="url">.*/$</requirement>
                <requirement key="_method">GET</requirement>
            </route>
        </routes>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add(
            'remove_trailing_slash',
            new Route(
                '/{url}',
                array(
                    '_controller' => 'AcmeDemoBundle:Redirecting:removeTrailingSlash',
                ),
                array(
                    'url' => '.*/$',
                    '_method' => 'GET',
                )
            )
        );

.. note::

    Redirecionar uma solicitação POST não funciona bem em navegadores antigos. Um 302
    em uma solicitação POST iria enviar uma solicitação GET após o redirecionamento por motivos
    legados. Por essa razão, a rota aqui corresponde apenas solicitações GET.

.. caution::

    Certifique-se de incluir esta rota em sua configuração de roteamento no
    final de sua lista de rotas. Caso contrário, você corre o risco de redirecionar
    rotas reais (incluindo as rotas principais do Symfony2) que realmente *têm* uma
    barra no final do seu caminho.
