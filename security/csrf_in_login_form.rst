.. index::
    single: Segurança; Proteção CSRF no Formulário de Login

Usando Proteção CSRF no Formulário de Login
===========================================

Ao usar um formulário de login, você deve certificar-se que ele está protegido contra CSRF
(`Cross-site request forgery`_). O componente de Segurança já possui suporte integrado
para CSRF. Neste artigo você vai aprender como pode usá-lo em seu formulário de login.

.. note::

    Ataques CSRF de login são um pouco menos conhecidos. Veja `Forjar Solicitações de Login`_
    se você está curioso para saber mais detalhes.

Configurando a Proteção CSRF
----------------------------

Primeiro, configure o componente de Segurança para que ele possa usar a proteção CSRF.
O componente de Segurança precisa de um provedor de token CSRF. Você pode usar o provider
padrão disponível no componente de Formulário:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    # ...
                    form_login:
                        # ...
                        csrf_provider: form.csrf_provider

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="secured_area">
                    <!-- ... -->

                    <form-login csrf-provider="form.csrf_provider" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'form_login' => array(
                        // ...
                        'csrf_provider' => 'form.csrf_provider',
                    )
                )
            )
        ));

O componente de Segurança pode ser configurado ainda, mas essa é toda a informação
que ele precisa para utilizar CSRF no formulário de login.

Renderizando o campo CSRF
-------------------------

Agora que o componente de Segurança irá verificar o token CSRF, você tem que adicionar
um campo *hidden* no formulário de login que contém o token CSRF. Por padrão,
esse campo é chamado ``_csrf_token``. Esse campo oculto deve conter o token
CSRF, que pode ser gerado usando a função ``csrf_token``. Essa
função requer um ID de token, que deve ser definido como ``authenticate`` ao
usar o formulário de login:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/SecurityBundle/Resources/views/Security/login.html.twig #}

        {# ... #}
        <form action="{{ path('login_check') }}" method="post">
            {# ... the login fields #}

            <input type="hidden" name="_csrf_token"
                value="{{ csrf_token('authenticate') }}"
            >

            <button type="submit">login</button>
        </form>

    .. code-block:: html+php

        <!-- src/Acme/SecurityBundle/Resources/views/Security/login.html.php -->

        <!-- ... -->
        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <!-- ... the login fields -->

            <input type="hidden" name="_csrf_token"
                value="<?php echo $view['form']->csrfToken('authenticate') ?>"
            >

            <button type="submit">login</button>
        </form>

Após isso, você tem seu formulário de login protegido contra ataques CSRF.

.. tip::

    Você pode alterar o nome do campo definindo ``csrf_parameter`` e alterando
    o ID de token setando ``intention`` em sua configuração:

    .. configuration-block::

        .. code-block:: yaml

            # app/config/security.yml
            security:
                firewalls:
                    secured_area:
                        # ...
                        form_login:
                            # ...
                            csrf_parameter: _csrf_security_token
                            intention: a_private_string

        .. code-block:: xml

            <!-- app/config/config.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <srv:container xmlns="http://symfony.com/schema/dic/security"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:srv="http://symfony.com/schema/dic/services"
                xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

                <config>
                    <firewall name="secured_area">
                        <!-- ... -->

                        <form-login csrf-parameter="_csrf_security_token"
                            intention="a_private_string" />
                    </firewall>
                </config>
            </srv:container>

        .. code-block:: php

            // app/config/security.php
            $container->loadFromExtension('security', array(
                'firewalls' => array(
                    'secured_area' => array(
                        // ...
                        'form_login' => array(
                            // ...
                            'csrf_parameter' => '_csrf_security_token',
                            'intention'      => 'a_private_string',
                        )
                    )
                )
            ));

.. _`Cross-site request forgery`: http://en.wikipedia.org/wiki/Cross-site_request_forgery
.. _`Forjar Solicitações de Login`: http://en.wikipedia.org/wiki/Cross-site_request_forgery#Forging_login_requests
