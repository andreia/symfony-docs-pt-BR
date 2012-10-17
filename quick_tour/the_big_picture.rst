Panorama Geral
==============

Comece a usar o Symfony2 em 10 minutos! Este capítulo irá orientá-lo através de alguns
dos conceitos mais importantes por trás do Symfony2 e explicar como você poderá
iniciar rapidamente, mostrando-lhe um projeto simples em ação.

Se você já usou um framework web antes, você deve se sentir em casa com
o Symfony2. Se não, bem-vindo à uma nova forma de desenvolver aplicações web!

.. tip::

    Quer saber por que e quando você precisa usar um framework? Leia o documento "`Symfony
    em 5 minutos`_" .

Baixando o Symfony2
-------------------

Primeiro, verifique se você tem um servidor web instalado e configurado (como
o Apache) com a versão mais recente possível do PHP (é recomendado o PHP 5.3.8 ou 
mais recente).

Pronto? Comece fazendo o download da "`Edição Standard do Symfony2`_", uma :term:`distribuição`
do Symfony que é pré-configurada para os casos de uso mais comuns e
contém também um código que demonstra como usar Symfony2 (obtenha o arquivo
com os *vendors* incluídos para começar ainda mais rápido).

Após descompactar o arquivo no seu diretório raiz do servidor web, você deve
ter um diretório ``Symfony/`` parecido com o seguinte:

.. code-block:: text

    www/ <- your web root directory
        Symfony/ <- the unpacked archive
            app/
                cache/
                config/
                logs/
                Resources/
            bin/
            src/
                Acme/
                    DemoBundle/
                        Controller/
                        Resources/
                        ...
            vendor/
                symfony/
                doctrine/
                ...
            web/
                app.php
                ...

.. note::

    Se você está familiarizado com o Composer, poderá executar o seguinte comando
    em vez de baixar o arquivo:

    .. code-block:: bash

        $ composer.phar create-project symfony/framework-standard-edition path/to/install 2.1.x-dev

        # remove the Git history
        $ rm -rf .git

        Para uma versão exata, substitua `2.1.x-dev` com a versão mais recente do Symfony
        (Ex. 2.1.1). Para detalhes, veja a `Página de Instalação do Symfony`_

.. tip::

    Se você tem o PHP 5.4, é possível usar o servidor web integrado:

    .. code-block:: bash

        # check your PHP CLI configuration
        $ php ./app/check.php

        # run the built-in web server
        $ php ./app/console server:run

    Então, a URL para a sua aplicação será "http://localhost:8000/app_dev.php"

    O servidor integrado deve ser usado apenas para fins de desenvolvimento, mas
    pode ajudá-lo a iniciar o seu projeto de forma rápida e fácil.

Verificando a configuração
--------------------------

O Symfony2 vem com uma interface visual para teste da configuração do servidor que ajuda a evitar 
algumas dores de cabeça que originam da má configuração do servidor Web ou do PHP. Use a seguinte
URL para ver o diagnóstico para a sua máquina:

.. code-block:: text

    http://localhost/config.php

.. note::

    Todos as URLs de exemplo assumem que você baixou e descompactou o Symfony
    diretamente no diretório raiz web do seu servidor web. Se você seguiu as instruções
    acima e descompactou o diretório `Symfony` em seu raiz web, então, adicione
    `/Symfony/web` após `localhost` em todas as URLs:

    .. code-block:: text

        http://localhost/Symfony/web/config.php

Se houver quaisquer questões pendentes informadas, corrija-as. Você também pode 
ajustar a sua configuração, seguindo todas as recomendações. Quando tudo estiver
certo, clique em "*Bypass configuration and go to the Welcome page*" para solicitar
a sua primeira página "real" do Symfony2:

.. code-block:: text

    http://localhost/app_dev.php/

O Symfony2 lhe dá as boas vindas e parabeniza-o por seu trabalho até agora!

.. image:: /images/quick_tour/welcome.jpg
   :align: center

Compreendendo os Fundamentos
----------------------------

Um dos objetivos principais de um framework é garantir a `Separação de Responsabilidades`_.
Isso mantém o seu código organizado e permite que a sua aplicação evolua facilmente ao longo 
do tempo, evitando a mistura de chamadas ao banco de dados, de tags HTML e de lógica de 
negócios no mesmo script. Para atingir este objetivo com o Symfony, primeiro você
precisa aprender alguns conceitos e termos fundamentais.

.. tip::

    Quer uma prova de que o uso de um framework é melhor do que misturar tudo
    no mesmo script? Leia o capítulo ":doc:`/book/from_flat_php_to_symfony2`"
    do livro.

A distribuição vem com um código de exemplo que você pode usar para aprender mais sobre 
os principais conceitos do Symfony2. Vá para a seguinte URL para ser cumprimentado pelo
Symfony2 (substitua *Fabien* pelo seu primeiro nome):

.. code-block:: text

    http://localhost/app_dev.php/demo/hello/Fabien

.. image:: /images/quick_tour/hello_fabien.png
   :align: center

O que está acontecendo aqui? Vamos dissecar a URL:

* ``app_dev.php``: Este é o :term:`front controller`. É o único ponto de entrada
  da aplicação e responde à todas as solicitações dos usuários;

* ``/demo/hello/Fabien``: Este é o *caminho virtual* para o recurso que o usuário
  quer acessar.

Sua responsabilidade como desenvolvedor é escrever o código que mapeia a *solicitação* 
do usuário (``/demo/hello/Fabien``) para o *recurso* associado à ela
(a página HTML ``Hello Fabien!``).

Roteamento
~~~~~~~~~~

O Symfony2 encaminha a solicitação para o código que lida com ela, tentando corresponder a
URL solicitada contra alguns padrões configurados. Por predefinição, esses padrões
(chamados de rotas) são definidos no arquivo de configuração ``app/config/routing.yml``.
Quando você está no :ref:`ambiente<quick-tour-big-picture-environments>` ``dev`` - 
indicado pelo front controller app_**dev**.php - o arquivo de configuração
``app/config/routing_dev.yml`` também é carregado. Na Edição Standard, as rotas para
estas páginas "demo" são colocadas no arquivo:

.. code-block:: yaml

    # app/config/routing_dev.yml
    _welcome:
        pattern:  /
        defaults: { _controller: AcmeDemoBundle:Welcome:index }

    _demo:
        resource: "@AcmeDemoBundle/Controller/DemoController.php"
        type:     annotation
        prefix:   /demo

    # ...

As três primeiras linhas (após o comentário) definem o código que é executado
quando o usuário solicita o recurso "``/``" (ou seja, a página de boas-vindas que você viu
anterioremente). Quando solicitado, o controlador ``AcmeDemoBundle:Welcome:index``
será executado. Na próxima seção, você vai aprender exatamente o que isso significa.

.. tip::

    A Edição Standard do Symfony2 usa `YAML`_ para seus arquivos de configuração,
    mas o Symfony2 também suporta XML, PHP e anotações nativamente. Os diferentes 
    formatos são compatíveis e podem ser utilizados alternadamente dentro de uma
    aplicação. Além disso, o desempenho de sua aplicação não depende do
    formato de configuração que você escolher, pois tudo é armazenado em cache na
    primeira solicitação.

Controladores
~~~~~~~~~~~~~

Controlador é um nome fantasia para uma função ou método PHP que manipula as *solicitações* 
de entrada e retorna *respostas* (código HTML, na maioria das vezes). Em vez de usar as 
variáveis ​​globais e funções do PHP (como ``$_GET`` ou ``header()``) para gerenciar
essas mensagens HTTP, o Symfony usa objetos: :class:`Symfony\\Component\\HttpFoundation\\Request`
e :class:`Symfony\\Component\\HttpFoundation\\Response`. O controlador mais simples possível
pode criar a resposta manualmente, com base na solicitação::

    use Symfony\Component\HttpFoundation\Response;

    $name = $request->query->get('name');

    return new Response('Hello '.$name, 200, array('Content-Type' => 'text/plain'));

.. note::

    O Symfony2 engloba a Especificação HTTP, que são as regras que regem
    toda a comunicação na Web. Leia o capítulo ":doc:`/book/http_fundamentals`"
    do livro para aprender mais sobre ela e o poder que
    isso acrescenta.

O Symfony2 escolhe o controlador com base no valor ``_controller`` da
configuração de roteamento: ``AcmeDemoBundle:Welcome:index``. Esta string é o
*nome lógico* do controlador, e ela referencia o método ``indexAction`` da
classe ``Acme\DemoBundle\Controller\WelcomeController``::

    // src/Acme/DemoBundle/Controller/WelcomeController.php
    namespace Acme\DemoBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class WelcomeController extends Controller
    {
        public function indexAction()
        {
            return $this->render('AcmeDemoBundle:Welcome:index.html.twig');
        }
    }

.. tip::

    Você poderia ter usado o nome completo da classe e do método - 
    ``Acme\DemoBundle\Controller\WelcomeController::indexAction`` - para o
    valor ``_controller``. Mas, se você seguir algumas convenções simples, o
    nome lógico é menor e permite mais flexibilidade.

A classe ``WelcomeController`` estende a classe nativa ``Controller``,
que fornece métodos de atalho úteis, tal como o método 
:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::render`
que carrega e renderiza um template (``AcmeDemoBundle:Welcome:index.html.twig``). 
O valor retornado é um objeto Response populado com o conteúdo processado. 
Assim, se as necessidades surgirem, o Response pode ser ajustado antes de ser 
enviado ao navegador::

    public function indexAction()
    {
        $response = $this->render('AcmeDemoBundle:Welcome:index.txt.twig');
        $response->headers->set('Content-Type', 'text/plain');

        return $response;
    }

