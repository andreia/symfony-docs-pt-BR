.. index::
    single: Atualização; Versão Menor

Atualizando uma Versão Menor (ex., 2.5.3 para 2.6.1)
====================================================

Se você estiver atualizando uma versão menor (onde o número do meio muda), então
você *não* deve encontrar mudanças significativas de compatibilidade com versões anteriores. Para
detalhes, consulte a :doc:`promessa Symfony de compatibilidade com versões anteriores </contributing/code/bc>`.

No entanto, algumas quebras de compatibilidade com versões anteriores *são* possíveis e você vai aprender em
um segundo como se preparar para elas.

Há duas etapas para atualizar uma versão menor:

#. :ref:`Atualizar a biblioteca Symfony via Composer <upgrade-minor-symfony-composer>`;
#. :ref:`Atualizar seu código para funcionar com a nova versão <upgrade-minor-symfony-code>`.

.. _`upgrade-minor-symfony-composer`:

1) Atualizar a Biblioteca Symfony via Composer
----------------------------------------------

Primeiro, você precisa atualizar o Symfony, modificando seu arquivo ``composer.json``
para usar a nova versão:

.. code-block:: json

    {
        "...": "...",

        "require": {
            "symfony/symfony": "2.6.*",
        },
        "...": "...",
    }

Em seguida, use o Composer para fazer o download de novas versões das bibliotecas:

.. code-block:: bash

    $ composer update symfony/symfony

.. include:: /cookbook/upgrade/_update_dep_errors.rst.inc

.. include:: /cookbook/upgrade/_update_all_packages.rst.inc

.. _`upgrade-minor-symfony-code`:

2) Atualizando seu Código para Funcionar com a nova Versão
----------------------------------------------------------

Em teoria, deve estar tudo pronto! No entanto, você *pode* precisar fazer algumas mudanças
em seu código para tudo funcionar. Além disso, alguns recursos que você está
usando podem ainda funcionar, porém podem agora estar obsoletos. Enquanto está tudo bem,
se você sabe sobre esses recursos obsoletos, você pode começar a corrigí-los ao longo do tempo.

Cada versão do Symfony vem com um arquivo de atualização (por exemplo, `UPGRADE-2.7.md`_)
incluído no diretório Symfony que descreve essas mudanças. Se você seguir
as instruções no documento e atualizar o seu código de forma adequada, deve ser
seguro atualizar no futuro.

Esses documentos também podem ser encontrados no `Repositório Symfony`_.

.. _`Repositório Symfony`: https://github.com/symfony/symfony
.. _`UPGRADE-2.7.md`: https://github.com/symfony/symfony/blob/2.7/UPGRADE-2.7.md
