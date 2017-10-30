Gerenciando CSS e JavaScript
===========================

Symfony vem com uma biblioteca de JavaScript pura – chamada Webpack Encore – isso faz com que o trabalho com o CSS e JavaScript seja mais alegre. Você pode usá-lo, com alguma coisa ou simplesmente criar arquivos CSS e JS no diretório ``web/`` e inclui-los em seus modelos.

.. _frontend-webpack-encore:

Webpack Encore
--------------

`Webpack Encore`_ é uma maneira mais simples de integrar `Webpack`_ na sua aplicação. Ele *envolve* Webpack, dando-lhe um uma API lima e poderosa para incluir módulos JavaScripts, pré-processando CSS e JS e compilação e mineração de ativos. Encore traz um sistema de ativos profissional que é uma *delicia* de usar.


Encore é inspirado por `Webpacker`_ e `Mix`_, mas permanece no espirito do Webpack: usando seus recursos, conceitos e conversões de nomeações para se ter uma sensação familiar. Ele visa resolver os casos mais comuns do Webpack.

.. Dica::

    Encore é feito pelo `Symfony`_ e funciona *muito bem* em aplicativos Symfony. Mas ele pode ser usado facilmente por qualquer aplicação... em qualquer linguagem!

Documentação Encore
--------------------

Começando
...............

* :doc:`Installation </frontend/encore/installation>`
* :doc:`First Example </frontend/encore/simple-example>`

Adicionando mais Recursos
....................

* :doc:`CSS Preprocessors: Sass, LESS, etc </frontend/encore/css-preprocessors>`
* :doc:`PostCSS and autoprefixing </frontend/encore/postcss>`
* :doc:`Enabling React.js </frontend/encore/reactjs>`
* :doc:`Enabling Vue.js (vue-loader) </frontend/encore/vuejs>`
* :doc:`Configuring Babel </frontend/encore/babel>`
* :doc:`Source maps </frontend/encore/sourcemaps>`
* :doc:`Enabling TypeScript (ts-loader) </frontend/encore/typescript>`

Otimizando
..........

* :doc:`Versioning (and the manifest.json file) </frontend/encore/versioning>`
* :doc:`Using A CDN </frontend/encore/cdn>`
* :doc:`Creating a "Shared" entry for re-used modules </frontend/encore/shared-entry>`

Guias
......

* :doc:`Using Bootstrap CSS & JS </frontend/encore/bootstrap>`
* :doc:`Creating Page-Specific CSS/JS </frontend/encore/page-specific-assets>`
* :doc:`jQuery and Legacy Applications </frontend/encore/legacy-apps>`
* :doc:`Passing Information from Twig to JavaScript </frontend/encore/server-data>`
* :doc:`webpack-dev-server and Hot Module Replacement (HMR) </frontend/encore/dev-server>`
* :doc:`Adding custom loaders & plugins </frontend/encore/custom-loaders-plugins>`
* :doc:`Advanced Webpack Configuration </frontend/encore/advanced-config>`

Problemas e Perguntas
..................

* :doc:`FAQ & Common Issues </frontend/encore/faq>`
* :doc:`/frontend/encore/versus-assetic`

API Completa
........

* `Full API`_: https://github.com/symfony/webpack-encore/blob/master/index.js

Assetic
-------

Assetic é uma biblioteca PHP pura que ajuda a processa e otimizar seus recursos (semelhante ao Encore). Mesmo que recomendamos o uso do Encore, o Assetic funciona de forma excelente. Para uma comparação, veja :doc:`/frontend/encore/versus-assetic`.

Para mais informações sobre Assetic, veja :doc:`/frontend/assetic`.

Outros Artigos Front-End
------------------------

.. toctree::
    :hidden:
    :glob:

    frontend/encore/installation
    frontend/encore/simple-example
    frontend/encore/*

.. toctree::
    :maxdepth: 1
    :glob:

    frontend/*

.. _`Webpack Encore`: https://www.npmjs.com/package/@symfony/webpack-encore
.. _`Webpack`: https://webpack.js.org/
.. _`Webpacker`: https://github.com/rails/webpacker
.. _`Mix`: https://laravel.com/docs/5.4/mix
.. _`Symfony`: http://symfony.com/
.. _`Full API`: https://github.com/symfony/webpack-encore/blob/master/index.js
