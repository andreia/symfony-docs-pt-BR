A View
======

Depois de ler a primeira parte desse tutorial, você decidiu que o Symfony2
vale pelo menos mais 10 minutos? Boa escolha! Nessa segunda parte, você vai
aprender sobre o sistema de template do Symfony2, o `Twig`_. Ele é um sistema
de templates para PHP flexível, rápido e seguro. Ele faz com que seus templates
sejam mais legíveis e concisos e também os torna mais amigáveis para os web
designers.

.. nota::

    Em vez do Twig, você também pode usar :doc:`PHP </cookbook/templating/PHP>`
    para os seus templates. Ambos são suportados pelo Symfony2.


Familiarizando-se com o Twig
----------------------------

.. dica::
	
	Se quiser aprender a usar o Twig, nós recomendamos fortemente que leia a
	`documentação`_ oficial dele. Essa seção é apenas uma visão geral sobre os
	principais conceitos.

Um template Twig é um arquivo de texto que pode gerar qualquer tipo de conteúdo
(HTML, XML, CSV, LaTex, ...). O Twig define dois tipos de delimitadores:

* ``{{ ... }}``: Imprime uma variável ou o resultado de uma expressão;

* ``{% ... %}``: Controla a lógica do template; é usado para executar
  loops ``for`` e instruções ``if``, por exemplo.

Abaixo temos um template mínimo que ilustra alguns comandos básicos usando as
duas váriaveis, ``page_title`` e ``navigation``, que são passadas para o
template:

.. code-block:: html+jinja

    <!DOCTYPE html>
    <html>
        <head>
            <title>My Webpage</title>
        </head>
        <body>
            <h1>{{ page_title }}</h1>

            <ul id="navigation">
                {% for item in navigation %}
                    <li><a href="{{ item.href }}">{{ item.caption }}</a></li>
                {% endfor %}
            </ul>
        </body>
    </html>


