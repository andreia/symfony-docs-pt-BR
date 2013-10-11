.. index::
   single: Symfony2 Fundamentals

Fundamentos de Symfony e HTTP
=============================

Parabéns! Aprendendo sobre o Symfony2, você está no caminho certo para ser um 
desenvolvedor web mais *produtivo*, *bem preparado* e *popular* (na verdade, 
este último é por sua própria conta). O Symfony2 foi criado para voltar ao básico: 
desenvolver ferramentas para ajuda-lo a criar aplicações mais robustas de 
uma maneira mais rápida sem ficar no seu caminho. Ele foi construído baseando-se  
nas melhores ideias de diversas tecnologias: as ferramentas e conceitos que você 
está prestes a aprender representam o esforço de milhares de pessoas, realizado 
durante muitos anos. Em outras palavras, você não está apenas aprendendo o "Symfony", 
você está aprendendo os fundamentos da web, boas práticas de desenvolvimento e 
como usar diversas biblotecas PHP impressionantes, dentro e fora do Symfony2. Então, 
prepare-se.

Seguindo a filosofia do Symfony2, este capítulo começa explicando o conceito fundamental 
para o desenvolvimento web: o HTTP. Independente do seu conhecimento anterior ou 
linguagem de programação preferida, esse capítulo é uma **leitura obrigatória** 
para todos.

HTTP é simples
--------------

HTTP (Hypertext Transfer Protocol, para os geeks) é uma linguagem textual que 
permite que duas máquinas se comuniquem entre si. É só isso! Por exemplo, quando 
você vai ler a última tirinha do `xkcd`_, acontece mais ou menos a seguinte conversa:

.. image:: /images/http-xkcd.png
   :align: center

Apesar da linguagem real ser um pouco mais formal, ainda assim ela é bastante simples. 
HTTP é o termo usado para descrever essa linguagem simples baseada em texto. Não 
importa como você desenvolva para a web, o objetivo do seu servidor *sempre* será 
entender simples requisições de texto e enviar simples respostas de texto.

O Symfony2 foi criado fundamentado nessa realidade. Você pode até não perceber,
mas o HTTP é algo que você utilizada todos os dias. Com o Symfony2 você irá
aprender a domina-lo.

.. index::
   single: HTTP; Request-response paradigm

Primeiro Passo: O Cliente envia uma Requisição
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Toda comunicação na web começa com uma *requisição*. Ela é uma mensagem de texto 
criada por um cliente (por exemplo, um navegador, um app para iPhone etc) em um 
formato especial conhecido como HTTP. O cliente envia essa requisição para um
servidor e, então, espera pela resposta.

Veja a primeira parte da interação (a requisição) entre um navegador e o servidor
web do xkcd:

.. image:: /images/http-xkcd-request.png
   :align: center

No linguajar do HTTP, essa requisição se parece com isso:

.. code-block:: text

    GET / HTTP/1.1
    Host: xkcd.com
    Accept: text/html
    User-Agent: Mozilla/5.0 (Macintosh)

Essa simples mensagem comunica *tudo* o que é necessário sobre o recurso exato
que o cliente está requisitando. A primeira linha de uma requisição HTTP é a mais
importante e contém duas coisas: a URI e o método HTTP.

A URI (por exemplo, ``/``, ``/contact`` etc) é um endereço único ou localização que
identifica o recurso que o cliente quer. O método HTTP (por exemplo, ``GET``)
define o que você quer *fazer* com o recurso. Os métodos HTTP são os *verbos* da
requisição e definem algumas maneiras comuns de agir em relação ao recurso:

+----------+---------------------------------------+
| *GET*    | Recupera o recurso do servidor        |
+----------+---------------------------------------+
| *POST*   | Cria um recurso no servidor           |
+----------+---------------------------------------+
| *PUT*    | Atualiza um recurso no servidor       |
+----------+---------------------------------------+
| *DELETE* | Exclui um recurso do servidor         |
+----------+---------------------------------------+

Tendo isso em mente, você pode imaginar como seria uma requisição HTTP para excluir
uma postagem específica de um blog, por exemplo:

.. code-block:: text

    DELETE /blog/15 HTTP/1.1

.. note::

    Existem na verdade nove métodos definidos pela especificação HTTP, mas 
    a maioria deles não são muito utilizados ou suportados. Na realidade, muitos dos 
    navegadores modernos não suportam os métodos ``PUT`` e ``DELETE``.

