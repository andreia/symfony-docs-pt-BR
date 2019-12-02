
.. index::
   single: Messenger

Messenger: Sincronização e Manipulação de Mensagens na Fila
===========================================================

O Messenger fornece um barramento de mensagens com capacidade de enviar mensagens e
manuseá-las imediatamente em sua aplicação ou enviá-las através de transportes
(por exemplo, filas) para serem tratadas mais tarde. Para aprender mais profundamente,
leia a :doc:`documentação do componente Messenger </components/messenger>`.

Instalação
----------

Em aplicações usando o :ref:`Symfony Flex <symfony-flex>`, execute o comando a seguir
para instalar o Messenger:

.. code-block:: terminal

    $ composer require messenger

Criando uma Mensagem e um Manipulador
-------------------------------------

O Messenger funciona em torno de duas classes que você deverá criar: (1) uma classe de
mensagem que contém dados e (2) uma classe de manipulador(es) que será chamada quando essa
mensagem é enviada. A classe de manipulador lerá a classe de mensagem e executará
alguma tarefa.

Não há requisitos específicos para uma classe de mensagem, exceto que ela pode ser
serializada::

    // src/Message/SmsNotification.php
    namespace App\Message;

    class SmsNotification
    {
        private $content;

        public function __construct(string $content)
        {
            $this->content = $content;
        }

        public function getContent(): string
        {
            return $this->content;
        }
    }

.. _messenger-handler:

Um manipulador de mensagens é um callable do PHP, a maneira recomendada para criá-lo é através de uma
classe que implementa ``MessageHandlerInterface`` e tem um método ``__invoke()`` que é
type-hinted com a classe de mensagem (ou uma interface de mensagem)::

    // src/MessageHandler/SmsNotificationHandler.php
    namespace App\MessageHandler;

    use App\Message\SmsNotification;
    use Symfony\Component\Messenger\Handler\MessageHandlerInterface;

    class SmsNotificationHandler implements MessageHandlerInterface
    {
        public function __invoke(SmsNotification $message)
        {
            // ... do some work - like sending an SMS message!
        }
    }

Graças a :ref:`autoconfiguração <services-autoconfigure>` e ao type-hint ``SmsNotification``,
o Symfony sabe que esse manipulador deve ser chamado quando uma mensagem ``SmsNotification``
é enviada. Na maioria das vezes, isso é tudo que você precisa fazer. Mas você também pode
:ref:`configurar manualmente os manuladores de mensagem <messenger-handler-config>`. Para
ver todos os manipuladores configurados, execute:

.. code-block:: terminal

    $ php bin/console debug:messenger

Disparando a Mensagem
---------------------

Tudo está pronto! Para disparar a mensagem (e chamar o manipulador), injete o
serviço ``message_bus`` (via ``MessageBusInterface``), como em um controlador::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use App\Message\SmsNotification;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Messenger\MessageBusInterface;

    class DefaultController extends AbstractController
    {
        public function index(MessageBusInterface $bus)
        {
            // will cause the SmsNotificationHandler to be called
            $bus->dispatch(new SmsNotification('Look! I created a message!'));

            // or use the shortcut
            $this->dispatchMessage(new SmsNotification('Look! I created a message!'));

            // ...
        }
    }

Transportes: Mensagens Assíncronas/Em Fila
------------------------------------------

Por padrão, as mensagens são manipuladas assim que são disparadas. Se você quiser
manipular uma mensagem de forma assíncrona, você pode configurar um transporte. Um transporte
é capaz de enviar mensagens (por exemplo, para um sistema de filas) e depois
:ref:`recebe-las através de um worker <messenger-worker>`. O Messenger suporta
:ref:`múltiplos transportes <messenger-transports-config>`.

.. note::

    Se você deseja usar um transporte não suportado, consulte o
    `transporte Enqueue`_, que suporta, por exemplo: Kafka, Amazon SQS e
    Pub/Sub Google.

Um transporte é registrado usando um "DSN". Graças à receita Flex do Messenger, seu
arquivo ``.env`` já possui alguns exemplos.

.. code-block:: bash

    # MESSENGER_TRANSPORT_DSN=amqp://guest:guest@localhost:5672/%2f/messages
    # MESSENGER_TRANSPORT_DSN=doctrine://default
    # MESSENGER_TRANSPORT_DSN=redis://localhost:6379/messages

Descomente o transporte que desejar (ou defina-o em ``.env.local``). Veja
:ref:`messenger-transports-config` para mais detalhes.

