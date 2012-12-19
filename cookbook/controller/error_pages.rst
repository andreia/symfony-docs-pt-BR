.. index::
   single: Controlador; Personalizar páginas de erro
   single: Páginas de erro

Como personalizar as páginas de erro
====================================

Quando qualquer exceção é lançada no Symfony2, ela é capturada dentro da
classe ``Kernel`` e, eventualmente, encaminhada para um controlador especial,
``TwigBundle:Exception:show``, para o tratamento. Este controlador, que reside
no interior do núcleo do ``TwigBundle``, determina qual template de erro será exibido
e o código de status que deve ser definido para a exceção.

Páginas de erro pode ser personalizadas de duas maneiras diferentes, dependendo da quantidade
de controle que você precisa:

1. Personalizar os templates de erro das diferentes páginas de erro (explicado abaixo);

2. Substituir o controlador de exceção padrão ``TwigBundle::Exception:show``
   pelo seu próprio controlador e manipulá-lo como quiser (veja
   :ref:`exception_controller in the Twig reference<config-twig-exception-controller>`);

.. tip::

    A personalização da manipulação de exceção é, na verdade, muito mais poderosa
    do que o que está escrito aqui. Um evento interno é lançado, ``kernel.exception``,
    que permite controle completo sobre o tratamento de exceção. Para mais
    informações, veja :ref:`kernel-kernel.exception`.

Todos os templates de erro residem dentro do ``TwigBundle``. Para sobrescrever os
templates, simplesmente contamos com o método padrão para sobrescrever templates que
reside dentro de um bundle. Para maiores informações, visite 
:ref:`overriding-bundle-templates`.

Por exemplo, para sobrescrever o template padrão de erro que é exibido ao
usuário final, adicione um novo template em
``app/Resources/TwigBundle/views/Exception/error.html.twig``:

.. code-block:: html+jinja

    <!DOCTYPE html>
    <html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title>An Error Occurred: {{ status_text }}</title>
    </head>
    <body>
        <h1>Oops! An Error Occurred</h1>
        <h2>The server returned a "{{ status_code }} {{ status_text }}".</h2>
    </body>
    </html>

.. tip::

    Se você não estiver familiarizado com o Twig, não se preocupe. O Twig é uma templating engine
    simples, poderosa e opcional que integra o ``Symfony2``. Para mais
    informações sobre o Twig, consulte :doc:`/book/templating`.

Além da página HTML de erro padrão, o Symfony fornece uma página padrão de erro
para muitos dos formatos de resposta mais comuns, incluindo JSON
(``error.json.twig``), XML (``error.xml.twig``) e até mesmo
JavaScript (``error.js.twig``), somente para citar alguns. Para sobrescrever qualquer
um destes templates, apenas adicione um novo arquivo com o mesmo nome no
diretório ``app/Resources/TwigBundle/views/Exception``. Esta é a forma padrão
de sobrescrever qualquer template que reside dentro de um bundle.

.. _cookbook-error-pages-by-status-code:

Personalizando a Página 404 e outras Páginas de Erro
----------------------------------------------------

Você também pode personalizar os templates de erro específicos de acordo com o código de
status HTTP. Por exemplo, crie um
template ``app/Resources/TwigBundle/views/Exception/error404.html.twig``
para exibir uma página especial para erros 404 (página não encontrada).

O Symfony usa o seguinte algoritmo para determinar qual o template que deve usar:

* Primeiro, ele procura por um template para o formato e código de status especificado (como
  ``error404.json.twig``);

* Se não existir, ele procura um template para o formato especificado (como
  ``error.json.twig``);

* Se ainda não existir, ele volta para o template HTML (como
  ``error.html.twig``).

.. tip::

    Para ver a lista completa de templates de erro padrão, consulte o
    diretório ``Resources/views/Exception`` do ``TwigBundle``. Na
    instalação Standard do Symfony2, o ``TwigBundle`` pode ser encontrado em
    ``vendor/symfony/src/Symfony/Bundle/TwigBundle``. Muitas vezes, a maneira mais fácil
    para personalizar uma página de erro é copiá-la do ``TwigBundle`` para o 
    ``app/Resources/TwigBundle/views/Exception`` e então modificá-la.

.. note::

    As páginas de exceção amigáveis de depuração, mostradas para o desenvolvedor, podem ser
    personalizadas da mesma forma, criando templates como
    ``exception.html.twig`` para a página de exceção padrão HTML ou
    ``exception.json.twig`` para a página de exceção JSON.
