.. index::
   single: Logging; Mensagens de Console

Como Configurar o Monolog para Exibir Mensagens de Console
==========================================================

É possível usar o console para imprimir mensagens para determinados
:ref:`níveis de verbosidade <verbosity-levels>` usando a
instância :class:`Symfony\\Component\\Console\\Output\\OutputInterface` que
é passada quando um comando é executado.

.. seealso::
    Alternativamente, você pode usar o
    :doc:`logger PSR-3 standalone</components/console/logger>` fornecido com
    o componente console.

Quando muito log aconteceu, é complicado imprimir informações
dependendo das definições de verbosidade (``-v``, ``-vv``, ``-vvv``) porque as
chamadas devem ser envoltas em condições. O código rapidamente torna-se verboso ou poluído.
Por exemplo::

    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        if ($output->getVerbosity() >= OutputInterface::VERBOSITY_DEBUG) {
            $output->writeln('Some info');
        }

        if ($output->getVerbosity() >= OutputInterface::VERBOSITY_VERBOSE) {
            $output->writeln('Some more info');
        }
    }

Em vez de utilizar esses métodos semânticos para testar cada um dos níveis de verbosidade
, o `` MonologBridge`_ fornece um `ConsoleHandler`_ que ouve
eventos do console e escreve mensagens de log para a saída do console, dependendo do
nível de atual de log e da verbosidade do console.

O exemplo acima poderia então ser reescrito assim::

    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        // assuming the Command extends ContainerAwareCommand...
        $logger = $this->getContainer()->get('logger');
        $logger->debug('Some info');

        $logger->notice('Some more info');
    }

Dependendo do nível de verbosidade que o comando é executado e da configuração do
usuário (ver abaixo), essas mensagens podem ou não ser exibidas no
console. Se elas são exibidas, elas são marcadas com data/hora e coloridas de forma adequada.
Além disso, os logs de erro são escritos na saída de erro (php://stderr).
Não há mais necessidade de lidar condicionalmente com as definições de verbosidade.

O handler de console do Monolog está ativado na configuração Monolog. Esse é
o padrão também na Edição Standard do Symfony 2.4.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        monolog:
            handlers:
                console:
                    type: console

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:monolog="http://symfony.com/schema/dic/monolog">

            <monolog:config>
                <monolog:handler name="console" type="console" />
            </monolog:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'console' => array(
                   'type' => 'console',
                ),
            ),
        ));

Com a opção ``verbosity_levels`` você pode adaptar o mapeamento entre
verbosidade e nível de log. No exemplo apresentado ele também irá mostrar avisos no
modo normal de verbosidade (em vez de apenas advertências). Além disso, ele só irá
usar mensagens de log do canal personalizado ``my_channel`` e ele altera o
estilo de exibição através de um formatador personalizado (veja a
:doc:`referência do MonologBundle </reference/configuration/monolog>` para mais
informações):

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        monolog:
            handlers:
                console:
                    type:   console
                    verbosity_levels:
                        VERBOSITY_NORMAL: NOTICE
                    channels: my_channel
                    formatter: my_formatter

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:monolog="http://symfony.com/schema/dic/monolog">

            <monolog:config>
                <monolog:handler name="console" type="console" formatter="my_formatter">
                    <monolog:verbosity-level verbosity-normal="NOTICE" />
                    <monolog:channel>my_channel</monolog:channel>
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'console' => array(
                    'type' => 'console',
                    'verbosity_levels' => array(
                        'VERBOSITY_NORMAL' => 'NOTICE',
                    ),
                    'channels' => 'my_channel',
                    'formatter' => 'my_formatter',
                ),
            ),
        ));

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            my_formatter:
                class: Symfony\Bridge\Monolog\Formatter\ConsoleFormatter
                arguments:
                    - "[%%datetime%%] %%start_tag%%%%message%%%%end_tag%% (%%level_name%%) %%context%% %%extra%%\n"

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                   xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

             <services>
                <service id="my_formatter" class="Symfony\Bridge\Monolog\Formatter\ConsoleFormatter">
                    <argument>[%%datetime%%] %%start_tag%%%%message%%%%end_tag%% (%%level_name%%) %%context%% %%extra%%\n</argument>
                </service>
             </services>

        </container>

    .. code-block:: php

        // app/config/services.php
        $container
            ->register('my_formatter', 'Symfony\Bridge\Monolog\Formatter\ConsoleFormatter')
            ->addArgument('[%%datetime%%] %%start_tag%%%%message%%%%end_tag%% (%%level_name%%) %%context%% %%extra%%\n')
        ;

.. _ConsoleHandler: https://github.com/symfony/MonologBridge/blob/master/Handler/ConsoleHandler.php
.. _MonologBridge: https://github.com/symfony/MonologBridge
