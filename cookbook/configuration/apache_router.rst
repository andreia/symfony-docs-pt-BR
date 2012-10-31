.. index::
   single: Apache Router

Como usar o Apache Router
=========================

O Symfony2, mesmo sendo rápido logo após a sua instalação, ainda fornece várias formas de aumentar essa velocidade com poucos ajustes.
Uma dessas formas é deixar o apache lidar com as rotas diretamente, em vez de usar o Symfony2 para esta tarefa.

Alterando os Parâmetros de Configuração do Router
-------------------------------------------------

Para fazer o dump das rotas do Apache precisamos primeiro ajustar alguns parâmetros de configuração e dizer ao
Symfony2 para usar o ``ApacheUrlMatcher`` em vez do padrão:

.. code-block:: yaml
    
    # app/config/config_prod.yml
    parameters:
        router.options.matcher.cache_class: ~ # disable router cache
        router.options.matcher_class: Symfony\Component\Routing\Matcher\ApacheUrlMatcher

.. tip::

    Note que o :class:`Symfony\\Component\\Routing\\Matcher\\ApacheUrlMatcher`
    estende :class:`Symfony\\Component\\Routing\\Matcher\\UrlMatcher` assim, mesmo
    se você não regerar as regras url_rewrite, tudo vai funcionar (porque
    no final do ``ApacheUrlMatcher::match()`` é realizada uma chamada para o 
    ``parent::match()``). 
    
Gerando regras do mod_rewrite
-----------------------------

Para testar se está funcionando, vamos criar uma rota bem básica para o bundle demo:

.. code-block:: yaml
    
    # app/config/routing.yml
    hello:
        pattern:  /hello/{name}
        defaults: { _controller: AcmeDemoBundle:Demo:hello }

Agora vamos gerar regras **url_rewrite**:

.. code-block:: bash

    $ php app/console router:dump-apache -e=prod --no-debug

Que deve produzir aproximadamente o seguinte:

.. code-block:: apache

    # skip "real" requests
    RewriteCond %{REQUEST_FILENAME} -f
    RewriteRule .* - [QSA,L]

    # hello
    RewriteCond %{REQUEST_URI} ^/hello/([^/]+?)$
    RewriteRule .* app.php [QSA,L,E=_ROUTING__route:hello,E=_ROUTING_name:%1,E=_ROUTING__controller:AcmeDemoBundle\:Demo\:hello]

Agora você pode reescrever o `web/.htaccess` para usar as novas regras, portanto, com o nosso exemplo,
ele deve parecer com o seguinte:

.. code-block:: apache

    <IfModule mod_rewrite.c>
        RewriteEngine On

        # skip "real" requests
        RewriteCond %{REQUEST_FILENAME} -f
        RewriteRule .* - [QSA,L]

        # hello
        RewriteCond %{REQUEST_URI} ^/hello/([^/]+?)$
        RewriteRule .* app.php [QSA,L,E=_ROUTING__route:hello,E=_ROUTING_name:%1,E=_ROUTING__controller:AcmeDemoBundle\:Demo\:hello]
    </IfModule>

.. note::

   O procedimento acima deve ser feito cada vez que você adicionar/alterar uma rota se deseja aproveitar o máximo desta configuração.

É isso!
Você agora está pronto para usar as regras do Apache Route.

Ajustes adicionais
------------------

Para economizar um pouco de tempo de processamento, altere as ocorrências de ``Request``
para ``ApacheRequest`` no ``web/app.php``::

    // web/app.php
    
    require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';
    //require_once __DIR__.'/../app/AppCache.php';

    use Symfony\Component\HttpFoundation\ApacheRequest;

    $kernel = new AppKernel('prod', false);
    $kernel->loadClassCache();
    //$kernel = new AppCache($kernel);
    $kernel->handle(ApacheRequest::createFromGlobals())->send();
