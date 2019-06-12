Servidor Web Local do Symfony
=============================

Você pode executar aplicações Symfony usando qualquer servidor web (Apache, nginx, o
servidor web interno do PHP, etc.). No entanto, o Symfony fornece seu próprio servidor web
para tornar ainda mais produtivo desenvolver suas aplicações.

Embora esse servidor não seja destinado ao uso em produção, ele suporta HTTP/2,
TLS/SSL, geração automática de certificados de segurança, domínios locais, e muitos
outros recursos que, mais cedo ou mais tarde, serão necessários ao desenvolver projetos web.
Além disso, o servidor não está ligado ao Symfony e você também pode usá-lo com qualquer
outra aplicação PHP e até mesmo com HTML/SPA (aplicações de uma única página).

Instalação
----------

O servidor Symfony é distribuído como um binário instalável gratuito, sem qualquer
dependência, e possui suporte para Linux, macOS e Windows. Acesse `symfony.com/download`_
e siga as instruções para o seu sistema operacional.

.. note::

    Se você quiser reportar um erro ou sugerir um novo recurso, crie um issue
    em `symfony/cli`_.

Começando
---------

O servidor Symfony é iniciado uma vez por projeto, logo, você poderá ter
várias instâncias (cada uma delas ouvindo uma porta diferente). Este é o
fluxo de trabalho comum para executar um projeto Symfony:

.. code-block:: terminal

    $ cd my-project/
    $ symfony server:start

      [OK] Web server listening on http://127.0.0.1:....
      ...

    # Agora, navegue na URL fornecida, ou execute este comando:
    $ symfony open:local

Executar o servidor dessa forma faz com que ele exiba as mensagens de log no console,
logo você não poderá executar outros comandos ao mesmo tempo. Se preferir, você pode
executar o servidor Symfony em segundo plano:

.. code-block:: terminal

    $ cd my-project/

    # inicia o servidor em background
    $ symfony server:start -d

    # continua trabalhando e executando outros comandos...

    # exibe as últimas mensagens de log
    $ symfony server:log

Ativando TLS
------------

Navegar localmente na versão segura das suas aplicações é importante para poder detectar no início
problemas com conteúdo misto, e para poder executar bibliotecas que só rodam com HTTPS.
Tradicionalmente, isso tem sido doloroso e complicado de configurar, mas o servidor Symfony
automatiza tudo. Primeiro, execute este comando:

.. code-block:: terminal

    $ symfony server:ca:install

Esse comando cria uma autoridade de certificação local, registra-a em seu sistema de
armazenamento confiável, registra-a no Firefox (isso é necessário apenas para esse navegador)
e cria um certificado padrão para ``localhost`` e ``127.0.0.1``. Em outras
palavras, faz tudo por você.

Antes de navegar na sua aplicação local com HTTPS em vez de HTTP, reinicie seu
servidor parando-o e iniciando-o novamente.

Configurações PHP Diferentes por Projeto
----------------------------------------

Selecionando uma Versão Diferente do PHP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você tem várias versões do PHP instaladas em seu computador, você pode dizer
ao Symfony qual delas usar, criando um arquivo chamado ``.php-version`` no
diretório raiz do projeto:

.. code-block:: terminal

    $ cd my-project/

    # use a specific PHP version
    $ echo "7.2" > .php-version

    # use any PHP 7.x version available
    $ echo "7" > .php-version

.. tip::

    O servidor Symfony atravessa a estrutura de diretórios até o diretório
    raiz, então, você pode criar um arquivo ``.php-version`` em algum diretório
    pai para definir a mesma versão do PHP para um grupo de projetos sob esse
    diretório.

Execute o comando abaixo se você não se lembrar de todas as versões do PHP instaladas no seu
computador:

.. code-block:: terminal

    $ symfony local:php:list

      # Você verá todos os SAPIs suportados (CGI, FastCGI, etc.) para cada versão.
      # FastCGI (php-fpm) é usado quando possível; então CGI (que atua como um servidor
      # FastCGI também) e, finalmente, o servidor retorna ao CGI simples.

Sobrescrevendo Opções de Configuração do PHP por Projeto
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você pode alterar o valor de qualquer opção de configuração runtime do PHP por projeto, criando um
arquivo chamado ``php.ini`` no diretório raiz do projeto. Adicione apenas as opções que você deseja
sobrescrever:

.. code-block:: terminal

    $ cd my-project/

    # this project only overrides the default PHP timezone
    $ cat php.ini
    [Date]
    date.timezone = Asia/Tokyo

Executando Comandos com Versões Diferentes do PHP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ao executar versões diferentes do PHP, é útil usar o comando principal ``symfony``
como um wrapper para o comando ``php``. Isso permite que você sempre selecione
a versão mais apropriada do PHP de acordo com o projeto que está executando os
comandos. E também carrega as variáveis env automaticamente, que é importante ao
executar comandos que não são do Symfony:

.. code-block:: terminal

    # runs the command with the default PHP version
    $ php -r "..."

    # runs the command with the PHP version selected by the project
    # (or the default PHP version if the project didn't select one)
    $ symfony php -r "..."

Se você está usando esse wrapper com freqüência, considere criar um alias no comando ``php``
para isso:

.. code-block:: terminal

    $ cd ~/.symfony/bin
    $ cp symfony php
    # agora você pode executar "php ..." e o comando "symfony" será executado ao invés dele

    # pode ser usado o wrapper para outros comandos PHP também usando esse truque
    $ cp symfony php-config
    $ cp symfony pear
    $ cp symfony pecl

Nomes de Domínio Local
----------------------

Por padrão, os projetos são acessíveis em alguma porta aleatória do IP local
``127.0.0.1``. No entanto, às vezes é preferível associar um nome de domínio a eles:

* É mais conveniente quando você trabalha continuamente no mesmo projeto porque
  números de porta podem mudar, mas os domínios não;
* O comportamento de algumas aplicações depende de seus domínios/subdomínios;
* Para ter endpoints estáveis, como a URL de redirecionamento local para OAuth2.

Configurando o Proxy Local
~~~~~~~~~~~~~~~~~~~~~~~~~~

Domínios locais são possíveis graças a um proxy local fornecido pelo servidor
Symfony. Primeiro, inicie o proxy:

.. code-block:: terminal

    $ symfony proxy:start

Se esta for a primeira vez que você executa o proxy, siga estas etapas adicionais:

* Abra a **configuração de rede** do seu sistema operacional;
* Encontre as **configurações de proxy** e selecione **"Configuração de Proxy Automática"**;
* Defina a seguinte URL como seu valor: ``http://127.0.0.1:7080/proxy.pac``

Definindo o Domínio Local
~~~~~~~~~~~~~~~~~~~~~~~~~

Por padrão, o Symfony propõe ``.wip`` (para *Work in Progress*) para os domínios
locais. Você pode definir um domínio local para o seu projeto da seguinte forma:

.. code-block:: terminal

    $ cd my-project/
    $ symfony proxy:domain:attach my-domain

Se você instalou o proxy local conforme explicado na seção anterior, você
agora pode navegar ``https://my-domain.wip`` para acessar seu projeto local com o
novo domínio personalizado.

.. tip::

    Navegue na URL http://127.0.0.1:7080 para obter a lista completa de diretórios dos projetos
    locais, seus domínios personalizados, e números de porta.

Ao executar comandos do console, adicione a variável env ``HTTPS_PROXY`` para que os
domínios personalizados funcionem:

.. code-block:: terminal

    $ HTTPS_PROXY=http://127.0.0.1:7080 curl https://my-domain.wip

.. tip::

    Se você preferir usar um TLD diferente, edite o arquivo ``~/.symfony/proxy.json``
    (onde ``~`` significa o caminho para o seu diretório de usuário) e altere o
    valor da opção ``tld`` de ``wip`` para qualquer outro TLD.

Comandos de Longa Duração
-------------------------

Comandos de longa duração, como os que compilam ativos web do front-end, bloqueiam
o terminal e você não pode executar outros comandos ao mesmo tempo. O servidor do
Symfony fornece um comando ``run`` para envolvê-los da seguinte maneira:

.. code-block:: terminal

    # compila assets Webpack usando o Symfony Encore ... mas faz isso
    # em background para não bloquear o terminal
    $ symfony run -d yarn encore dev --watch

    # continua trabalhando e executando outros comandos...

    # de tempo em tempo, verifique os logs do comando se você desejar
    $ symfony server:log

    # e você também pode verificar se o comando ainda está em execução
    $ symfony server:status
    Web server listening on ...
    Command "yarn ..." running with PID ...

    # parar o servidor web (e todos os comandos associados) quando tiver terminado
    $ symfony server:stop

