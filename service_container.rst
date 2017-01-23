.. index::
   single: Service Container
   single: Container de Injeção de Dependência

Container de Serviço
====================

Uma aplicação PHP moderna é cheia de objetos. Um objeto pode facilitar a entrega 
de mensagens de e-mail enquanto outro pode permitir persistir as informações
em um banco de dados. Em sua aplicação, você pode criar um objeto que gerencia
seu estoque de produtos, ou outro objeto que processa dados de uma API de 
terceiros. O ponto é que uma aplicação moderna faz muitas coisas e é organizada
em muitos objetos que lidam com cada tarefa.

Neste capítulo, vamos falar sobre um objeto PHP especial no Symfony2 que ajuda
você a instanciar, organizar e recuperar os muitos objetos da sua aplicação.
Esse objeto, chamado de container de serviço, lhe permitirá padronizar e
centralizar a forma como os objetos são construídos em sua aplicação. O container
facilita a sua vida, é super rápido e enfatiza uma arquitetura que promove 
código reutilizável e desacoplado. E, como todas as classes principais do Symfony2
usam o container, você vai aprender como estender, configurar e usar qualquer objeto
no Symfony2. Em grande parte, o container de serviço é o maior contribuinte à 
velocidade e extensibilidade do Symfony2.

Finalmente, configurar e usar o container de serviço é fácil. Ao final deste 
capítulo, você estará confortável criando os seus próprios objetos através do container 
e personalizando objetos a partir de qualquer bundle de terceiros. Você vai começar
a escrever código que é mais reutilizável, testável e desacoplado, simplesmente porque
o container de serviço torna fácil escrever bom código.

.. index::
   single: Container de Serviço; O que é um Serviço?

O que é um Serviço?
-------------------

Simplificando, um :term:`Serviço` é qualquer objeto PHP que realiza algum tipo de
tarefa "global". É um nome genérico proposital, usado em ciência da computação,
para descrever um objeto que é criado para uma finalidade específica (por exemplo, 
entregar e-mails). Cada serviço é usado em qualquer parte da sua aplicação sempre que 
precisar da funcionalidade específica que ele fornece. Você não precisa fazer nada de 
especial para construir um serviço: basta escrever uma classe PHP com algum código que 
realiza uma tarefa específica. Parabéns, você acabou de criar um serviço!

.. note::

    Como regra geral, um objeto PHP é um serviço se ele é usado globalmente em sua
    aplicação. Um único serviço ``Mailer`` é utilizado globalmente para enviar
    mensagens de e-mail, enquanto os muitos objetos ``Message`` que ele entrega
    *não* são serviços. Do mesmo modo, um objeto ``Product`` não é um serviço, mas 
    um objeto que persiste os objetos ``Product`` para um banco de dados *é* um serviço.

Então, porque ele é especial? A vantagem de pensar em "serviços" é que você começa 
a pensar em separar cada pedaço de funcionalidade de sua aplicação em uma série de 
serviços. Uma vez que cada serviço realiza apenas um trabalho, você pode facilmente acessar 
cada serviço e usar a sua funcionalidade, sempre que você precisar. Cada serviço pode 
também ser mais facilmente testado e configurado já que ele está separado das outras 
funcionalidades em sua aplicação. Esta idéia é chamada de `arquitetura orientada a 
serviços`_ e não é exclusiva do Symfony2 ou até mesmo do PHP. Estruturar a sua aplicação 
em torno de um conjunto de classes de serviços independentes é uma das melhores práticas 
de orientação à objeto bem conhecida e confiável. Essas habilidades são a chave para 
ser um bom desenvolvedor em praticamente qualquer linguagem.

.. index::
   single: Container de Serviço; O que é?

O que é um Service Container?
----------------------------

Um :term:`Container de Serviço` (ou *container de injeção de dependência*) é 
simplesmente um objeto PHP que gerencia a instanciação de serviços (ex. objetos).
Por exemplo, imagine que temos uma classe PHP simples que envia mensagens de e-mail.
Sem um container de serviço, precisamos criar manualmente o objeto sempre que 
precisarmos dele:

.. code-block:: php

    use Acme\HelloBundle\Mailer;

    $mailer = new Mailer('sendmail');
    $mailer->send('ryan@foobar.net', ... );

