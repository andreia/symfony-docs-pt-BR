.. index::
   pair: Monolog; Configuration Reference

Referência de Configuração
==========================

.. configuration-block::

    .. code-block:: yaml

        monolog:
            handlers:
                syslog:
                    type: stream
                    path: /var/log/symfony.log
                    level: error
                    bubble: false
                    formatter: my_formatter
                    processors:
                        - some_callable
                main:
                    type: fingerscrossed
                    action_level: warning
                    buffer_size: 30
                    handler: custom
                custom:
                    type: service
                    id: my_handler
            processors:
                - @my_processor

    .. code-block:: xml

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/monolog http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler
                    name="syslog"
                    type="stream"
                    path="/var/log/symfony.log"
                    level="error"
                    bubble="false"
                    formatter="my_formatter"
                >
                    <monolog:processor callback="some_callable" />
                </monolog:handler />
                <monolog:handler
                    name="main"
                    type="fingerscrossed"
                    action-level="warning"
                    handler="custom"
                />
                <monolog:handler
                    name="custom"
                    type="service"
                    id="my_handler"
                />
                <monolog:processor callback="@my_processor" />
            </monolog:config>
        </container>

.. note::
  
    Quando o profiler é ativado, um handler é adicionado para armazenar as 
    mensagens dos logs' no profiler. O Profiler usa o nome "debug", então, ele
    é reservado e não pode ser utilizado na configuração.