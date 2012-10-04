.. index::
   single: Cache

HTTP Cache
==========

A natureza das aplicações web ricas é que elas sejam dinâmicas. Não importa
quão eficiente seja sua aplicação, cada uma das requisições sempre terá uma
carga maior do que se ela servisse um arquivo estático.

E, para a maior parte das aplicações web, isso não é problema. O Symfony 2 é
extremamente rápido e, a menos que você esteja fazendo algo extramente pesado,
as requisições serão retornadas rapidamente sem sobrecarregar demais o seu
servidor.

Mas, a medida que seu site cresce, essa carga adicional pode se tornar um
problema. O processamento que normalmente é efetuado a cada requisição deveria
ser feito apenas uma vez. É exatamente esse o objetivo do cache.

Fazendo Cache nos Ombros de Gigantes
------------------------------------

O modo mais efetivo de melhorar a performance de uma aplicação é fazendo cache
da saída completa de uma página e então ignorar a aplicação totalmente nas
requisições seguintes. É claro, nem sempre isso é possível para sites altamente
dinâmicos. Ou será que é? Nesse capítulo, veremos como o sistema de cache do
Symfony2 trabalha e por que nós acreditamos que esta é a melhor abordagem
possível.

O sistema de cache do Symfony2 é diferente porque ele se baseia na simplicidade
e no poder do cache HTTP como definido na :term:`especificação HTTP`. Em vez de
inventar uma nova metodologia de cache, o Symfony2 segue o padrão que define a
comunicação básica na Web. Quando você entender os modelos fundamentais de
validação e expiração de cache HTTP estará pronto para dominar o sistema de
cache do Symfony2.

Para os propósitos de aprender como fazer cache com o Symfony2, cobriremos o
assunto em quatro passos:

* **Passo 1**: Um :ref:`gateway cache <gateway-caches>`, ou proxy reverso,
  é uma camada independente que fica na frente da sua aplicação. O proxy
  reverso faz o cache das respostas quando elas são retornadas pela sua
  aplicação e responde as requisições com respostas cacheadas antes que elas
  atinjam sua aplicação. O Symfony2 fornece um proxy reverso próprio, mas
  qualquer proxy reverso pode ser usado.

* **Passo 2**: Cabeçalhos de cache:ref:`cache HTTP<http-cache-introduction>` são
  usados para comunicar com o gateway cache e qualquer outro cache entre sua
  aplicação e o cliente. O Symfony2 fornece padrões razoáveis e uma interface
  poderosa para interagir com os cabeçalhos de cache.

* **Passo 3**: :ref:`Expiração e validação <http-expiration-validation>` HTTP 
  são dois modelos usados para determinar se o conteúdo cacheado é *atual/fresh*
  (pode ser reutilizado a partir do cache) ou se o conteúdo é *antigo/stale*
  (deve ser recriado pela aplicação).

* **Passo 4**: :ref:`Edge Side Includes <edge-side-includes>` (ESI) permitem
  que sejam usados caches HTTP para fazer o cache de fragmentos de páginas
  (mesmo fragmentos aninhados) independentemente. Com o ESI, você pode até
  fazer o cache de uma página inteira por 60 minutos, e uma barra lateral
  embutida por apenas 5 minutos.

Como fazer cache com HTTP não é uma coisa apenas do Symfony, já existem muitos
artigos sobre o assunto. Se você for iniciante em cache HTTP, recomendamos
*fortemente* o artigo `Things Caches Do`_ do Ryan Tomayko. Outra fonte
aprofundada é o `Cache Tutorial`_ do Mark Nottingham.

.. index::
   single: Cache; Proxy
   single: Cache; Reverse Proxy
   single: Cache; Gateway

.. _gateway-caches:

Fazendo Cache com um Gateway Cache
----------------------------------

Quando se faz cache com HTTP, o *cache* é separado completamente da sua
aplicação e se coloca entre sua aplicação e o cliente que está fazendo a
requisição.

O trabalho do cache é receber requisições do cliente e transferi-las para sua
aplicação. O cache também receberá de volta respostas da sua aplicação e
as encaminhará para o cliente. O cache é o "middle-man" da comunicação
requisição-resposta entre o cliente e sua aplicação.

Ao longo do caminho, o cache guardará toda resposta que seja considerada
"cacheável" (Veja :ref:`http-cache-introduction`). Se o mesmo recurso for
requisitado novamente, o cache irá mandar a resposta cacheada para o cliente,
ignorando completamente sua aplicação.

Esse tipo de cache é conhecido como um gateway cache HTTP e existem vários como
o `Varnish`_, o `Squid in reverse proxy mode`_ e o proxy reverso do Symfony2.

.. index::
   single: Cache; Types of

Tipos de Cache
~~~~~~~~~~~~~~

Mas um gateway cache não é o único tipo de cache. Na verdade, os cabeçalhos de
cache HTTP enviados pela sua aplicação são consumidos e interpretados por
três tipos diferentes de cache:

* *Caches de Navegador*: Todo navegador vem com seu próprio cache local que
  é útil principalmente quando você aperta o "voltar" ou para imagens e outros
  assets. O cache do navegador é um cache *privado* assim os recursos cacheados
  não são compartilhados com ninguém mais.

* *Caches de Proxy*: Um proxy é um cache *compartilhado* assim muitas pessoas
  podem utilizar um único deles. Ele geralmente é instalado por grandes empresas
  e ISPs para reduzir a latência e o tráfego na rede.