Em seguida, em ``config/packages/messenger.yaml``, vamos definir um transporte chamado ``async``
que usa esta configuração:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async: "%env(MESSENGER_TRANSPORT_DSN)%"

                    # or expanded to configure more options
                    #async:
                    #    dsn: "%env(MESSENGER_TRANSPORT_DSN)%"
                    #    options: []

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:transport name="async">%env(MESSENGER_TRANSPORT_DSN)%</framework:transport>

                    <!-- or expanded to configure more options -->
                    <framework:transport name="async"
                        dsn="%env(MESSENGER_TRANSPORT_DSN)%"
                    >
                        <option key="...">...</option>
                    </framework:transport>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async' => '%env(MESSENGER_TRANSPORT_DSN)%',

                    // or expanded to configure more options
                    'async' => [
                       'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                       'options' => []
                    ],
                ],
            ],
        ]);

.. _messenger-routing:

Roteando Mensagens para um Transporte
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora que você tem um transporte configurado, em vez de manipular uma mensagem imediatamente,
você pode configurá-las para serem enviadas para um transporte:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async: "%env(MESSENGER_TRANSPORT_DSN)%"

                routing:
                    # async is whatever name you gave your transport above
                    'App\Message\SmsNotification':  async

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="App\Message\SmsNotification">
                        <!-- async is whatever name you gave your transport above -->
                        <framework:sender service="async"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'routing' => [
                    // async is whatever name you gave your transport above
                    'App\Message\SmsNotification' => 'async',
                ],
            ],
        ]);

Graças a isso, a ``App\Message\SmsNotification`` será enviada para o transporte
``async`` e seu(s) manipulador(es) *não* serão chamados imediatamente. Qualquer mensagem não
correspondida sob ``routing`` ainda será tratada imediatamente.

Você também pode rotear classes por sua classe pai ou interface. Ou enviar mensagens
para múltiplos transportes:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                routing:
                    # route all messages that extend this example base class or interface
                    'App\Message\AbstractAsyncMessage': async
                    'App\Message\AsyncMessageInterface': async

                    'My\Message\ToBeSentToTwoSenders': [async, audit]

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <!-- route all messages that extend this example base class or interface -->
                    <framework:routing message-class="App\Message\AbstractAsyncMessage">
                        <framework:sender service="async"/>
                    </framework:routing>
                    <framework:routing message-class="App\Message\AsyncMessageInterface">
                        <framework:sender service="async"/>
                    </framework:routing>
                    <framework:routing message-class="My\Message\ToBeSentToTwoSenders">
                        <framework:sender service="async"/>
                        <framework:sender service="audit"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'routing' => [
                    // route all messages that extend this example base class or interface
                    'App\Message\AbstractAsyncMessage' => 'async',
                    'App\Message\AsyncMessageInterface' => 'async',
                    'My\Message\ToBeSentToTwoSenders' => ['async', 'audit'],
                ],
            ],
        ]);

Entidades do Doctrine em Mensagens
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você precisar passar uma entidade do Doctrine em uma mensagem, é melhor passar a chave primária
da entidade (ou qualquer informação relevante que o manipulador realmente precise, como ``email``,
etc) em vez do objeto::

    class NewUserWelcomeEmail
    {
        private $userId;

        public function __construct(int $userId)
        {
            $this->userId = $userId;
        }

        public function getUserId(): int
        {
            return $this->userId;
        }
    }

Então, em seu manipulador, você pode consultar um novo objeto::

    // src/MessageHandler/NewUserWelcomeEmailHandler.php
    namespace App\MessageHandler;

    use App\Message\NewUserWelcomeEmail;
    use App\Repository\UserRepository;
    use Symfony\Component\Messenger\Handler\MessageHandlerInterface;

    class NewUserWelcomeEmailHandler implements MessageHandlerInterface
    {
        private $userRepository;

        public function __construct(UserRepository $userRepository)
        {
            $this->userRepository = $userRepository;
        }

        public function __invoke(NewUserWelcomeEmail $welcomeEmail)
        {
            $user = $this->userRepository->find($welcomeEmail->getUserId());

            // ... send an email!
        }
    }

Isso garante que a entidade contenha dados atualizados.

Manipulando Mensagens de Forma Síncrona
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se uma mensagem não :ref:`corresponder a qualquer regra de roteamento <messenger-routing>`, ela não será
enviada para qualquer transporte e será manipulada imediatamente. Em alguns casos (como
no `bind de manipuladores para transportes diferentes`_),
é mais fácil ou mais flexível lidar com isso explicitamente: criando um transporte
``sync`` e "enviando" mensagens para serem manipuladas imediatamente:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    # ... other transports

                    sync: 'sync://'

                routing:
                    App\Message\SmsNotification: sync

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <!-- ... other transports -->

                    <framework:transport name="sync" dsn="sync://"/>

                    <framework:routing message-class="App\Message\SmsNotification">
                        <framework:sender service="sync"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    // ... other transports

                    'sync' => 'sync://',
                ],
                'routing' => [
                    'App\Message\SmsNotification' => 'sync',
                ],
            ],
        ]);

Criando o seu Próprio Transporte
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode criar seu próprio transporte se precisar enviar ou receber mensagens
a partir de algo que não é suportado. Veja :doc:`/messenger/custom-transport`.

