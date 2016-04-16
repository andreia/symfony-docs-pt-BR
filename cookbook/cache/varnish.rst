.. index::
    single: Cache; Varnish

Como usar Varnish para aumentar a velocidade do meu Website
===========================================================

Pelo cache do Symfony2 usar os cache headers padrões do HTTP, o
:ref:`symfony-gateway-cache` pode facilmente se substituido por qualquer por qualquer outro
proxy reverso. Varnish é um poderoso, open-source, HTTP accelerator capaz de servir
conteúdo em cache de forma rápida em incluindo suporte à :ref:`Edge Side
Includes<edge-side-includes>` (ESI).

.. index::
    single: Varnish; configuration

Configuração
------------

Como visto anteriormente, Symfony2 é esperto o bastente para detectar se fala com um
proxy reverso que compreende ESI ou não. Ele trabalha fora da caixa quando você
usa o proxy reverso do Symfony2, mas você precisa de um configuração especial para poder
trabalhar com Varnish. Felizmente, Symfony2 depende ainda de outro padrão
escrito por Akamaï (`Edge Architecture`_), então as dicas de configuração desde
capítulo podem ser úteis mesmo se você não usar Symfony2.

.. note::

    Varnish suporta apenas o atributo ``src`` para tags ESI (atributos ``onerror`` e
    ``alt`` são ignorados).

Primeiro, configure o Varnish para que ele informe o suporte à ESI adicionando um
``Surrogate-Capability`` ao cabeçalho nas requisições enviadas ao servidor de
aplicação:

.. code-block:: text

    sub vcl_recv {
        set req.http.Surrogate-Capability = "abc=ESI/1.0";
    }

Após isso, otimize o Varnish para que ele apenas analise o conteúdo da resposta quando
existir ao menos uma tag ESI, verificando o cabeçalho ``Surrogate-Control`` que
é adicionado automaticamente pelo Symfony2.

.. code-block:: text

    sub vcl_fetch {
        if (beresp.http.Surrogate-Control ~ "ESI/1.0") {
            unset beresp.http.Surrogate-Control;

            // para Varnish >= 3.0
            set beresp.do_esi = true;
            // para Varnish < 3.0
            // esi;
        }
    }

.. caution::

    Compressão com ESI não é suportada pelo Varnish até a versão 3.0
    (leia `GZIP and Varnish`_). Se você não está utilizando Varnish 3.0,
    coloque um servidor web a frente do Varnish para executar a compressão.

.. index::
    single: Varnish; Invalidation

Invalidação do Cache
--------------------

Você nunca deverá precisar invalidar os dados em cache porque a invalidação é colocada
nativamente nos modelos de cache HTTP (veja :ref:`http-cache-invalidation`).

Ainda assim, o Varnish pode ser configurado para aceitar um método HTTP ``PURGE`` especial
que invalidará o cache para derterminado recurso:

.. code-block:: text

    sub vcl_hit {
        if (req.request == "PURGE") {
            set obj.ttl = 0s;
            error 200 "Purgado";
        }
    }

    sub vcl_miss {
        if (req.request == "PURGE") {
            error 404 "Não Purgado";
        }
    }

.. caution::

    Você deve proteger o método HTTP``PURGE`` para evitar que qualquer pessoa possa
    purgar os dados em cache.

.. _`Edge Architecture`: http://www.w3.org/TR/edge-arch
.. _`GZIP and Varnish`: https://www.varnish-cache.org/docs/3.0/phk/gzip.html