* *Caches Gateway*: Como um proxy, ele também é um cache *compartilhado* mas
  no lado do servidor. Instalado por administradores de rede, ele torna os sites
  mais escaláveis, confiáveis e performáticos.

.. tip::

    Caches gateway algumas vezes são referenciados como caches de proxy reverso,
    surrogate caches ou até aceleradores HTTP.

.. note::

    A diferença entre os caches *privados* e os *compartilhados* se torna mais
    óbvia a medida que começamos a falar sobre fazer cache de respostas com
    conteúdo que é específico para exatamente um usuário (e.g. informação de
    uma conta).

Toda resposta da sua aplicação irá provavelmente passar por um ou ambos os
dois primeiros tipos de cache. Esses caches estão fora de seu controle mas
eles seguem o direcionamento do cache HTTP definido na resposta.

.. index::
   single: Cache; Symfony2 Reverse Proxy

.. _`symfony-gateway-cache`:

Proxy Reverso do Symfony2
~~~~~~~~~~~~~~~~~~~~~~~~~

O Symfony2 vem com um proxy reverso (também chamado de gateway cache) escrito
em PHP. É só habilitá-lo e as respostas cacheáveis da sua aplicação começaram
a ser cacheadas no mesmo momento. Sua instalação é bem simples. Toda nova
aplicação Symfony2 vem com um kernel de cache pré-configurado (``AppCache``)
que encapsula o kernel padrão (``AppKernel``). O Kernel de cache *é* o proxy
reverso.

Para habilitar o cache, altere o código do front controller para utilizar o
kernel de cache::

    // web/app.php

    require_once __DIR__.'/../app/bootstrap.php.cache';
    require_once __DIR__.'/../app/AppKernel.php';
    require_once __DIR__.'/../app/AppCache.php';

    use Symfony\Component\HttpFoundation\Request;

    $kernel = new AppKernel('prod', false);
    $kernel->loadClassCache();
    // wrap the default AppKernel with the AppCache one
    $kernel = new AppCache($kernel);
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);
    $response->send();
    $kernel->terminate($request, $response);

O kernel de cache funcionará imediatamente como um proxy reverso - fazendo
cache das respostas da sua aplicação e retornando-as para o cliente.

.. tip::

    O kernel de cache tem um método especial ``getLog()`` que retorna uma
    representação em texto do que ocorreu na camada de cache. No ambiente de
    desenvolvimento, utilize-o para depurar e validar sua estratégia de cache::

        error_log($kernel->getLog());

O objeto ``AppCache`` tem uma configuração padrão razoável, mas ela pode
receber um ajuste fino por meio de um conjunto de opções que podem ser definidas
sobrescrevendo o método ``getOptions()``::

    // app/AppCache.php
    class AppCache extends Cache
    {
        protected function getOptions()
        {
            return array(
                'debug'                  => false,
                'default_ttl'            => 0,
                'private_headers'        => array('Authorization', 'Cookie'),
                'allow_reload'           => false,
                'allow_revalidate'       => false,
                'stale_while_revalidate' => 2,
                'stale_if_error'         => 60,
            );
        }
    }

.. tip::

    A menos que seja sobrescrita em ``getOptions()``, a opção ``debug``
    será definida como o valor padrão de depuração no ``AppKernel`` envolvido.

Aqui vai uma lista das opções principais:

* ``default_ttl``: O número de segundos que uma entrada do cache deve ser
  considerada como atual quando nenhuma informação de atualização for passada
  na resposta. Os cabeçalhos explícitos ``Cache-Control`` e ``Expires``
  sobrescrevem esse valor (padrão: ``0``);

* ``private_headers``: Conjunto de cabeçalhos de requisição que acionam o
  comportamento ``Cache-Control`` "privado" nas respostas que não declaram
  explicitamente se a resposta é ``public`` ou ``private`` por meio de uma
  diretiva ``Cache-Control``. (padrão: ``Authorization`` e ``Cookie``);

* ``allow_reload``: Diz se o cliente pode forçar um recarregamento do
  cache incluindo uma diretiva ``Cache-Control`` "no-cache" na requisição.
  Defina ele como ``true`` para seguir a RFC 2616 (padrão: ``false``);

* ``allow_revalidate``: Diz se o cliente pode forçar uma revalidação do
  cache incluindo uma diretiva ``Cache-Control`` "max-age=0" na requisição.
  Defina ele como ``true`` para seguir a RFC 2616 (padrão: false);

* ``stale_while_revalidate``: Diz o número padrão de segundos (a granularidade
  é o segundo como na precisão da Resposta TTL) durante o qual o cache pode
  retornar imediatamente uma resposta antiga enquanto ele faz a revalidação
  dela no segundo plano (padrão: ``2``); essa configuração é sobrescrita pela
  extensão ``stale-while-revalidate`` do ``Cache-Control`` HTTP (veja RFC 5861);

* ``stale_if_error``: Diz o número padrão de segundos (a granularidade
  é o segundo) durante o qual o cache pode fornecer uma resposta antiga
  quando um erro for encontrado (padrão: ``60``). Essa configuração é
  sobrescrita pela extensão ``stale-if-error`` do ``Cache-Control``
  HTTP (veja RFC 5861).

Se ``debug`` for ``true``, o Symfony2 adiciona automaticamente um cabeçalho
``X-Symfony-Cache`` na resposta contendo informações úteis sobre o que o
cache serviu ou deixou passar.

