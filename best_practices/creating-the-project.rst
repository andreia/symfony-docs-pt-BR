Criando o projeto
=================

Instalando o Symfony
--------------------

Há apenas uma maneira recomendada para instalar o Symfony:

.. best-practice::

    Use sempre o `Composer`_ para instalar o Symfony.

Composer é o gerenciador de dependência usado por aplicações PHP modernas. Adicionar
ou remover requisitos para o seu projeto e atualizar as bibliotecas de terceiros
usadas em seu código é muito fácil graças ao Composer.

Gerenciamento de Dependência com o Composer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Antes de instalar o Symfony, você precisa certificar-se de que tem o Composer instalado
globalmente. Abra o seu terminal (também chamado de *console de comando*) e execute o seguinte
comando:

.. code-block:: bash

    $ composer --version
    Composer version 1e27ff5e22df81e3cd0cd36e5fdd4a3c5a031f4a 2014-08-11 15:46:48

Você provavelmente verá um identificador de versão diferente. Não se preocupe porque o Composer
é atualizado em uma base contínua e sua versão específica não importa.

Instalando o Composer Globalmente
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No caso de você não ter o Composer instalado globalmente, execute os dois
comandos seguintes se você usa Linux ou Mac OS X (o segundo comando irá pedir a sua
senha de usuário):

.. code-block:: bash

    $ curl -sS https://getcomposer.org/installer | php
    $ sudo mv composer.phar /usr/local/bin/composer

.. note::

    Dependendo da sua distribuição Linux, você pode precisar executar o comando ``su``
    em vez de ``sudo``.

Se você usa um sistema Windows, baixe o instalador executável da
`página de download do Composer`_ e siga os passos para instalá-lo.

Criando a Aplicação Blog
------------------------

Agora que tudo está configurado corretamente, você pode criar um novo projeto usando o
Symfony. Em seu console de comando, navegue até o diretório onde você tem permissão
para criar arquivos e execute os seguintes comandos:

.. code-block:: bash

    $ cd projects/
    $ composer create-project symfony/framework-standard-edition blog/

Esse comando irá criar um novo diretório chamado ``blog`` que conterá
um novo projeto usando a versão estável mais recente disponível do Symfony.

Verificando a Instalação do Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Uma vez que a instalação estiver concluída, entre no diretório ``blog/`` e verifique se
o Symfony está instalado corretamente, executando o seguinte comando:

.. code-block:: bash

    $ cd blog/
    $ php app/console --version

    Symfony version 2.6.* - app/dev/debug

Se você ver a versão do Symfony instalado, tudo funcionou como esperado. Se não,
você pode executar o *script* a seguir para verificar o que impede o seu sistema
de executar as aplicações Symfony corretamente:

.. code-block:: bash

    $ php app/check.php

Dependendo do seu sistema, você pode ver até duas listas diferentes ao executar o
script `check.php`. A primeira mostra os requisitos obrigatórios que o seu
sistema deve atender para executar aplicações Symfony. A segunda lista mostra o
requisitos opcionais sugeridos para uma ótima execução das aplicações Symfony:

.. code-block:: bash

    Symfony2 Requirements Checker
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    > PHP is using the following php.ini file:
      /usr/local/zend/etc/php.ini

    > Checking Symfony requirements:
      .....E.........................W.....

    [ERROR]
    Your system is not ready to run Symfony2 projects

    Fix the following mandatory requirements
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

     * date.timezone setting must be set
       > Set the "date.timezone" setting in php.ini* (like Europe/Paris).

    Optional recommendations to improve your setup
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

     * short_open_tag should be disabled in php.ini
       > Set short_open_tag to off in php.ini*.


.. tip::

    Os lançamentos do Symfony são assinados digitalmente por razões de segurança. Se você quiser
    verificar a integridade da instalação do Symfony, dê uma olhada no
    `repositório público de checksums`_ e siga `esses passos`_ para verificar as
    assinaturas.

Estruturando a Aplicação
------------------------

Depois de criar a aplicação, entre no diretório ``blog/`` e você verá
arquivos e diretórios que foram gerados automaticamente:

.. code-block:: text

    blog/
    ├─ app/
    │  ├─ console
    │  ├─ cache/
    │  ├─ config/
    │  ├─ logs/
    │  └─ Resources/
    ├─ src/
    │  └─ AppBundle/
    ├─ vendor/
    └─ web/

