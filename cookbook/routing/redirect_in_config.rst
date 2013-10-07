.. index::
   single: Roteamento; Configurar redirecionamento para outra rota sem um controller personalizado

Como configurar um redirecionamento para outra rota sem um controlador personalizado
====================================================================================

Este guia explica como configurar um redirecionamento de uma rota para outra
sem o uso de um controlador personalizado.

Suponha que não há nenhum controlador padrão útil para o caminho ``/`` da
sua aplicação e você quer redirecionar os pedidos para ``/app``.

Sua configuração será parecida com a seguinte:

.. code-block:: yaml

    AppBundle:
        resource: "@App/Controller/"
        type:     annotation
        prefix:   /app

    root:
        pattern: /
        defaults:
            _controller: FrameworkBundle:Redirect:urlRedirect
            path: /app
            permanent: true

Neste exemplo, você configura uma rota para o caminho ``/`` e deixa o class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\RedirectController`
lidar com ela. Este controlador vem com o Symfony e oferece duas ações
para redirecionar o pedido:

* ``urlRedirect`` redireciona para outro *caminho*. Você deve fornecer o parâmetro
  ``path`` contendo o caminho do recurso para o qual deseja redirecionar.

* ``redirect`` (não mostrado aqui) redireciona para outra *rota*. Você deve fornecer o parâmetro
  ``route`` com o *nome* da rota para a qual você quer redirecionar.

O ``permanent`` informa ambos os métodos para emitir um código de status HTTP 301
em vez do código de status padrão ``302``.