Não importa como você faz isso, o objetivo final do seu controlador sempre será retornar
o objeto ``Response`` que deve ser devolvido ao usuário. Este objeto ``Response`` pode 
ser populado com código HTML, representar um redirecionamento do cliente, ou mesmo
retornar o conteúdo de uma imagem JPG com um cabeçalho ``Content-Type`` de ``image/jpg``.

.. tip::

    Estender a classe base ``Controller`` é opcional. De fato 
    um controlador pode ser uma função PHP simples ou até mesmo uma closure PHP.
    O capítulo ":doc:`O Controlador</book/controller>`" do livro lhe ensina
    tudo sobre os controladores do Symfony2.

O nome do template, ``AcmeDemoBundle:Welcome:index.html.twig``, é o *nome lógico* 
do template e faz referência ao arquivo ``Resources/views/Welcome/index.html.twig`` 
dentro do ``AcmeDemoBundle`` (localizado em ``src/Acme/DemoBundle``). 
A seção bundles abaixo irá explicar
porque isso é útil.

Agora, dê uma olhada novamente na configuração de roteamento e encontre a chave
``_demo``.

.. code-block:: yaml

    # app/config/routing_dev.yml
    _demo:
        resource: "@AcmeDemoBundle/Controller/DemoController.php"
        type:     annotation
        prefix:   /demo

