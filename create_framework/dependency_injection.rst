O Componente DependencyInjection
================================

No capítulo anterior, esvaziamos a classe ``Simplex\\Framework`` 
estendendo a classe ``HttpKernel`` do componente de mesmo nome. Vendo esta
classe vazia, você pode ser tentado a mover algum código do
*front controller* para ela::

    // example.com/src/Simplex/Framework.php

    namespace Simplex;

    use Symfony\Component\Routing;
    use Symfony\Component\HttpKernel;
    use Symfony\Component\EventDispatcher\EventDispatcher;

    class Framework extends HttpKernel\HttpKernel
    {
        public function __construct($routes)
        {
            $context = new Routing\RequestContext();
            $matcher = new Routing\Matcher\UrlMatcher($routes, $context);
            $resolver = new HttpKernel\Controller\ControllerResolver();

            $dispatcher = new EventDispatcher();
            $dispatcher->addSubscriber(new HttpKernel\EventListener\RouterListener($matcher));
            $dispatcher->addSubscriber(new HttpKernel\EventListener\ResponseListener('UTF-8'));

            parent::__construct($dispatcher, $resolver);
        }
    }

O código do *front controller* tornaria-se mais conciso::

    // example.com/web/front.php

    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;

    $request = Request::createFromGlobals();
    $routes = include __DIR__.'/../src/app.php';

    $framework = new Simplex\Framework($routes);

    $framework->handle($request)->send();

Ter um *front controller* conciso permite que você tenha vários *front controllers*
para uma única aplicação. Por que seria útil? Para permitir diferentes
configurações para o ambiente de desenvolvimento e de produção, por exemplo. 
No ambiente de desenvolvimento, você pode desejar ter um relatório de erro
ativo e os erros exibidos no navegador para facilitar a depuração::

    ini_set('display_errors', 1);
    error_reporting(-1);

... mas você certamente não vai desejar a mesma configuração no ambiente de 
produção. Ter dois *front controllers* diferentes lhe fornece a oportunidade
de possuir uma configuração ligeiramente diferente para cada um deles.

Então, mover o código do *front controller* para a classe framework, torna o nosso
framework mais configurável, mas, ao mesmo tempo, introduz uma série de
problemas:

* Não podemos mais registrar *listeners* personalizados pois o *dispatcher* não
  está disponível fora da classe Framework (uma solução fácil pode ser a
  adição de um método ``Framework::getEventDispatcher()``);

* Perdemos a flexibilidade que tínhamos antes, você não pode mais alterar a 
  implementação do ``UrlMatcher`` ou do ``ControllerResolver``
  .

* Em relação ao ponto anterior, não podemos mais testar o nosso framework facilmente
  pois é impossível realizar *mock* de objetos internos;

* Não podemos mais mudar o charset passado ​​para o ``ResponseListener`` (uma
  solução poderia ser passá-lo como um argumento do construtor).

O código anterior não apresenta os mesmos problemas porque usamos injeção de dependência
; todas as dependências dos nossos objetos foram injetadas em seus
construtores (por exemplo, os *dispatchers* de eventos foram injetados no
framework para que tivéssemos controle total da sua criação e configuração).

Isso significa que temos que fazer uma escolha entre personalização, flexibilidade,
facilidade de testes e não copiar e colar o mesmo código no *front controller* em cada 
aplicação? Como você poderia esperar, há uma solução. Podemos resolver todos
esses problemas e mais alguns utilizando o *container* de injeção de dependência do
Symfony:

.. code-block:: bash

    $ composer require symfony/dependency-injection

Crie um novo arquivo para hospedar a configuração do *container* de injeção de dependência::

    // example.com/src/container.php

    use Symfony\Component\DependencyInjection;
    use Symfony\Component\DependencyInjection\Reference;

    $sc = new DependencyInjection\ContainerBuilder();
    $sc->register('context', 'Symfony\Component\Routing\RequestContext');
    $sc->register('matcher', 'Symfony\Component\Routing\Matcher\UrlMatcher')
        ->setArguments(array($routes, new Reference('context')))
    ;
    $sc->register('resolver', 'Symfony\Component\HttpKernel\Controller\ControllerResolver');

    $sc->register('listener.router', 'Symfony\Component\HttpKernel\EventListener\RouterListener')
        ->setArguments(array(new Reference('matcher')))
    ;
    $sc->register('listener.response', 'Symfony\Component\HttpKernel\EventListener\ResponseListener')
        ->setArguments(array('UTF-8'))
    ;
    $sc->register('listener.exception', 'Symfony\Component\HttpKernel\EventListener\ExceptionListener')
        ->setArguments(array('Calendar\\Controller\\ErrorController::exceptionAction'))
    ;
    $sc->register('dispatcher', 'Symfony\Component\EventDispatcher\EventDispatcher')
        ->addMethodCall('addSubscriber', array(new Reference('listener.router')))
        ->addMethodCall('addSubscriber', array(new Reference('listener.response')))
        ->addMethodCall('addSubscriber', array(new Reference('listener.exception')))
    ;
    $sc->register('framework', 'Simplex\Framework')
        ->setArguments(array(new Reference('dispatcher'), new Reference('resolver')))
    ;

    return $sc;

