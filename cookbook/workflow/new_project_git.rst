Como Criar e Armazenar um Projeto Symfony2 no git
=================================================

.. tip::

    Embora este artigo seja especificamente sobre git, os mesmos princípios genéricos
    aplicam-se caso você estiver armazenando o seu projeto no Subversion.

Uma vez que você leu :doc:`/book/page_creation` e familiarizou-se com 
o Symfony, já está, sem dúvida, pronto para começar o seu próprio projeto. Neste artigo 
do cookbook, você aprenderá a melhor maneira de iniciar um novo projeto Symfony2
que será armazenado usando o sistema gerenciador de controle de versão `git`_.

Configuração Inicial do Projeto 
-------------------------------

Para começar, você precisa baixar o Symfony e inicializar o seu repositório 
git local:

1. Baixe a `Edição Standard do Symfony2`_ sem vendors.

2. Descompacte a distribuição. Será criado um diretório chamado Symfony com
   a sua nova estrutura do projeto, arquivos de configuração, etc. Renomeie-o como desejar.

3. Crie um novo arquivo chamado ``.gitignore`` no raiz de seu novo projeto
   (Ex.: junto com o arquivo ``deps``) e cole o conteúdo seguinte nele. Os arquivos
   correspondentes a estes padrões serão ignorados pelo git:

    .. code-block:: text

        /web/bundles/
        /app/bootstrap*
        /app/cache/*
        /app/logs/*
        /vendor/  
        /app/config/parameters.ini

4. Copie ``app/config/parameters.ini`` para ``app/config/parameters.ini.dist``.
   O arquivo ``parameters.ini`` é ignorado pelo git (veja acima), de modo que as configurações específicas da máquina, 
   como senhas de banco de dados, não sejam comitadas. Criando o arquivo ``parameters.ini.dist``, 
   novos desenvolvedores podem, rapidamente, clonar o projeto, copiar este arquivo para
   ``parameters.ini``, personalizá-lo e iniciar o desenvolvimento.

5. Inicialize o seu repositório git:

    .. code-block:: bash
    
        $ git init

6. Adicione todos os arquivos iniciais ao git:

    .. code-block:: bash
    
        $ git add .

7. Crie um commit inicial para o seu projeto:

    .. code-block:: bash
    
        $ git commit -m "Initial commit"

8. Finalmente, baixe todas as bibliotecas vendor de terceiros:

    .. code-block:: bash
    
        $ php bin/vendors install

Neste ponto, você tem um projeto Symfony2 totalmente funcional que está corretamente
comitado com o git. Você pode iniciar imediatamente o desenvolvimento, comitando as novas
alterações em seu repositório git.

.. tip::

    Após executar o comando:

    .. code-block:: bash

        $ php bin/vendors install

    seu projeto irá conter um histórico git completo de todos os bundles 
    e bibliotecas definidos no arquivo ``deps``. Ele pode ter até 100 MB!
    Se você salvar as versões atuais de todas as suas dependências com o comando:

    .. code-block:: bash

        $ php bin/vendors lock

    então você pode remover o histórico de diretórios git com o seguinte comando:

    .. code-block:: bash

        $ find vendor -name .git -type d | xargs rm -rf

    O comando remove todos os diretórios ``.git`` contidos dentro do 
    diretório ``vendor``.

    Se você deseja atualizar os bundles definidos no arquivo ``deps`` após isto, você
    terá que reinstalá-los:

    .. code-block:: bash

        $ php bin/vendors install --reinstall

Você pode continuar acompanhando o capítulo :doc:`/book/page_creation`
para saber mais sobre como configurar e desenvolver dentro da sua aplicação.

.. tip::

    A Edição Standard do Symfony2 vem com algumas funcionalidades exemplo. Para
    remover o código de exemplo, siga as instruções contidas no `Readme da Edição Standard`_.

.. _cookbook-managing-vendor-libraries:

Gerenciando Bibliotecas Vendor com bin/vendors e deps
-----------------------------------------------------

Cada projeto Symfony usa um grande grupo de bibliotecas "vendor" de terceiros.

Por padrão, estas bibliotecas são baixadas executando o script 
``php bin/vendors install``. Este script lê o arquivo ``deps``, e baixa as 
bibliotecas ali informadas no diretório ``vendor/``. Ele também lê o arquivo ``deps.lock``, 
fixando cada biblioteca listada ao respectivo hash do commit git.

Nesta configuração, as bibliotecas vendor não fazem parte de seu repositório git,
nem mesmo como sub-módulos. Em vez disso, contamos com os arquivos ``deps`` e ``deps.lock``
e o script ``bin/vendors`` para gerenciar tudo. Esses arquivos são
parte de seu repositório, então, as versões necessárias de cada biblioteca de terceiros
tem controle de versão no git, e você pode usar o script vendors para trazer 
o seu projeto atualizado.

Sempre que um desenvolvedor clona um projeto, ele(a) deve executar o script ``php bin/vendors install``
para garantir que todas as bibliotecas vendor necessárias foram baixadas.

.. sidebar:: Atualizando o Symfony

    Uma vez que o Symfony é apenas um grupo de bibliotecas de terceiros e estas 
    bibliotecas são totalmente controladas através do ``deps`` e ``deps.lock``,
    atualizar o Symfony significa, simplesmente, atualizar cada um desses arquivos para combinar
    seu estado na última Edição Standard do Symfony.

    Claro, se você adicionou novas entradas ao ``deps`` ou ``deps.lock``, certifique-se
    de substituir apenas as partes originais (ou seja, não excluir as
    suas entradas personalizadas).

.. caution::

    Há também um comando ``php bin/vendors update``, mas isso não tem nada
    a ver com a atualização do seu projeto e você normalmente não precisará 
    utilizá-lo. Este comando é usado para congelar as versões de todas as suas bibliotecas vendor
    atualizando-as para a versão especificada em ``deps`` e gravando-as
    no arquivo ``deps.lock``.

    Além disso, se você deseja simplesmente atualizar o arquivo ``deps.lock``
    para o que já tem instalado, então, você pode simplesmente executar ``php bin/vendors lock``
    para armazenar os identificadores git SHA apropriados no arquivo deps.lock.

Vendors e sub-módulos
~~~~~~~~~~~~~~~~~~~~~

Em vez de usar o sistema ``deps`` e ``bin/vendors`` para gerenciar suas blibliotecas vendor, 
você pode optar por usar o `git submodules`_ nativo. Não há
nada de errado com esta abordagem, embora o sistema ``deps`` é a forma oficial
de resolver este problema e o ``git submodules`` pode ser difícil trabalhar 
às vezes.

Armazenando o seu Projeto em um Servidor Remoto
-----------------------------------------------

Agora, você tem um projeto Symfony2 totalmente funcional armazenado com o git. Entretanto,
na maioria dos casos, você também vai querer guardar o seu projeto em um servidor remoto
tanto para fins de backup quanto para que outros desenvolvedores possam colaborar com
o projeto.

A maneira mais fácil para armazenar o seu projeto em um servidor remoto é via `GitHub`_.
Repositórios públicos são gratuitos, porém, você terá que pagar uma taxa mensal
para hospedar repositórios privados.

Alternativamente, você pode armazenar seu repositório git em qualquer servidor, criando
um `repositório barebones`_ e, então, enviá-lo. Uma biblioteca que ajuda
neste gerenciamento é a `Gitolite`_.

.. _`git`: http://git-scm.com/
.. _`Edição Standard do Symfony2`: http://symfony.com/download
.. _`Readme da Edição Standard`: https://github.com/symfony/symfony-standard/blob/master/README.md
.. _`git submodules`: http://book.git-scm.com/5_submodules.html
.. _`GitHub`: https://github.com/
.. _`repositório barebones`: http://progit.org/book/ch4-4.html
.. _`Gitolite`: https://github.com/sitaramc/gitolite