Além da primeira linha, uma requisição HTTP invariavelmente contém outras linhas
de informação chamadas de cabeçalhos da requisição. Os cabeçalhos podem fornecer
uma vasta quantidade de informações, tais como o ``Host`` que foi requisitado, os
formatos de resposta que o cliente aceita (``Accept``) e a aplicação que o
cliente está utilizando para enviar a requisição (``User-Agent``). Muitos outros
cabeçalhos existem e podem ser encontrados na Wikipedia, no artigo 
`List of HTTP header fields`_

Segundo Passo: O Servidor envia uma resposta
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Uma vez que o servidor recebeu uma requisição, ele sabe exatamente qual recurso
o cliente precisa (através do URI) e o que o cliente quer fazer com ele (através
do método). Por exemplo, no caso de uma requisição GET, o servidor prepara o
o recurso e o retorna em uma resposta HTTP. Considere a resposta do servidor web
do xkcd:

.. image:: /images/http-xkcd.png
   :align: center

Traduzindo para HTTP, a resposta enviada para o navegador será algo como:

.. code-block:: text

    HTTP/1.1 200 OK
    Date: Sat, 02 Apr 2011 21:05:05 GMT
    Server: lighttpd/1.4.19
    Content-Type: text/html

    <html>
      <!-- HTML for the xkcd comic -->
    </html>

A resposta HTTP contém o recurso requisitado (nesse caso, o conteúdo HTML), bem
como outras informações. A primeira linha é especialmente importante e contém o
código de status da resposta HTTP (nesse caso, 200). Esse código de status é uma 
representação geral da resposta enviada à requisição do cliente. A requisição
foi bem sucedida? Ocorreu algum erro? Existem diferentes códigos de status para
indentificar sucesso, um erro, ou que o cliente precisa fazer alguma coisa (por
exemplo, redirecionar para outra página). Uma lista completa pode ser encontrada
na Wikipedia, no artigo `List of HTTP status codes`_.

Assim como uma requisição, uma resposta HTTP também contém informações adicionais
conhecidas como cabeçalhos HTTP. Por exemplo, um cabeçalho importante nas respostas
HTTP é o ``Content-Type``. O conteúdo de um mesmo recurso pode ser retornado em
vários formatos diferentes, incluindo HTML, XML ou JSON, só para citar alguns. 
O cabeçalho ``Content-Type`` diz ao cliente qual é o formato que está sendo 
retornado.

Existem diversos outros cabeçalhos, alguns deles bastante poderosos. Certos
cabeçalhos, por exemplo, podem ser utilizados para criar um poderoso sistema de 
cache.

Requisições, Respostas e o Desenvolvimento Web
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Essa conversação de requisição-resposta é o processo fundamental que dirige toda a
comunicação na web. Apesar de tão importante e poderoso esse processo, ainda assim,
é inevitavelmente simples.

O fato mais importante é: independente da linguagem que você utiliza, o tipo de
aplicação que você desenvolva (web, mobile, API em JSON) ou a filosofia de
desenvolvimento que você segue, o objetivo final da aplicação **sempre** será 
entender cada requisição e criar e enviar uma resposta apropriada.

O Symfony foi arquitetado para atender essa realidade.

.. tip::

    Para aprender mais sobre a especificação HTTP, leia o original `HTTP 1.1 RFC`_
    ou `HTTP Bis`_, que trata-se de um esforço para facilitar o entendimento da
    especificação original. Para verificar as requisições e respostas enviadas
    enquanto navega em um site, você pode utilizar a extensão do Firefox chamada
    `Live HTTP Headers`_.

.. index::
   single: Symfony2 Fundamentals; Requests and responses

Requisições e Respostas no PHP
------------------------------

Como interagir com a "requisição" e criar uma "resposta" utilizando o PHP? Na 
verdade, o PHP abstrai um pouco desse processo:

.. code-block:: php

    <?php
    $uri = $_SERVER['REQUEST_URI'];
    $foo = $_GET['foo'];

    header('Content-type: text/html');
    echo 'The URI requested is: '.$uri;
    echo 'The value of the "foo" parameter is: '.$foo;

Por mais estranho que possa parecer, essa pequena aplicação, de fato, lê
informações da requisição HTTP e a está utilizando para criar um resposta HTTP. Em 
vez de interpretar a requisição pura, o PHP prepara algumas variáveis superglobais
, tais como ``$_SERVER`` e ``$_GET``, que contém toda a informação da requisição.
Da mesma forma, em vez de retornar o texto da resposta no formato do HTTP, você
pode utilizar a função ``header()`` para criar os cabeçalhos e simplesmente 
imprimir o que será o conteúdo da mensagem da reposta. O PHP irá criar uma 
reposta HTTP verdadeira que será retornada para o cliente.