O Symfony2 pode ler/importar as informações de roteamento de diferentes arquivos escritos
em YAML, XML, PHP ou até mesmo incorporado em anotações PHP. Aqui, o *nome lógico* do
arquivo é ``@AcmeDemoBundle/Controller/DemoController.php`` e refere-se
ao arquivo ``src/Acme/DemoBundle/Controller/DemoController.php`` . Neste
arquivo, as rotas são definidas como anotações nos métodos da ação::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    class DemoController extends Controller
    {
        /**
         * @Route("/hello/{name}", name="_demo_hello")
         * @Template()
         */
        public function helloAction($name)
        {
            return array('name' => $name);
        }

        // ...
    }

A anotação ``@Route()`` define uma nova rota com um padrão 
``/hello/{name}`` que executa o método ``helloAction`` quando corresponder. A
string entre chaves como ``{name}`` é chamada de placeholder. Como
você pode ver, o seu valor pode ser obtido através do argumento do método ``$name``.

.. note::

    Mesmo as anotações não sendo suportadas nativamente pelo PHP, você as usará
    extensivamente no Symfony2 como uma forma conveniente de configurar o comportamento
    do framework e manter a configuração próxima ao código.

Se você verificar o código do controlador, poderá ver que em vez de
renderizar um template e retornar um objeto ``Response`` como antes,
ele apenas retorna um array de parâmetros. A anotação ``@Template()`` diz ao
Symfony para renderizar o template para você, passando cada variável do array
ao template. O nome do template que é renderizado segue o nome
do controlador. Assim, neste exemplo, o template ``AcmeDemoBundle:Demo:hello.html.twig``
é renderizado (localizado em ``src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig``).

