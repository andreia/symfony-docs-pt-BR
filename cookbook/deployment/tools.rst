.. index::
   single: Implantação; Ferramentas de Implantação

.. _how-to-deploy-a-symfony2-application:

Como Implantar uma Aplicação Symfony
====================================

.. note::

    A implantação pode ser uma tarefa complexa e variada, dependendo da configuração e das
    exigências da sua aplicação. Este artigo não é um guia passo-a-passo,
    mas uma lista geral dos requisitos mais comuns e idéias para implantação.

.. _symfony2-deployment-basics:

Noções Básicas sobre Implantação Symfony
----------------------------------------

As etapas comuns realizadas durante a implantação de uma aplicação Symfony incluem:

#. Upload do seu código no servidor de produção;
#. Instalar suas dependências (normalmente feito através Composer e pode ser feito
   antes de fazer o upload);
#. Executar migrações do banco de dados ou tarefas similares para atualizar quaisquer estruturas de dados alteradas;
#. Limpar (e, opcionalmente, o warming up) seu cache.

A implantação pode também incluir outras tarefas, tais como:

* Adicão de tag para uma determinada versão do seu código como um lançamento em seu repositório de controle de 
  versão;
* Criação de uma área de armazenamento temporária (staging) para construir sua configuração atualizada "offline";
* Execução de quaisquer testes disponíveis para garantir a estabilidade do código e/ou servidor;
* Remoção de quaisquer arquivos desnecessários do diretório ``web/`` para manter o seu
  ambiente de produção limpo;
* Limpeza de sistemas de cache externos (como `Memcached`_ ou `Redis`_).

Como Implantar uma Aplicação Symfony
------------------------------------

Existem várias formas de implantar uma aplicação Symfony. Comece com algumas
estratégias de implantação básicas e construa a partir daí.

Transferência de Arquivo Básica
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A forma mais básica de implantação de uma aplicação é copiar os arquivos manualmente
via ftp/scp (ou método similar). Isso tem suas desvantagens pois não há controle
sobre o sistema conforme a atualização progride. Esse método também requer que você realize
algumas etapas manuais depois de transferir os arquivos (veja `Tarefas Comuns Após a Implantação`_)

Usando Controle de Versão
~~~~~~~~~~~~~~~~~~~~~~~~~

Se você estiver usando controle de versão (por exemplo, Git ou SVN), pode simplificar tendo
sua instalação ao vivo também sendo uma cópia do seu repositório. Quando estiver pronto
para atualizá-lo é tão simples quanto buscar as atualizações mais recentes de seu sistema
de controle de versão.

Isso torna a atualização de seus arquivos mais *fácil*, porém você ainda precisa se preocupar
em realizar manualmente outras etapas (veja `Tarefas Comuns Após a Implantação`_).

Usando Build Scripts e outras Ferramentas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Há também ferramentas para ajudar na implantação. Algumas delas foram
especificamente adaptadas às exigências do Symfony.