O objetivo desse arquivo é configurar os seus objetos e respectivas dependências.
Nada é instanciado durante esta etapa de configuração. Isso é puramente uma
descrição estática dos objetos que você precisa manipular e como criar
eles. Os objetos serão criados sob demanda quando você acessá-los a partir do
*container* ou quando o *container* precisar deles para criar outros objetos.

Por exemplo, para criar o *listener router*, dizemos ao Symfony que o nome da sua 
classe é ``Symfony\Component\HttpKernel\EventListener\RouterListener``, e que seu 
construtor recebe um objeto correspondente (``new Reference('matcher')``). Como
pode-se ver, cada objeto é referenciado por um nome, uma string que exclusivamente
identifica cada objeto. O nome nos permite obter um objeto e referenciá-lo
em outras definições de objetos.

.. note::

    Por padrão, toda vez que receber um objeto do *container*, ele retorna
    exatamente a mesma instância. Isso porque um *container* gerencia os seus 
    objetos "globais".

O *front controller* agora é apenas o fio que junta todos os elementos::

    // example.com/web/front.php

    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\HttpFoundation\Request;

    $routes = include __DIR__.'/../src/app.php';
    $sc = include __DIR__.'/../src/container.php';

    $request = Request::createFromGlobals();

    $response = $sc->get('framework')->handle($request);

    $response->send();

Como todos os objetos são criados agora no *container* de injeção de dependência, o
código do framework deve ser a versão simples anterior::

    // example.com/src/Simplex/Framework.php

    namespace Simplex;

    use Symfony\Component\HttpKernel\HttpKernel;

    class Framework extends HttpKernel
    {
    }

.. note::

    Se você quer uma alternativa leve para o seu *container*, considere usar o `Pimple`_, um
    *container* de injeção de dependência simples em cerca de 60 linhas de código PHP.

Agora, aqui está como você pode registrar um *listener* personalizado no *front controller*::

    $sc->register('listener.string_response', 'Simplex\StringResponseListener');
    $sc->getDefinition('dispatcher')
        ->addMethodCall('addSubscriber', array(new Reference('listener.string_response')))
    ;

Além de descrever seus objetos, o *container* de injeção de dependência também pode ser
configurado via parâmetros. Vamos criar um que define se estamos em modo de depuração
ou não::

    $sc->setParameter('debug', true);

    echo $sc->getParameter('debug');

Esses parâmetros podem ser usados ​​para especificar as definições de objetos. Vamos tornar o
``charset`` configurável::

    $sc->register('listener.response', 'Symfony\Component\HttpKernel\EventListener\ResponseListener')
        ->setArguments(array('%charset%'))
    ;

Após essa alteração, você deve definir o ``charset`` antes de utilizar o *listener* do objeto 
de resposta::

    $sc->setParameter('charset', 'UTF-8');

Em vez de confiar na convenção que as rotas são definidas pelas
variáveis ``$routes``, vamos usar um parâmetro novamente::

    $sc->register('matcher', 'Symfony\Component\Routing\Matcher\UrlMatcher')
        ->setArguments(array('%routes%', new Reference('context')))
    ;

E a alteração relacionada no *front controller*::

    $sc->setParameter('routes', include __DIR__.'/../src/app.php');

Nós, obviamente, mal arranhamos a superfície do que você pode fazer com o *container*: 
desde nomes de classe como parâmetros, até sobrescrever definições de objetos 
existentes, desde suporte a escopo até o *dump* do *container* para uma classe PHP simples,
e muito mais. O *container* de injeção de dependência do Symfony é realmente poderoso
e capaz de gerenciar qualquer tipo de classe PHP.

Não grite comigo, se você não quiser ter um *container* de injeção de dependência em
seu framework. Se você não gosta, não use. É o seu framework, não
meu.

Essa (já) é o último capítulo deste livro sobre a criação de um framework utilizando os
os componentes do Symfony. Estou ciente de que muitos temas não foram abordados em
grandes detalhes, mas, espero que ela lhe forneça informações suficientes para começar o
seu próprio e para entender melhor como o framework Symfony trabalha internamente.

Se você quiser saber mais, leia o código fonte do micro-framework `Silex`_, 
e, especialmente, sua classe `Application`_.

Divirta-se! 

.. _`Pimple`:      https://github.com/fabpot/Pimple
.. _`Silex`: https://silex.sensiolabs.org/
.. _`Application`: https://github.com/fabpot/Silex/blob/master/src/Silex/Application.php
