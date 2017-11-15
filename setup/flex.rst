.. index:: Flex

Usando o Symfony Flex para Gerenciar Aplicações Symfony
=======================================================

O `Symfony Flex`_ é a nova forma de instalar e gerenciar aplicações Symfony. O Flex
não é uma nova versão do Symfony, mas uma ferramenta que substitui e melhora o
`Instalador do Symfony`_.

O Symfony Flex **automatiza as tarefas mais comuns das aplicações Symfony**, tais como
a instalação e remoção de bundles e outras dependências. O Symfony Flex funciona
com o Symfony 3.3 e versões mais recentes. A partir do Symfony 4.0, o Flex será usado
por padrão, mas ainda será opcional seu uso.

Como Funciona o Flex
--------------------

Internamente, o Symfony Flex é um plugin do Composer que modifica o comportamento dos
comandos ``require`` e ``update``. Ao instalar ou atualizar dependências em uma
aplicação com Flex habilitado, o Symfony pode executar tarefas antes e depois da
execução das tarefas do Composer.

Considere o seguinte exemplo:

.. code-block:: terminal

    $ cd my-project/
    $ composer require mailer

Se você executar esse comando em uma aplicação Symfony que não usa o Flex,
você verá um erro do Composer explicando que ``mailer`` não é um nome de pacote
válido. No entanto, se a aplicação tiver o Symfony Flex instalado, esse comando termina
instalando e habilitando o SwiftmailerBundle, qual é a melhor forma de
integrar o Swiftmailer, o mailer oficial para aplicações Symfony.

Quando o Symfony Flex está instalado na aplicação e você executa ``composer require``,
a aplicação faz uma requisição aos servidores do Symfony Flex antes de tentar instalar
o pacote com o Composer:

* Se não houver informações sobre esse pacote, o servidor Flex não retorna nada e
  a instalação do pacote segue o procedimento usual com base no Composer;
* Se houver informações especiais sobre esse pacote, o Flex o retorna em um arquivo
  chamado "receita" e a aplicação o usa para decidir qual pacote instalar
  e quais tarefas automatizadas executar após a instalação.

No exemplo acima, o Symfony Flex pede pelo pacote ``mailer`` e o
O servidor Symfony Flex detecta que ``mailer`` é de fato um alias para SwiftmailerBundle
e retorna a "receita" para ele.

Receitas do Symfony Flex
~~~~~~~~~~~~~~~~~~~~~~~~

As receitas são definidas em um arquivo ``manifest.json`` e podem conter qualquer número de
outros arquivos e diretórios. Por exemplo, este é o ``manifest.json`` para o SwiftmailerBundle:

.. code-block:: javascript

    {
        "bundles": {
            "Symfony\\Bundle\\SwiftmailerBundle\\SwiftmailerBundle": ["all"]
        },
        "copy-from-recipe": {
            "config/": "%CONFIG_DIR%/"
        },
        "env": {
            "MAILER_URL": "smtp://localhost:25?encryption=&auth_mode="
        },
        "aliases": ["mailer", "mail"]
    }

A opção ``aliases`` permite ao Flex instalar pacotes usando nomes curtos e fáceis de
lembrar (``composer require mailer`` vs ``composer require symfony/swiftmailer-bundle``).
A opção ``bundles`` diz ao Flex em quais ambientes esse bundle deve ser
habilitado automaticamente (``all`` neste caso). A opção ``env`` faz o Flex
adicionar novas variáveis ​​de ambiente na aplicação. Finalmente, a opção
``copy-from-recipe`` permite que a receita copie arquivos e diretórios para sua aplicação.

As instruções definidas neste arquivo ``manifesto.json`` também são usadas pelo Symfony
Flex ao desinstalar dependências (por exemplo, ``composer remove mailer``) para desfazer
todas as mudanças. Isso significa que o Flex pode remover o SwiftmailerBundle da
aplicação, excluir a variável de ambiente ``MAILER_URL`` e qualquer outro arquivo
e diretório criado por essa receita.

As receitas do Symfony Flex são contribuídas pela comunidade e são armazenadas em
dois repositórios públicos:

* `Repositório de Receita Principal`_, é uma lista com curadoria de receitas para pacotes mantidos e
  de alta qualidade. Por padrão, o Symfony Flex procura somente nesse repositório.