.. tip::

    As anotações ``@Route()`` e ``@Template()`` são mais poderosas do que
    os exemplos simples mostrados neste tutorial. Saiba mais sobre "`anotações
    em controladores`_" na documentação oficial.

Templates
~~~~~~~~~

O controlador renderiza o
template ``src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig`` (ou
``AcmeDemoBundle:Demo:hello.html.twig`` se você usar o nome lógico):

.. code-block:: jinja

    {# src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig #}
    {% extends "AcmeDemoBundle::layout.html.twig" %}

    {% block title "Hello " ~ name %}

    {% block content %}
        <h1>Hello {{ name }}!</h1>
    {% endblock %}

Por padrão, o Symfony2 usa o `Twig`_ como seu template engine, mas você também pode usar
templates tradicionais PHP se você escolher. No próximo capítulo apresentaremos como
os templates funcionam no Symfony2.

Bundles
~~~~~~~

Você pode ter se perguntado por que a palavra :term:`bundle` é usada em muitos nomes que
vimos até agora. Todo o código que você escreve para a sua aplicação está organizado em
bundles. Na forma de falar do Symfony2, um bundle é um conjunto estruturado de arquivos (arquivos PHP,
folhas de estilo, JavaScripts, imagens, ...) que implementam uma funcionalidade única (um
blog, um fórum, ...) e que podem ser facilmente compartilhados com outros desenvolvedores. Até
agora, manipulamos um bundle, ``AcmeDemoBundle``. Você vai aprender
mais sobre bundles no último capítulo deste tutorial.

.. _quick-tour-big-picture-environments:

Trabalhando com Ambientes
-------------------------

Agora que você tem uma compreensão melhor de como funciona o Symfony2, verifique 
a parte inferior de qualquer página renderizada com o Symfony2. Você deve observar uma pequena
barra com o logotipo do Symfony2. Isso é chamado de "Barra de ferramentas para Debug Web" e
é a melhor amiga do desenvolvedor.

.. image:: /images/quick_tour/web_debug_toolbar.png
   :align: center

Mas, o que você vê inicialmente é apenas a ponta do iceberg; clique sobre o estranho
número hexadecimal para revelar mais uma ferramenta de depuração muito útil do Symfony2:
o profiler.

.. image:: /images/quick_tour/profiler.png
   :align: center

É claro, você não vai querer mostrar essas ferramentas quando implantar a sua aplicação
em produção. É por isso que você vai encontrar um outro front controller no
diretório ``web/`` (``app.php``), que é otimizado para o ambiente de produção:

.. code-block:: text

    http://localhost/app.php/demo/hello/Fabien

E, se você usar o Apache com o ``mod_rewrite`` habilitado, poderá até omitir a
parte ``app.php`` da URL:

.. code-block:: text

    http://localhost/demo/hello/Fabien

Por último, mas não menos importante, nos servidores de produção, você deve apontar seu diretório
raiz web para o diretório ``web/`` para proteger sua instalação e ter uma URL
ainda melhor:

.. code-block:: text

    http://localhost/demo/hello/Fabien

.. note::

    Note que as três URLs acima são fornecidas aqui apenas como **exemplos** de
    como uma URL parece quando o front controller de produção é usado (com ou
    sem mod_rewrite). Se você realmente experimentá-los em uma
    instalação do *Symfony Standard Edition* você receberá um erro 404 pois
    o *AcmeDemoBundle* está habilitado somente no ambiente dev e suas rotas importam
    o *app/config/routing_dev.yml*.

Para fazer a sua aplicação responder mais rápido, o Symfony2 mantém um cache sob o
diretório ``app/cache/``. No ambiente de desenvolvimento (``app_dev.php``),
esse cache é liberado automaticamente sempre que fizer alterações em qualquer código ou
configuração. Mas esse não é o caso do ambiente de produção
(``app.php``) onde o desempenho é fundamental. É por isso que você deve sempre usar
o ambiente de desenvolvimento ao desenvolver a sua aplicação.

Diferentes :term:`ambientes<environment>` de uma dada aplicação diferem
apenas na sua configuração. Na verdade, uma configuração pode herdar de 
outra:

.. code-block:: yaml

    # app/config/config_dev.yml
    imports:
        - { resource: config.yml }

    web_profiler:
        toolbar: true
        intercept_redirects: false

O ambiente ``dev`` (que carrega o arquivo de configuração ``config_dev.yml``)
importa o arquivo global ``config.yml`` e, em seguida, modifica-o, neste exemplo,
habilitando a barra de ferramentas para debug web.

Considerações Finais
--------------------

Parabéns! Você já teve a sua primeira amostra de código do Symfony2. Isso não foi tão
difícil, foi? Há muito mais para explorar, mas, você já deve ter notado como
o Symfony2 torna muito fácil implementar web sites de forma melhor e mais rápida. 
Se você está ansioso para aprender mais sobre o Symfony2, mergulhe na próxima seção:
":doc:`A Visão<the_view>`".

.. _Edição Standard do Symfony2:       http://symfony.com/download
.. _Symfony em 5 minutos:              http://symfony.com/symfony-in-five-minutes
.. _Separação de Responsabilidades:    http://en.wikipedia.org/wiki/Separation_of_concerns
.. _YAML:                              http://www.yaml.org/
.. _anotações nos controladores:       http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/index.html#annotations-for-controllers
.. _Twig:                              http://twig.sensiolabs.org/
.. _`Página de Instalação do Symfony`: http://symfony.com/download
