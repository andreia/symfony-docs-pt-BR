.. index::
   single: Log; Enviar erros por e-mail

Como configurar o Monolog para enviar erros por e-mail
======================================================

O Monolog_ pode ser configurado para enviar um e-mail quando ocorrer um erro
na aplicação. A configuração para isto requer alguns manipuladores aninhados
a fim de evitar o recebimento de muitos e-mails. Esta configuração parece
complicada no início, mas cada manipulador é bastante simples quando
é separado.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_prod.yml
        monolog:
            handlers:
                mail:
                    type:         fingers_crossed
                    action_level: critical
                    handler:      buffered
                buffered:
                    type:    buffer
                    handler: swift
                swift:
                    type:       swift_mailer
                    from_email: error@example.com
                    to_email:   error@example.com
                    subject:    An Error Occurred!
                    level:      debug

    .. code-block:: xml

        <!-- app/config/config_prod.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/monolog http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler
                    name="mail"
                    type="fingers_crossed"
                    action-level="critical"
                    handler="buffered"
                />
                <monolog:handler
                    name="buffered"
                    type="buffer"
                    handler="swift"
                />
                <monolog:handler
                    name="swift"
                    from-email="error@example.com"
                    to-email="error@example.com"
                    subject="An Error Occurred!"
                    level="debug"
                />
            </monolog:config>
        </container>
        
    .. code-block:: php
            
        // app/config/config_prod.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'mail' => array(
                    'type'         => 'fingers_crossed',
                    'action_level' => 'critical',
                    'handler'      => 'buffered',
                ),    
                'buffered' => array(
                    'type'    => 'buffer',
                    'handler' => 'swift',
                ),    
                'swift' => array(
                    'type'       => 'swift_mailer',
                    'from_email' => 'error@example.com',
                    'to_email'   => 'error@example.com',
                    'subject'    => 'An Error Occurred!',
                    'level'      => 'debug',
                ),    
            ),
        ));

O manipulador ``mail`` é um manipulador ``fingers_crossed``, significando que
ele é acionado apenas quando o nível de ação, neste caso, ``critical`` é alcançado.
Em seguida, ele faz log de tudo, incluindo mensagens abaixo do nível de ação. O
nível ``critical`` só é acionado para erros HTTP de código 5xx. A definição ``handler``
significa que a saída é, então, transferida para o manipulador ``buffered``.

.. tip::

    Se você deseja que tanto erros de nível 400 quanto de nível 500 acionem um e-mail,
    defina o ``action_level`` para ``error`` ao invés de ``critical``.

O manipulador ``buffered`` simplesmente mantém todas as mensagens para um pedido e
em seguida, passa elas para o manipulador aninhado de uma só vez. Se você não usar este
manipulador, então cada mensagem será enviada separadamente. Em seguida, é passado
para o manipulador ``swift``. Este é o manipulador que efetivamente trata
de enviar o erro para o e-mail. As configurações para isso são simples, os
endereços de (to), para (from) e o assunto (subject).

Você pode combinar esses manipuladores com outros manipuladores de modo que os erros
ainda serão registrados no servidor, bem como enviados os e-mails:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_prod.yml
        monolog:
            handlers:
                main:
                    type:         fingers_crossed
                    action_level: critical
                    handler:      grouped
                grouped:
                    type:    group
                    members: [streamed, buffered]
                streamed:
                    type:  stream
                    path:  "%kernel.logs_dir%/%kernel.environment%.log"
                    level: debug
                buffered:
                    type:    buffer
                    handler: swift
                swift:
                    type:       swift_mailer
                    from_email: error@example.com
                    to_email:   error@example.com
                    subject:    An Error Occurred!
                    level:      debug

    .. code-block:: xml

        <!-- app/config/config_prod.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/monolog http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler
                    name="main"
                    type="fingers_crossed"
                    action_level="critical"
                    handler="grouped"
                />                
                <monolog:handler
                    name="grouped"
                    type="group"
                >
                    <member type="stream"/>
                    <member type="buffered"/>
                </monolog:handler>
                <monolog:handler
                    name="stream"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                />
                <monolog:handler
                    name="buffered"
                    type="buffer"
                    handler="swift"
                />
                <monolog:handler
                    name="swift"
                    from-email="error@example.com"
                    to-email="error@example.com"
                    subject="An Error Occurred!"
                    level="debug"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // app/config/config_prod.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'main' => array(
                    'type'         => 'fingers_crossed',
                    'action_level' => 'critical',
                    'handler'      => 'grouped',
                ),    
                'grouped' => array(
                    'type'    => 'group',
                    'members' => array('streamed', 'buffered'),
                ),    
                'streamed'  => array(
                    'type'  => 'stream',
                    'path'  => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level' => 'debug',
                ),    
                'buffered'    => array(
                    'type'    => 'buffer',
                    'handler' => 'swift',
                ),    
                'swift' => array(
                    'type'       => 'swift_mailer',
                    'from_email' => 'error@example.com',
                    'to_email'   => 'error@example.com',
                    'subject'    => 'An Error Occurred!',
                    'level'      => 'debug',
                ),    
            ),
        ));

Isto usa o manipulador ``group`` para enviar as mensagens para os dois membros
do grupo, os manipuladores ``buffered`` e ``stream``. As mensagens serão
agora gravadas no arquivo de log e enviadas por e-mail.

.. _Monolog: https://github.com/Seldaek/monolog