Isso é bastante fácil. A classe imaginária ``Mailer`` nos permite configurar o método 
utilizado para entregar as mensagens de e-mail (por exemplo: ``sendmail``, ``smtp``, etc).
Mas, e se quiséssemos usar o serviço de mailer em outro lugar? Nós certamente não 
desejamos repetir a configuração do mailer *sempre* que nós precisamos usar o objeto 
``Mailer``. E se precisarmos mudar o ``transport`` de ``sendmail`` para ``smtp`` em 
toda a aplicação? Nós precisaremos localizar cada lugar em que adicionamos um serviço 
``Mailer`` e alterá-lo.

.. index::
   single: Container de Serviço; Configurando serviços

Criando/Configurando Serviços no Container
------------------------------------------

Uma resposta melhor é deixar o container de serviço criar o objeto ``Mailer`` 
para você. Para que isso funcione, é preciso *ensinar* o container como
criar o serviço ``Mailer``. Isto é feito através de configuração, que pode
ser especificada em YAML, XML ou PHP:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            my_mailer:
                class:        Acme\HelloBundle\Mailer
                arguments:    [sendmail]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <services>
            <service id="my_mailer" class="Acme\HelloBundle\Mailer">
                <argument>sendmail</argument>
            </service>
        </services>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setDefinition('my_mailer', new Definition(
            'Acme\HelloBundle\Mailer',
            array('sendmail')
        ));