.. sidebar:: Mudando de um Proxy Reverso para Outro

    O proxy reverso do Symfony2 é uma ferramenta importante 
    quando estiver desenvolvendo o seu site ou quando você faz o deploy de seu
    site num servidor compartilhado onde você não pode instalar nada mais
    do que código PHP. Mas como ele é escrito em PHP, não há como ele ser tão
    rápido quanto um proxy escrito em C. É por isso que recomendamos fortemente
    que você utilize o Varnish ou o Squid no seu servidor de produção quando for
    possível. A boa notícia é que mudar entre um servidor de proxy para outro é
    fácil e transparente pois nenhuma alteração de código é necessária em sua
    aplicação. Inicie de forma simples com o proxy reverso do Symfony2 e depois
    atualize para o Varnish quando o seu tráfego aumentar.

    Para mais informações de como usar o Varnish com o Symfony2, veja o
    capítulo :doc:`How to use Varnish </cookbook/cache/varnish>` do cookbook.

.. note::

    A performance do proxy reverso do Symfony2 não depende da complexidade da
    sua aplicação. Isso acontece porque o kernel da aplicação só é carregado
    quando a requisição precisar ser passada para ele.

.. index::
   single: Cache; HTTP

.. _http-cache-introduction:

Introdução ao Cache HTTP
------------------------

Para tirar vantagem das camadas de cache disponíveis, sua aplicação precisa ser
capaz de informar quais respostas são cacheáveis e as regras que governam
quando/como o cache o se torna antigo. Isso é feito configurando os cabeçalhos
HTTP na sua resposta.

.. tip::

    Lembre que o "HTTP" nada mais é do que uma linguagem (um linguagem de texto
    simples) que os clientes web (e.g navegadores) e os servidores web utilizam
    para se comunicar uns com os outros. Quando falamos sobre o cache HTTP,
    estamos falando sobre a parte dessa linguagem que permite que os clientes e
    servidores troquem informações relacionadas ao cache.

O HTTP define quatro cabeçalhos de cache para as respostas que devemos nos
preocupar:

* ``Cache-Control``
* ``Expires``
* ``ETag``
* ``Last-Modified``

O cabeçalho mais importante e versátil é o cabeçalho ``Cache-Control``, que na
verdade é uma coleção de várias informações de cache.

.. note::

    Cada um dos cabeçalhos será explicado detalhadamente na seção 
    :ref:`http-expiration-validation`.

.. index::
   single: Cache; Cache-Control Header
   single: HTTP headers; Cache-Control

O Cabeçalho Cache-Control
~~~~~~~~~~~~~~~~~~~~~~~~~

O cabeçalho ``Cache-Control`` é único pois ele contém não um, mas vários
pedaços de informação sobre a possibilidade de cache de uma resposta. Cada
pedaço de informação é separada por uma vírgula:

     Cache-Control: private, max-age=0, must-revalidate

     Cache-Control: max-age=3600, must-revalidate

O Symfony fornece uma abstração em volta do cabeçalho ``Cache-Control`` para
deixar sua criação mais gerenciável:

.. code-block:: php

    $response = new Response();

    // mark the response as either public or private
    $response->setPublic();
    $response->setPrivate();

    // set the private or shared max age
    $response->setMaxAge(600);
    $response->setSharedMaxAge(600);

    // set a custom Cache-Control directive
    $response->headers->addCacheControlDirective('must-revalidate', true);

Respostas Públicas vs Privadas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tanto o gateway cache quando o proxy cache são considerados caches
"compartilhados" pois o conteúdo cacheado é compartilhado por mais de um
usuário. Se uma resposta específica de um usuário for incorretamente armazenada
por um cache compartilhado, ela poderia ser retornada posteriormente para
um número incontável de usuários diferentes. Imagine se a informação da sua
conta fosse cacheada e depois retornada para todos os usuários que em seguida
solicitassem a página da conta deles!

Para lidar com essa situação, cada resposta precisa ser configurada para ser
pública ou privada:

* *public*: Indica que a resposta pode ser cacheada tanto por caches privados
  quanto pelos compartilhados;

* *private*: Indica que a mensagem toda ou parte da resposta é destinada para
  um único usuário e não deve ser cacheada por um cache compartilhado.

O Symfony tem como padrão conservador definir toda resposta como privada. Para
se beneficiar dos caches compartilhados (como o proxy reverso do Symfony2), a
resposta precisa ser definida como pública explicitamente.

.. index::
   single: Cache; Safe methods

Métodos Seguros
~~~~~~~~~~~~~~~

O cache HTTP só funciona para os métodos HTTP "seguros" (como o GET e o HEAD).
Ser seguro significa que você não consegue alterar o estado da aplicação no
servidor quando estiver respondendo a requisição (é claro que você pode logar a
informação, fazer cache dos dados etc). Isso tem duas consequências importantes:

* Você *nunca* deve alterar o estado de sua aplicação quando estiver respondendo
  uma requisição GET ou HEAD. Mesmo se você não usar um gateway cache, a
  presença de caches proxy faz com que qualquer requisição GET ou HEAD possa
  atingir ou não seu servidor.

* Não espere que os métodos PUT, POST ou DELETE sejam cacheados. Esses métodos
  são destinados para serem utilizados quando se quer alterar o estado da sua
  aplicação (e.g. excluir uma postagem de um blog). Fazer cache desses métodos
  poderia fazer com que certas requisições não chegassem na sua aplicação
  e a alterasse.

