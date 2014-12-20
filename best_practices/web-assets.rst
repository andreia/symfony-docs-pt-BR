Recursos da Web
===============

Recursos da web são coisas como arquivos CSS, JavaScript e imagens que fazem o frontend
do seu site parecer e funcionar ótimo. Os desenvolvedores Symfony têm, tradicionalmente,
esses recursos armazenados no diretório ``Resources/public/`` de cada bundle.

.. best-practice::

    Armazene seus recursos no diretório ``web/``.

Espalhar seus recursos da web através de dezenas de bundles diferentes torna mais
difícil gerenciá-los. A vida dos seus designers será muito mais fácil se todos
os recursos da aplicação estiverem em um único local.

Os templates também se beneficiam da centralização dos recursos, porque os links são
muito mais concisos:

.. code-block:: html+jinja

    <link rel="stylesheet" href="{{ asset('css/bootstrap.min.css') }}" />
    <link rel="stylesheet" href="{{ asset('css/main.css') }}" />

    {# ... #}

    <script src="{{ asset('js/jquery.min.js') }}"></script>
    <script src="{{ asset('js/bootstrap.min.js') }}"></script>

.. note::

    Tenha em mente que ``web/`` é um diretório público e que qualquer coisa armazenada
    nele será acessível publicamente. Por essa razão, você deve colocar ali seus recursos
    da web compilados, mas não seus arquivos de origem (por exemplo, arquivos SASS).

Usando o Assetic
----------------

Atualmente, você provavelmente não pode simplesmente criar arquivos CSS e JavaScript estáticos
e incluí-los no seu template. Em vez disso, provavelmente vai querer combinar
e minificar eles para melhorar o desempenho ao lado do cliente. Você também pode querer
usar LESS ou Sass (por exemplo), o que significa que vai precisar, de alguma forma, processar
estes em arquivos CSS.

Existem muitas ferramentas para resolver esses problemas, incluindo ferramentas puras para frontend (não-PHP)
como GruntJS.

.. best-practice::

    Use o Assetic para compilar, combinar e minimizar os recursos da web, a menos que você esteja
    confortável com ferramentas frontend como o GruntJS.

:doc:`Assetic </cookbook/assetic/asset_management>` é um gestor de recursos capaz
de compilar recursos desenvolvidos com várias tecnologias diferentes de frontend
como LESS, Sass e CoffeeScript.
Combinar todos os seus recursos com o Assetic é uma questão de envolver eles
com uma única tag Twig:

.. code-block:: html+jinja

    {% stylesheets
        'css/bootstrap.min.css'
        'css/main.css'
        filter='cssrewrite' output='css/compiled/all.css' %}
        <link rel="stylesheet" href="{{ asset_url }}" />
    {% endstylesheets %}

    {# ... #}

    {% javascripts
        'js/jquery.min.js'
        'js/bootstrap.min.js'
        output='js/compiled/all.js' %}
        <script src="{{ asset_url }}"></script>
    {% endjavascripts %}

Aplicações Fundamentadas no Frontend
------------------------------------

Recentemente, tecnologias frontend como AngularJS tornaram-se bastante populares
para o desenvolvimento de aplicações web frontend que conversam com uma API.

Se você estiver desenvolvendo uma aplicação como essa, deve usar as ferramentas
que são recomendadas pela tecnologia, como Bower e GruntJS. Você deve
desenvolver sua aplicação frontend separadamente do seu backend Symfony (mesmo
separando os repositórios se você quiser).

Saiba mais sobre o Assetic
--------------------------

O Assetic também pode minimizar recursos CSS e JavaScript 
:doc:`using UglifyCSS/UglifyJS </cookbook/assetic/uglifyjs>` para acelerar os seus
websites. Você pode até mesmo :doc:`compactar imagens </cookbook/assetic/jpeg_optimize>`
com o Assetic para reduzir seu tamanho antes de servi-las para o usuário. Verifique
a `documentação oficial do Assetic`_ para saber mais sobre todas as características
disponíveis.

.. _`documentação oficial do Assetic`: https://github.com/kriswallsmith/assetic