.. note::

    Quando o Symfony2 é inicializado, ele constrói o container de serviço usando a
    configuração da aplicação (por padrão (``app/config/config.yml``). O arquivo 
    exato que é carregado é ditado pelo método ``AppKernel::registerContainerConfiguration()``
    , que carrega um arquivo de configuração do ambiente específico (por exemplo
    ``config_dev.yml`` para o ambiente ``dev`` ou ``config_prod.yml``
    para o ``prod``).

Uma instância do objeto ``Acme\HelloBundle\Mailer`` está agora disponível através do
container de serviço. O container está disponível em qualquer controlador tradicional do 
Symfony2 onde você pode acessar os serviços do container através do método de atalho 
``get()``::

    class HelloController extends Controller
    {
        // ...

        public function sendEmailAction()
        {
            // ...
            $mailer = $this->get('my_mailer');
            $mailer->send('ryan@foobar.net', ... );
        }
    }

Quando requisitamos o serviço ``my_mailer`` a partir do container, o container
constrói o objeto e o retorna. Esta é outra vantagem importante do uso do container
de serviço. Ou seja, um serviço *nunca* é construído até que ele seja necessário. 
Se você definir um serviço e nunca usá-lo em um pedido, o serviço nunca será criado. 
Isso economiza memória e aumenta a velocidade de sua aplicação. Isto também significa 
que não há nenhuma perda ou apenas uma perda insignificante de desempenho ao definir
muitos serviços. Serviços que nunca são usados, nunca são construídos.

Como um bônus adicional, o serviço ``Mailer`` é criado apenas uma vez e a mesma
instância é retornada cada vez que você requisitar o serviço. Isso é quase sempre
o comportamento que você precisa (é mais flexível e poderoso), mas vamos aprender,
mais tarde, como você pode configurar um serviço que possui várias instâncias.

.. _book-service-container-parameters:

Parâmetros do Serviço
---------------------

A criação de novos serviços (objetos, por exemplo) através do container é bastante
simples. Os parâmetros tornam a definição dos serviços mais organizada e flexível:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            my_mailer.class:      Acme\HelloBundle\Mailer
            my_mailer.transport:  sendmail

        services:
            my_mailer:
                class:        %my_mailer.class%
                arguments:    [%my_mailer.transport%]

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="my_mailer.class">Acme\HelloBundle\Mailer</parameter>
            <parameter key="my_mailer.transport">sendmail</parameter>
        </parameters>

        <services>
            <service id="my_mailer" class="%my_mailer.class%">
                <argument>%my_mailer.transport%</argument>
            </service>
        </services>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setParameter('my_mailer.class', 'Acme\HelloBundle\Mailer');
        $container->setParameter('my_mailer.transport', 'sendmail');

        $container->setDefinition('my_mailer', new Definition(
            '%my_mailer.class%',
            array('%my_mailer.transport%')
        ));

O resultado final é exatamente o mesmo de antes - a diferença está apenas em *como* 
definimos o serviço. Quando as strings ``my_mailer.class`` e ``my_mailer.transport`` 
estão envolvidas por sinais de porcentagem (``%``), o container sabe que deve procurar 
por parâmetros com esses nomes. Quando o container é construído, ele procura o valor de 
cada parâmetro e utiliza-o na definição do serviço.

.. note::

    O sinal de porcentagem dentro de um parâmetro ou argumento, como parte da string, deve 
    ser escapado com um outro sinal de porcentagem:
    
    .. code-block:: xml

        <argument type="string">http://symfony.com/?foo=%%s&bar=%%d</argument>

A finalidade dos parâmetros é alimentar informação para os serviços. Naturalmente, 
não há nada de errado em definir o serviço sem o uso de quaisquer parâmetros.
Os parâmetros, no entanto, possuem várias vantagens:

* separação e organização de todas as "opções" dos serviços sob uma única
  chave ``parameters``;

* os valores do parâmetro podem ser usados em múltiplas definições de serviços;

* ao criar um serviço em um bundle (vamos mostrar isso em breve), o uso de parâmetros
  permite que o serviço seja facilmente customizado em sua aplicação.

A escolha de usar ou não os parâmetros é com você. Bundles de terceiros de alta qualidade
*sempre* utilizarão parâmetros, pois, eles tornam mais configurável o serviço armazenado no 
container. Para os serviços em sua aplicação, entretanto, você pode não precisar da 
flexibilidade dos parâmetros.

Parâmetros Array
~~~~~~~~~~~~~~~~

Os parâmetros não precisam ser strings, eles também podem ser arrays. Para o formato XML
, você precisará usar o atributo type="collection" em todos os parâmetros que são
arrays.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            my_mailer.gateways:
                - mail1
                - mail2
                - mail3
            my_multilang.language_fallback:
                en:
                    - en
                    - fr
                fr:
                    - fr
                    - en

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="my_mailer.gateways" type="collection">
                <parameter>mail1</parameter>
                <parameter>mail2</parameter>
                <parameter>mail3</parameter>
            </parameter>
            <parameter key="my_multilang.language_fallback" type="collection">
                <parameter key="en" type="collection">
                    <parameter>en</parameter>
                    <parameter>fr</parameter>
                </parameter>
                <parameter key="fr" type="collection">
                    <parameter>fr</parameter>
                    <parameter>en</parameter>
                </parameter>
            </parameter>
        </parameters>

    .. code-block:: php

        // app/config/config.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setParameter('my_mailer.gateways', array('mail1', 'mail2', 'mail3'));
        $container->setParameter('my_multilang.language_fallback',
                                 array('en' => array('en', 'fr'),
                                       'fr' => array('fr', 'en'),
                                ));


Importando outros Recursos de Configuração do Container
-------------------------------------------------------

.. tip::

    Nesta seção, vamos referenciar os arquivos de configuração de serviço como *recursos*.
    Isto é para destacar o fato de que, enquanto a maioria dos recursos de configuração
    será em arquivos (por exemplo, YAML, XML, PHP), o Symfony2 é tão flexível que a configuração
    pode ser carregada de qualquer lugar (por exemplo, de um banco de dados ou até mesmo através 
    de um web service externo).

O service container é construído usando um único recurso de configuração 
(por padrão ``app/config/config.yml``). Todas as outras configurações de serviço
(incluindo o núcleo do Symfony2 e configurações de bundles de terceiros) devem
ser importadas dentro desse arquivo de uma forma ou de outra. Isso lhe dá absoluta
flexibilidade sobre os serviços em sua aplicação.

Configurações de serviços externos podem ser importadas de duas maneiras distintas. 
Primeiro, vamos falar sobre o método que você vai usar mais frequentemente na sua 
aplicação: a diretiva ``imports``. Na seção seguinte, vamos falar sobre o segundo método, 
que é mais flexível e preferido para a importação de configuração de serviços dos bundles 
de terceiros.

.. index::
   single: Container de Serviço; imports

.. _service-container-imports-directive:

Importando configuração com ``imports``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Até agora, adicionamos a nossa definição do container de serviço ``my_mailer`` 
diretamente no arquivo de configuração da aplicação (por exemplo: ``app/config/config.yml``). 
Naturalmente, já que a própria classe ``Mailer`` reside dentro do ``AcmeHelloBundle``, 
faz mais sentido colocar a definição do container ``my_mailer`` dentro do
bundle também.

Primeiro, mova a definição do container ``my_mailer`` dentro de um novo arquivo container de recurso 
dentro do ``AcmeHelloBundle``. Se os diretórios ``Resources`` ou ``Resources/config``
não existirem, adicione eles.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/services.yml
        parameters:
            my_mailer.class:      Acme\HelloBundle\Mailer
            my_mailer.transport:  sendmail

        services:
            my_mailer:
                class:        %my_mailer.class%
                arguments:    [%my_mailer.transport%]

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
        <parameters>
            <parameter key="my_mailer.class">Acme\HelloBundle\Mailer</parameter>
            <parameter key="my_mailer.transport">sendmail</parameter>
        </parameters>

        <services>
            <service id="my_mailer" class="%my_mailer.class%">
                <argument>%my_mailer.transport%</argument>
            </service>
        </services>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setParameter('my_mailer.class', 'Acme\HelloBundle\Mailer');
        $container->setParameter('my_mailer.transport', 'sendmail');

        $container->setDefinition('my_mailer', new Definition(
            '%my_mailer.class%',
            array('%my_mailer.transport%')
        ));

A definição em si não mudou, apenas a sua localização. Claro que o container de 
serviço não sabe sobre o novo arquivo de recurso. Felizmente, nós podemos facilmente 
importar o arquivo de recurso usando a chave ``imports`` na configuração da 
aplicação.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        imports:
            - { resource: @AcmeHelloBundle/Resources/config/services.yml }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <imports>
            <import resource="@AcmeHelloBundle/Resources/config/services.xml"/>
        </imports>

    .. code-block:: php

        // app/config/config.php
        $this->import('@AcmeHelloBundle/Resources/config/services.php');

A diretiva ``imports`` permite à sua aplicação incluir os recursos de configuração 
do container de serviço de qualquer outra localização (mais comumente, de bundles).
A localização do ``resource``, para arquivos, é o caminho absoluto para o arquivo de 
recurso. A sintaxe especial ``@AcmeHello`` resolve o caminho do diretório do
bundle ``AcmeHelloBundle``. Isso ajuda você a especificar o caminho para o recurso
sem preocupar-se mais tarde caso mover o ``AcmeHelloBundle`` para um diretório 
diferente.

.. index::
   single: Container de Serviço; Configuração de extensão

.. _service-container-extension-configuration:

Importando Configuração através de Extensões do Container
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ao desenvolver no Symfony2, você vai mais comumente usar a diretiva ``imports``
para importar a configuração do container dos bundles que você criou especificamente
para a sua aplicação. As configurações de container de bundles de terceiros, incluindo
os serviços do núcleo do Symfony2, são geralmente carregadas usando um outro método 
que é mais flexível e fácil de configurar em sua aplicação.

Veja como ele funciona. Internamente, cada bundle define os seus serviços de forma 
semelhante a que vimos até agora. Ou seja, um bundle utiliza um ou mais arquivos de 
configurações de recurso (normalmente XML) para especificar os parâmetros e serviços 
para aquele bundle. No entanto, em vez de importar cada um desses recursos diretamente a partir
da sua configuração da aplicação usando a diretiva ``imports``, você pode simplesmente
invocar uma *extensão do container de serviço* dentro do bundle que faz o trabalho para
você. Uma extensão do container de serviço é uma classe PHP criada pelo autor do bundle
para realizar duas coisas:

* importar todos os recursos de container de serviço necessários para configurar 
  os serviços para o bundle;

* fornecer configuração simples e semântica para que o bundle possa 
  ser configurado sem interagir com os parâmetros da configuração 
  do container de serviço.

Em outras palavras, uma extensão do container de serviço configura os serviços 
para um bundle em seu nome. E, como veremos em breve, a extensão fornece
uma interface sensível e de alto nível para configurar o bundle.

Considere o ``FrameworkBundle`` - o bundle do núcleo do framework Symfony2 - 
como um exemplo. A presença do código a seguir na configuração da sua aplicação
invoca a extensão do container de serviço dentro do ``FrameworkBundle``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            secret:          xxxxxxxxxx
            form:            true
            csrf_protection: true
            router:        { resource: "%kernel.root_dir%/config/routing.yml" }
            # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config secret="xxxxxxxxxx">
            <framework:form />
            <framework:csrf-protection />
            <framework:router resource="%kernel.root_dir%/config/routing.xml" />
            <!-- ... -->
        </framework>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'secret'          => 'xxxxxxxxxx',
            'form'            => array(),
            'csrf-protection' => array(),
            'router'          => array('resource' => '%kernel.root_dir%/config/routing.php'),
            // ...
        ));