.. _messenger-worker:

Consumindo Mensagens (Executando o Worker)
------------------------------------------

Depois que suas mensagens forem roteadas, na maioria dos casos, você precisará "consumi-las".
Você pode fazer isso através do comando ``messenger:consume``:

.. code-block:: terminal

    $ php bin/console messenger:consume async

    # use -vv to see details about what's happening
    $ php bin/console messenger:consume async -vv

O primeiro argumento é o nome do receptor (ou o ID do serviço, se você rotear para um
serviço customizado). Por padrão, o comando será executado para sempre: procurando novas mensagens
no seu transporte e manuseando elas. Este comando é chamado seu "worker".

Implantando em Produção
~~~~~~~~~~~~~~~~~~~~~~~

Em produção, há alguns detalhes importantes a considerar:

**Use o Supervisor para Manter seu(s) Worker(s) em Execução**
    Você deseja que um ou mais "workers" sejam executados o tempo todo. Para fazer isso, use um
    sistema de controle de processo como o :ref:`Supervisor <messenger-supervisor>`.

**Não deixe os Workers Executarem para Sempre**
    Alguns serviços (como o EntityManager do Doctrine) consumirão mais memória
    ao longo do tempo. Portanto, em vez de permitir que seu worker execute para sempre, use uma flag
    como ``messenger:consume --limit=10`` para dizer ao seu worker para lidar com apenas com 10
    mensagens antes de sair (o Supervisor criará um novo processo). Também existem
    outras opções como ``--memory-limit=128M`` e ``--time-limit=3600``.

**Reinicie os Workers ao Implantar**
    Cada vez que você implantar, precisará reiniciar todos os processos do worker para que
    que eles vejam o código recém-implantado. Para fazer isso, execute ``messenger:stop-workers``
    na implantação. Isso sinalizará para cada worker que ele deve terminar a mensagem
    que está sendo manipulada atualmente e desligar normalmente. Em seguida, o Supervisor criará
    novos processos worker. O comando usa o :ref:`app <cache-configuration-with-frameworkbundle>`
    cache internamente - portanto, verifique se está configurado para usar um adaptador que você gosta.

Transportes Priorizados
~~~~~~~~~~~~~~~~~~~~~~~

Às vezes, certos tipos de mensagens devem ter uma prioridade mais alta e serem manipulados
antes dos outros. Para tornar isso possível, você pode criar vários transportes e rotear
mensagens diferentes para eles. Por exemplo:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_high:
                        dsn: '%env(MESSENGER_TRANSPORT_DSN)%'
                        options:
                            # queue_name is specific to the doctrine transport
                            queue_name: high

                            # for amqp send to a separate exchange then queue
                            #exchange:
                            #    name: high
                            #queues:
                            #    messages_high: ~
                            # or redis try "group"
                    async_priority_low:
                        dsn: '%env(MESSENGER_TRANSPORT_DSN)%'
                        options:
                            queue_name: low

                routing:
                    'App\Message\SmsNotification':  async_priority_low
                    'App\Message\NewUserWelcomeEmail':  async_priority_high

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:transport name="async_priority_high" dsn="%env(MESSENGER_TRANSPORT_DSN)%">
                        <option key="queue_name">high</option>
                    </framework:transport>
                    <framework:transport name="async_priority_low" dsn="%env(MESSENGER_TRANSPORT_DSN)%">
                        <option key="queue_name">low</option>
                    </framework:transport>

                    <framework:routing message-class="App\Message\SmsNotification">
                        <framework:sender service="async_priority_low"/>
                    </framework:routing>
                    <framework:routing message-class="App\Message\NewUserWelcomeEmail">
                        <framework:sender service="async_priority_high"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async_priority_high' => [
                        'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                        'options' => [
                            'queue_name' => 'high',
                        ],
                    ],
                    'async_priority_low' => [
                        'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                        'options' => [
                            'queue_name' => 'low',
                        ],
                    ],
                ],
                'routing' => [
                    'App\Message\SmsNotification' => 'async_priority_low',
                    'App\Message\NewUserWelcomeEmail' => 'async_priority_high',
                ],
            ],
        ]);

Você pode executar workers individuais para cada transporte ou instruir um worker
para manipular mensagens em uma ordem de prioridade:

.. code-block:: terminal

    $ php bin/console messenger:consume async_priority_high async_priority_low

O worker sempre procurará primeiro as mensagens aguardando em ``async_priority_high``. Caso
não houver nenhuma, *então* ele consumirá mensagens de ``async_priority_low``.

.. _messenger-supervisor:

Configuração do Supervisor
~~~~~~~~~~~~~~~~~~~~~~~~~~

O Supervisor é uma ótima ferramenta para garantir que seus processos workers estejam
*sempre* em execução (mesmo se fechar devido a falha, atingindo um limite de mensagens
ou graças ao ``messenger:stop-workers``). Você pode instalá-lo no Ubuntu, por
exemplo, via:

