A API Estável do Symfony2
=========================

A API estável do Symfony2 é um subconjunto de todos os métodos públicos
publicados no Symfony2 (componentes e core bundles) que compartilham as
seguintes propriedades:

* O namespace e nome de classe não mudarão;
* O nome do método não mudará;
* A assinatura do método (argumentos e tipo de valor de retorno) não mudará;
* A semântica do que é o método não mudará.

A implementação em si pode ser alterada. O caso só é válido para uma mudança na
API estável para corrigir algum problema de segurança.

A API estável é baseada em uma whitelist, marcada com `@api`. Então, tudo não
marcado explícitamente não faz parte da API estável.

.. tip::

    Qualquer bundle de terceiros também devem publicar sua própria API estável.

A partir do Symfony 2.0, os seguintes componentes têm uma API pública estável marcada:

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
