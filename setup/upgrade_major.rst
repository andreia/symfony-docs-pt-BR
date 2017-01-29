.. index::
    single: Atualização; Versão Maior

Atualizando uma Versão Maior (ex., 2.7.0 para 3.0.0)
====================================================

Em um intervalo de poucos anos, o Symfony lança uma nova versão maior (o primeiro número
muda). Esses lançamentos são mais complicados para atualizar, pois eles são autorizados a
conter quebras de compatibilidade com versões anteriores. No entanto, o Symfony tenta tornar
esse processo de atualização o mais suave possível.

Isso significa que você pode atualizar a maioria de seu código antes que o lançamento da versão maior
seja realmente liberado. Isso chama-se tornar o seu código *compatível futuramente*.

Há alguns passos para atualizar para uma versão maior:

#. :ref:`Tornar seu código livre de partes obsoletas <upgrade-major-symfony-deprecations>`;
#. :ref:`Atualizar para a nova versão maior via Composer <upgrade-major-symfony-composer>`;
#. :ref:`Atualizar seu código para funcionar com a nova versão <upgrade-major-symfony-after>`.

.. _upgrade-major-symfony-deprecations:

1) Tornar seu Código Livre de Partes Obsoletas
----------------------------------------------

Durante o ciclo de vida de um lançamento de versão maior, novas funcionalidades são adicionadas e
assinaturas de métodos e usos de API pública são alterados. Contudo,
:doc:`versões menores </cookbook/upgrade/minor_version>` não devem conter quaisquer
mudanças de incompatibilidade com versões anteriores. Para conseguir isso, o código "antigo" (ex., funções,
classes, etc) ainda funciona, mas está marcado como *obsoleto*, indicando que
ele será removido/alterado futuramente e que você deve parar de usá-lo.

Quando a versão maior é lançada (ex., 3.0.0), todos os recursos e funcionalidades
obsoletas são removidos. Então, desde que você atualizou seu código para parar
de usar esses recursos obsoletos na última versão antes da maior (ex.,
2.8.*), você poderá atualizar sem problema.

Para ajudá-lo com isso, avisos de descontinuação são acionados sempre que você acaba
usando um recurso obsoleto. Ao visitar a sua aplicação no
:doc:`ambiente dev </cookbook/configuration/environments>`
em seu navegador, esses avisos são mostrados na barra de ferramentas de desenvolvimento web:

.. image:: /images/cookbook/deprecations-in-profiler.png

É claro que, em última análise, você quer parar de usar a funcionalidade obsoleta.
Às vezes, isso é fácil: o aviso pode dizer exatamente o que mudar.

Mas, outras vezes, o aviso pode não ser claro: uma configuração em algum lugar pode
fazer com que uma classe mais profunda acione o aviso. Nesse caso, o Symfony faz seu melhor
para fornecer uma mensagem clara, mas pode ser necessário uma investigação adicional do aviso.

E, às vezes, o aviso pode vir de uma biblioteca ou um bundle de terceiro
que você está usando. Se isso for verdade, há uma boa chance de que essas descontinuações
já foram atualizadas. Nesse caso, atualize a biblioteca para corrigir.

Depois que todos os avisos de descontinuação sumiram, você pode atualizar com muito
mais confiança.

Descontinuações no PHPUnit
~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando você executar os seus testes usando PHPUnit, nenhum aviso de descontinuação é exibido.
Para ajudá-lo aqui, o Symfony fornece uma ponte PHPUnit. Essa ponte vai mostrar
um belo resumo de todos os avisos de descontinuação no final do relatório de ensaio.

Tudo que você precisa fazer é instalar a ponte PHPUnit:

.. code-block:: bash

    $ composer require --dev symfony/phpunit-bridge

Agora, você pode começar a corrigir os avisos:

.. code-block:: text

    $ phpunit
    ...

    OK (10 tests, 20 assertions)

    Remaining deprecation notices (6)

    The "request" service is deprecated and will be removed in 3.0. Add a typehint for
    Symfony\Component\HttpFoundation\Request to your controller parameters to retrieve the
    request instead: 6x
        3x in PageAdminTest::testPageShow from Symfony\Cmf\SimpleCmsBundle\Tests\WebTest\Admin
        2x in PageAdminTest::testPageList from Symfony\Cmf\SimpleCmsBundle\Tests\WebTest\Admin
        1x in PageAdminTest::testPageEdit from Symfony\Cmf\SimpleCmsBundle\Tests\WebTest\Admin

Uma vez que você corrigiu todos, o comando termina com ``0`` (success)  e está
tudo pronto!

.. sidebar:: Usando o Modo de Descontinuação Fraco (weak)

    Às vezes, você não pode corrigir todas as descontinuações (ex., algo tornou-se obsoleto
    em 2.6 e você ainda precisa suportar a 2.3). Nesses casos, você ainda pode
    usar a ponte para corrigir tantas descontinuações quanto possível e, em seguida, trocar
    para o modo de teste fraco para fazer seus testes passarem novamente. Você pode fazer isso
    usando a variável env ``SYMFONY_DEPRECATIONS_HELPER``:

    .. code-block:: xml

        <!-- phpunit.xml.dist -->
        <phpunit>
            <!-- ... -->

            <php>
                <env name="SYMFONY_DEPRECATIONS_HELPER" value="weak"/>
            </php>
        </phpunit>

    (você também pode executar o comando como ``SYMFONY_DEPRECATIONS_HELPER=weak phpunit``).

.. _upgrade-major-symfony-composer:

2) Atualizar para a Nova Versão Maior via Composer
--------------------------------------------------

Uma vez que o código está livre de partes obsoletas, você pode atualizar a biblioteca Symfony via
Composer, modificando seu arquivo ``composer.json``:

.. code-block:: json

    {
        "...": "...",

        "require": {
            "symfony/symfony": "3.0.*",
        },
        "...": "..."
    }

Em seguida, use o Composer para fazer o download de novas versões das bibliotecas:

.. code-block:: bash

    $ composer update --with-dependencies symfony/symfony

.. include:: /cookbook/upgrade/_update_dep_errors.rst.inc

.. include:: /cookbook/upgrade/_update_all_packages.rst.inc

.. _upgrade-major-symfony-after:

3) Atualizar seu Código para Funcionar com a Nova Versão
--------------------------------------------------------

Há uma boa chance de que tudo está pronto agora! No entanto, a próxima versão maior também *pode* 
conter novas quebras de compatibilidade com versões antiores pois uma camada BC não é sempre possível.
Certifique-se de ler ``UPGRADE-X.0.md`` (onde X é a nova versão maior)
incluído no repositório Symfony para qualquer quebra BC que você precisa estar
ciente.