``Capistrano`_ com o `plugin Symfony`_
    `Capistrano`_ é uma ferramenta para automação de servidor remoto e implantação escrita em Ruby. 
    `Symfony plugin`_ é um plugin para facilitar tarefas relacionadas ao Symfony, inspirado pelo `Capifony`_
    (que funciona apenas com o Capistrano 2)

`sf2debpkg`_
    Ajuda a construir um pacote Debian nativo para seu projeto Symfony.

`Magallanes`_
    Essa ferramenta de implantação, semalhante ao Capistrano, é construída em PHP e pode ser mais fácil
    para desenvolvedores PHP estenderem para suas necessidades.

`Fabric`_
    Essa biblioteca baseada em Python fornece um conjunto básico de operações para a execução de
    comandos shell locais ou remotos e upload/download de arquivos.

Bundles
    Há alguns `bundles que agregam funcionalidades de implantação`_ diretamente em seu
    Console Symfony.

Scripting básico
    Você pode, naturalmente, utilizar shell, `Ant`_ ou qualquer outra ferramenta de construção para escrever 
    o script de implantação do seu projeto.

Plataforma como Fornecedores de Serviços
    O Cookbook do Symfony inclui artigos detalhados para alguns dos provedores mais conhecidos
    de Plataforma como Serviço (PaaS):

    * :doc:`Microsoft Azure </cookbook/deployment/azure-website>`
    * :doc:`Heroku </cookbook/deployment/heroku>`
    * :doc:`Platform.sh </cookbook/deployment/platformsh>`

Tarefas Comuns Após a Implantação
---------------------------------

Depois de implantar o código fonte, há uma série de coisas comuns
que você precisa fazer:

A) Verifique os Requisitos
~~~~~~~~~~~~~~~~~~~~~~~~~~

Verifique se o servidor atende aos requisitos executando:

.. code-block:: bash

    $ php app/check.php

B) Configure o seu arquivo ``app/config/parameters.yml``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Esse arquivo *não* deve ser implantado, mas gerenciado através dos utilitários automáticos
fornecidos pelo Symfony.

C) Instale/Atualize seus Vendors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Os seus vendors podem ser atualizados antes de transferir seu código fonte (ou seja,
atualizar o diretório ``vendor``, e então transferir com seu código
fonte) ou mais tarde no servidor. De qualquer maneira, apenas atualize seus vendors
como faria normalmente:

.. code-block:: bash

    $ composer install --no-dev --optimize-autoloader

.. tip::

    A flag ``--optimize-autoloader`` melhora o desempenho do autoloader do Composer
    significativamente através da construção de um "mapa de classe". A flag ``--no-dev`` garante que
    o~s pacotes de desenvolvimento não serão instalados no ambiente de produção.

.. caution::

    Se você receber um erro "classe não encontrada" durante essa etapa, você pode precisar
    executar ``export SYMFONY_ENV=prod`` antes de executar esse comando para que
    os scripts``post-install-cmd`` `executem no ambiente ``prod``.

D) Limpe seu Cache Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~

Certifique-se de que você limpou (e warm-up) o cache do Symfony:

.. code-block:: bash

    $ php app/console cache:clear --env=prod --no-debug

E) Faça o Dump de seus Assets Assetic
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você estiver usando o Assetic, também vai querer fazer o dump de seus assets:

.. code-block:: bash

    $ php app/console assetic:dump --env=prod --no-debug

F) Outras Coisas!
~~~~~~~~~~~~~~~~~

Pode haver várias outras coisas que você precisa fazer, dependendo da sua
configuração:

* Executar quaisquer migrações de banco de dados
* Limpar o cache APC
* Executar ``assets:install`` (já cuidado no ``composer install``)
* Adicionar/editar CRON jobs
* Publicar assets para um CDN
* ...

Ciclo de Vida da Aplicação: Integração Contínua, QA, etc
--------------------------------------------------------

Enquanto esse artigo abrange os detalhes técnicos da implantação, o ciclo de vida completo
do código de desenvolvimento até produção pode ter vários outros passos
(implantação para staging, QA (Quality Assurance), execução de testes, etc).

O uso de staging, testes, QA, integração contínua, migrações de banco de dados
e a capacidade de reverter em caso de falha são fortemente aconselhados. Existem
ferramentas simples e mais complexas que podem tornar a implantação tão fácil
(ou sofisticada) quanto o seu ambiente requer.

Não se esqueça que implantar sua aplicação também envolve a atualização de qualquer dependência
(geralmente via Composer), a migração do seu banco de dados, limpar o cache e
outras coisas potenciais, como publicar assets para um CDN (veja `Tarefas Comuns Após a Implantação`_).

.. _`Capifony`: http://capifony.org/
.. _`Capistrano`: http://capistranorb.com/
.. _`sf2debpkg`: https://github.com/liip/sf2debpkg
.. _`Fabric`: http://www.fabfile.org/
.. _`Magallanes`: https://github.com/andres-montanez/Magallanes
.. _`Ant`: http://blog.sznapka.pl/deploying-symfony2-applications-with-ant
.. _`bundles que agregam funcionalidades de implantação`: http://knpbundles.com/search?q=deploy
.. _`Memcached`: http://memcached.org/
.. _`Redis`: http://redis.io/
.. _`Symfony plugin`: https://github.com/capistrano/symfony/
