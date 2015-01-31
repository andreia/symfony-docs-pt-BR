.. index::
    single: Cache; CSRF; Formulários

Armazenamento em Cache de Páginas que contêm Formulários Protegidos com CSRF
============================================================================

Tokens CSRF são destinados a serem diferentes para cada usuário. Esse é o motivo porque você
precisa ser cauteloso ao tentar fazer cache de páginas que incluem formulários.

Para obter mais informações sobre como a proteção CSRF funciona no Symfony, por favor
verifique :ref:`Proteção CSRF<forms-csrf>`.

Por que o Cache de Páginas com um Token CSRF é Problemático
-----------------------------------------------------------

Tipicamente, para cada usuário é atribuído um token CSRF único, que é armazenado em
sessão para validação. Isso significa que se você *fazer* cache de uma página com
um formulário contendo um token CSRF, você vai armazenar em cache o token CSRF do *primeiro*
usuário somente. Quando um usuário submete o formulário, o token não corresponde ao token
armazenado na sessão e, para todos os usuários (exceto o primeiro), irá falhar a validação CSRF
ao enviar o formulário.

De fato, muitos proxies reversos (como o Varnish) vão recusar a colocar em cache uma página
com um token CSRF. Isso porque um cookie é enviado, a fim de preservar
a sessão PHP aberta e o comportamento padrão do Varnish e não armazenar em cache solicitações
HTTP com cookies.

Como fazer Cache da Maior Parte da Página e ainda poder Usar a Proteção CSRF
----------------------------------------------------------------------------

Para armazenar em cache uma página que contém um token CSRF, você pode usar técnicas de cache mais avançadas
como :ref:`fragmentos ESI <edge-side-includes>`, onde você armazena em cache
a página inteira e incorpora o formulário dentro de uma tag ESI sem cache.

Outra opção seria carregar o formulário através de uma solicitação AJAX sem cache, mas
fazer o cache do resto da resposta HTML.

Ou, você ainda pode carregar apenas o token CSRF com uma requisição AJAX e substituir o
valor do campo do formulário com ele.

.. _`Cross-site request forgery`: http://en.wikipedia.org/wiki/Cross-site_request_forgery
.. _`Security CSRF Component`: https://github.com/symfony/security-csrf