Regras e Padrões de Cache
~~~~~~~~~~~~~~~~~~~~~~~~~

O HTTP 1.1 permite por padrão fazer o cache de qualquer coisa a menos que seja
explícito que não num cabeçalho ``Cache-Control``. Na prática, a maioria dos
caches não faz nada quando as requisições tem um cookie, um cabeçalho de
autorização, usam um método inseguro (i.e PUT, POST, DELETE) ou quando as
respostas tem código de estado para redirecionamento.

O Symfony2 define automaticamente um cabeçalho ``Cache-Control`` conservador
quando o desenvolvedor não definir nada diferente seguindo essas regras:

* Se não for definido cabeçalho de cache (``Cache-Control``, ``Expires``,
  ``ETag`` or ``Last-Modified``), ``Cache-Control`` é configurado como
  ``no-cache``, indicando que não será feito cache da resposta;

* Se ``Cache-Control`` estiver vazio (mas outros cabeçalhos de cache estiverem
  presentes), seu valor é configurado como ``private, must-revalidate``;

* Mas se pelo menos uma diretiva``Cache-Control`` estiver definida, e nenhuma
  diretiva 'public' ou ``privada`` tiver sido adicionada explicitamente, o
  Symfony2 adiciona automaticamente a diretiva ``private`` (exceto quando
  ``s-maxage`` estiver definido).

.. _http-expiration-validation:

Expiração e Validação HTTP
--------------------------

A especificação HTTP define dois modelos de cache:

* Com o `expiration model`_, você especifica simplesmente quanto tempo uma
  resposta deve ser considerada "atual" incluindo um cabeçalho ``Cache-Control``
  e/ou um ``Expires``. Os caches que entendem a expiração não farão a mesma
  requisição até que a versão cacheada atinja o tempo de expiração e se torne
  "antiga".

* Quando as páginas são realmente dinâmicas (i.e. sua representação muda
  constantemente), o `validation model`_ é frequentemente necessário. Com esse
  modelo, o cache armazena a resposta, mas pergunta ao servidor a cada
  requisição se a resposta cacheada continua válida ou não. A aplicação utiliza
  um identificador único da resposta (o cabeçalho ``Etag``) e/ou um timestamp
  (o cabeçalho ``Last-Modified``) para verificar se a página mudou desde quando
  tinha sido cacheada.

O objetivo de ambos os modelos é nunca ter que gerar a mesma resposta duas vezes
contando com o cache para guardar e retornar respostas "atuais".

.. sidebar:: Lendo a Especificação HTTP

    A especificação HTTP define uma linguagem simples mas poderosa com a qual
    clientes e servidores podem se comunicar. Como um desenvolvedor web, o
    modelo requisição-resposta da especificação domina o seu trabalho.
    Infelizmente o documento real da especificação - `RFC 2616`_ - pode ser
    difícil de ler.

    Existe um trabalho em andamento (`HTTP Bis`_) para reescrever a RFC 2616.
    Ele não descreve uma nova versão do HTTP, mas principalmente esclarece a
    especificação HTTP original. A organização também melhorou pois a
    especificação foi dividida em sete partes; tudo que for relacionado ao
    cache HTTP pode ser encontrado em duas partes dedicadas (
    `P4 - Conditional Requests`_ e
    `P6 - Caching: Browser and intermediary caches`_)

    Como desenvolvedor web, nós recomendamos fortemente que você leia a
    especificação. Sua clareza e poder - ainda mais depois de dez anos da sua
    criação - é incalculável. Não se engane com a aparência da especificação -
    o conteúdo dela é muito mais bonito que sua capa.

.. index::
   single: Cache; HTTP Expiration

Expiração
~~~~~~~~~

O modelo de expiração é o mais eficiente e simples dos dois modelos de cache e
deve ser usado sempre que possível. Quando uma resposta é cacheada com uma
expiração, o cache irá armazenar a resposta e retorná-la diretamente sem
acessar a aplicação até que a resposta expire.

O modelo de expiração pode ser aplicado usando um desses dois, quase idênticos,
cabeçalhos HTTP: ``Expires`` ou ``Cache-Control``.

.. index::
   single: Cache; Expires header
   single: HTTP headers; Expires

Expiração com o Cabeçalho ``Expires``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

De acordo com a especificação HTTP, "o campo do cabeçalho ``Expires`` diz a
data/horário a partir do qual a resposta é considerada antiga." O cabeçalho
``Expires`` pode ser definido com o método ``setExpires()`` ``Response``. Ele
recebe uma instância de ``DateTime`` como argumento::

    $date = new DateTime();
    $date->modify('+600 seconds');

    $response->setExpires($date);

O cabeçalho HTTP resultante se parecerá com isso::

    Expires: Thu, 01 Mar 2011 16:00:00 GMT

.. note::

    O método ``setExpires()`` converte automaticamente a data para o fuso
    horário GMT como exigido na especificação.

O cabeçalho ``Expires`` sofre com duas limitações. Primeiro, o relógio no
servidor web e o cache (e.g. o navegador) precisam estar sincronizados. A outra
é que a especificação define que "servidores HTTP/1.1 não devem mandar datas
``Expires`` com mais de um ano no futuro."

.. index::
   single: Cache; Cache-Control header
   single: HTTP headers; Cache-Control

