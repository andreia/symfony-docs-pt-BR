.. index::
   single: Sessões, Diretório de Sessões

Configurando o Diretório onde os Arquivos da Sessão são Salvos
==============================================================

Por padrão, a Edição Standard do Symfony usa os valores globais do ``php.ini``
para ``session.save_handler`` e ``session.save_path`` para determinar onde
armazenar os dados da sessão. Isso é devido a seguinte configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session:
                # handler_id set to null will use default session handler from php.ini
                handler_id: ~

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >
            <framework:config>
                <!-- handler-id set to null will use default session handler from php.ini -->
                <framework:session handler-id="null" />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'session' => array(
                // handler_id set to null will use default session handler from php.ini
                'handler_id' => null,
            ),
        ));

Com essa configuração, alterar *onde* os metadados da sessão são armazenados
é inteiramente através da configuração do seu ``php.ini``.

No entanto, se você tem a seguinte configuração, o Symfony irá armazenar os dados da sessão
em arquivos no diretório cache ``%kernel.cache_dir%/sessions``. Isso
significa que quando você limpar o cache, todas as sessões atuais também serão excluídas:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session: ~

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >
            <framework:config>
                <framework:session />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'session' => array(),
        ));

Usar um diretório diferente para salvar os dados da sessão é um método para garantir
que suas sessões atuais não são perdidas quando você limpar o cache do Symfony.

.. tip::

    Usar um manipulador para salvar sessão diferente é um excelente (contudo mais complexo)
    método de gerenciamento de sessão disponível com o Symfony. Veja
    :doc:`/components/http_foundation/session_configuration` para uma
    discussão sobre os manipuladores para salvar sessão. Há também um artigo no cookbook
    sobre o armazenamento de sessões no :doc:`banco de dados</cookbook/configuration/pdo_session_storage>`.

Para alterar o diretório no qual o Symfony salva os dados da sessão, você só precisa
alterar a configuração do framework. Neste exemplo, você irá alterar o
diretório da sessão para ``app/sessions``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session:
                handler_id: session.handler.native_file
                save_path: "%kernel.root_dir%/sessions"

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >
            <framework:config>
                <framework:session handler-id="session.handler.native_file"
                    save-path="%kernel.root_dir%/sessions"
                />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'session' => array(
                'handler_id' => 'session.handler.native_file',
                'save_path'  => '%kernel.root_dir%/sessions',
            ),
        ));
