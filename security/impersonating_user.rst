.. index::
    single: Segurança; Personificar Usuário

Como Personificar um usuário
============================

Às vezes, é útil poder trocar de um usuário para outro, sem
ter que sair e entrar novamente (por exemplo, quando você está depurando ou tentando
entender um bug que um usuário vê e que você não consegue reproduzir). Isto pode ser feito facilmente
ativando o listener firewall ``switch_user``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    # ...
                    switch_user: true

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <config>
                <firewall>
                    <!-- ... -->
                    <switch-user />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => true
                ),
            ),
        ));

Para trocar para outro usuário, basta adicionar uma query string na URL atual com
o parâmetro ``_switch_user`` com o nome do usuário como valor:

.. code-block:: text

    http://example.com/somewhere?_switch_user=thomas

Para voltar para o usuário original, use o nome de usuário especial ``_exit``:

.. code-block:: text

    http://example.com/somewhere?_switch_user=_exit

Durante a personificação, o usuário é fornecido com um papel (role) especial chamado
``ROLE_PREVIOUS_ADMIN``. No template, por exemplo, esse papel pode ser usado
para mostrar um link para sair da personificação:

.. configuration-block::

    .. code-block:: html+jinja

        {% if is_granted('ROLE_PREVIOUS_ADMIN') %}
            <a href="{{ path('homepage', {'_switch_user': '_exit'}) }}">Exit impersonation</a>
        {% endif %}

    .. code-block:: html+php

        <?php if ($view['security']->isGranted('ROLE_PREVIOUS_ADMIN')): ?>
            <a
                href="<?php echo $view['router']->generate('homepage', array(
                    '_switch_user' => '_exit',
                ) ?>"
            >
                Exit impersonation
            </a>
        <?php endif; ?>

Claro, esse recurso deve ser disponibilizado a um pequeno grupo de usuários.
Por padrão, o acesso é restrito a usuários que tenham o papel
``ROLE_ALLOWED_TO_SWITCH``. O nome desse papel pode ser modificado através da configuração ``role``. Para
segurança extra, você também pode alterar o nome do parâmetro query através da configuração
``parameter``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                main:
                    # ...
                    switch_user: { role: ROLE_ADMIN, parameter: _want_to_be_this_user }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <config>
                <firewall>
                    <!-- ... -->
                    <switch-user role="ROLE_ADMIN" parameter="_want_to_be_this_user" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => array(
                        'role' => 'ROLE_ADMIN',
                        'parameter' => '_want_to_be_this_user',
                    ),
                ),
            ),
        ));