Expiração com o Cabeçalho ``Cache-Control``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Devido às limitações do cabeçalho ``Expires``, na maioria das vezes, você deve
usar no lugar dele o cabeçalho ``Cache-Control``. Lembre-se que o cabeçalho
``Cache-Control`` é usado para especificar várias diretivas de cache diferentes.
Para expiração, existem duas diretivas: ``max-age`` e ``s-maxage``. A primeira
é usada para todos os caches enquanto a segunda somente é utilizada por caches
compartilhados::

    // Define o número de segundos após o qual a resposta
    // não será mais considerada atual
    $response->setMaxAge(600);

    // O mesmo que acima, mas apenas para caches compartilhados
    $response->setSharedMaxAge(600);

O cabeçalho ``Cache-Control`` deve ter o seguinte formato (ele pode ter
diretivas adicionais)::

    Cache-Control: max-age=600, s-maxage=600

.. index::
   single: Cache; Validation

Validação
~~~~~~~~~

Quando um recurso precisa ser atualizado logo que uma mudança for feita em dados
relacionados, o modelo de expiração é insuficiente. Com o modelo de expiração,
a aplicação não será acionada para retornar a resposta atualizada até que o
cache finalmente se torne antigo.

O modelo de validação resolve esse problema. Nesse modelo, o cache continua a
armazenar as respostas. A diferença é que, para cada requisição, o cache
pede para a aplicação verificar se a respostas cacheada continua válida ou não.
Se o cache ainda *for* válido, sua aplicação deve retornar um código de estado
304 e nenhum conteúdo. Isso diz para o cache que ele pode retornar a resposta
cacheada.

Nesse modelo, você economiza principalmente banda pois a representação não é
enviada duas vezes para o mesmo cliente (uma resposta 304 é mandada no lugar).
Mas se projetar sua aplicação com cuidado, você pode ser capaz de pegar o
mínimo de dados necessário para enviar uma resposta 304 e também economizar CPU
(veja abaixo um exemplo de uma implementação).

.. tip::

    O código de estado 304 significa "Not Modified". Isso é importante pois
    com esse código de estado *não* é colocado o conteúdo real que está sendo
    requisitado. Em vez disso, a resposta é simplesmente um conjunto leve
    de direções que dizem ao cache para ele utilizar uma versão armazenada. 

Assim como com a expiração, existem dois cabeçalhos HTTP diferentes que podem
ser utilizados para implementar o modelo de validação: ``ETag`` e
``Last-Modified``.

.. index::
   single: Cache; Etag header
   single: HTTP headers; Etag

Validação com o Cabeçalho ``ETag``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O cabeçalho``ETag`` é um cabeçalho em texto (chamado de "entity-tag") que
identifica de forma única uma representação do recurso alvo. Ele é totalmente
gerado e configurado pela sua aplicação, dessa forma você pode dizer, por exemplo, se
o recurso ``/about`` que foi armazenado pelo cache está atualizado com o que
sua aplicação poderia retornar. Uma ``ETag`` é como uma impressão digital e é
utilizada para comparar rapidamente se duas versões diferentes de um recurso
são equivalentes. Como as impressões digitais, cada ``ETag`` precisa ser única
em todos as representações do mesmo recurso.

Vamos analisar uma implementação simples que gera a ETag como o hash md5 do
conteúdo::

    public function indexAction()
    {
        $response = $this->render('MyBundle:Main:index.html.twig');
        $response->setETag(md5($response->getContent()));
        $response->isNotModified($this->getRequest());

        return $response;
    }

O método ``Response::isNotModified()`` compara o ``ETag`` enviado na
``Request`` com o que está definido na ``Response``. Se os dois combinarem,
o método define automaticamente o código de estado da ``Response`` como 304.

Esse algoritmo é simples o suficiente e bem genérico, mas você precisa criar
a ``Response`` inteira antes de ser capaz de calcular a Etag, o que não é o
melhor possível. Em outras palavras, isso economiza banda de rede mas não faz
o mesmo com os ciclos de CPU.

Na seção :ref:`optimizing-cache-validation`, nós mostraremos como a validação
pode ser utilizada de forma mais inteligente para determinar a validade de
um cache sem muito trabalho.

.. tip::

    O Symfony2 também suporta ETags fracas passando ``true`` como segundo
    argumento para o método
    :method:`Symfony\\Component\\HttpFoundation\\Response::setETag`.

.. index::
   single: Cache; Last-Modified header
   single: HTTP headers; Last-Modified

Validação com o Cabeçalho ``Last-Modified``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O cabeçalho ``Last-Modified`` é a segunda forma de validação. De acordo com a
especificação HTTP, "O campo do cabeçalho ``Last-Modified`` indica a data e o
horário que o servidor de origem acredita que a representação foi modificada
pela última vez." Em outras palavras, a aplicação decide se o conteúdo cacheado
foi atualizado ou não tendo como base se ele foi atualizado desde que a
resposta foi cacheada.

Por exemplo, você pode usar a última data de atualização de todos os objetos
necessários para calcular a representação do recurso como o valor para o
cabeçalho ``Last-Modified``::

    public function showAction($articleSlug)
    {
        // ...

        $articleDate = new \DateTime($article->getUpdatedAt());
        $authorDate = new \DateTime($author->getUpdatedAt());

        $date = $authorDate > $articleDate ? $authorDate : $articleDate;

        $response->setLastModified($date);
        $response->isNotModified($this->getRequest());

        return $response;
    }

