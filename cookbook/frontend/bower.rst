.. index::
    single: Front-end; Bower

Usando o Bower com o Symfony
============================

O Symfony e todos seus pacotes são perfeitamente gerenciados pelo Composer. O Bower é uma
ferramenta de gerenciamento de dependências front-end, como Bootstrap ou
jQuery. Como o Symfony é puramente uma estrutura back-end, ele não pode ajudá-lo muito com
o Bower. Felizmente, ele é muito fácil de usar!

Instalando o Bower
------------------

O Bower_ é construído sobre o `Node.js`_. Certifique-se de que você tenha ele instalado e
em seguida, execute:

.. code-block:: bash

    $ npm install -g bower

Após esse comando terminar, execute ``bower`` em seu terminal para saber se
ele está instalado corretamente.

.. tip::

    Se você não quer instalar o NodeJS no seu computador, você também pode usar o
    BowerPHP_ (uma adaptação não oficial PHP do Bower). Tenha em mente que ele ainda está em
    estado alfa. Se você estiver usando o BowerPHP, use ``bowerphp`` ao invés de
    ``bower`` nos exemplos.

Configurando o Bower em seu Projeto
-----------------------------------

Normalmente, o Bower transfere tudo para dentro de um diretório ``bower_components``. No
Symfony, apenas os arquivos no diretório ``web/`` são publicamente acessíveis, então, você
precisa configurar o Bower para baixar as dependências front-end lá. Para fazer isso, basta
criar um arquivo ``.bowerrc`` com um novo destino (como ``web/assets/vendor``):

.. code-block:: json

    {
        "directory": "web/assets/vendor/"
    }

.. tip::

    Se você estiver usando uma ferramenta de automação de tarefas front-end como `Gulp`_ or `Grunt`_, então,
    você pode definir o diretório para o que quiser. Normalmente, você vai usar
    essas ferramentas para, em última instância, mover todos os assets para o diretório ``web/``.

Um Exemplo: Instalando o Bootstrap
----------------------------------

Acredite ou não, mas você está agora pronto para usar o Bower em sua aplicação
Symfony. Como um exemplo, você vai instalar o Bootstrap em seu projeto e
incluí-lo em seu layout.

Instalando a Dependência
~~~~~~~~~~~~~~~~~~~~~~~~

Para criar um arquivo ``bower.json``, basta executar ``bower init``. Agora você está pronto para
começar a adicionar as dependências em seu projeto. Por exemplo, para adicionar o Bootstrap_ em seu
``bower.json`` e baixá-lo, basta executar:

.. code-block:: bash

    $ bower install --save bootstrap

Isso irá instalar o Bootstrap e suas dependências em ``web/assets/vendor/`` (ou
qualquer diretório que você configurou no ``.bowerrc``).

.. seealso::

    Para mais detalhes sobre como usar o Bower, verifique a `documentação do Bower`_.

Incluindo a Dependência em seu Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora que as dependências estão instaladas, você pode incluir o bootstrap em seu
template normalmente como faria com qualquer arquivo CSS/JS:

.. configuration-block::

    .. code-block:: html+jinja

        {# app/Resources/views/layout.html.twig #}
        <!doctype html>
        <html>
            <head>
                {# ... #}

                <link rel="stylesheet"
                    href="{{ asset('assets/vendor/bootstrap/dist/css/bootstrap.min.css') }}">
            </head>

            {# ... #}
        </html>

    .. code-block:: html+php

        <!-- app/Resources/views/layout.html.php -->
        <!doctype html>
        <html>
            <head>
                {# ... #}

                <link rel="stylesheet" href="<?php echo $view['assets']->getUrl(
                    'assets/vendor/bootstrap/dist/css/bootstrap.min.css'
                ) ?>">
            </head>

            {# ... #}
        </html>

Ótimo trabalho! Seu site está agora usando Bootstrap. E você pode atualizar facilmente o
bootstrap para a última versão e gerenciar outras dependências front-end também.

Devo Adicionar os Assets do Bower no Gitignore ou fazer Commit deles?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Atualmente, você provavelmente faz o *commit* dos assets baixados pelo Bower ao invés
de adicionar o diretório (por exemplo, ``web/assets/vendor``) ao seu arquivo ``.gitignore``
:

.. code-block:: bash

    $ git add web/assets/vendor

Por quê? Ao contrário do Composer, o Bower atualmente não tem um recurso de "lock",
significando que não há garantia de que a execução do comando ``bower install`` em um servidor diferente
irá lhe fornecer os assets *exatos* que você tem em outras máquinas.
Para mais detalhes, leia o artigo `Checking in front-end dependencies`_.

Mas, é bem provável que o Bower irá adicionar um recurso de lock futuramente
(por exemplo, `bower/bower#1748`_).

.. _Bower: http://bower.io
.. _`Node.js`: https://nodejs.org
.. _BowerPHP: http://bowerphp.org/
.. _`documentação do Bower`: http://bower.io/
.. _Bootstrap: http://getbootstrap.com/
.. _Gulp: http://gulpjs.com/
.. _Grunt: http://gruntjs.com/
.. _`Checking in front-end dependencies`: http://addyosmani.com/blog/checking-in-front-end-dependencies/
.. _`bower/bower#1748`: https://github.com/bower/bower/pull/1748