.. code-block:: terminal

    $ sudo apt-get install supervisor

Os arquivos de configuração do supervisor geralmente residem no diretório ``/etc/supervisor/conf.d``
. Por exemplo, você pode criar um novo arquivo ``messenger-worker.conf``
para garantir que 2 instâncias do ``messenger:consume`` estejam sempre em
execução:

.. code-block:: ini

    ;/etc/supervisor/conf.d/messenger-worker.conf
    [program:messenger-consume]
    command=php /path/to/your/app/bin/console messenger:consume async --time-limit=3600
    user=ubuntu
    numprocs=2
    autostart=true
    autorestart=true
    process_name=%(program_name)s_%(process_num)02d

Altere o argumento ``async`` para usar o nome do seu transporte (ou transportes)
e ``user`` para o usuário Unix no seu servidor. Em seguida, diga ao Supervisor para ler sua
configuração e iniciar seus workers:

.. code-block:: terminal

    $ sudo supervisorctl reread

    $ sudo supervisorctl update

    $ sudo supervisorctl start messenger-consume:*

Veja a `documentação do Supervisor`_ para mais detalhes.

.. _messenger-retries-failures:

Tentativas e Falhas
-------------------

Se uma exceção for lançada ao consumir uma mensagem de um transporte, ela será
reenviada automaticamente para o transporte para tentar novamente. Por padrão, uma mensagem
será tentada novamente três vezes antes de ser descartada ou
:ref:`enviada para o transporte de falha <messenger-failure-transport>`. Cada nova tentativa
também será adiada, caso a falha tenha ocorrido devido a um problema temporário. Tudo
isso é configurável para cada transporte:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_high:
                        dsn: '%env(MESSENGER_TRANSPORT_DSN)%'

                        # default configuration
                        retry_strategy:
                            max_retries: 3
                            # milliseconds delay
                            delay: 1000
                            # causes the delay to be higher before each retry
                            # e.g. 1 second delay, 2 seconds, 4 seconds
                            multiplier: 2
                            max_delay: 0
                            # override all of this with a service that
                            # implements Symfony\Component\Messenger\Retry\RetryStrategyInterface
                            # service: null

Evitando Tentar Novamente
~~~~~~~~~~~~~~~~~~~~~~~~~

Às vezes, lidar com uma mensagem pode falhar de uma maneira que você *sabe* que é permanente
e não deve ser tentado novamente. Se você lançar
:class:`Symfony\\Component\\Messenger\\Exception\\UnrecoverableMessageHandlingException`,
a mensagem não será tentada novamente.

.. _messenger-failure-transport:

Salvando e Tentando Novamente Mensagens com Falha
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se uma mensagem falhar, ela é repetida várias vezes (``max_retries``) e depois será
descartada. Para evitar que isso aconteça, você pode configurar um ``failure_transport``:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                # after retrying, messages will be sent to the "failed" transport
                failure_transport: failed

                transports:
                    # ... other transports

                    failed: 'doctrine://default?queue_name=failed'

Neste exemplo, se a manipulação de uma mensagem falhar três vezes (o padrão ``max_retries``),
ela será enviada para o transporte ``failed``. Enquanto você *pode* usar
``messenger:consume failed`` para consumir isso como um transporte normal, você
geralmente desejará visualizar manualmente as mensagens no transporte de falha e escolher
tentar novamente elas:

.. code-block:: terminal

    # see all messages in the failure transport
    $ php bin/console messenger:failed:show

    # see details about a specific failure
    $ php bin/console messenger:failed:show 20 -vv

    # view and retry messages one-by-one
    $ php bin/console messenger:failed:retry -vv

    # retry specific messages
    $ php bin/console messenger:failed:retry 20 30 --force

    # remove a message without retrying it
    $ php bin/console messenger:failed:remove 20

Se a mensagem falhar novamente, ela será reenviada para o transporte de falha
devido as :ref:`regras de tentativa <messenger-retries-failures>` normais. Uma vez que o máximo
de tentativas for atingido, a mensagem será descartada permanentemente.

.. _messenger-transports-config:

Configuração de Transporte
--------------------------

O Messenger suporta vários tipos de transporte diferentes, cada um com suas próprias
opções.

Transporte AMQP
~~~~~~~~~~~~~~~

A configuração de transporte ``amqp`` é assim:

.. code-block:: bash

    # .env
    MESSENGER_TRANSPORT_DSN=amqp://guest:guest@localhost:5672/%2f/messages

Para usar o transporte AMQP embutido do Symfony, você precisa da extensão AMQP do PHP.

.. note::

    Por padrão, o transporte criará automaticamente quaisquer exchanges, filas e
    binding keys necessárias. Isso pode ser desativado, mas algumas funcionalidades
    pode não funcionar corretamente (como filas adiadas).

