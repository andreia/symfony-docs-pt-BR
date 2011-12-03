API estável do Symfony2
=======================

A API estável do Symfony2 é um subconjunto de todos métodos públicos publicados
(componentes e bundles principais) que possuem as seguintes propriedades:

* O namespace e o nome da classe não mudarão;
* O nome do método não mudará;
* A assinatura do método (parâmetros e tipo de retorno) não mudará;
* A função do método (o que ele faz) não mudará;

A implementação em si pode mudar. O único caso que pode causar alteração na API
estável será para correção de algum problema de segurança.

A API estável é baseada em uma lista (whitelist), marcada com `@api`.
Assim, tudo que não esteja marcado com `@api` não é parte da API estável.

.. tip::

    Qualquer bundle de terceiros deveria também publicar sua própria API estável.

Atualmente na versão Symfony 2.0 os seguintes componentes têm API pública marcada:

* BrowserKit
* ClassLoader
* Console
* CssSelector
* DependencyInjection
* DomCrawler
* EventDispatcher
* Finder
* HttpFoundation
* HttpKernel
* Locale
* Process
* Routing
* Templating
* Translation
* Validator
* Yaml
