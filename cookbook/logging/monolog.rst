.. index::
   single: Log

Como usar o Monolog para escrever Logs
======================================

O Monolog_ é uma biblioteca de log para o PHP 5.3 usada pelo Symfony2. É
inspirado pela biblioteca LogBook do Python.

Utilização
----------

Para fazer o log de uma mensagem, basta obter o serviço logger a partir do container em
seu controlador::

    public function indexAction()
    {
        $logger = $this->get('logger');
        $logger->info('I just got the logger');
        $logger->err('An error occurred');

        // ...
    }

O serviço ``logger`` possui métodos diferentes para os níveis de log.
Para mais detalhes sobre os métodos que estão disponíveis, veja
:class:`Symfony\\Component\\HttpKernel\\Log\\LoggerInterface`.

Manipuladores (handlers) e Canais (channels): Escrevendo logs em locais diferentes
----------------------------------------------------------------------------------

No Monolog, cada logger define um canal de log, que organiza as suas mensagens de log
em diferentes "categorias". Então, cada canal tem uma pilha de manipuladores
para escrever os logs (os manipuladores podem ser compartilhados).

.. tip::

    Ao injetar o logger em um serviço você pode
    :ref:`usar um canal personalizado<dic_tags-monolog>` para controlar que "canal"
    o logger irá realizar o log.

O manipulador básico é o ``StreamHandler``, que grava logs em um stream
(por padrão em ``app/logs/prod.log`` no ambiente prod e
``app/logs/dev.log`` no ambiente dev).

O Monolog também vem com um poderoso manipulador integrado para realizar log
no ambiente prod: ``FingersCrossedHandler``. Ele permite que você armazene as
mensagens em um buffer e registre o log somente se a mensagem alcança o
nível de ação (ERROR na configuração fornecida na edição 
standard), enviando as mensagens para outro manipulador.

Usando vários manipuladores
~~~~~~~~~~~~~~~~~~~~~~~~~~~

O logger usa uma pilha de manipuladores que são chamados sucessivamente. Isto
permite registrar facilmente as mensagens de várias maneiras.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        monolog:
            handlers:
                applog:
                    type: stream
                    path: /var/log/symfony.log
                    level: error
                main:
                    type: fingers_crossed
                    action_level: warning
                    handler: file
                file:
                    type: stream
                    level: debug
                syslog:
                    type: syslog
                    level: error

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/monolog http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler
                    name="applog"
                    type="stream"
                    path="/var/log/symfony.log"
                    level="error"
                />
                <monolog:handler
                    name="main"
                    type="fingers_crossed"
                    action-level="warning"
                    handler="file"
                />
                <monolog:handler
                    name="file"
                    type="stream"
                    level="debug"
                />
                <monolog:handler
                    name="syslog"
                    type="syslog"
                    level="error"
                />
            </monolog:config>
        </container>

    .. code-block:: php
        
        // app/config/config.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'applog' => array(
                    'type'  => 'stream',
                    'path'  => '/var/log/symfony.log',
                    'level' => 'error',
                ),    
                'main' => array(
                    'type'         => 'fingers_crossed',
                    'action_level' => 'warning',
                    'handler'      => 'file',
                ),    
                'file' => array(
                    'type'  => 'stream',
                    'level' => 'debug',
                ),   
                'syslog' => array(
                    'type'  => 'syslog',
                    'level' => 'error',
                ),    
            ),
        ));

A configuração acima define uma pilha de manipuladores que será chamada
na ordem em que eles são definidos.

.. tip::

    O manipulador chamado "file" não será incluído na própria pilha, pois,
    ele é usado como um manipulador aninhado do manipulador ``fingers_crossed``.

.. note::

    Se você quiser alterar a configuração do MonologBundle em outro arquivo de
    configuração, você precisa redefinir toda a pilha. Não pode ser feito o merge
    porque a ordem é importante e um merge não permite controlar a
    ordem.

Alterando o formatador
~~~~~~~~~~~~~~~~~~~~~~