O método ``Response::isNotModified()`` compara o cabeçalho
``If-Modified-Since`` mandado pela requisição com o cabeçalho ``Last-Modified``
definido na resposta. Se eles forem equivalentes, a ``Response`` será 
configurada com um código de estado 304.

.. note::

    O cabeçalho da requisição ``If-Modified-Since`` é igual ao cabeçalho
    ``Last-Modified`` de uma resposta enviada ao cliente para um recurso
      específico. Essa é a forma como o cliente e o servidor se comunicam entre
      si e decidem se o recurso foi ou não atualizado desde que ele foi
    cacheado.

.. index::
   single: Cache; Conditional Get
   single: HTTP; 304

.. _optimizing-cache-validation:

Otimizando seu Código com Validação
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O objetivo principal de qualquer estratégia de cache é aliviar a carga sobre a
aplicação. Colocando de outra forma, quanto menos coisas você fizer na sua aplicação
para retornar uma resposta 304, melhor. O método ``Response::isNotModified()``
faz exatamente isso expondo um padrão simples e eficiente::

    public function showAction($articleSlug)
    {
        // Pega a informação mínima para calcular
        // a ETag ou o valor Last-Modified
        // (baseado na Requisição, o dado é recuperado
        // de um banco de dados ou de um armazenamento chave-valor)

        $article = // ...

        // cria uma Resposta com uma ETag e/ou um cabeçalho Last-Modified
        $response = new Response();
        $response->setETag($article->computeETag());
        $response->setLastModified($article->getPublishedAt());

        // Verifica se a Resposta não é diferente da Requisição
        if ($response->isNotModified($this->getRequest())) {
            // retorna imediatamente a Resposta 304
            return $response;
        } else {
            // faça mais algumas coisas aqui - como buscar mais dados
            $comments = // ...
            
            // ou renderize um template com a $response que você já
            // inicializou
            return $this->render(
                'MyBundle:MyController:article.html.twig',
                array('article' => $article, 'comments' => $comments),
                $response
            );
        }
    }

Quando a ``Response`` não tiver sido modificada, ``isNotModified()`` automaticamente
define o código de estado para ``304``, remove o conteúdo e remove alguns
cabeçalhos que não podem estar presentes em respostas ``304`` (veja
:method:`Symfony\\Component\\HttpFoundation\\Response::setNotModified`).

.. index::
   single: Cache; Vary
   single: HTTP headers; Vary

Variando a Resposta
~~~~~~~~~~~~~~~~~~~

Até agora assumimos que cada URI tem exatamente uma representação do recurso
alvo. Por padrão, o cache HTTP é feito usando a URI do recurso como a chave do
cache. Se duas pessoas requisitarem a mesma URI de um recurso passível de cache,
a segunda pessoa receberá a versão cacheada.

Algumas vezes isso não é suficiente e diferentes versões da mesma URI precisam
ser cacheadas baseando-se em um ou mais valores dos cabeçalhos de requisição.
Por exemplo, se você comprimir a página quando o cliente suportar compressão,
cada URI terá duas representações: uma quando o cliente suportar compressão e
uma quando o cliente não suportar. Essa decisão é feita usando o valor do
cabeçalho ``Accept-Encoding`` da requisição.

Nesse caso, precisamos que o cache armazene ambas as versões da requisição,
para uma determinada URI, comprimida e não, e retorne-as se baseando no valor
``Accept-Encoding`` da requisição. Isso é feito utilizando o cabeçalho ``Vary``
da resposta, que é uma lista separada por vírgulas dos diferentes cabeçalhos
cujos valores acionam um representação diferente do recurso requisitado::

    Vary: Accept-Encoding, User-Agent

.. tip::

    Esse cabeçalho ``Vary`` específico pode fazer o cache de diferentes versões
    de cada recurso baseado na URI e no valor dos cabeçalhos ``Accept-Encoding``
    e ``User-Agent`` da requisição.

O objeto ``Response`` fornece um interface limpa para gerenciar o cabeçalho
``Vary``::

    // define um cabeçalho vary
    $response->setVary('Accept-Encoding');

    // define múltiplos cabeçalhos vary
    $response->setVary(array('Accept-Encoding', 'User-Agent'));

O método ``setVary()`` recebe o nome de um cabeçalho ou um array de nomes de
cabeçalhos para os quais a resposta varia.

Expiração e Validação
~~~~~~~~~~~~~~~~~~~~~

É claro que você pode usar ambas a validação e a expiração dentro da mesma
``Response``. Como a expiração é mais importante que a validação, você pode se
beneficiar facilmente do melhor dos dois mundos. Em outras palavras, utilizando
ambas a expiração e a validação, você pode ordenar que o cache sirva o conteúdo
cacheado, ao mesmo tempo que verifica em algum intervalo de tempo (a expiração)
para ver se o conteúdo continua válido.

.. index::
    pair: Cache; Configuration

Mais Métodos Response
~~~~~~~~~~~~~~~~~~~~~

A classe Response fornece muitos outros métodos relacionados ao cache. Aqui
seguem os mais úteis::

    // Marca a Resposta como antiga
    $response->expire();

    // Força a resposta para retornar um 304 apropriado sem conteúdo
    $response->setNotModified();

