.. index::
   single: Controlador

Controlador
===========

Um controlador é uma função PHP que você cria e que pega informações da
requisição HTTP para criar e retornar uma resposta HTTP (como um objeto
``Response`` do Symfony2). A resposta pode ser uma página HTML, um documento
XML, um array JSON serializado, uma imagem, um redirecionamento, um erro 404
ou qualquer coisa que você imaginar. O controlador contém toda e qualquer lógica
arbritária que *sua aplicação* precisa para renderizar o conteúdo de uma
página.

Para ver quão simples é isso, vamos ver um controlador do Symfony2 em ação.
O seguinte controlador deve renderizar uma página que mostra apenas 
``Hello world!``:

    use Symfony\Component\HttpFoundation\Response;

    public function helloAction()
    {
        return new Response('Hello world!');
    }

O objetivo de um controlador é sempre o mesmo: criar e retornar um objeto
``Response``. Ao longo do caminho, ele pode ler informações da requisição,
carregar um recurso do banco de dados, mandar um e-mail ou gravar informações
na sessão do usuário. Mas em todos os casos, o controlador acabará retornando
o objeto ``Response`` que será mandado de volta para o cliente.

Não há nenhuma mágica e nenhum outro requisito para se preocupar! Aqui temos
alguns exemplos comuns:

* O *Controlador A* prepara um objeto ``Response`` representando o conteúdo
  da página inicial do site.

* O *Controlador B* lê o parâmetro ``slug`` da requisição para carregar uma
  entrada do blog no banco de dados e cria um um objeto ``Response`` mostrando
  o blog. Se o ``slug`` não for encontrado no banco de dados, ele cria e
  retorna um objeto ``Response`` com um código de status 404.

* O *Controlador C* trata a o envio de um formulário de contato. Ele lê a
  informação do formulário a partir da requisição, salva a informação de
  contato no banco de dados e envia por e-mail a informação de contato
  para o webmaster. Finalmente, ele cria um objeto ``Response`` que
  redireciona o navegador do cliente para a página "thank you" do formulário
  de contato.

.. index::
   single: Controlador; O ciclo de vida requisição-controlador-resposta 

O Ciclo de Vida da Requisição, Controlador e Resposta
-----------------------------------------------------

Toda requisição tratada por um projeto com Symfony 2 passa pelo mesmo ciclo de
vida simples. O framework cuida das tarefas repetitivas e por fim executa um
controlador onde reside o código personalizado da sua aplicação:

#. Toda requisição é tratada por um único arquivo front controlador (por exemplo, 
   ``app.php`` ou ``app_dev.php``) que inicializa a aplicação;

#. O ``Router`` lê a informação da requisição (por exemplo, a URI), encontra uma rota
   que casa com aquela informação e lê o parâmetro ``_controller`` da rota;

#. O controlador que casou com a rota é executado e o código dentro do
   controlador cria e retorna um objeto ``Response``;

#. Os cabeçalhos HTTP e o conteúdo do objeto ``Response`` são enviados de
   volta para o cliente.