O transporte possui várias outras opções, incluindo maneiras de configurar
exchange, enfileirar binding keys e muito mais. Consulte a documentação em
:class:`Symfony\\Component\\Messenger\\Transport\\AmqpExt\\Connection`.

Você também pode definir configurações específicas do AMQP na sua mensagem adicionando
:class:`Symfony\\Component\\Messenger\\Transport\\AmqpExt\\AmqpStamp` em
seu Envelope::

    use Symfony\Component\Messenger\Transport\AmqpExt\AmqpStamp;
    // ...

    $attributes = [];
    $bus->dispatch(new SmsNotification(), [
        new AmqpStamp('custom-routing-key', AMQP_NOPARAM, $attributes)
    ]);

.. caution::

    Os consumidores não aparecem em um painel de administração, pois esse transporte não depende de
    ``\AmqpQueue::consume()`` que está bloqueando. Ter um receptor bloqueando torna
    as opções ``--time-limit/--memory-limit`` do comando ``messenger:consume``, bem como
    o comando ``messenger:stop-workers`` é ineficiente, pois todos confiam no fato de que
    o receptor retorna imediatamente, independentemente de encontrar uma mensagem ou não. O worker
    consumidor é responsável pela iteração até receber uma mensagem para manipular e/ou até que uma
    das condições de parada é atingida. Assim, a lógica de parada do worker não pode ser alcançada se
    está preso em uma chamada bloqueada.

Transporte Doctrine
~~~~~~~~~~~~~~~~~~~

O transporte Doctrine pode ser usado para armazenar mensagens em uma tabela no banco de dados.

.. code-block:: bash

    # .env
    MESSENGER_TRANSPORT_DSN=doctrine://default

O formato é ``doctrine://<connection_name>``, caso você tenha várias conexões
e deseja usar outra que não seja "default". O transporte criará automaticamente
uma tabela chamada ``messenger_messages`` (isso é configurável) quando o transporte é
usado pela primeira vez. Você pode desativar isso com a opção ``auto_setup`` e definir a tabela
manualmente, chamando o comando ``messenger:setup-transports``.

.. tip::

    Para evitar que ferramentas como Migrations do Doctrine tentem remover essa tabela pois
    não faz parte do seu esquema normal, você pode definir a opção ``schema_filter``:

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
                schema_filter: '~^(?!messenger_messages)~'

O transporte possui várias opções:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_high: "%env(MESSENGER_TRANSPORT_DSN)%?queue_name=high_priority"
                    async_normal:
                        dsn: "%env(MESSENGER_TRANSPORT_DSN)%"
                        options:
                            queue_name: normal_priority

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:transport name="async_priority_high" dsn="%env(MESSENGER_TRANSPORT_DSN)%?queue_name=high_priority"/>
                    <framework:transport name="async_priority_low" dsn="%env(MESSENGER_TRANSPORT_DSN)%">
                        <framework:option queue_name="normal_priority"/>
                    </framework:transport>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async_priority_high' => 'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%?queue_name=high_priority',
                    'async_priority_low' => [
                        'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                        'options' => [
                            'queue_name' => 'normal_priority'
                        ]
                    ],
                ],
            ],
        ]);

As opções definidas sob ``options`` têm precedência sobre as definidas no DSN.

==================  ===================================  ======================
     Opção          Descrição                            Default
==================  ===================================  ======================
table_name          Nome da tabela                       messenger_messages
queue_name          Nome da fila (uma coluna na tabela   default
                    , para usar uma tabela para
                    múltiplos transportes)
redeliver_timeout   Timeout antes de tentar uma mensagem 3600
                    que está na file mas no estado
                    "handling" (se um worker morreu
                    por alguma razão, isso vai ocorrer,
                    eventualmente, você deve tentar
                    novamente a mensagem) - em segundos.
auto_setup          Se a tabela deve ser criada automa-. true
                    ticamente durante envio/recebimento
==================  ===================================  ======================

Transporte Redis
~~~~~~~~~~~~~~~~

O transporte Redis usa `streams`_ para enfileirar mensagens.

.. code-block:: bash

    # .env
    MESSENGER_TRANSPORT_DSN=redis://localhost:6379/messages
    # Full DSN Example
    MESSENGER_TRANSPORT_DSN=redis://password@localhost:6379/messages/symfony/consumer?auto_setup=true&serializer=1&stream_max_entries=0&dbindex=0

Para usar o transporte Redis, você precisará da extensão Redis do PHP (>=4.3) e
um servidor Redis em execução (^5.0).

Várias opções podem ser configuradas através do DSN ou através da chave ``options``
sob o transporte em ``messenger.yaml``:

==================  =====================================  =========================
     Opção               Descrição                         Default
==================  =====================================  =========================
stream              O nome da stream do Redis              messages
group               O nome do grupo consumidor Redis       symfony
consumer            Nome do consumidor usado no Redis      consumer
auto_setup          Criar o grupo Redis automaticamente?   true
auth                A senha Redis 
serializer          Como serializar o payload final        ``Redis::SERIALIZER_PHP``
                    no Redis (a opção
                    ``Redis::OPT_SERIALIZER``)
