.. index::
   single: Log

Como fazer log de Mensagens em Arquivos Diferentes
==================================================

.. versionadded:: 2.1
    A capacidade de especificar os canais para um manipulador específico foi adicionada ao
    MonologBundle para o Symfony 2.1.

A Edição Standard do Symfony contém vários canais para log: ``doctrine``,
``event``, ``security`` e ``request``. Cada canal corresponde a um serviço logger
(``monolog.logger.XXX``) no container e, é injetado no
serviço em questão. A finalidade dos canais é de serem capazes de organizar diferentes
tipos de mensagens de log.

Por padrão, o Symfony2 realiza o log de todas as mensagens em um único arquivo (independentemente do
canal).

Mudando um canal para um Handler diferente
------------------------------------------

Agora, suponha que você queira registrar o canal ``doctrine`` para um arquivo diferente.

Para isso, basta criar um novo manipulador (handler) e configurá-lo como segue:

.. configuration-block::

    .. code-block:: yaml

        monolog:
            handlers:
                main:
                    type: stream
                    path: /var/log/symfony.log
                    channels: !doctrine
                doctrine:
                    type: stream
                    path: /var/log/doctrine.log
                    channels: doctrine

    .. code-block:: xml

        <monolog:config>
            <monolog:handlers>
                <monolog:handler name="main" type="stream" path="/var/log/symfony.log">
                    <monolog:channels>
                        <type>exclusive</type>
                        <channel>doctrine</channel>
                    </monolog:channels>
                </monolog:handler>

                <monolog:handler name="doctrine" type="stream" path="/var/log/doctrine.log" />
                    <monolog:channels>
                        <type>inclusive</type>
                        <channel>doctrine</channel>
                    </monolog:channels>
                </monolog:handler>
            </monolog:handlers>
        </monolog:config>

Especificação Yaml
------------------

Você pode especificar a configuração de várias formas:

.. code-block:: yaml

    channels: ~    # Include all the channels

    channels: foo  # Include only channel "foo"
    channels: !foo # Include all channels, except "foo"

    channels: [foo, bar]   # Include only channels "foo" and "bar"
    channels: [!foo, !bar] # Include all channels, except "foo" and "bar"

    channels:
        type:     inclusive # Include only those listed below
        elements: [ foo, bar ]
    channels:
        type:     exclusive # Include all, except those listed below
        elements: [ foo, bar ]

Criando o seu próprio Canal
---------------------------

Você pode mudar o canal que monolog faz o log para um serviço de cada vez. Isto é feito
adicionando a tag ``monolog.logger`` ao seu serviço e especificando qual canal
o serviço deve fazer o log. Ao fazer isso, o logger que é injetado
naquele serviço é pré-configurado para usar o canal que você especificou.

Para mais informações - incluindo um exemplo completo - leia ":ref:`dic_tags-monolog`"
na seção de referência das Tags de Injeção de Dependência.

Aprenda mais no Cookbook
------------------------

* :doc:`/cookbook/logging/monolog`