Essa hierarquia de arquivos e diretórios é a convenção proposta pelo Symfony para
estruturar suas aplicações. O objetivo recomendado de cada diretório é o
seguinte:

* ``app/cache/``, armazena todos os arquivos de cache gerados pela aplicação;
* ``app/config/``, armazena todas as configurações definidas para qualquer ambiente;
* ``app/logs/``, armazena todos os arquivos de log gerados pela aplicação;
* ``app/Resources/``, armazena todos os templates e os arquivos de tradução da
  aplicação.
* ``src/AppBundle/``, armazena o código específico do Symfony (controladores e rotas),
  seu código de domínio (por exemplo, classes do Doctrine) e toda a sua lógica de negócio;
* ``vendor/``, esse é o diretório onde o Composer instala as dependências da
  aplicação e você nunca deve modificar quaisquer dos seus conteúdos;
* ``web/``, armazena todos os arquivos de front controller e todos os assets web, tais
  como folhas de estilo, arquivos JavaScript e imagens.

Bundles da aplicação
~~~~~~~~~~~~~~~~~~~~

Quando o Symfony 2.0 foi lançado, a maioria dos desenvolvedores naturalmente adotou a forma
de dividir as aplicações em módulos lógicos do Symfony 1.x. É por isso que muitas apps Symfony
usam bundles para dividir seu código em recursos lógicos: ``UserBundle``,
``ProductBundle``, ``InvoiceBundle``, etc.

Mas um bundle é *designado* para ser algo que possa ser reutilizado como um pedaço de software
stand-alone. Se ``UserBundle`` não pode ser usado *"como é"* em outras apps Symfony
, então não deve ter seu próprio bundle. Além disso, ``InvoiceBundle`` depende
do ``ProductBundle``, então, não há nenhuma vantagem em ter dois bundles separados.

.. best-practice::

    Criar apenas um bundle chamado ``AppBundle`` para a lógica da aplicação

A implementação de um único bundle ``AppBundle`` em seus projetos vai tornar o seu código
mais conciso e fácil de entender. A partir do Symfony 2.6, a documentação
oficial do Symfony usa o nome ``AppBundle``.

.. note::

    Não há necessidade de prefixar o ``AppBundle`` com seu próprio vendor (por exemplo,
    ``AcmeAppBundle``), pois este bundle de aplicação nunca será
    compartilhado.

Ao todo, esta é a estrutura de diretórios típica de uma aplicação Symfony
que segue as melhores práticas:

.. code-block:: text

    blog/
    ├─ app/
    │  ├─ console
    │  ├─ cache/
    │  ├─ config/
    │  ├─ logs/
    │  └─ Resources/
    ├─ src/
    │  └─ AppBundle/
    ├─ vendor/
    └─ web/
       ├─ app.php
       └─ app_dev.php

.. tip::

    Se você estiver usando o Symfony 2.6 ou uma versão mais recente, o bundle ``AppBundle``
    já foi gerado para você. Se você estiver usando uma versão mais antiga do Symfony,
    você pode gerá-lo, executando o seguinte comando manualmente:

    .. code-block:: bash

        $ php app/console generate:bundle --namespace=AppBundle --dir=src --format=annotation --no-interaction

Estendendo a Estrutura de Diretório
-----------------------------------

Se o seu projeto ou a infraestrutura requer algumas alterações na estrutura de diretórios padrão
do Symfony, você pode `sobrescrever a localização dos diretórios principais`_:
``cache/``, ``logs/`` e ``web/``.

Além disso, o Symfony3 usará uma estrutura de diretório ligeiramente diferente quando
ele for lançado:

.. code-block:: text

    blog-symfony3/
    ├─ app/
    │  ├─ config/
    │  └─ Resources/
    ├─ bin/
    │  └─ console
    ├─ src/
    ├─ var/
    │  ├─ cache/
    │  └─ logs/
    ├─ vendor/
    └─ web/

As mudanças são bastante superficiais, mas, por hora, nós recomendamos que você use
a estrutura de diretórios do Symfony2.

.. _`Composer`: https://getcomposer.org/
.. _`Get Started`: https://getcomposer.org/doc/00-intro.md
.. _`página de download do Composer`: https://getcomposer.org/download/
.. _`sobrescrever a localização dos diretórios principais`: http://symfony.com/doc/current/cookbook/configuration/override_dir_structure.html
.. _`repositório público de checksums`: https://github.com/sensiolabs/checksums
.. _`esses passos`: http://fabien.potencier.org/article/73/signing-project-releases