stream_max_entries  O número máximo de entradas que        ``0`` (significa "no trimming")
                    a stream será cortada. Setar
                    para um número grande o sufiente
                    p/ evitar perder mensagens pendentes
==================  =====================================  =========================

Transporte In-Memory
~~~~~~~~~~~~~~~~~~~~

O transporte ``in-memory`` não entrega realmente as mensagens. Em vez disso,
mantém-as na memória durante a solicitação, o que pode ser útil para testes.
Por exemplo, se você tiver um transporte ``async_priority_normal``, poderá
sobrescrevê-lo no ambiente ``test`` para usar esse transporte:

.. code-block:: yaml

    # config/packages/test/messenger.yaml
    framework:
        messenger:
            transports:
                async_priority_normal: 'in-memory:///'

Então, durante o teste, as mensagens *não* serão entregues ao transporte real.
Melhor ainda: em um teste, você pode verificar se exatamente uma mensagem foi enviada
durante uma solicitação::

    // tests/DefaultControllerTest.php
    namespace App\Tests;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
    use Symfony\Component\Messenger\Transport\InMemoryTransport;

    class DefaultControllerTest extends WebTestCase
    {
        public function testSomething()
        {
            $client = static::createClient();
            // ...

            $this->assertSame(200, $client->getResponse()->getStatusCode());

            /* @var InMemoryTransport $transport */
            $transport = self::$container->get('messenger.transport.async_priority_normal');
            $this->assertCount(1, $transport->getSent());
        }
    }

.. note::

        Todos os transportes ``in-memory`` serão resetados automaticamente após cada teste **em**
        classes de teste estendendo
        :class:`Symfony\\Bundle\\FrameworkBundle\\Test\\KernelTestCase`
        ou :class:`Symfony\\Bundle\\FrameworkBundle\\Test\\WebTestCase`.

Serializando Mensagens
~~~~~~~~~~~~~~~~~~~~~~

Quando as mensagens são enviadas para (e recebidas de) um transporte, elas são serializadas
usando as funções nativas do PHP ``serialize()`` & ``unserialize()``. Você pode mudar
isso globalmente (ou para cada transporte) para um serviço que implementa
:class:`Symfony\\Component\\Messenger\\Transport\\Serialization\\SerializerInterface`:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                serializer:
                    default_serializer: messenger.transport.symfony_serializer
                    symfony_serializer:
                        format: json
                        context: { }

                transports:
                    async_priority_normal:
                        dsn: # ...
                        serializer: messenger.transport.symfony_serializer

O ``messenger.transport.symfony_serializer`` é um serviço interno que usa
o :doc:`componente Serializer </serializer>` e pode ser configurado de algumas maneiras.
Se você optar por usar o serializer do Symfony, poderá controlar o contexto
caso a caso, através da :class:`Symfony\\Component\\Messenger\\Stamp\\SerializerStamp`
(consulte `Envelopes & Stamps`_).

Personalizando Manipuladores
----------------------------

.. _messenger-handler-config:

Configurando Manipuladores Manualmente
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O Symfony irá normalmente :ref:`encontrar e registrar seu manipulador automaticamente <messenger-handler>`.
Mas, você também pode configurar um manipulador manualmente - e passar a ele algumas configurações extras -
ao taguear o serviço do manipulador com ``messenger.message_handler``

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\MessageHandler\SmsNotificationHandler:
                tags: [messenger.message_handler]

                # or configure with options
                tags:
                    -
                        name: messenger.message_handler
                        # only needed if can't be guessed by type-hint
                        handles: App\Message\SmsNotification

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\MessageHandler\SmsNotificationHandler">
                     <!-- handles is only needed if it can't be guessed by type-hint -->
                     <tag name="messenger.message_handler"
                          handles="App\Message\SmsNotification"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Message\SmsNotification;
        use App\MessageHandler\SmsNotificationHandler;

        $container->register(SmsNotificationHandler::class)
            ->addTag('messenger.message_handler', [
                // only needed if can't be guessed by type-hint
                'handles' => SmsNotification::class,
            ]);


As opções possíveis para configurar com tags são:

* ``bus``
* ``from_transport``
* ``handles``
* ``method``
* ``priority``