Adicionalmente, a maioria dos cabeçalhos HTTP relacionados ao cache podem ser
definidos por meio do método ``setCache()`` apenas::

    // Define as configurações de cache em uma única chamada
    $response->setCache(array(
        'etag'          => $etag,
        'last_modified' => $date,
        'max_age'       => 10,
        's_maxage'      => 10,
        'public'        => true,
        // 'private'    => true,
    ));

.. index::
  single: Cache; ESI
  single: ESI

.. _edge-side-includes:

Usando Edge Side Includes
------------------------

Os gateway caches são uma excelente maneira de fazer o seu site ficar mais
performático. Mas ele tem uma limitação: só conseguem fazer cache de páginas
completas. Se você não conseguir cachear páginas completas ou se partes de uma
página tiverem partes "mais" dinâmicas, você está sem sorte. Felizmente, o
Symfony2 fornece uma solução para esses casos, baseado numa tecnologia chamada
`ESI`_, ou Edge Side Includes. A Akamai escreveu essa especificação quase 10
anos atrás, e ela permite que partes específicas de uma página tenham
estratégias de cache diferentes da página principal.

A especificação ESI descreve tags que podem ser embutidas em suas páginas para
comunicar com o gateway cache. Apenas uma tag é implementada no Symfony2,
``include``, pois ela é a única que é útil fora do contexto da Akamai:

.. code-block:: html

    <html>
        <body>
            Some content

            <!-- Embed the content of another page here -->
            <esi:include src="http://..." />

            More content
        </body>
    </html>

.. note::

    Perceba pelo exemplo que cada tag ESI tem uma URL completamente válida.
    Uma tag ESI representa um fragmento de página que pode ser recuperado
    pela URL informada.

Quando uma requisição é tratada, o gateway cache busca a página inteira do
cache ou faz a requisição no backend da aplicação. Se a resposta contiver uma
ou mais tags ESI, elas são processadas da mesma forma. Em outras palavras, o
cache gateway pega o fragmento da página inserido ou do seu cache ou faz a
requisição novamente para o backend da aplicação. Quado todas as tags ESI forem
resolvidas, o gateway cache mescla cada delas na página principal e envia o
conteúdo finalizado para o cliente.

Tudo isso acontece de forma transparente no nível do gateway cache (i.e. fora
de sua aplicação). Como você pode ver, se você escolher tirar proveito das tags
ESI, o Symfony2 faz com que o processo de incluí-las seja quase sem esforço.

Usando ESI no Symfony2
~~~~~~~~~~~~~~~~~~~~~~

Primeiro, para usar ESI, tenha certeza de ter feito sua habilitação na configuração
da sua aplicação:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            esi: { enabled: true }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config ...>
            <!-- ... -->
            <framework:esi enabled="true" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'esi'    => array('enabled' => true),
        ));

Agora, suponha que temos uma página que seja relativamente estática, exceto
por um atualizador de notícias na parte inferior do conteúdo. Com o ESI,
podemos fazer o cache do atualizador de notícias de forma independente do resto
da página.

.. code-block:: php

    public function indexAction()
    {
        $response = $this->render('MyBundle:MyController:index.html.twig');
        $response->setSharedMaxAge(600);

        return $response;
    }

Nesse exemplo, informamos o tempo de vida para o cache da página inicial como
dez minutos. Em seguida, vamos incluir o atualizador de notícias no template
embutindo uma action. Isso é feito pelo helper ``render`` (Veja
:ref:`templating-embedding-controller` para mais detalhes).

Como o conteúdo embutido vem de outra página (ou controller nesse caso), o
Symfony2 usa o helper padrão ``render`` para configurar as tags ESI:

.. configuration-block::

    .. code-block:: jinja

        {% render '...:news' with {}, {'standalone': true} %}

    .. code-block:: php

        <?php echo $view['actions']->render('...:news', array(), array('standalone' => true)) ?>

Definindo ``standalone`` como ``true``, você diz ao Symfony2 que a action deve
ser renderizada como uma tag ESI. Você pode estar imaginando porque você iria
querer utilizar um helper em vez de escrever a tag ESI você mesmo. Isso acontece
porque a utilização de um helper faz a sua aplicação funcionar mesmo se não existir
nenhum gateway cache instalado. Vamos ver como isso funciona.

Quando standalone está ``false`` (o padrão), o Symfony2 mescla o conteúdo da
página incluída com a principal antes de mandar a resposta para o cliente. Mas
quando standalone está ``true``, *e* se o Symfony detectar que ele está falando
com um gateway cache que suporta ESI, ele gera uma tag ESI de inclusão. Mas se
não houver um gateway cache ou se ele não suportar ESI, o Symfony2 apenas
mesclará o conteúdo da página incluída com a principal como se tudo tivesse
sido feito com o standalone definido como ``false``.

.. note::

    O Symfony2 detecta se um gateway cache suporta ESI por meio de outra
    especificação da Akamai que é suportada nativamente pelo proxy reverso
    do Symfony2.

A action embutida agora pode especificar suas próprias regras de cache, de forma
totalmente independente da página principal.

.. code-block:: php

    public function newsAction()
    {
      // ...

      $response->setSharedMaxAge(60);
    }

Com o ESI, o cache da página completa pode ficar válido por 600 segundos, mas
o cache do componente de notícias será válido apenas nos últimos 60 segundos.