Quando é realizado o parse da configuração, o container procura uma extensão que
pode lidar com a diretiva de configuração ``framework``. A extensão em questão,
que reside no ``FrameworkBundle``, é chamada e a configuração do serviço para o 
``FrameworkBundle`` é carregada. Se você remover totalmente a chave ``framework``
de seu arquivo de configuração da aplicação, os serviços do núcleo do Symfony2
não serão carregados. O ponto é que você está no controle: o framework Symfony2
não contém qualquer tipo de magia ou realiza quaisquer ações que você não tenha 
controle.

Claro que você pode fazer muito mais do que simplesmente "ativar" a extensão do 
container de serviço do ``FrameworkBundle``. Cada extensão permite que você facilmente
personalize o bundle, sem se preocupar com a forma como os serviços internos são
definidos.

Neste caso, a extensão permite que você personalize as configurações ``error_handler``, 
``csrf_protection``, ``router`` e muito mais. Internamente,
o ``FrameworkBundle`` usa as opções especificadas aqui para definir e configurar
os serviços específicos para ele. O bundle se encarrega de criar todos os ``parameters``
e ``services`` necessários para o container de serviço, permitindo ainda que grande parte 
da configuração seja facilmente personalizada. Como um bônus adicional, a maioria das 
extensões do container de serviço também são inteligentes o suficiente para executar a validação -
notificando as opções que estão faltando ou que estão com o tipo de dados incorreto.