* `Repositório de Receita Contrib`_, contém todas as receitas criadas pela comunidade.
  Todas são garantidas que funcionam, mas seus pacotes associados podem não ser
  mantidos. O Symfony Flex ignora essas receitas por padrão, mas você pode executar
  este comando para começar a usá-las em seu projeto:

  .. code-block:: terminal

        $ cd your-project/
        $ composer config extra.symfony.allow-contrib true

Leia a `Documentação das Receitas Symfony`_ para aprender tudo sobre como
criar receitas para seus próprios pacotes.

Usando o Symfony Flex em Novas Aplicações
-----------------------------------------

Symfony publicou um novo projeto "esqueleto", que é um projeto Symfony mínimo
recomendado para criar novas aplicações. Esse "esqueleto" já inclui o Symfony
Flex como uma dependência, para que você possa criar uma nova aplicação Symfony
com Flex habilitado executando o seguinte comando:

.. code-block:: terminal

    $ composer create-project symfony/skeleton my-project

.. note::

    O uso do Instalador do Symfony para criar novas aplicações não é mais recomendado
    desde o Symfony 3.3. Ao invés, use o comando ``create-project`` do Composer.

Atualizando Aplicações Existentes para o Flex
---------------------------------------------

O uso do Symfony Flex é opcional, mesmo no Symfony 4, onde o Flex será usado por
padrão. No entanto, o Flex é tão conveniente e melhora tanto a sua produtividade
que é altamente recomendável atualizar suas aplicações existentes para ele.

A única ressalva é que o Symfony Flex exige que as aplicações usem a
seguinte estrutura de diretório, que é a mesma usada por padrão no Symfony 4:

.. code-block:: text

    your-project/
    ├── config/
    │   ├── bundles.php
    │   ├── packages/
    │   ├── routes.yaml
    │   └── services.yaml
    ├── public/
    │   └── index.php
    ├── src/
    │   ├── ...
    │   └── Kernel.php
    ├── templates/
    └── vendor/

Isso significa que instalar a dependência ``symfony/flex`` em sua aplicação
não é o suficiente. Você também deve atualizar a estrutura de diretório para a mostrada
acima. Infelizmente, não existe uma ferramenta automática para fazer essa atualização, então você deve
seguir estas etapas manuais:

#. Crie uma nova aplicação Symfony vazia (``composer create-project symfony/skeleton my-project-flex``)
#. Copie as dependências ``require`` e ``require-dev`` definidas no seu arquivo ``composer.json``
   do projeto original para o arquivo ``composer.json`` do novo projeto.
#. Instale as dependências no novo projeto executando ``composer install``. Isso
   irá fazer com que o Symfony Flex gere todos os arquivos de configuração em ``config/packages/``
#. Revise os arquivos ``config/packages/*.yaml`` gerados e faça qualquer alteração
   necesssária de acordo com a configuração definida no arquivo ``app/config/config_*.yml``
   do seu projeto original. Cuidado com o fato de que este é o passo mais demorado
   e propenso a erros do processo de atualização.
#. Mova os parâmetros originais definidos em ``app/config/parameters.*.yml`` para os
   novos arquivos ``config/services.yaml`` e ``.env`` dependendo de suas necessidades.
#. Mova o código fonte original de ``src/{App,...}Bundle/`` para ``src/`` e
   atualize os namespaces de cada arquivo PHP (IDEs avançados podem fazer isso automaticamente).
#. Mova os templates originais de ``app/Resources/views/`` para ``templates/``
#. Faça qualquer outra alteração necessária para sua aplicação. Por exemplo, se os seus front
   controllers ``web/app_*.php`` originais foram personalizados, adicione essas alterações ao
   novo controlador ``public/index.php``.

.. _`Symfony Flex`: https://github.com/symfony/flex
.. _`Instalador do Symfony`: https://github.com/symfony/symfony-installer
.. _`Repositório de Receita Principal`: https://github.com/symfony/recipes
.. _`Repositório de Receita Contrib`: https://github.com/symfony/recipes-contrib
.. _`Documentação das Receitas Symfony`: https://github.com/symfony/recipes/blob/master/README.rst