Subscriber do Manipulador e Opções
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Uma classe de manipulador pode manipular várias mensagens ou configurar-se implementando
:class:`Symfony\\Component\\Messenger\\Handler\\MessageSubscriberInterface`::

    // src/MessageHandler/SmsNotificationHandler.php
    namespace App\MessageHandler;

    use App\Message\OtherSmsNotification;
    use App\Message\SmsNotification;
    use Symfony\Component\Messenger\Handler\MessageSubscriberInterface;

    class SmsNotificationHandler implements MessageSubscriberInterface
    {
        public function __invoke(SmsNotification $message)
        {
            // ...
        }

        public function handleOtherSmsNotification(OtherSmsNotification $message)
        {
            // ...
        }

        public static function getHandledMessages(): iterable
        {
            // handle this message on __invoke
            yield SmsNotification::class;

            // also handle this message on handleOtherSmsNotification
            yield OtherSmsNotification::class => [
                'method' => 'handleOtherSmsNotification',
                //'priority' => 0,
                //'bus' => 'messenger.bus.default',
            ];
        }
    }

Vinculando Manipuladores a Diferentes Transportes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cada mensagem pode ter vários manipuladores e, quando uma mensagem é consumida
*todos* seus manipuladores são chamados. Mas, você também pode configurar um manipulador para
ser chamado apenas quando for recebido de um transporte *específico*. Isso permite que você
tenha uma única mensagem onde cada manipulador é chamado por um "worker" diferente
que está consumindo um transporte diferente.

Suponha que você tenha uma mensagem ``UploadedImage`` com dois manipuladores:

* ``ThumbnailUploadedImageHandler``: você deseja que isso seja manipulado por
  um transporte chamado ``image_transport``

* ``NotifyAboutNewUploadedImageHandler``: você deseja que isso seja manipulado
  por um transporte chamado ``async_priority_normal``

Para fazer isso, adicione a opção ``from_transport`` a cada manipulador. Por exemplo::

    // src/MessageHandler/ThumbnailUploadedImageHandler.php
    namespace App\MessageHandler;

    use App\Message\UploadedImage;
    use Symfony\Component\Messenger\Handler\MessageSubscriberInterface;

    class ThumbnailUploadedImageHandler implements MessageSubscriberInterface
    {
        public function __invoke(UploadedImage $uploadedImage)
        {
            // do some thumbnailing
        }

        public static function getHandledMessages(): iterable
        {
            yield UploadedImage::class => [
                'from_transport' => 'image_transport',
            ];
        }
    }

E da mesma forma::

    // src/MessageHandler/NotifyAboutNewUploadedImageHandler.php
    // ...

    class NotifyAboutNewUploadedImageHandler implements MessageSubscriberInterface
    {
        // ...

        public static function getHandledMessages(): iterable
        {
            yield UploadedImage::class => [
                'from_transport' => 'async_priority_normal',
            ];
        }
    }

Em seguida, certifique-se de "rotear" sua mensagem para *ambos* os transportes:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_normal: # ...
                    image_transport: # ...

                routing:
                    # ...
                    'App\Message\UploadedImage': [image_transport, async_priority_normal]

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:transport name="async_priority_normal" dsn="..."/>
                    <framework:transport name="image_transport" dsn="..."/>

                    <framework:routing message-class="App\Message\UploadedImage">
                        <framework:sender service="image_transport"/>
                        <framework:sender service="async_priority_normal"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async_priority_normal' => '...',
                    'image_transport' => '...',
                ],
                'routing' => [
                    'App\Message\UploadedImage' => ['image_transport', 'async_priority_normal']
                ]
            ],
        ]);

É isso! Agora você pode consumir cada transporte:

.. code-block:: terminal

    # will only call ThumbnailUploadedImageHandler when handling the message
    $ php bin/console messenger:consume image_transport -vv

    $ php bin/console messenger:consume async_priority_normal -vv

.. caution::

    Se um manipulador *não* tiver a configuração ``from_transport``, ele será executado
    em *todo* transporte do qual a mensagem é recebida.

Estendendo o Messenger
----------------------

Envelopes & Stamps
~~~~~~~~~~~~~~~~~~

Uma mensagem pode ser qualquer objeto PHP. Às vezes, pode ser necessário configurar algo
extra sobre a mensagem - como a maneira como ela deve ser tratada dentro do Amqp ou adicionando
um atraso antes da mensagem ser manipulada. Você pode fazer isso adicionando um "stamp"
em sua mensagem::

    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\MessageBusInterface;
    use Symfony\Component\Messenger\Stamp\DelayStamp;

    public function index(MessageBusInterface $bus)
    {
        $bus->dispatch(new SmsNotification('...'), [
            // wait 5 seconds before processing
            new DelayStamp(5000)
        ]);

        // or explicitly create an Envelope
        $bus->dispatch(new Envelope(new SmsNotification('...'), [
            new DelayStamp(5000)
        ]));

        // ...
    }

Internamente, cada mensagem é envolvida em um ``Envelope``, que contém a mensagem
e stamps. Você pode criar isso manualmente ou permitir que o barramento de mensagens faça isso.
Existem uma variedade de stamps para diferentes fins e eles são usados internamente para
rastrear informações sobre uma mensagem - como o barramento de mensagens que está lidando com ela
ou se ela estiver sendo tentada novamente após falha.

Middleware
~~~~~~~~~~