Ao instalar ou configurar um bundle, consulte a documentação do bundle para
verificar como os serviços para o bundle devem ser instalados e configurados. As opções
disponíveis para os bundles do núcleo podem ser encontradas no :doc:`Reference Guide</reference/index>`.

.. note::

   Nativamente, o container de serviço somente reconhece as diretivas
   ``parameters``, ``services`` e ``imports``. Quaisquer outras diretivas
   são manipuladas pela extensão do container de serviço.

Se você deseja expor configuração amigável em seus próprios bundles, leia o
":doc:`/cookbook/bundles/extension`" do cookbook.

.. index::
   single: Container de Serviço; Referenciando serviços

Referenciando (Injetando) Serviços
----------------------------------

Até agora, nosso serviço original ``my_mailer`` é simples: ele recebe apenas um argumento
em seu construtor, que é facilmente configurável. Como você verá, o poder real
do container é percebido quando você precisa criar um serviço que depende de um ou 
mais outros serviços no container.

Vamos começar com um exemplo. Suponha que temos um novo serviço, ``NewsletterManager``,
que ajuda a gerenciar a preparação e entrega de uma mensagem de e-mail para 
uma coleção de endereços. Claro que o serviço ``my_mailer`` já está muito bom 
ao entregar mensagens de e-mail, por isso vamos usá-lo dentro do ``NewsletterManager`` 
para lidar com a entrega das mensagens. Esta simulação de classe seria parecida 
com::

    namespace Acme\HelloBundle\Newsletter;

    use Acme\HelloBundle\Mailer;

    class NewsletterManager
    {
        protected $mailer;

        public function __construct(Mailer $mailer)
        {
            $this->mailer = $mailer;
        }

        // ...
    }

Sem usar o container de serviço, podemos criar um novo ``NewsletterManager``
facilmente dentro de um controlador::

    public function sendNewsletterAction()
    {
        $mailer = $this->get('my_mailer');
        $newsletter = new Acme\HelloBundle\Newsletter\NewsletterManager($mailer);
        // ...
    }

Esta abordagem é boa, mas, e se decidirmos mais tarde que a classe ``NewsletterManager``
precisa de um segundo ou terceiro argumento? E se decidirmos refatorar nosso 
código e renomear a classe? Em ambos os casos, você precisa encontrar todos os
locais onde o ``NewsletterManager`` é instanciado e modificá-lo. Certamente, 
o container de serviço nos dá uma opção muito mais atraente:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/services.yml
        parameters:
            # ...
            newsletter_manager.class: Acme\HelloBundle\Newsletter\NewsletterManager

        services:
            my_mailer:
                # ...
            newsletter_manager:
                class:     %newsletter_manager.class%
                arguments: [@my_mailer]

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
        <parameters>
            <!-- ... -->
            <parameter key="newsletter_manager.class">Acme\HelloBundle\Newsletter\NewsletterManager</parameter>
        </parameters>

        <services>
            <service id="my_mailer" ... >
              <!-- ... -->
            </service>
            <service id="newsletter_manager" class="%newsletter_manager.class%">
                <argument type="service" id="my_mailer"/>
            </service>
        </services>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->setParameter('newsletter_manager.class', 'Acme\HelloBundle\Newsletter\NewsletterManager');

        $container->setDefinition('my_mailer', ... );
        $container->setDefinition('newsletter_manager', new Definition(
            '%newsletter_manager.class%',
            array(new Reference('my_mailer'))
        ));

Em YAML, a sintaxe especial ``@my_mailer`` diz ao container para procurar
por um serviço chamado ``my_mailer`` e passar esse objeto para o construtor 
do ``NewsletterManager``. Neste caso, entretanto, o serviço especificado ``my_mailer``
deve existir. Caso ele não existir, será gerada uma exceção. Você pode marcar suas
dependências como opcionais - o que será discutido na próxima seção.