O manipulador usa um ``Formatter`` para formatar o registro antes de efetuar
o log. Todos os manipuladores do Monolog usam uma instância do
``Monolog\Formatter\LineFormatter`` por padrão, mas você pode substituí-lo
facilmente. O formatador deve implementar
``Monolog\Formatter\FormatterInterface``.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            my_formatter:
                class: Monolog\Formatter\JsonFormatter
        monolog:
            handlers:
                file:
                    type: stream
                    level: debug
                    formatter: my_formatter

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/monolog http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="my_formatter" class="Monolog\Formatter\JsonFormatter" />
            </services>

            <monolog:config>
                <monolog:handler
                    name="file"
                    type="stream"
                    level="debug"
                    formatter="my_formatter"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container
            ->register('my_formatter', 'Monolog\Formatter\JsonFormatter');

        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'file' => array(
                    'type'      => 'stream',
                    'level'     => 'debug',
                    'formatter' => 'my_formatter',
                ),
            ),
        ));

Adicionando alguns dados extras nas mensagens de log
----------------------------------------------------

O Monolog permite processar o registro antes de fazer o log para adicionar alguns
dados extras. Um processador pode ser aplicado para toda a pilha do manipulador
ou somente para um manipulador específico.

Um processador é simplesmente um callable recebendo o registro como seu primeiro argumento.

Os processadores são configurados usando a tag DIC ``monolog.processor``. Veja a
:ref:`referencia sobre ela<dic_tags-monolog-processor>`.

Adicionando uma Sessão/Token de Pedido
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Às vezes, é difícil dizer que entradas no log pertencem a qual sessão
e/ou pedido. O exemplo a seguir irá adicionar um token exclusivo para cada pedido
utilizando um processador.

.. code-block:: php

    namespace Acme\MyBundle;

    use Symfony\Component\HttpFoundation\Session\Session;

    class SessionRequestProcessor
    {
        private $session;
        private $token;

        public function __construct(Session $session)
        {
            $this->session = $session;
        }

        public function processRecord(array $record)
        {
            if (null === $this->token) {
                try {
                    $this->token = substr($this->session->getId(), 0, 8);
                } catch (\RuntimeException $e) {
                    $this->token = '????????';
                }
                $this->token .= '-' . substr(uniqid(), -8);
            }
            $record['extra']['token'] = $this->token;

            return $record;
        }
    }

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            monolog.formatter.session_request:
                class: Monolog\Formatter\LineFormatter
                arguments:
                    - "[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%%\n"

            monolog.processor.session_request:
                class: Acme\MyBundle\SessionRequestProcessor
                arguments:  ["@session"]
                tags:
                    - { name: monolog.processor, method: processRecord }

        monolog:
            handlers:
                main:
                    type: stream
                    path: "%kernel.logs_dir%/%kernel.environment%.log"
                    level: debug
                    formatter: monolog.formatter.session_request

    .. code-block:: xml

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/monolog http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="monolog.formatter.session_request" class="Monolog\Formatter\LineFormatter">
                    <argument>[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%%&#xA;</argument>
                </service>

                <service id="monolog.processor.session_request" class="Acme\MyBundle\SessionRequestProcessor">
                    <argument type="service" id="session" />
                    <tag name="monolog.processor" method="processRecord" />
                </service>
            </services>

            <monolog:config>
                <monolog:handler
                    name="main"
                    type="stream"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                    formatter="monolog.formatter.session_request"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container
            ->register('monolog.formatter.session_request', 'Monolog\Formatter\LineFormatter')
            ->addArgument('[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%%\n');

        $container
            ->register('monolog.processor.session_request', 'Acme\MyBundle\SessionRequestProcessor')
            ->addArgument(new Reference('session'))
            ->addTag('monolog.processor', array('method' => 'processRecord'));

        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'main' => array(
                    'type'      => 'stream',
                    'path'      => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level'     => 'debug',
                    'formatter' => 'monolog.formatter.session_request',
                ),
            ),
        ));

.. note::

    Se você usa vários manipuladores, você também pode registrar o processador no
    nível do manipulador, em vez de globalmente.

.. _Monolog: https://github.com/Seldaek/monolog
