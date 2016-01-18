.. index::
    single: Container de Serviço; Serviços Compartilhados

Como Definir Serviços não Compartilhados
========================================

.. versionadded:: 2.8
    A configuração ``shared`` foi introduzida no Symfony 2.8. Antes do Symfony 2.8,
    era necessário usar o escopo ``prototype``.

No container de serviço, todos os serviços são compartilhados por padrão. Isso significa que,
cada vez que você recuperar o serviço, vai obter a *mesma* instância. Esse é
muitas vezes o comportamento que você quer, mas, em alguns casos, você pode querer obter sempre
uma *nova* instância.

A fim de obter sempre uma nova instância, defina a configuração ``shared`` para ``false``
em sua definição de serviço:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.some_not_shared_service:
                class: ...
                shared: false
                # ...

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <services>
            <service id="app.some_not_shared_service" class="..." shared="false" />
        </services>

    .. code-block:: php

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $definition = new Definition('...');
        $definition->setShared(false);

        $container->setDefinition('app.some_not_shared_service', $definition);

Agora, sempre que você chamar ``$container->get('app.some_not_shared_service')`` ou
injetar esse serviço, você receberá uma nova instância.
