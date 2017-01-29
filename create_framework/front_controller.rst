O Front Controller
==================

Até agora, a nossa aplicação é simplista, pois, temos apenas uma página. Para
apimentar as coisas um pouco, vamos adicionar uma outra página que diz
adeus::

    // framework/bye.php

    require_once __DIR__.'/vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    $request = Request::createFromGlobals();

    $response = new Response('Goodbye!');
    $response->send();

Como você pode ver, grande parte do código é exatamente o mesmo que nós escrevemos
na primeira página. Vamos extrair o código comum que podemos compartilhar entre
todas as nossas páginas. Compartilhamento de código soa como um bom plano para criar
o nosso primeiro framework "real"!

A forma de fazer o refatoramento com o PHP seria, provavelmente, criando um
arquivo include::

    // framework/init.php

    require_once __DIR__.'/vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    $request = Request::createFromGlobals();
    $response = new Response();

Vamos vê-lo em ação::

    // framework/index.php

    require_once __DIR__.'/init.php';

    $input = $request->get('name', 'World');

    $response->setContent(sprintf('Hello %s', htmlspecialchars($input, ENT_QUOTES, 'UTF-8')));
    $response->send();

E para a página de "adeus"::

    // framework/bye.php

    require_once __DIR__.'/init.php';

    $response->setContent('Goodbye!');
    $response->send();

De fato movemos a maior parte do código compartilhado para um lugar central,
mas, não parece ser uma boa abstração, não é? Ainda temos o método 
``send()`` em todas as páginas, as nossas páginas não se parecem com 
templates, e nós ainda não podemos testar esse código adequadamente.

Além disso, adicionar uma nova página significa que, precisamos criar um novo 
script PHP, o qual terá o nome exposto para o usuário final através da URL
(``http://127.0.0.1:4321/bye.php``): não há um mapeamento direto entre o nome do script
PHP e a URL do cliente. Isso ocorre devido ao envio do pedido ser feito diretamente
pelo servidor web. Pode ser uma boa idéia mover esse envio para o nosso código
para melhorar a flexibilidade. Isso pode ser facilmente realizado através do
encaminhamento de todos os pedidos do cliente para um único script PHP.

.. tip::

    A exposição de um único script PHP para o usuário final é um padrão de projeto
    chamado de "`front controller`_".

Um script como este pode parecer com o seguinte::

    // framework/front.php

    require_once __DIR__.'/vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    $request = Request::createFromGlobals();
    $response = new Response();

    $map = array(
        '/hello' => __DIR__.'/hello.php',
        '/bye'   => __DIR__.'/bye.php',
    );

    $path = $request->getPathInfo();
    if (isset($map[$path])) {
        require $map[$path];
    } else {
        $response->setStatusCode(404);
        $response->setContent('Not Found');
    }

    $response->send();

E aqui está, por exemplo, o novo script ``hello.php``::

    // framework/hello.php

    $input = $request->get('name', 'World');
    $response->setContent(sprintf('Hello %s', htmlspecialchars($input, ENT_QUOTES, 'UTF-8')));

No script ``front.php``, o ``$map`` associa os caminhos da URL com os 
caminhos dos scripts PHP correspondentes.

Como bônus, se o cliente chamar um caminho que não está definido no mapa de URLs,
retornaremos uma página 404 personalizada, você está agora no controle do seu website.

Para acessar uma página, você deve usar agora o script ``front.php``:

* ``http://127.0.0.1:4321/front.php/hello?name=Fabien``

* ``http://127.0.0.1:4321/front.php/bye``

``/hello`` e ``/bye`` são os *caminhos* das páginas.

.. tip::

    A maioria dos servidores web como o Apache ou nginx são capazes de reescrever 
    as URLs de entrada e remover o script *front controller* para que os usuários 
    possam escrever ``http://example.com/hello?name=Fabien``, que tem um aspecto muito
    melhor.

Então, o truque é usar o método ``Request::getPathInfo()`` que
retorna o caminho do Pedido removendo o nome do script *front controller*
e incluindo os seus sub-diretórios (apenas se necessário -- ver a dica acima).

.. tip::

    Você nem precisa configurar um servidor web para testar o código. Em vez disso,
    substitua a chamada ``$request = Request::createFromGlobals();`` para algo
    como ``$request = Request::create('/hello?name=Fabien');`` onde o
    argumento é o caminho da URL que você deseja simular.

Agora que o servidor web sempre acessa o mesmo script (``front.php``) para todas as
nossas páginas, podemos proteger o nosso código ainda mais, movendo todos os outros 
arquivos PHP para fora do diretório raiz web::

    example.com
    ├── composer.json
    │   src
    │   └── pages
    │       ├── hello.php
    │       └── bye.php
    ├── vendor
    └── web
        └── front.php

Agora, configure o seu diretório raiz do servidor web para apontar para ``web/`` e todos os
outros arquivos não serão mais acessíveis pelo cliente.

Para testar as suas alterações no navegador (``http://localhost:4321/?name=Fabien``), execute
o servidor web embutido do PHP:

.. code-block:: bash

    $ php -S 127.0.0.1:4321 -t web/ web/front.php

.. note::

    Para esta nova estrutura funcionar, você terá que ajustar alguns caminhos em
    vários arquivos PHP; as mudanças são deixadas como um exercício para o leitor.

A última coisa que se repete em cada página é a chamada para ``setContent()``.
Podemos converter todas as páginas para "templates" apenas exibindo o conteúdo e
chamando o ``setContent()`` diretamente do script *front controller*::

    // example.com/web/front.php

    // ...

    $path = $request->getPathInfo();
    if (isset($map[$path])) {
        ob_start();
        include $map[$path];
        $response->setContent(ob_get_clean());
    } else {
        $response->setStatusCode(404);
        $response->setContent('Not Found');
    }

    // ...

E o script ``hello.php`` agora pode ser convertido para um template::

    <!-- example.com/src/pages/hello.php -->

    <?php $name = $request->get('name', 'World') ?>

    Hello <?php echo htmlspecialchars($name, ENT_QUOTES, 'UTF-8') ?>

Temos a primeira versão do nosso framework::

    // example.com/web/front.php

    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    $request = Request::createFromGlobals();
    $response = new Response();

    $map = array(
        '/hello' => __DIR__.'/../src/pages/hello.php',
        '/bye'   => __DIR__.'/../src/pages/bye.php',
    );

    $path = $request->getPathInfo();
    if (isset($map[$path])) {
        ob_start();
        include $map[$path];
        $response->setContent(ob_get_clean());
    } else {
        $response->setStatusCode(404);
        $response->setContent('Not Found');
    }

    $response->send();

A adição de uma nova página é um processo de duas etapas: adicionar uma entrada em ``map`` e 
criar um template PHP em ``src/pages/``. No template, obtenha os dados do Pedido
através da variável ``$request`` e ajuste os cabeçalhos da Resposta através da variável 
``$response``.

.. note::

    Se você decidir parar por aqui, provavelmente poderá melhorar o seu framework
    extraindo o mapa de URLs para um arquivo de configuração.

.. _`front controller`: http://symfony.com/doc/current/book/from_flat_php_to_symfony2.html#a-front-controller-to-the-rescue