.. code-block:: text

    HTTP/1.1 200 OK
    Date: Sat, 03 Apr 2011 02:14:33 GMT
    Server: Apache/2.2.17 (Unix)
    Content-Type: text/html

    The URI requested is: /testing?foo=symfony
    The value of the "foo" parameter is: symfony

Requisições e Respostas no Symfony
----------------------------------

O Symfony fornece uma alternativa à abordagem feita com o PHP puro, utilizando 
duas classes que permitem a interação com as requisições e respostas HTTP de uma
maneira mais fácil. A classe :class:`Symfony\\Component\\HttpFoundation\\Request` 
é uma simples representação orientada a objetos de uma requisição HTTP. Com ela,
você tem todas as informações da requisição nas pontas dos dedos::

    use Symfony\Component\HttpFoundation\Request;

    $request = Request::createFromGlobals();

    // the URI being requested (e.g. /about) minus any query parameters
    $request->getPathInfo();

    // retrieve GET and POST variables respectively
    $request->query->get('foo');
    $request->request->get('bar');

    // retrieves an instance of UploadedFile identified by foo
    $request->files->get('foo');

    $request->getMethod();          // GET, POST, PUT, DELETE, HEAD
    $request->getLanguages();       // an array of languages the client accepts

Como um bônus, a classe ``Request`` faz um monte de trabalho com o qual você
nunca precisará se preocupar. Por exemplo, o método ``isSecure()`` verifica os
três valores diferentes que o PHP utiliza para indicar ser o usuário está
utilizando uma conexão segura (``https``, por exemplo).

O Symfony também fornece a classe ``Response``: uma simples representação em PHP de
uma resposta HTTP. Assim é possível que sua aplicação utilize uma interface
orientada a objetos para construir a resposta que precisa ser enviada ao cliente::

    use Symfony\Component\HttpFoundation\Response;
    $response = new Response();

    $response->setContent('<html><body><h1>Hello world!</h1></body></html>');
    $response->setStatusCode(200);
    $response->headers->set('Content-Type', 'text/html');

    // prints the HTTP headers followed by the content
    $response->send();

Com tudo isso, mesmo que o Symfony não oferecesse mais nada, você já teria um kit de
ferramentas para facilmente acessar informações sobre a requisição e uma interface 
orientada a objetos para criar a resposta. Mesmo depois de aprender muitos dos
poderosos recursos do Symfony, tenha em mente que o objetivo da sua aplicação 
sempre será *interpretar uma requisição e criar a resposta apropriada baseada na 
lógica da sua aplicação*.

.. tip::

    As classes ``Request`` e ``Response`` fazem parte de um componente do Symfony 
    chamado ``HttpFoundation``. Esse componente pode ser utilizado de forma
    independente ao framework e também possui classes para tratar sessões e 
    upload de arquivos.

A Jornada da Requisição até a Resposta
--------------------------------------

Como o próprio HTTP, os objetos ``Request`` e ``Response`` são bastante simples.
A parte difícil de se construir uma aplicação é escrever o que acontece entre eles.
Em outras palavras, o trabalho de verdade é escrever o código que interpreta a 
requisição e cria a resposta.

A sua aplicação provavelmente faz muitas coisas como enviar emails, tratar do
envio de formulários, salvar coisas no banco de dados, renderizar páginas HTML e
proteger o conteúdo com segurança. Como cuidar de tudo isso e ainda ter um código
organizado e de fácil manutenção?

O Symfony foi criado para que ele resolva esses problemas, não você.

O Front Controller
~~~~~~~~~~~~~~~~~~~~

Tradicionalmente, aplicações são construídas para que cada página do site seja
um arquivo físico:

.. code-block:: text

    index.php
    contact.php
    blog.php

Existem diversos problemas para essa abordagem, incluindo a falta de flexibilidade
das URLs (e se você quiser mudar o arquivo ``blog.php`` para ``news.php`` sem
quebrar todos os seus links?) e o fato de que cada arquivo *deve* ser alterado
manualmente para incluir um certo conjunto de arquivos essenciais de forma que a 
segurança, conexões com banco de dados e a "aparência" do site continue consistente.

Uma solução muito melhor é utilizar um :term:`front controller`: um único arquivo
PHP que trata todas as requisições enviadas para a sua aplicação. Por exemplo:

+------------------------+------------------------+
| ``/index.php``         | executa  ``index.php`` |
+------------------------+------------------------+
| ``/index.php/contact`` | executa  ``index.php`` |
+------------------------+------------------------+
| ``/index.php/blog``    | executa  ``index.php`` |
+------------------------+------------------------+