O uso de referências é uma ferramenta muito poderosa que lhe permite criar classes de 
serviços independentes com dependências bem definidas. Neste exemplo, o serviço 
``newsletter_manager`` precisa do serviço ``my_mailer`` para funcionar. Quando você 
define essa dependência no container de serviço, o container cuida de todo o 
trabalho de instanciar os objetos.

Dependências Opcionais: Injeção do Setter 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Injetando dependências no construtor dessa maneira é uma excelente forma de 
assegurar que a dependência estará disponível para o uso. No entanto, se você possuir 
dependências opcionais para uma classe, a "injeção do setter" pode ser uma opção melhor. 
Isto significa injetar a dependência usando uma chamada de método ao invés do
construtor. A classe ficaria assim::

    namespace Acme\HelloBundle\Newsletter;

    use Acme\HelloBundle\Mailer;

    class NewsletterManager
    {
        protected $mailer;

        public function setMailer(Mailer $mailer)
        {
            $this->mailer = $mailer;
        }

        // ...
    }

Para injetar a dependência pelo método setter somente é necessária uma mudança da sintaxe:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/services.yml
        parameters:
            # ...
            newsletter_manager.class: Acme\HelloBundle\Newsletter\NewsletterManager

        services:
            my_mailer:
                # ...
            newsletter_manager:
                class:     %newsletter_manager.class%
                calls:
                    - [ setMailer, [ @my_mailer ] ]

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
        <parameters>
            <!-- ... -->
            <parameter key="newsletter_manager.class">Acme\HelloBundle\Newsletter\NewsletterManager</parameter>
        </parameters>

        <services>
            <service id="my_mailer" ... >
              <!-- ... -->
            </service>
            <service id="newsletter_manager" class="%newsletter_manager.class%">
                <call method="setMailer">
                     <argument type="service" id="my_mailer" />
                </call>
            </service>
        </services>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->setParameter('newsletter_manager.class', 'Acme\HelloBundle\Newsletter\NewsletterManager');

        $container->setDefinition('my_mailer', ... );
        $container->setDefinition('newsletter_manager', new Definition(
            '%newsletter_manager.class%'
        ))->addMethodCall('setMailer', array(
            new Reference('my_mailer')
        ));

.. note::

    As abordagens apresentadas nesta seção são chamadas de "injeção de construtor"
    e "injeção de setter". O container de serviço do Symfony2 também suporta a
    "injeção de propriedade".

Tornando Opcionais as Referências
---------------------------------

Às vezes, um de seus serviços pode ter uma dependência opcional, ou seja,
a dependência não é exigida por seu serviço para funcionar corretamente. No
exemplo acima, o serviço ``my_mailer`` *deve* existir, caso contrário, uma exceção
será gerada. Ao modificar a definição do serviço ``newsletter_manager`` ,
você pode tornar esta referência opcional. O container irá, então, injetá-lo se
ele existir e não irá fazer nada caso contrário:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/services.yml
        parameters:
            # ...

        services:
            newsletter_manager:
                class:     %newsletter_manager.class%
                arguments: [@?my_mailer]

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/services.xml -->

        <services>
            <service id="my_mailer" ... >
              <!-- ... -->
            </service>
            <service id="newsletter_manager" class="%newsletter_manager.class%">
                <argument type="service" id="my_mailer" on-invalid="ignore" />
            </service>
        </services>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;
        use Symfony\Component\DependencyInjection\ContainerInterface;

        // ...
        $container->setParameter('newsletter_manager.class', 'Acme\HelloBundle\Newsletter\NewsletterManager');

        $container->setDefinition('my_mailer', ... );
        $container->setDefinition('newsletter_manager', new Definition(
            '%newsletter_manager.class%',
            array(new Reference('my_mailer', ContainerInterface::IGNORE_ON_INVALID_REFERENCE))
        ));

Em YAML, a sintaxe especial ``@?`` diz ao container de serviço que a dependência
é opcional. Naturalmente, o ``NewsletterManager`` também deve ser escrito para
permitir uma dependência opcional:

.. code-block:: php

        public function __construct(Mailer $mailer = null)
        {
            // ...
        }

Serviços do Núcleo do Symfony e de Bundles de Terceiros 
-------------------------------------------------------

