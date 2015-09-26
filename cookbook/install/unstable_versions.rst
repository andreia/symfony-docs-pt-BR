Como Instalar ou Atualizar para a Última Versão do Symfony não Lançada
======================================================================

Neste artigo, você vai aprender como instalar e usar novas versões do Symfony antes
delas serem liberadas como versões estáveis.

Criando um Novo Projeto com Base em uma Versão do Symfony não Estável
---------------------------------------------------------------------

Suponha que a versão do Symfony 2.7 ainda não foi lançada e que você pretende criar
um novo projeto para testar suas funcionalidades. Em primeiro lugar, :doc:`instale o Composer 
</cookbook/composer>` para gerenciar as dependências. Em seguida, abra um console de comando,
entre no diretório do seu projeto e execute o seguinte comando:

.. code-block:: bash

    $ composer create-project symfony/framework-standard-edition my_project "2.7.*" --stability=dev

Quando o comando terminar de executar, você terá um novo projeto Symfony criado
no diretório ``my_project/`` com base no código mais recente encontrado na
branch ``2.7``.

Se você quiser testar uma versão beta, use `` beta`` como o valor na opção
``stability``:

.. code-block:: bash

    $ composer create-project symfony/framework-standard-edition my_project "2.7.*" --stability=beta

Atualizar o seu Projeto para uma Versão do Symfony não Estável
--------------------------------------------------------------

Suponha novamente que o Symfony 2.7 ainda não foi lançado e você deseja atualizar
uma aplicação existente para testar se o seu projeto funciona com ele.

Primeiro, abra o arquivo ``composer.json`` localizado no diretório raiz do seu
projeto. Em seguida, edite o valor da versão definida para a dependência ``symfony / symfony``
como segue:

.. code-block:: json

    {
        "require": {
            // ...
            "symfony/symfony" : "2.7.*@dev"
        }
    }

Finalmente, abra um console de comando, entre no diretório do seu projeto e execute o
comando a seguir para atualizar as dependências do seu projeto:

.. code-block:: bash

    $ composer update symfony/symfony

Se preferir testar uma versão beta do Symfony, substitua a constraint ``"2.7.*@dev"``
por ``"2.7.0-beta1"`` para instalar um número específico beta ou ``2.7.*@beta`` para obter
a versão beta mais recente.

Depois de atualizar a versão do Symfony, leia o :doc:`Guia de Atualização do Symfony </cookbook/upgrade/index>`
para saber como proceder para atualizar o código da sua aplicação no caso de uma nova
versão do Symfony ter tornado algumas de suas funcionalidades obsoletas.

.. tip::

    Se você usa o Git para gerenciar o código do projeto, é uma boa prática criar
    uma nova branch para testar a nova versão do Symfony. Essa solução evita a introdução
    de qualquer problema na sua aplicação e permite testar a nova versão com
    total confiança:

    .. code-block:: bash

        $ cd projects/my_project/
        $ git checkout -b testing_new_symfony
        # ... update composer.json configuration
        $ composer update symfony/symfony

        # ... after testing the new Symfony version
        $ git checkout master
        $ git branch -D testing_new_symfony