Integração Docker
------------------

O servidor local do Symfony fornece integração completa com `Docker`_ para projetos que
utilizam ele. Primeiro, certifique-se de expor as portas do contêiner:

.. code-block:: yaml

    # docker-compose.override.yaml
    services:
        database:
            ports:
                - "3306"

        redis:
            ports:
                - "6379"

        # ...

Em seguida, verifique seus nomes de serviço e atualize-os, se necessário (o Symfony cria
variáveis de ambiente seguindo o nome dos serviços, para que possam ser
autoconfigurados):

.. code-block:: yaml

    # docker-compose.yaml
    services:
        # DATABASE_URL
        database: ...
        # MONGODB_DATABASE, MONGODB_SERVER
        mongodb: ...
        # REDIS_URL
        redis: ...
        # ELASTISEARCH_HOST, ELASTICSEARCH_PORT
        elasticsearch: ...
        # RABBITMQ_DSN
        rabbitmq: ...

Se o seu arquivo ``docker-compose.yaml`` não usa os nomes das variáveis de ambiente
esperadas pelo Symfony (por exemplo, você usa ``MYSQL_URL`` em vez de ``DATABASE_URL``)
então, você precisa renomear todas as ocorrências dessas variáveis de ambiente em sua
aplicação Symfony. Uma alternativa mais simples é usar o arquivo ``.env.local`` para
reatribuir as variáveis de ambiente:

.. code-block:: bash

    # .env.local
    DATABASE_URL=${MYSQL_URL}
    # ...

Agora você pode iniciar os contêineres e todos os seus serviços serão expostos. Navegue
para qualquer página da sua aplicação e verifique a seção "Servidor Symfony" na
barra de ferramentas de depuração web. Você verá que "Docker Compose" está "Up".

Integração com SymfonyCloud
---------------------------

O servidor local do Symfony fornece integração completa, mas opcional, com
`SymfonyCloud`_, um serviço otimizado para executar suas aplicações Symfony na
nuvem. Ele fornece recursos como criação de ambientes, backups/snapshots,
e até mesmo acesso a uma cópia dos dados de produção de sua máquina local para ajudar
a depurar quaisquer problemas.

`Leia os documentos técnicos do SymfonyCloud`_.

Recursos Bônus
--------------

Além de ser um servidor web local, o servidor Symfony fornece outros
recursos úteis:

Procurando Vulnerabilidades de Segurança
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Em vez de instalar o :doc:`Verificador de Segurança do Symfony </security/security_checker>`
como uma dependência em seus projetos, você pode executar o seguinte comando:

.. code-block:: terminal

    $ symfony security:check

Esse comando usa o mesmo banco de dados de vulnerabilidades que o Verificador de 
Segurança do Symfony, mas não faz chamadas HTTP para o endpoint oficial da API. Tudo
(exceto clonar o banco de dados público) é feito localmente, o que é melhor para cenários
de CI (*integração contínua*).

Criando Projetos Symfony
~~~~~~~~~~~~~~~~~~~~~~~~

Além das `formas diferentes de instalar o Symfony`_, você pode usar estes
comandos do servidor Symfony:

.. code-block:: terminal

    # cria um novo projeto baseado em symfony/skeleton
    $ symfony new my_project_name

    # cria um novo projeto baseado em symfony/website-skeleton
    $ symfony new --full my_project_name

    # cria um novo projeto baseado em symfony/demo
    $ symfony new --demo my_project_name

Você pode criar um projeto dependendo de uma versão de **desenvolvimento** também (note
que o Composer também irá configurar a estabilidade para ``dev`` para todas as dependências root):

.. code-block:: terminal

    # cria um novo projeto baseado na branch master do Symfony
    $ symfony new --version=dev-master my_project_name

    # cria um novo projeto baseado na branch 4.3 dev do Symfony
    $ symfony new --version=4.3.x-dev my_project_name

.. _`symfony.com/download`: https://symfony.com/download
.. _`symfony/cli`: https://github.com/symfony/cli
.. _`formas diferentes de instalar o Symfony`: https://symfony.com/download
.. _`Docker`: https://en.wikipedia.org/wiki/Docker_(software)
.. _`SymfonyCloud`: https://symfony.com/cloud/
.. _`Leia os documentos técnicos do SymfonyCloud`: https://symfony.com/doc/master/cloud/intro.html