Desde que o Symfony2 e todos os bundles de terceiros configuram e recuperam os seus serviços
através do container, você pode acessá-los facilmente ou até mesmo usá-los em seus próprios
serviços. Para manter tudo simples, o Symfony2, por padrão, não exige que controladores sejam 
definidos como serviços. Além disso, o Symfony2 injeta o container de serviço inteiro em seu 
controlador. Por exemplo, para gerenciar o armazenamento de informações em uma sessão do 
usuário, o Symfony2 fornece um serviço ``session``, que você pode acessar dentro de um 
controlador padrão da seguinte forma::

    public function indexAction($bar)
    {
        $session = $this->get('session');
        $session->set('foo', $bar);

        // ...
    }

No Symfony2, você vai usar constantemente os serviços fornecidos pelo núcleo do Symfony ou
outros bundles de terceiros para executar tarefas como renderização de templates (``templating``),
envio de e-mails (``mailer``), ou acessar informações sobre o pedido(``request``).

Podemos levar isto um passo adiante, usando esses serviços dentro dos serviços que
você criou para a sua aplicação. Vamos modificar o ``NewsletterManager``
para utilizar o serviço de ``mailer`` real do Symfony2 (no lugar do ``my_mailer``).
Vamos também passar o serviço de templating engine para o ``NewsletterManager``
então ele poderá gerar o conteúdo de e-mail através de um template::

    namespace Acme\HelloBundle\Newsletter;

    use Symfony\Component\Templating\EngineInterface;

    class NewsletterManager
    {
        protected $mailer;

        protected $templating;

        public function __construct(\Swift_Mailer $mailer, EngineInterface $templating)
        {
            $this->mailer = $mailer;
            $this->templating = $templating;
        }

        // ...
    }

A configuração do service container é fácil:

.. configuration-block::

    .. code-block:: yaml

        services:
            newsletter_manager:
                class:     %newsletter_manager.class%
                arguments: [@mailer, @templating]

    .. code-block:: xml

        <service id="newsletter_manager" class="%newsletter_manager.class%">
            <argument type="service" id="mailer"/>
            <argument type="service" id="templating"/>
        </service>

    .. code-block:: php

        $container->setDefinition('newsletter_manager', new Definition(
            '%newsletter_manager.class%',
            array(
                new Reference('mailer'),
                new Reference('templating')
            )
        ));

O serviço ``newsletter_manager`` agora tem acesso aos serviços do núcleo 
``mailer`` and ``templating``. Esta é uma forma comum de criar serviços 
específicos para sua aplicação que aproveitam o poder de diferentes serviços 
dentro do framework.

.. tip::

    Certifique-se de que a entrada ``swiftmailer`` aparece na configuração da 
    sua aplicação. Como mencionamos em :ref:`service-container-extension-configuration`,
    a chave ``swiftmailer`` invoca a extensão de serviço do 
    ``SwiftmailerBundle``, que registra o serviço ``mailer``.

.. index::
   single: Container de Serviço; Configuração avançada

Configuração Avançada do Container
----------------------------------

Como vimos, a definição de serviços no interior do container é fácil, geralmente
envolvendo uma chave de configuração ``service`` e alguns parâmetros. No entanto,
o container possui várias outras ferramentas disponíveis que ajudam a adicionar uma *tag* 
para uma funcionalidade especial nos serviços, criar serviços mais complexos e executar 
operações após o container estar construído.

Marcando Serviços como público / privado
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ao definir os serviços, normalmente você vai desejar poder acessar essas definições
dentro do código da sua aplicação. Esses serviços são chamados ``publicos``. Por exemplo,
o serviço ``doctrine`` registrado com o container quando se utiliza o DoctrineBundle
é um serviço público, já que você pode acessá-lo via::

   $doctrine = $container->get('doctrine');

No entanto, existem casos de uso em que você não deseja que um serviço seja público. 
Isto é comum quando um serviço é definido apenas porque poderia ser usado como um
argumento para um outro serviço.

.. note::

    Se você usar um serviço privado como um argumento para mais de um serviço,
    isto irá resultar em duas instâncias diferentes sendo usadas, ​​já que, a instanciação
    do serviço privado é realizada inline (por exemplo: ``new PrivateFooBar()``).

Simplesmente falando: Um serviço será privado quando você não quiser acessá-lo
diretamente em seu código.

Aqui está um exemplo:

.. configuration-block::

    .. code-block:: yaml

        services:
           foo:
             class: Acme\HelloBundle\Foo
             public: false

    .. code-block:: xml

        <service id="foo" class="Acme\HelloBundle\Foo" public="false" />

    .. code-block:: php

        $definition = new Definition('Acme\HelloBundle\Foo');
        $definition->setPublic(false);
        $container->setDefinition('foo', $definition);

Agora que o serviço é privado, você *não* pode chamar::

    $container->get('foo');