O que acontece quando você envia uma mensagem para um barramento de mensagens depende de sua
coleção de middleware e sua respectiva ordem. Por padrão, o middleware configurado
para cada barramento é assim:

#. ``add_bus_name_stamp_middleware`` - adiciona um stamp para registrar qual barramento
   esta mensagem foi enviada;

#. ``dispatch_after_current_bus``- veja :doc:`/messenger/message-recorder`;

#. ``failed_message_processing_middleware`` - processa as mensagens que estão sendo
   tentadas novamente através do :ref:`transporte de falha <messenger-failure-transport>` para fazer
   com que funcionem adequadamente como se estivessem sendo recebidas do transporte original;

#. Sua própria coleção de middleware_;

#. ``send_message`` - se o roteamento estiver configurado para o transporte, isso enviará
   mensagens para esse transporte e interromperá a cadeia de middleware;

#. ``handle_message`` - chama o s) manipulador(es) de mensagem para a mensagem fornecida.

.. note::

    Esses nomes de middleware são na verdade nomes de atalhos. Os Ids de serviço reais
    são prefixados com ``messenger.middleware.``.

Você pode adicionar seu próprio middleware a esta lista ou desativar completamente o middleware
padrão e *somente* incluem os seus:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    messenger.bus.default:
                        middleware:
                            # service ids that implement Symfony\Component\Messenger\Middleware
                            - 'App\Middleware\MyMiddleware'
                            - 'App\Middleware\AnotherMiddleware'

                        #default_middleware: false

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:middleware id="App\Middleware\MyMiddleware"/>
                    <framework:middleware id="App\Middleware\AnotherMiddleware"/>
                    <framework:bus name="messenger.bus.default" default-middleware="false"/>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'buses' => [
                    'messenger.bus.default' => [
                        'middleware' => [
                            'App\Middleware\MyMiddleware',
                            'App\Middleware\AnotherMiddleware',
                        ],
                        'default_middleware' => false,
                    ],
                ],
            ],
        ]);


.. note::

    Se um serviço de middleware for abstrato, uma instância diferente do serviço será
    criada por barramento.

Middleware para o Doctrine
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 1.11

    O middleware Doctrine a seguir foi introduzido no DoctrineBundle 1.11.

Se você usa o Doctrine em sua aplicação, existem vários middlewares opcionais que você
pode querer usar:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    command_bus:
                        middleware:
                            # each time a message is handled, the Doctrine connection
                            # is "pinged" and reconnected if it's closed. Useful
                            # if your workers run for a long time and the database
                            # connection is sometimes lost
                            - doctrine_ping_connection

                            # After handling, the Doctrine connection is closed,
                            # which can free up database connections in a worker,
                            # instead of keeping them open forever
                            - doctrine_close_connection

                            # wraps all handlers in a single Doctrine transaction
                            # handlers do not need to call flush() and an error
                            # in any handler will cause a rollback
                            - doctrine_transaction

                            # or pass a different entity manager to any
                            #- doctrine_transaction: ['custom']

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:bus name="command_bus">
                        <framework:middleware id="doctrine_transaction"/>
                        <framework:middleware id="doctrine_ping_connection"/>
                        <framework:middleware id="doctrine_close_connection"/>

                        <!-- or pass a different entity manager to any -->
                        <!--
                        <framework:middleware id="doctrine_transaction">
                            <framework:argument>custom</framework:argument>
                        </framework:middleware>
                        -->
                    </framework:bus>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'buses' => [
                    'command_bus' => [
                        'middleware' => [
                            'doctrine_transaction',
                            'doctrine_ping_connection',
                            'doctrine_close_connection',
                            // Using another entity manager
                            ['id' => 'doctrine_transaction', 'arguments' => ['custom']],
                        ],
                    ],
                ],
            ],
        ]);

Eventos do Messenger
~~~~~~~~~~~~~~~~~~~~

Além do middleware, o Messenger também dispara vários eventos. Você pode
:doc:`criar um listener de evento </event_dispatcher>` para conectar-se a várias partes
do processo. Para cada um, a classe do evento é o nome do evento:

* :class:`Symfony\\Component\\Messenger\\Event\\WorkerStartedEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerMessageReceivedEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\SendMessageToTransportsEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerMessageFailedEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerMessageHandledEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerRunningEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerStoppedEvent`

Múltiplos Barramentos, Baramentos de Comando e Evento
-----------------------------------------------------

O Messenger fornece um único serviço de barramento de mensagem por padrão. Mas você pode configurar
quantos você quiser, criando barramentos de "comando", "query" ou "evento" e controlando
seus middlewares. Veja :doc:`/messenger/multiple_buses`.

Saiba mais
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /messenger/*

.. _`transporte Enqueue`: https://github.com/php-enqueue/messenger-adapter
.. _`streams`: https://redis.io/topics/streams-intro
.. _`documentação do Supervisor`: http://supervisord.org/
