.. index::
   single: Depuração

Como otimizar seu ambiente de desenvolvimento para a depuraração
================================================================

Ao trabalhar em um projeto Symfony na sua máquina local, você deve usar o
ambiente ``dev`` (front controller ``app_dev.php``). Esta configuração de 
ambiente é otimizada para dois principais propósitos:

* Fornecer ao desenvolvedor um feedback preciso sempre que algo der errado (barra 
  de ferramentas de depuração web, páginas de exceção agradáveis, profiler, ...);

* Ser o mais semelhante possível do ambiente de produção para evitar problemas
  ao implantar o projeto.

.. _cookbook-debugging-disable-bootstrap:

Desabilitando o Arquivo de Bootstrap e o Cache de Classe
--------------------------------------------------------

E para tornar o ambiente de produção o mais rápido possível, o Symfony cria
grandes arquivos PHP em seu cache que contém a agregação das classes PHP que o
projeto precisa para cada solicitação. No entanto, este comportamento pode confundir a sua IDE
ou seu depurador. Esta receita mostra como você pode ajustar este mecanismo de
cache para torná-lo mais amigável quando você precisar depurar código que envolve
classes do Symfony.

O front controller ``app_dev.php``, por padrão, é o seguinte::

    // ...

    require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('dev', true);
    $kernel->loadClassCache();
    $kernel->handle(Request::createFromGlobals())->send();

Para deixar o seu depurador ainda mais feliz, desabilite todos os caches de classe PHP,
removendo a chamada para ``loadClassCache()`` e substituindo as declarações require como
abaixo::

    // ...

    // require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/autoload.php';
    require_once __DIR__.'/../app/AppKernel.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('dev', true);
    // $kernel->loadClassCache();
    $kernel->handle(Request::createFromGlobals())->send();

.. tip::

    Se você desativar os caches PHP, não se esqueça de voltar depois da sua sessão de
    depuração.

Algumas IDEs não gostam do fato de que algumas classes são armazenadas em locais diferentes.
Para evitar problemas, você pode dizer a sua IDE para ignorar os arquivos de cache PHP
, ou você pode mudar a extensão usada pelo Symfony para esses arquivos::

    $kernel->loadClassCache('classes', '.php.cache');