Criar uma página é tão fácil quanto criar um controlador (#3) e fazer uma rota
que mapeie uma URL para aquele controlador (#2).

.. note::

    Embora tenha um nome similar, um "front controller" é diferente dos
    "controladores" dos quais vamos falar nesse capítulo. Um front controller é
    um pequeno arquivo PHP que fica no seu diretório web e através do qual
    todas as requisições são direcionadas. Uma aplicação típica terá um front
    controller de produção (por exemplo, ``app.php``) e um front controller de
    desenvolvimento (por exemplo, ``app_dev.php``). Provavelmente você nunca precisará
    editar, visualizar ou se preocupar com os front controllers da sua
    aplicação.

.. index::
   single: Controlador; Exemplo simples

Um Controlador Simples
----------------------

Embora um controlador possa ser qualquer código PHP que possa ser chamado (uma
função, um método em um objeto ou uma ``Closure``), no Symfony2 um controlador
geralmente é um único método dentro de um objeto controlador. Os controladores
também são chamados de *ações*:

.. code-block:: php
    :linenos:

    // src/Acme/HelloBundle/Controller/HelloController.php

    namespace Acme\HelloBundle\Controller;
    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
        public function indexAction($name)
        {
          return new Response('<html><body>Hello '.$name.'!</body></html>');
        }
    }

.. tip::

    Note que o *controlador* é o método ``indexAction``, que fica dentro de
    uma *classe controladora* (``HelloController``). Não se confunda com a
    nomenclatura: uma *classe controladora* é apenas um forma conveniente de
    agrupar vários controladores/ações juntos. Geralmente a classe controladora
    irá agrupar vários controladores/ações (por exemplo, ``updateAction``,
    ``deleteAction`` etc).
    
Esse controlador é bem simples, mas vamos explicá-lo:

* *linha 3*: O Symfony2 se beneficia da funcionalidade de namespace do PHP 5.3
  colocando a classe controladora inteira dentro de um namespace. A palavra chave
  ``use`` importa a classe ``Response`` que nosso controlador tem que retornar.

* *linha 6*: O nome da classe é a concatenação de um nome para a classe
  controlador (ou seja, ``Hello``) com a palavra ``Controller``. Essa é uma
  convenção que fornece consistência aos controladores e permite que eles sejam
  referenciados usando apenas a primeira parte do nome (ou seja, ``Hello``) na
  configuração de roteamento.

* *linha 8*: Toda ação em uma classe controladora é sufixada com ``Action`` e
  é referenciada na configuração de roteamento pelo nome da ação (``index``).
  Na próxima seção, você criará uma rota que mapeia uma URI para essa action.
  Você aprenderá como os marcadores de posição das rotas (``{name}``) tornam-se
  argumentos no método da action (``$name``).

* *linha 10*: O controlador cria e retorna um objeto ``Response``.

.. index::
   single: Controlador; Rotas e controladores

Mapeando uma URL para um Controlador
------------------------------------

O novo controlador retorna uma página HTML simples. Para ver realmente essa
página no seu navegador você precisa criar uma rota que mapeia um padrão
específico de URL para o controlador:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        hello:
            pattern:      /hello/{name}
            defaults:     { _controller: AcmeHelloBundle:Hello:index }

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <route id="hello" pattern="/hello/{name}">
            <default key="_controller">AcmeHelloBundle:Hello:index</default>
        </route>

    .. code-block:: php

        // app/config/routing.php
        $collection->add('hello', new Route('/hello/{name}', array(
            '_controller' => 'AcmeHelloBundle:Hello:index',
        )));

Agora, acessar ``/hello/ryan`` executa o controlador
``HelloController::indexAction()`` e passa ``ryan`` para a variável ``$name``.
A criação de uma "página" significa simplesmente criar um método controlador e
associar uma rota.

Note a sintaxe usada para referenciar o controlador:
``AcmeHelloBundle:Hello:index``. O Symfony2 usa uma notação flexível de string
para referenciar diferentes controladores. Essa é a sintaxe mais comum e diz ao
Symfony2 para buscar por uma classe controladora chamada ``helloController``
dentro de um bundle chamado ``AcmeHelloBundle``. Então o método
``indexAction()`` é executado.

Para mais detalhes sobre o formato de string usado para referenciar diferentes
controladores, veja :ref:`controller-string-syntax`.

.. note::

    Esse exemplo coloca a configuração de roteamento diretamente no diretório
    ``app/config/``. Uma forma melhor de organizar suas rotas é colocar cada
    uma das rotas no bundle a qual elas pertencem. Para mais informações, veja
    :ref:`routing-include-external-resources`.

.. tip::

    Você pode aprender muito mais sobre o sistema de roteamento no
    :doc:`capítulo Roteamento</book/routing>`.

.. index::
   single: Controlador; Argumentos do Controlador

.. _route-parameters-controller-arguments:

Parâmetros de Rota como Argumentos do Controlador
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você já sabe que o parâmetro ``_controller`` em ``AcmeHelloBundle:Hello:index``
se refere ao método ``HelloController::indexAction()`` que está dentro do
bundle ``AcmeHelloBundle``. O que é mais interessante são os argumentos que
são passados para o método:

.. code-block:: php

    <?php
    // src/Acme/HelloBundle/Controller/HelloController.php

    namespace Acme\HelloBundle\Controller;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class HelloController extends Controller
    {
        public function indexAction($name)
        {
          // ...
        }
    }

O controlador tem um único argumento, ``$name``, que corresponde ao parâmetro
``{name}`` da rota casada (``ryan`` no nosso exemplo). Na verdade quando
executa seu controlador, o Symfony2 casa cada um dos argumentos do controlador
com um parâmetro da rota casada. Veja o seguinte exemplo:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        hello:
            pattern:      /hello/{first_name}/{last_name}
            defaults:     { _controller: AcmeHelloBundle:Hello:index, color: green }

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <route id="hello" pattern="/hello/{first_name}/{last_name}">
            <default key="_controller">AcmeHelloBundle:Hello:index</default>
            <default key="color">green</default>
        </route>

    .. code-block:: php

        // app/config/routing.php
        $collection->add('hello', new Route('/hello/{first_name}/{last_name}', array(
            '_controller' => 'AcmeHelloBundle:Hello:index',
            'color'       => 'green',
        )));

O controlador dessa rota pode receber vários argumentos:

    public function indexAction($first_name, $last_name, $color)
    {
        // ...
    }

Observe que tanto as variáveis de marcadores de posição (``{first_name}``,
``{last_name}``) quanto a váriavel padrão ``color`` estão disponíveis como
argumentos no controlador. Quando uma rota é casada, as variáveis marcadoras
de posição são mescladas com as variáveis ``default`` criando um array
que fica disponível para o seu controlador.

O mapeamento de parâmetros de rota com argumentos do controlador é fácil e
flexível. Tenha em mente as seguintes orientações enquanto estiver
desenvolvendo.

* **A ordem dos argumentos do controlador não importa**

    O Symfony é capaz de casar os nomes dos parâmetros da rota com os nomes
    das variáveis na assinatura do método do controlador. Em outras palavras,
    ele sabe que o parâmetro ``{last_name}`` casa com o argumento
    ``$last_name``. Os argumentos do controlador podem ser totalmente
    reordenados e continuam funcionando perfeitamente:

        public function indexAction($last_name, $color, $first_name)
        {
            // ..
        }

* **Todo argumento obrigatório do controlador tem que corresponder a um parâmetro de roteamento**

    O seguinte deveria lançar uma ``RuntimeException`` porque não existe nenhum
    parâmetro ``foo`` definido na rota:

        public function indexAction($first_name, $last_name, $color, $foo)
        {
            // ..
        }

	Deixando o argumento opcional, no entanto, tudo corre bem. O
	seguinte exemplo não lança uma exceção:

        public function indexAction($first_name, $last_name, $color, $foo = 'bar')
        {
            // ..
        }

* **Nem todos os parâmetros de roteamento precisam ser argumentos no seu controlador**

    Se, por exemplo, ``last_name`` não for importante para o seu controlador,
    você pode omitir inteiramente ele:

        public function indexAction($first_name, $color)
        {
            // ..
        }

.. tip::

    Cada uma das rotas tem um parâmetro ``_route`` especial, que é igual ao
    nome da rota que foi casada (por exemplo, ``hello``). Embora não seja útil
    geralmente, ele também fica disponível como um argumento do controlador.

.. _book-controller-request-argument:

O ``Request`` como um Argumento do Controlador
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Por conveniência, você também pode fazer com que o Symfony passe o objeto
``Request`` como um argumento para seu controlador. Isso é conveniente
especialmente quando você estiver trabalhando com formulários, por exemplo:

    use Symfony\Component\HttpFoundation\Request;

    public function updateAction(Request $request)
    {
        $form = $this->createForm(...);
        
        $form->bind($request);
        // ...
    }

.. index::
   single: Controlador; Classe base do controlador

A Classe Base do Controlador
----------------------------

Por conveniência, o Symfony2 vem com uma classe ``Controller`` base que ajuda
com algumas das tarefas mais comuns dos controladores e fornece às suas classes
controladoras acesso à qualquer recurso que elas possam precisar. Estendendo essa
classe ``Controller``, você se beneficia com vários métodos helper.

Adicione a instrução ``use`` no topo da sua classe ``Controller`` e então
modifique o ``HelloController`` para estendê-lo:

.. code-block:: php

    // src/Acme/HelloBundle/Controller/HelloController.php

    namespace Acme\HelloBundle\Controller;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Response;

    class HelloController extends Controller
    {
        public function indexAction($name)
        {
          return new Response('<html><body>Hello '.$name.'!</body></html>');
        }
    }

Isso não muda realmente nada o jeito que seu controlador trabalha. Na próxima
seção você aprenderá sobre os métodos helper que a classe controladora base
disponibiliza. Esses métodos são apenas atalhos para usar funcionalidades do
núcleo do Symfony2 que estão disponíveis para você usando ou não a classe base
``Controller``. Uma boa maneira de ver a funcionalidade do núcleo em ação
é olhar a própria classe
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`.

.. tip::

    Estender a classe base é *opcional* no Symfony; ela contém atalhos úteis
    mas nada que seja mandatório. Você também pode estender
    ``Symfony\Component\DependencyInjection\ContainerAware``. O objeto
    contêiner de serviços então será acessível por meio da propriedade
    ``container``.
    
.. note::

    Você também pode definir seus :doc:`Controllers como Serviços
    </cookbook/controller/service>`.

.. index::
   single: Controlador; Tarefas Comuns

Tarefas Comuns dos Controladores
--------------------------------

Embora virtualmente um controlador possa fazer qualquer coisa, a maioria dos
controladores irão realizar as mesmas tarefas básicas repetidas vezes. Essas
tarefas, como redirecionamentos, direcionamentos, renderização de templates e
acesso a serviços nucleares são muitos fáceis de gerenciar no Symfony2.

.. index::
   single: Controlador; Redirecionado

Redirecionando
~~~~~~~~~~~~~~

Se você quiser redirecionar o usuário para outra página, use o método
``redirect()``::

    public function indexAction()
    {
        return $this->redirect($this->generateUrl('homepage'));
    }

O método ``generateUrl()`` é apenas uma função helper que gera a URL de uma
determinada rota. Para mais informações, veja o capítulo
:doc:`Roteamento </book/routing>`.

Por padrão, o método ``redirect()`` efetua um redirecionamento 302
(temporário). Para realizar um redirecionamento 301 (permanente), modifique o
segundo argumento::

    public function indexAction()
    {
        return $this->redirect($this->generateUrl('homepage'), 301);
    }

.. tip::

    O método ``redirect()`` é simplesmente um atalho que cria um objeto
    ``Response`` especializado em redirecionar o usuário. Ele é equivalente a:

    .. code-block:: php

        use Symfony\Component\HttpFoundation\RedirectResponse;

        return new RedirectResponse($this->generateUrl('homepage'));

.. index::
   single: Controlador; Direcionando

Direcionando
~~~~~~~~~~~~

Você também pode facilmente direcionar internamente para outro controlador com o
método ``forward()``. Em vez de redirecionar o navegador do usuário, ele faz
uma sub-requisição interna e chama o controlador especificado. O método
``forward()`` retorna o objeto ``Response`` que é retornado pelo controlador::

    public function indexAction($name)
    {
        $response = $this->forward('AcmeHelloBundle:Hello:fancy', array(
            'name'  => $name,
            'color' => 'green'
        ));

        // pode modificar a resposta ou retorná-la diretamente
        
        return $response;
    }

Note que o método `forward()` usa a mesma representação em string do
controlador que foi usada na configuração de roteamento. Nesse caso, a classe
controlador alvo será ``HelloController`` dentro de ``AcmeHelloBundle``.
O array passado para o método se torna os argumentos no controlador
resultante. Essa mesma interface é usada quando se embutem controladores em
templates (veja :ref:`templating-embedding-controller`). O método controlador
alvo deve se parecer com o seguinte::

    public function fancyAction($name, $color)
    {
        // ... cria e retorna um objeto Response
    }

E da mesma forma, quando criamos um controlador para uma rota, a ordem dos
argumentos para ``fancyAction`` não importa. O Symfony2 combina os nomes
das chaves dos índices (por exemplo, ``name``) com os nomes dos argumentos do
método (por exemplo, ``$name``). Se você mudar a ordem dos argumentos, o Symfony2
continuará passando os valores corretos para cada variável.

.. tip::

    Assim como em outros métodos do ``Controller`` base, o método ``forward`` é
    apenas um atalho para uma funcionalidade nuclear do Symfony2. Um
    direcionamento pode ser realizado diretamente por meio do serviço
    ``http_kernel``. Um direcionamento retorna um objeto ``Response``::
    
        $httpKernel = $this->container->get('http_kernel');
        $response = $httpKernel->forward('AcmeHelloBundle:Hello:fancy', array(
            'name'  => $name,
            'color' => 'green',
        ));

.. index::
   single: Controlador; Renderizando templates

.. _controller-rendering-templates:

Renderizando Templates
~~~~~~~~~~~~~~~~~~~~~~

Apesar de não ser um requisito, a maioria dos controladores irá, no fim das contas,
renderizar um template que é responsável por gerar o HTML (ou outro formato)
para o controlador. O método ``renderView()`` renderiza um template e retorna
seu conteúdo. O conteúdo do template pode ser usado para criar um objeto
``Response``::

    $content = $this->renderView('AcmeHelloBundle:Hello:index.html.twig', array('name' => $name));

    return new Response($content);

Isso pode ser feito até em um único passo usando o método ``render()``, que
retorna um objeto ``Response`` com o conteúdo do template::

    return $this->render('AcmeHelloBundle:Hello:index.html.twig', array('name' => $name));

Em ambos os casos, o template ``Resources/views/Hello/index.html.twig`` dentro
do ``AcmeHelloBundle`` será renderizado.

O sistema de template do Symfony é explicado com mais detalhes no capítulo
:doc:`Templating </book/templating>`.

.. tip::

    O método ``renderView`` é um atalho para usar diretamente o serviço
    ``templating``. O serviço ``templating`` também pode ser usado
    diretamente::
    
        $templating = $this->get('templating');
        $content = $templating->render('AcmeHelloBundle:Hello:index.html.twig', array('name' => $name));

.. index::
   single: Controlador; Acessando Serviços

Acessando outros Serviços
~~~~~~~~~~~~~~~~~~~~~~~~~

Quando se estende a classe controladora base, você pode acessar qualquer um dos
serviços Symfony2 através do método ``get()``. Aqui estão alguns dos serviços
mais comuns que você pode precisar::

    $request = $this->getRequest();

    $templating = $this->get('templating');

    $router = $this->get('router');

    $mailer = $this->get('mailer');

Existem outros inúmeros serviços disponíveis e você é encorajado a definir os
seus próprios. Para listar todos os serviços disponíveis, use o comando do
console ``container:debug``:

.. code-block:: bash

    php app/console container:debug

Para mais informações, veja o capítulo :doc:`/book/service_container`.


.. index::
   single: Controlador; Gerenciando Erros
   single: Controlador; Páginas 404

Gerenciando Erros e Páginas 404
-------------------------------

Quando algo não for encontrado, você deve usar de forma correta o protocolo
HTTP e retornar uma resposta 404. Para isso, você lançará um tipo especial de
exceção. Se estiver estendendo a classe controlador base, faça o seguinte::

    public function indexAction()
    {
        $product = // retrieve the object from database
        if (!$product) {
            throw $this->createNotFoundException('The product does not exist');
        }

        return $this->render(...);
    }

O método ``createNotFoundException()`` cria um objeto especial
``NotFoundHttpException``, que no fim dispara uma resposta HTTP 404 de dentro
do Symfony.

É lógico que você é livre para lançar qualquer classe ``Exception`` no seu
controlador - o Symfony irá retornar automaticamente uma resposta HTTP código
500.

.. code-block:: php

    throw new \Exception('Something went wrong!');

Em todo caso, uma página de erro estilizada é mostrada para o usuário final e
uma página de erro com informações de debug completa é mostrada para o
desenvolvedor (no caso de visualizar a página no modo debug). Ambas as páginas
podem ser personalizadas. Para detalhes, leia a receita
":doc:`/cookbook/controller/error_pages`" no cookbook.

.. index::
   single: Controlador; A Sessão
   single: Sessão

Gerenciando a Sessão
--------------------

O Symfony2 fornece um objeto de sessão muito bom que você pode usar para
guardar informações sobre o usuário (seja ele uma pessoa real usando um
navegador, um robô ou um web service) entre requisições. Por padrão, o
Symfony2 guarda os atributos em um cookie usando as sessões nativas do PHP.

O armazenamento e a recuperação de informações da sessão são feitos
facilmente de qualquer controlador::

    $session = $this->getRequest()->getSession();

    // store an attribute for reuse during a later user request
    $session->set('foo', 'bar');

    // in another controller for another request
    $foo = $session->get('foo');

    // use a default value if the key doesn't exist
    $filters = $session->get('filters', array());

Esses atributos permanecerão no usuário até o fim da sessão.

.. index::
   single Session; Flash messages

Mensagens Flash
~~~~~~~~~~~~~~~

Você também guardar pequenas mensagens que serão armazenadas na sessão do
usuário apenas por uma requisição. Isso é útil no processamento de formulários:
você pode redirecionar o usuário e mostrar uma mensagem especial na requisição
*seguinte*. Esses tipos de mensagens são chamadas de mensagens "flash".

Por exemplo, imagine que você esteja processando a submissão de um formulário::

    public function updateAction()
    {
        $form = $this->createForm(...);

        $form->bind($this->getRequest());
        if ($form->isValid()) {
            // do some sort of processing

            $this->get('session')->getFlashBag()->add('notice', 'Your changes were saved!');

            return $this->redirect($this->generateUrl(...));
        }

        return $this->render(...);
    }

Depois do processamento da requisição, o controlador define uma mensagem flash
`notice` e então faz o redirecionamento. O nome (``notice``) não é
importante - é apenas o que você usa para identificar o tipo da mensagem.

No template da próxima action, o código a seguir poderia ser usado para
renderizar a mensagem ``notice``:

.. configuration-block::

    .. code-block:: html+jinja

        {% for flashMessage in app.session.flashbag.get('notice') %}
            <div class="flash-notice">
                {{ flashMessage }}
            </div>
        {% endfor %}

    .. code-block:: php
    
        <?php foreach ($view['session']->getFlashBag()->get('notice') as $message): ?>
            <div class="flash-notice">
                <?php echo "<div class='flash-error'>$message</div>" ?>
            </div>
        <?php endforeach; ?>

Por definição, as mensagens flash são feitas para existirem por exatamente uma
requisição (elas "se vão num instante" - "gone in a flash"). Elas foram
projetadas para serem usadas entre redirecionamentos exatamente como você fez
nesse exemplo.

.. index::
   single: Controlador; Objeto Response 

O Objeto Response
-----------------

O único requisito de um controlador é retornar um objeto ``Response``. A classe
:class:`Symfony\\Component\\HttpFoundation\\Response` é uma abstração PHP em
volta da resposta HTTP - a mensagem em texto cheia de cabeçalhos HTTP e
conteúdo que é mandado de volta para o cliente::

    // create a simple Response with a 200 status code (the default)
    $response = new Response('Hello '.$name, 200);
    
    // create a JSON-response with a 200 status code
    $response = new Response(json_encode(array('name' => $name)));
    $response->headers->set('Content-Type', 'application/json');

.. tip::

	A propriedade ``headers`` é a classe
	:class:`Symfony\\Component\\HttpFoundation\\HeaderBag` com vários métodos
	úteis para ler e modificar os cabeçalhos do ``Response``. Os nomes dos
	cabeçalhos são normalizados de forma que usar ``Content-Type`` seja
	equivalente a ``content-type`` ou mesmo ``content_type``.

.. index::
   single: Controlador; Objeto Request 

O Objeto Request
----------------

Além dos valores nos marcadores de roteamento, o controlador também tem acesso
ao objeto ``Request`` quando está estendendo a classe ``Controller`` base::

    $request = $this->getRequest();

    $request->isXmlHttpRequest(); // is it an Ajax request?

    $request->getPreferredLanguage(array('en', 'fr'));

    $request->query->get('page'); // get a $_GET parameter

    $request->request->get('page'); // get a $_POST parameter

Assim como com o objeto ``Response``, os cabeçalhos da requisição são guardados
em um objeto ``HeaderBag`` e são facilmente acessados.

Considerações Finais
--------------------

Sempre que criar uma página, no final você precisará escrever algum código que
contenha a lógica dessa página. No Symfony, isso é chamado de controlador,
e ele é uma função PHP que faz tudo que for necessário para no fim retornar
o objeto ``Response`` final que será retornado ao usuário.

Para facilitar a vida, você pode escolher estender uma classe ``Controller``
base, que contém métodos que são atalhos para muitas tarefas comuns dos
controladores. Por exemplo, uma vez que você não queira colocar código HTML
no seu controlador, você pode usar o método ``render()`` para renderizar e
retornar o conteúdo de um template.

Em outros capítulos, você verá como o controlador pode ser usado para persistir
e buscar objetos em um banco de dados, processar submissões de formulários,
gerenciar cache e muito mais.

Continue!
---------

Em seguida, aprenda tudo sobre :doc:`renderizar templates com o Twig </templating>`.

Aprenda mais sobre Controladores
--------------------------------

.. toctree::
    :hidden:

    templating

.. toctree::
    :maxdepth: 1
    :glob:

    controller/*

.. _`unvalidated redirects security vulnerability`: https://www.owasp.org/index.php/Open_redirect
