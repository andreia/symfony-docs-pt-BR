.. index::
   single: Implantação

Como implantar uma aplicação Symfony2
=====================================

.. note::

    A implantação pode ser uma tarefa complexa e variada, dependendo das configurações e necessidades.
    Este artigo não tenta explicar tudo, mas oferece os requisitos 
    e idéias mais comuns para a implantação.

Noções básicas sobre implantação no Symfony2
--------------------------------------------

As etapas comuns realizadas ao implantar uma aplicação Symfony2 incluem:

#. Fazer o upload do seu código modificado para o servidor;
#. Atualizar as suas dependências vendor (normalmente feita com o Composer, e pode
   ser feita antes do upload);
#. Executar migrações de banco de dados ou tarefas similares para atualizar quaisquer estruturas de dados modificadas;
#. Limpeza (e talvez mais importante, warming up) do cache.

A implantação também pode incluir outras coisas, tais como:

* Marcação (Tag) de uma versão específica do seu código como um release em seu repositório de controle de código fonte;
* Criação de uma "staging area" temporária para construir a sua configuração atualizada "offline";
* Execução de testes disponíveis para garantir a estabilidade do código e/ou do servidor;
* Remoção de todos os arquivos desnecessários do ``web`` para manter o seu ambiente de produção limpo;
* Limpeza de sistemas de cache externos (como `Memcached`_ ou `Redis`_).

Como implantar uma aplicação Symfony2
-------------------------------------

Existem várias maneiras de implantar uma aplicação Symfony2.

Vamos começar com algumas estratégias de implantação básicas e construir a partir daí.

Transferência de Arquivo Básica
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A forma mais básica de implantar uma aplicação é copiando os arquivos manualmente
via FTP/SCP (ou método similar). Isto tem as suas desvantagens, pois você não têm controle
sobre o sistema com o progresso de atualização. Este método também requer que você realize
algumas etapas manuais após transferir os arquivos (consulte `Tarefas Comuns Pós-Implantação`_)

Usando Controle de Código Fonte
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você estiver usando controle de código fonte (por exemplo, git ou svn), você pode simplificar com
a sua instalação ao vivo também sendo uma cópia de seu repositório. Quando estiver pronto
para atualizá-lo é tão simples quanto buscar as últimas atualizações de seu sistema
de controle de código fonte.

Isso torna a atualização de seus arquivos *fácil*, mas você ainda precisa se preocupar com
a realização manual de outros passos (consulte `Tarefas Comuns Pós-Implantação`_).

Usando Build scripts e outras ferramentas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Há também ferramentas de alta qualidade para ajudar a aliviar o sofrimento da implantação. Existem
até mesmo algumas ferramentas que foram especificamente adaptadas às exigências do
Symfony2, e que tem um cuidado especial para garantir que tudo, antes, durante
e após uma implantação ocorreu corretamente.

Veja `As ferramentas`_ para uma lista de ferramentas que podem ajudar com a implantação.

Tarefas Comuns Pós-Implantação
------------------------------

Depois de implantar o seu código fonte, há uma série de tarefas comuns
que precisam ser feitas:

A) Configurar o seu arquivo ``app/config/parameters.ini``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Este arquivo deve ser personalizado em cada sistema. O método utilizado para
implantar o seu código fonte *não* deve implantar esse arquivo. Em vez disso, você deve
configurá-lo manualmente (ou através de algum processo de construção) em seu(s) servidor(es).

B) Atualizar os seus vendors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Seus vendors podem ser atualizados antes de transferir o seu código fonte (ou seja,
atualizar o diretório ``vendor/``, e, em seguida, transferir com seu código
fonte) ou depois no servidor. De qualquer forma, apenas atualize os seus vendors
como faria normalmente:

.. code-block:: bash

    $ php composer.phar install --optimize-autoloader

.. tip::

    A flag ``--optimize-autoloader`` deixa o autoloader do Composer com mais
    performance através da construção de um "mapa de classe".

C) Limpar o cache do Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Certifique-se de limpar (e o warm-up) o cache do Symfony:

.. code-block:: bash

    $ php app/console cache:clear --env=prod --no-debug

D) Dump dos assets do Assetic
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você estiver usando o Assetic, você também vai desejar fazer o dump de seus assets:

.. code-block:: bash

    $ php app/console assetic:dump --env=prod --no-debug

E) Outras coisas!
~~~~~~~~~~~~~~~~~

Podem haver muitas outras coisas que você precisa fazer, dependendo de sua
configuração:

* A execução de quaisquer migrações de banco de dados
* Limpar o cache do APC
* Executar ``assets:install`` (já considerado em ``composer.phar install``)
* Adicionar/editar CRON jobs
* Mover os assets para um CDN
* ...

Ciclo de Vida da Aplicação: Integração Contínua, QA, etc
--------------------------------------------------------

Embora este artigo abrange os detalhes técnicos da implantação, o ciclo de vida completo
de buscar o código do desenvolvimento até a produção pode ter muito mais passos
(pense na implantação para staging, QA, execução de testes, etc.)

O uso de staging, testes, QA, integração contínua, as migrações de banco de dados
e a capacidade de reverter em caso de falha são todos fortemente aconselhados. Existem
ferramentas simples e outras mais complexas, que podem fazer a implantação tão fácil
(ou sofisticada) quanto o seu ambiente requer.

Não se esqueça que a implantação de sua aplicação também envolve a atualização de qualquer dependência
(normalmente através do Composer), a migração do seu banco de dados, limpar o cache e
outras coisas potenciais como mover os assets para um CDN (consulte `Tarefas Comuns Pós-Implantação`_).

As Ferramentas
--------------

`Capifony`_:

    Esta ferramenta fornece um conjunto especializado de ferramentas no topo do Capistrano, especificamente sob
    medida para projetos symfony e Symfony2.

`sf2debpkg`_:

    Esta ferramenta ajuda a criar um pacote Debian nativo para o seu projeto Symfony2.

`Magallanes`_:

    Esta ferramenta de implantação, semelhante ao Capistrano, é construída em PHP, e pode ser mais fácil,
    para os desenvolvedores PHP, estendê-la para as suas necessidades.

Bundles:

    Há muitos `bundles que adicionam recursos de implantação`_ diretamente em seu
    console do Symfony2.

Script básico:

    Você pode, com certeza, usar o shell, `Ant`_, ou qualquer outra ferramenta de construção para criar o script
    de implantação de seu projeto.

Plataforma como Prestadora de Serviços:

    PaaS é uma forma relativamente nova para implantar sua aplicação. Tipicamente uma PaaS
    vai usar um único arquivo de configuração no diretório raiz do seu projeto para
    determinar como construir um ambiente em tempo real que suporta o seu software.
    Um provedor com suporte confirmado para o Symfony2 é o `PagodaBox`_.

.. tip::

    Procurando mais? Fale com a comunidade no `Canal IRC do Symfony`_ #symfony
    (no freenode) para obter mais informações.

.. _`Capifony`: http://capifony.org/
.. _`sf2debpkg`: https://github.com/liip/sf2debpkg
.. _`Ant`: http://blog.sznapka.pl/deploying-symfony2-applications-with-ant
.. _`PagodaBox`: https://github.com/jmather/pagoda-symfony-sonata-distribution/blob/master/Boxfile
.. _`Magallanes`: https://github.com/andres-montanez/Magallanes
.. _`bundles que adicionam recursos de implantação`: http://knpbundles.com/search?q=deploy
.. _`Canal IRC do Symfony`: http://webchat.freenode.net/?channels=symfony
.. _`Memcached`: http://memcached.org/
.. _`Redis`: http://redis.io/
