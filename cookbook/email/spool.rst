Como fazer Spool de E-mail
==========================

Quando você estiver usando o ``SwiftmailerBundle`` para enviar um email de uma aplicação Symfony2,
ele irá, por padrão, enviar o e-mail imediatamente. Você pode, entretanto,
desejar evitar um impacto no desempenho da comunicação entre o ``Swiftmailer``
e o transporte do e-mail, o que poderia fazer com que o usuário tenha que aguardar 
a próxima página carregar, enquanto está enviando o e-mail. Isto pode ser evitado escolhendo
pelo "spool" dos e-mails em vez de enviá-los diretamente. Isto significa que o ``Swiftmailer`` 
não tentará enviar o email, mas, ao invés, salvará a mensagem em algum lugar, 
como um arquivo. Outro processo poderá então ler a partir do spool e cuidar
de enviar os e-mails no spool. Atualmente, apenas o spool para arquivo é suportado
pelo ``Swiftmailer``.

Para utilizar o spool, use a seguinte configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            # ...
            spool:
                type: file
                path: /path/to/spool

    .. code-block:: xml

        <!-- app/config/config.xml -->

        <!--
        xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
        http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config>
             <swiftmailer:spool
                 type="file"
                 path="/path/to/spool" />
        </swiftmailer:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
             // ...
            'spool' => array(
                'type' => 'file',
                'path' => '/path/to/spool',
            )
        ));

.. tip::

    Se você deseja armazenar o spool em algum lugar no diretório do seu projeto,
    lembre-se que você pode usar o parâmetro `%kernel.root_dir%` para referenciar
    o raiz do seu projeto:

    .. code-block:: yaml

        path: %kernel.root_dir%/spool

Agora, quando a sua aplicação enviar um e-mail, ele não será realmente enviado, ao invés, 
será adicionado ao spool. O envio de mensagens do spool é feito separadamente.
Existe um comando do console para enviar as mensagens que encontram-se no spool:

.. code-block:: bash

    php app/console swiftmailer:spool:send

Ele tem uma opção para limitar o número de mensagens a serem enviadas:

.. code-block:: bash

    php app/console swiftmailer:spool:send --message-limit=10

Você também pode definir o limite de tempo em segundos:

.. code-block:: bash

    php app/console swiftmailer:spool:send --time-limit=10

Claro que, na realidade, você não vai querer executar ele manualmente. Em vez disso, o
comando do console deve ser disparado por um cron job ou tarefa agendada e executar
em um intervalo regular.