.. dica::

   Podem ser incluídos comentários nos templates usando o delimitador 
   ``{# ... #}``.

Para renderizar um template no Symfony, use o método ``render`` a partir do
controller, e passe para ele todas as variávels necessárias ao template::

    $this->render('AcmeDemoBundle:Demo:hello.html.twig', array(
        'name' => $name,
    ));

As variáveis passadas para o template podem ser strings, arrays ou até objetos.
O Twig abstrai a diferença entre eles e deixa acessar os "atributos" de uma
variável usando dot notation (``.``):

.. code-block:: jinja

    {# array('name' => 'Fabien') #}
    {{ name }}

    {# array('user' => array('name' => 'Fabien')) #}
    {{ user.name }}

    {# force array lookup #}
    {{ user['name'] }}

    {# array('user' => new User('Fabien')) #}
    {{ user.name }}
    {{ user.getName }}

    {# force method name lookup #}
    {{ user.name() }}
    {{ user.getName() }}

    {# pass arguments to a method #}
    {{ user.date('Y-m-d') }}

.. nota::

	É importante saber que as chaves não fazem parte da variável mas sim do
	comando de impressão. Se você acessar variáveis em tags não coloque as 
	chaves em volta delas.


Decorando os Templates
----------------------

É frequente em um projeto que os templates compartilhem elementos comuns, como
os bem-conhecidos cabeçalho e rodapé. No Symfony2, gostamos de enxergar essa
situação de uma forma diferente: um template pode ser decorado por outro.
Funciona exatamente do mesmo jeito que nas classes PHP: a herança de templates
permite que se construa o template base "layout", que contém todos os elementos
comuns do seu site, e define "blocos" que os templates filhos podem
sobrescrever.

O template ``hello.html.twig`` herda do ``layout.html.twig``, graças a tag
``extends``:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig #}
    {% extends "AcmeDemoBundle::layout.html.twig" %}

    {% block title "Hello " ~ name %}

    {% block content %}
        <h1>Hello {{ name }}!</h1>
    {% endblock %}

A notação ``AcmeDemoBundle::layout.html.twig`` parece familiar, não é mesmo?
Ela é a mesma notação usada para referenciar um template normal. A parte ``::``
significa simplesmente que o elemento controller está vazio, então o arquivo
correspondente é guardado diretamente no diretório ``Resources/views/``.

Agora, vamos dar uma olhada em um ``layout.html.twig`` simplificado:

.. code-block:: jinja

    {# src/Acme/DemoBundle/Resources/views/layout.html.twig #}
    <div class="symfony-content">
        {% block content %}
        {% endblock %}
    </div>

As tags ``{% block %}`` definem blocos que os templates filhos podem preencher.
Tudo o que essas tags fazem é dizer ao sistema de template que um filho pode
sobrescrever aquelas partes de seu template pai.

Nesse exemplo, o template ``hello.html.twig` sobrescreve o bloco ``content``,
que significa que o texto "Hello Fabien" é renderizado dentro do elemento
``div.symfony-content``

Usando Tags, Filtros e Funções
------------------------------

Uma das melhores funcionalidades do Twig é sua extensibilidade por meio de
tags, filtros e funções. O Symfony2 já vem com muitos desses embutidos
facilitando o trabalho do designer de templates.

Incluindo outros Templates
~~~~~~~~~~~~~~~~~~~~~~~~~~

A melhor forma de compartilhar um trecho de código entre vários templates
distintos é criar um novo desses que possa ser incluído nos outros.

Crie um template ``embedded.html.twig``:

.. code-block:: jinja

    {# src/Acme/DemoBundle/Resources/views/Demo/embedded.html.twig #}
    Hello {{ name }}

E altere o template ``index.html.twig`` para incluí-lo:

.. code-block:: jinja

    {# src/Acme/DemoBundle/Resources/views/Demo/hello.html.twig #}
    {% extends "AcmeDemoBundle::layout.html.twig" %}

    {# override the body block from embedded.html.twig #}
    {% block content %}
        {% include "AcmeDemoBundle:Demo:embedded.html.twig" %}
    {% endblock %}

Incorporando outros Controllers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

E o que fazer se você quiser incorporar o resultado de um outro controller em
um template? Isso é muito útil quando estiver trabalhado com Ajax, ou quando
o template incorporado precisa de alguma variável que não está disponível no 
template principal.

Suponha que você tenha criado uma action ``fancy``, e quer incluí-la dentro do
template ``index``. Para fazer isso, use a tag ``render``:

.. code-block:: jinja

    {# src/Acme/DemoBundle/Resources/views/Demo/index.html.twig #}
    {% render "AcmeDemoBundle:Demo:fancy" with { 'name': name, 'color': 'green' } %}

Aqui, a string ``AcmeDemoBundle:Demo:fancy`` se refere a action ``fancy`` do
controller ``Demo``. Os argumentos (``name``e ``color``) agem como variáveis de
requisições simuladas (como se ``fancyAction`` estivesse manipulando uma
requisição totalmente nova) e ficam disponíveis para o controller::

    // src/Acme/DemoBundle/Controller/DemoController.php

    class DemoController extends Controller
    {
        public function fancyAction($name, $color)
        {
            // create some object, based on the $color variable
            $object = ...;

            return $this->render('AcmeDemoBundle:Demo:fancy.html.twig', array('name' => $name, 'object' => $object));
        }

        // ...
    }

Criando Links entre Páginas
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando estamos falando de aplicações web, a criação de links entre páginas é
uma obrigação. Em vez de fazer "hardcode" das URLS nos templates, usamos a
função ``path`` que sabe como gerar URLs baseando-se na configuração das rotas.
Dessa forma, todas as URLs podem ser atualizadas facilmente apenas mudando essa
configuração:

.. code-block:: html+jinja

    <a href="{{ path('_demo_hello', { 'name': 'Thomas' }) }}">Greet Thomas!</a>

A função ``path`` pega o nome da rota e um array de parâmetros como argumentos.
O nome da rota é a chave principal sob a qual as rotas são referenciadas e os
parâmetros são os valores dos marcadores definidos no padrão da rota::

    // src/Acme/DemoBundle/Controller/DemoController.php
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    /**
     * @Route("/hello/{name}", name="_demo_hello")
     * @Template()
     */
    public function helloAction($name)
    {
        return array('name' => $name);
    }

.. dica::

    A função ``url`` cria URLs *absolutas*: ``{{ url('_demo_hello', {
    'name': 'Thomas' }) }}``.

Incluindo Assets: imagens, JavaScripts e folhas de estilo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O que seria da Internet sem as imagens, os JavaScripts e as folhas de estilo?
O Symfony2 fornece a função ``asset`` para lidar com eles de forma fácil:

.. code-block:: jinja

    <link href="{{ asset('css/blog.css') }}" rel="stylesheet" type="text/css" />

    <img src="{{ asset('images/logo.png') }}" />

O objetivo principal da função ``asset`` é deixar sua aplicação mais portátil.
Graças a ela, você pode mover o diretório raiz da aplicação para qualquer lugar
no diretório web root sem mudar nem uma linha no código de seus templates.

Escapando Variáveis
-------------------

O Twig é configurado por padrão para escapar automaticamente toda a saída
de dados. Leia a `documentação`_ do Twig para aprender mais sobre como escapar
a saída de dados e sobre a extensão Escaper.

Considerações Finais
--------------------

O Twig é simples mas poderoso. Graças a inclusão de layouts, blocos, templates
e actions, é muito fácil organizar seus templates de uma maneira lógica e
extensível. No entanto se você não estiver confortável com o Twig sempre
poderá usar templates PHP no Symfony sem problemas.

Você está trabalhando com o Symfony2 há apenas 20 minutos, mas já pode fazer
coisas incríveis com ele. Esse é o poder do Symfony2. Aprender a base é fácil,
e logo você aprenderá que essa simplicidade está escondida debaixo de uma
arquitetura muito flexível.

Mas eu já estou me adiantando. Primeiro, você precisa aprender mais sobre o
controller e esse é exatamente o assunto da :doc:próxima parte do tutorial<the_controller>.
Pronto para mais 10 minutos de Symfony2?

.. _Twig:          http://twig.sensiolabs.org/
.. _documentação:  http://twig.sensiolabs.org/documentation