Um requisito do ESI, no entanto, é que a action embutida seja acessível por
uma URL dessa forma o gateway cache pode acessá-la independentemente do
restante da página. É claro que uma action não pode ser acessada pela URL
a menos que exista uma rota que aponte para ela. O Symfony2 trata disso por
meio de uma rota e um controller genéricos. Para que a tag ESI de inclusão
funcione adequadamente, você precisa definir a rota ``_internal``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        _internal:
            resource: "@FrameworkBundle/Resources/config/routing/internal.xml"
            prefix:   /_internal

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@FrameworkBundle/Resources/config/routing/internal.xml" prefix="/_internal" />
        </routes>

    .. code-block:: php

        // app/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection->addCollection($loader->import('@FrameworkBundle/Resources/config/routing/internal.xml', '/_internal'));

        return $collection;

.. tip::

    Como essa rota permite que todas as actions sejam acessadas por uma URL,
    talvez você queira protegê-la utilizando a funcionalidade de firewall
    do Symfony2 (restringindo o acesso para a faixa de IP do seu proxy reverso).
    Veja a seção :ref:`Securing by IP<book-security-securing-ip>` do
    :doc:`Security Chapter </book/security>` para mais informações de como
    fazer isso.

Uma grande vantagem dessa estratégia de cache é que você pode fazer com que
sua aplicação seja tão dinâmica quanto for necessário e ao mesmo tempo, acessar
a aplicação o mínimo possível.

.. note::

    Assim que você começar a utilizar ESI, lembre-se de sempre usar a diretiva
    ``s-maxage`` em vez de ``max-age``. Como o navegador somente recebe o
    recurso agregado, ele não tem ciência dos sub-componentes e assim ele irá
    obedecer a diretiva ``max-age`` e fazer o cache da página inteira. E você
    não quer isso.

O helper ``render`` suporta duas outras opções úteis:

* ``alt``: usada como o atributo ``alt`` na tag ESI, que permite que você
  especifique uma URL alternativa para ser usada se o ``src`` não for
  encontrado;

* ``ignore_errors``: se for configurado como true, um atributo ``onerror``
  será adicionado ao ESI com um valor ``continue`` indicando que, em caso
  de falha, o gateway cache irá simplesmente remover a tag ESI silenciosamente.

.. index::
    single: Cache; Invalidation

.. _http-cache-invalidation:

Invalidação do Cache
--------------------

    "Só tem duas coisas difíceis em Ciência da Computação: invalidação de
    cache e nomear coisas." --Phil Karlton

Você nunca deveria ter que invalidar dados em cache porque a invalidação já
é feita nativamente nos modelos de cache HTTP. Se você usar validação, por
definição você nunca precisaria invalidar algo; e se você usar expiração e
precisar invalidar um recurso, isso significa que você definiu a data de
expiração com um valor muito longe.

.. note::

    Como invalidação é um tópico específico para cada tipo de proxy
    reverso, se você não se preocupa com invalidação, você pode alternar
    entre proxys reversos sem alterar nada no código da sua aplicação.

Na verdade, todos os proxys reversos fornecem maneiras de expurgar dados do
cache, mas você deveria evitá-los o máximo possível. A forma mais padronizada
é expurgar o cache de uma determinada URL requisitando-a com o método HTTP
especial ``PURGE``.

Aqui vai como você pode configurar o proxy reverso do Symfony2 para suportar
o método HTTP ``PURGE``::

    // app/AppCache.php
    class AppCache extends Cache
    {
        protected function invalidate(Request $request)
        {
            if ('PURGE' !== $request->getMethod()) {
                return parent::invalidate($request);
            }

            $response = new Response();
            if (!$this->getStore()->purge($request->getUri())) {
                $response->setStatusCode(404, 'Not purged');
            } else {
                $response->setStatusCode(200, 'Purged');
            }

            return $response;
        }
    }

.. caution::

    Você tem que proteger o método HTTP ``PURGE`` de alguma forma para evitar
    que pessoas aleatórias expurguem seus dados cacheados.

Sumário
-------

O Symfony2 foi desenhado para seguir as regras já testadas: HTTP. O cache não
é exceção. Dominar o sistema de cache do Symfony2 significa se familiarizar
com os modelos de cache HTTP e utilizá-los de forma efetiva. Para isso, em
vez de depender apenas da documentação do Symfony2 e exemplos de código, você
deveria buscar mais conteúdo relacionado com o cache HTTP e caches
gateway como o Varnish.


Learn more from the Cookbook
----------------------------

* :doc:`/cookbook/cache/varnish`

.. _`Things Caches Do`: http://tomayko.com/writings/things-caches-do
.. _`Cache Tutorial`: http://www.mnot.net/cache_docs/
.. _`Varnish`: http://www.varnish-cache.org/
.. _`Squid in reverse proxy mode`: http://wiki.squid-cache.org/SquidFaq/ReverseProxy
.. _`expiration model`: http://tools.ietf.org/html/rfc2616#section-13.2
.. _`validation model`: http://tools.ietf.org/html/rfc2616#section-13.3
.. _`RFC 2616`: http://tools.ietf.org/html/rfc2616
.. _`HTTP Bis`: http://tools.ietf.org/wg/httpbis/
.. _`P4 - Conditional Requests`: http://tools.ietf.org/html/draft-ietf-httpbis-p4-conditional-12
.. _`P6 - Caching: Browser and intermediary caches`: http://tools.ietf.org/html/draft-ietf-httpbis-p6-cache-12
.. _`ESI`: http://www.w3.org/TR/esi-lang
