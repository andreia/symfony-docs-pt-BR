.. index::
   single: E-mails; Gmail

Como usar o Gmail para enviar E-mails
=====================================

Durante o desenvolvimento, em vez de usar um servidor SMTP regular para enviar e-mails, você
pode descobrir que usar o Gmail é mais fácil e prático. O bundle Swiftmailer torna esta tarefa
realmente fácil.

.. tip::

    Em vez de usar a sua conta do Gmail normal, é, com certeza, recomendado
    que você crie uma conta especial para este propósito.

No arquivo de configuração de desenvolvimento, altere a definição ``transport`` para
``gmail`` e defina o ``username`` e ``password`` com as credenciais do Google:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        swiftmailer:
            transport: gmail
            username:  your_gmail_username
            password:  your_gmail_password

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->

        <!--
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            transport="gmail"
            username="your_gmail_username"
            password="your_gmail_password" />

    .. code-block:: php

        // app/config/config_dev.php
        $container->loadFromExtension('swiftmailer', array(
            'transport' => "gmail",
            'username'  => "your_gmail_username",
            'password'  => "your_gmail_password",
        ));

Está pronto!

.. note::

    O transporte ``gmail`` é simplesmente um atalho que usa o transporte ``smtp``
    e seta as definições ``encryption``, ``auth_mode`` e ``host`` para funcionar com o Gmail.
