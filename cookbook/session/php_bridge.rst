.. index::
   single: Sessões

Ponte para uma Aplicação legada com Sessões Symfony
===================================================

.. versionadded:: 2.3
    A capacidade de integrar com uma sessão legada PHP foi introduzida no Symfony 2.3.

Se você está integrando o Framework Symfony full-stack em uma aplicação legada
que inicia a sessão com ``session_start()``, você ainda pode ser capaz de
usar o gerenciamento de sessão do Symfony usando uma ponte com sessão PHP.

Se a aplicação tiver definido seu próprio manipulador de armazenamento PHP, você pode
especificar null para ``handler_id``:

.. configuration-block::

    .. code-block:: yaml

        framework:
            session:
                storage_id: session.storage.php_bridge
                handler_id: ~

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony">

            <framework:config>
                <framework:session storage-id="session.storage.php_bridge"
                    handler-id="null"
                />
            </framework:config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('framework', array(
            'session' => array(
                'storage_id' => 'session.storage.php_bridge',
                'handler_id' => null,
        ));

Caso contrário, se o problema é simplesmente que você não pode evitar que a aplicação
inicie a sessão com ``session_start()``, você ainda pode fazer uso de um manipulador de
armazenamento de sessão com base no Symfony especificando o manipulador de armazenamento
como no exemplo a seguir:

.. configuration-block::

    .. code-block:: yaml

        framework:
            session:
                storage_id: session.storage.php_bridge
                handler_id: session.handler.native_file

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony">

            <framework:config>
                <framework:session storage-id="session.storage.php_bridge"
                    handler-id="session.storage.native_file"
                />
            </framework:config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('framework', array(
            'session' => array(
                'storage_id' => 'session.storage.php_bridge',
                'handler_id' => 'session.storage.native_file',
        ));

.. note::

    Se a aplicação legada requer seu próprio manipulador de armazenamento de sessão, não
    sobrescreva ele. Em vez disso defina ``handler_id: ~``. Note que o manipulador de armazenamento
    não pode ser alterado uma vez que a sessão foi iniciada. Se a aplicação
    inicia a sessão antes do Symfony ser inicializado, o manipulador de armazenamento já
    terá sido definido. Nesse caso, será necessário ``handler_id: ~``.
    Apenas sobrescreva o manipulador de armazenamento se você tem certeza que a aplicação legada
    pode usar o manipulador de armazenamento do Symfony sem efeitos colaterais e que a sessão
    não foi iniciada antes do Symfony ser inicializado.

Para mais detalhes, consulte :doc:`/components/http_foundation/session_php_bridge`.
