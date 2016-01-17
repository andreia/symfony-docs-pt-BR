.. index::
    single: Atualização; Versão de Correção (Patch)

Atualizando uma Versão de Correção (Patch) (ex., 2.6.0 para 2.6.1)
==================================================================

Quando uma nova versão de correção é lançada (apenas o último número alterado), é um
lançamento que contém apenas correções de bugs. Isso significa que a atualização para uma nova
versão de correção é *realmente* fácil:

.. code-block:: bash

    $ composer update symfony/symfony

É isso! Você não deve encontrar quaisquer quebras de compatibilidade com versões anteriores
ou precisar alterar algo em seu código. Isso porque quando você começou
seu projeto, o seu ``composer.json`` incluiu o Symfony usando uma restrição
como ``2.6.*``, onde apenas o *último* número da versão mudará quando você
atualizar.

.. tip::

    Recomenda-se atualizar para uma nova versão de correção o mais rápido possível, pois
    erros importantes e vazamentos de segurança podem ser corrigidos nesses novos lançamentos.

.. include:: /cookbook/upgrade/_update_all_packages.rst.inc