No entanto, se o serviço foi marcado como privado, você ainda pode adicionar um alias
para ele (veja abaixo) para acessar este serviço (através do alias).

.. note::

   Os serviços são públicos por padrão.

Definindo Alias
~~~~~~~~~~~~~~~

Ao usar bundles do núcleo ou de terceiros dentro de sua aplicação, você pode desejar
usar atalhos para acessar alguns serviços. Isto é possível adicionando alias à eles 
e, além disso, você pode até mesmo adicionar alias para serviços que não são públicos.

.. configuration-block::

    .. code-block:: yaml

        services:
           foo:
             class: Acme\HelloBundle\Foo
           bar:
             alias: foo

    .. code-block:: xml

        <service id="foo" class="Acme\HelloBundle\Foo"/>

        <service id="bar" alias="foo" />

    .. code-block:: php

        $definition = new Definition('Acme\HelloBundle\Foo');
        $container->setDefinition('foo', $definition);

        $containerBuilder->setAlias('bar', 'foo');

Isto significa que, ao usar o container diretamente, você pode acessar o serviço
``foo`` pedindo pelo serviço ``bar`` como segue::

    $container->get('bar'); // retorna o serviço foo

Incluindo Arquivos
~~~~~~~~~~~~~~~~~~

Podem haver casos de uso quando é necessário incluir outro arquivo antes do 
serviço em si ser carregado. Para fazer isso, você pode usar a diretiva ``file``.

.. configuration-block::

    .. code-block:: yaml

        services:
           foo:
             class: Acme\HelloBundle\Foo\Bar
             file: %kernel.root_dir%/src/path/to/file/foo.php

    .. code-block:: xml

        <service id="foo" class="Acme\HelloBundle\Foo\Bar">
            <file>%kernel.root_dir%/src/path/to/file/foo.php</file>
        </service>

    .. code-block:: php

        $definition = new Definition('Acme\HelloBundle\Foo\Bar');
        $definition->setFile('%kernel.root_dir%/src/path/to/file/foo.php');
        $container->setDefinition('foo', $definition);

Observe que o symfony irá, internamente, chamar a função PHP require_once 
o que significa que seu arquivo será incluído apenas uma vez por pedido.

.. _book-service-container-tags:

Tags (``tags``)
~~~~~~~~~~~~~~~

Da mesma forma que podem ser definidas tags para um post de um blog na web com palavras 
como "Symfony" ou "PHP", também podem ser definidas tags para os serviços configurados 
em seu container. No container de serviço, a presença de uma tag significa que o serviço 
destina-se ao uso para um fim específico. Veja o seguinte exemplo:

.. configuration-block::

    .. code-block:: yaml

        services:
            foo.twig.extension:
                class: Acme\HelloBundle\Extension\FooExtension
                tags:
                    -  { name: twig.extension }

    .. code-block:: xml

        <service id="foo.twig.extension" class="Acme\HelloBundle\Extension\FooExtension">
            <tag name="twig.extension" />
        </service>

    .. code-block:: php

        $definition = new Definition('Acme\HelloBundle\Extension\FooExtension');
        $definition->addTag('twig.extension');
        $container->setDefinition('foo.twig.extension', $definition);

A tag ``twig.extension`` é uma tag especial que o ``TwigBundle`` usa durante a 
configuração. Ao definir a tag ``twig.extension`` para este serviço, o bundle 
saberá que o serviço ``foo.twig.extension`` deve ser registrado como uma extensão 
Twig com o Twig. Em outras palavras, o Twig encontra todos os serviços com a tag
``twig.extension`` e automaticamente registra-os como extensões.

Tags, então, são uma forma de dizer ao Symfony2 ou outros bundles de terceiros que
o seu serviço deve ser registrado ou usado de alguma forma especial pelo bundle.

Segue abaixo uma lista das tags disponíveis com os bundles do núcleo do Symfony2.
Cada uma delas tem um efeito diferente no seu serviço e muitas tags requerem
argumentos adicionais (além do parâmetro ``name``).

* assetic.filter
* assetic.templating.php
* data_collector
* form.field_factory.guesser
* kernel.cache_warmer
* kernel.event_listener
* monolog.logger
* routing.loader
* security.listener.factory
* security.voter
* templating.helper
* twig.extension
* translation.loader
* validator.constraint_validator

Aprenda mais
------------

.. toctree::
    :maxdepth: 1
    :glob:

    /service_container/*

.. _`arquitetura orientada a serviços`: https://en.wikipedia.org/wiki/Service-oriented_architecture