.. tip::

    Utilizando o ``mod_rewrite`` do Apache (ou o equivalente em outros servidores 
    web), as URLs podem ser simplificadas facilmente para ser somente ``/``, ``/contact``
    e ``/blog``.

Agora, cada requisição é tratada exatamente do mesmo jeito. Em vez de arquivos
PHP individuais para executar cada URL, o front controller *sempre* será executado,
e o roteamento de cada URL para diferentes partes da sua aplicação é feito internamente.
Assim resolve-se os dois problemas da abordagem original. Quase todas as aplicações
modernas fazem isso - incluindo apps como o Wordpress.

Mantenha-se Organizado
~~~~~~~~~~~~~~~~~~~~~~

Dentro do front controller, como você sabe qual página deve ser renderizada e como 
renderiza-las de uma maneira sensata? De um jeito ou de outro, você precisará 
verificar a URI requisitada e executar partes diferentes do seu código dependendo 
do seu valor. Isso pode acabar ficando feio bem rápido:

.. code-block:: php

    // index.php

    $request = Request::createFromGlobals();
    $path = $request->getPathInfo(); // the URL being requested

    if (in_array($path, array('', '/')) {
        $response = new Response('Welcome to the homepage.');
    } elseif ($path == '/contact') {
        $response = new Response('Contact us');
    } else {
        $response = new Response('Page not found.', 404);
    }
    $response->send();

Resolver esse problema pode ser difícil. Felizmente é *exatamente* o que o Symfony
foi projetado para fazer.

O Fluxo de uma Aplicação Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando você deixa que o Symfony cuide de cada requisição, sua vida fica muito mais
fácil. O framework segue um simples padrão para toda requisição:

.. _request-flow-figure:

.. figure:: /images/request-flow.png
   :align: center
   :alt: Symfony2 request flow

   As requisições recebidas são interpretadas pelo roteamento e passadas para as 
   funções controller que retornam objetos do tipo ``Response``.

Cada "página" do seu site é definida no arquivo de configuração de roteamento que
mapeia diferentes URLs para diferentes funções PHP. O trabalho de cada função, 
chamadas de :term:`controller`, é usar a informação da requisição - junto com
diversas outras ferramentas disponíveis no Symfony - para criar e retornar um 
objeto ``Response``. Em outras palavras, o *seu* código deve estar nas funções controller: 
lá é onde você interpreta a requisição e cria uma resposta.

É fácil! Vamos fazer uma revisão:

* Cada requisição executa um arquivo front controller;

* O sistema de roteamento determina qual função PHP deve ser executada, baseado
  na informação da requisição e na configuração de roteamento que você criou;

* A função PHP correta é executada, onde o seu código cria e retorna o objeto 
  ``Response`` apropriado.

Uma Requisição Symfony em Ação
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sem entrar em muitos detalhes, vamos ver esse processo em ação. Suponha que
você quer adicionar a página ``/contact`` na sua aplicação Symfony. Primeiro, 
adicione uma entrada para ``/contact`` no seu arquivo de configuração de roteamento:

.. code-block:: yaml

    contact:
        pattern:  /contact
        defaults: { _controller: AcmeDemoBundle:Main:contact }

.. note::

   Esse exemplo utiliza :doc:`YAML</reference/YAML>` para definir a configuração de
   roteamento. Essa configuração também pode ser escrita em outros formatos, tais
   como XML ou PHP.

Quando alguém visitar a página ``/contact``, essa rota será encontrada e o
controller específico será executado. Como você irá aprender no :doc:`capítulo sobre roteamento</book/routing>`,
a string ``AcmeDemoBundle:Main:contact`` é uma sintaxe encurtada para apontar para
o método ``contactAction`` dentro de uma classe chamada ``MainController``:

.. code-block:: php

    class MainController
    {
        public function contactAction()
        {
            return new Response('<h1>Contact us!</h1>');
        }
    }

Nesse exemplo extremamente simples, o controller simplesmente cria um objeto
``Response`` com o HTML "<h1>Contact us!</h1>". No :doc:`capítulo sobre controller</book/controller>`,
você irá aprender como um controller pode renderizar templates, fazendo com que o seu código de 
"apresentação" (por exemplo, qualquer coisa que gere HTML) fique em um arquivo de
template separado. Assim deixamos o controller livre para se preocupar apenas com
a parte complicada: interagir com o banco de dados, tratar os dados enviados ou
enviar emails.

Symfony2: Construa sua aplicação, não suas Ferramentas
------------------------------------------------------

Agora você sabe que o objetivo de qualquer aplicação é interpretar cada requisição
recebida e criar uma resposta apropriada. Conforme uma aplicação cresce, torna-se
mais difícil de manter o seu código organizado e de fácil manutenção. Invariavelmente,
as mesmas tarefas complexas continuam a aparecer: persistir dados no banco,
renderizar e reutilizar templates, tratar envios de formulários, enviar emails,
validar entradas dos usuários e cuidar da segurança.

A boa notícia é que nenhum desses problemas é único. O Symfony é um framework cheio
de ferramentas para você construir a sua aplicação e não as suas ferramentas.
Com o Symfony2, nada é imposto: você é livre para utilizar o framework completo ou
apenas uma parte dele.

.. index::
   single: Symfony2 Components

Ferramentas Independentes: Os *Componentes* do Symfony2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Então, o que *é* o Symfony2? Primeiramente, trata-se de uma coleção de vinte
bibliotecas independentes que podem ser utilizadas dentro de *qualquer* projeto
PHP. Essas bibliotecas, chamadas de *Components do Symfony2*, contém coisas úteis
para praticamente qualquer situação, independente de como o seu projeto é desenvolvido.
Alguns desses componentes são:

* `HttpFoundation`_ - Contém as classes ``Request`` e ``Response``, bem como outras
  classes para tratar de sessões e upload de arquivos;

* `Routing`_ - Um poderoso e rápido sistema de roteamento que permite mapear uma
  URI específica (por exemplo, ``/contact``) para uma informação sobre como a
  requisição deve ser tratada (por exemplo, executar o método ``contactAction()``);

* `Form`_ - Um framework completo e flexível para criar formulários e tratar os
  dados enviados por eles;

* `Validator`_ Um sistema para criar regras sobre dados e validar se os dados
  enviados pelos usuários seguem ou não essas regras;

* `ClassLoader`_ Uma biblioteca de autoloading que faz com que classes PHP possam
  ser utilizadas sem precisar adicionar manualmente um ``require`` para cada arquivo
  que as contém;

* `Templating`_ Um conjunto de ferramentas para renderizar templates, tratar da 
  herança de templates (por exemplo, um template decorado com um layout) e executar 
  outras tarefas comuns relacionadas a templates;

* `Security`_ - Uma biblioteca poderosa para tratar qualquer tipo de segurança 
  dentro de sua aplicação;

* `Translation`_ Um framework para traduzir strings na sua aplicação.

Cada um desses componentes funcionam de forma independente e podem ser utilizados 
em *qualquer* projeto PHP, não importa se você utiliza o Symfony2 ou não. Cada 
parte foi feita para ser utilizada e substituída quando for necessário.

A solução completa: O *framework* Symfony2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Então, o que *é* o *framework* Symfony2? Ele é uma biblioteca PHP que realiza
duas tarefas distintas:

#. Fornecer uma seleção de componentes (os componentes do Symfony2, por exemplo) e
   bibliotecas de terceiros (por exemplo, a ``Swiftmailer``, utilizada para enviar emails);

#. Fornecer as configurações necessárias e uma "cola" para manter todas as peças 
   juntas.

O objetivo do framework é integrar várias ferramentas independentes para criar
uma experiência consistente para o desenvolvedor. Até próprio próprio framework é 
um pacote Symfony2 (um plugin, por exemplo) que pode ser configurado ou completamente
substituído.

O Symfony2 fornece um poderoso conjunto de ferramentas para desenvolver aplicações
web rapidamente sem impor nada. Usuários normais podem iniciar o desenvolvimento rapidamente 
utilizando uma distribuição do Symfony2, que contém o esqueleto de um projeto com 
as princpais itens padrão. Para os usuários mais avançados, o céu é o limite.

.. _`xkcd`: http://xkcd.com/
.. _`HTTP 1.1 RFC`: http://www.w3.org/Protocols/rfc2616/rfc2616.html
.. _`HTTP Bis`: http://datatracker.ietf.org/wg/httpbis/
.. _`Live HTTP Headers`: https://addons.mozilla.org/en-US/firefox/addon/3829/
.. _`List of HTTP status codes`: http://en.wikipedia.org/wiki/List_of_HTTP_status_codes
.. _`List of HTTP header fields`: http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
.. _`HttpFoundation`: https://github.com/symfony/HttpFoundation
.. _`Routing`: https://github.com/symfony/Routing
.. _`Form`: https://github.com/symfony/Form
.. _`Validator`: https://github.com/symfony/Validator
.. _`ClassLoader`: https://github.com/symfony/ClassLoader
.. _`Templating`: https://github.com/symfony/Templating
.. _`Security`: https://github.com/symfony/Security
.. _`Translation`: https://github.com/symfony/Translation
