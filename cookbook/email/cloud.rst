.. index::
   single: E-mails; Usando a nuvem

Como usar a nuvem para enviar e-mails
=====================================

Os requisitos para enviar e-mails a partir de um sistema de produção diferem da sua
configuração de desenvolvimento pois você não quer ficar limitado ao número de e-mails,
a taxa de envio ou o endereço do remetente. Assim,
:doc:`usar o Gmail </cookbook/email/gmail>` ou serviços semelhantes não é uma
opção. Se configurar e manter o seu próprio servidor de e-mail causa
dores de cabeça, há uma solução simples: Aproveite a nuvem para enviar os seus
e-mails.

Este artigo mostra como é fácil integrar
o `Simple Email Service (SES) da Amazon`_ no Symfony.

.. note::

    Você pode usar a mesma técnica para outros serviços de e-mail pois, na maioria da
    vezes, não há nada mais do que a configuração de endpoint SMTP para o
    Swift Mailer.

Na configuração do Symfony, altere as configurações do Swift Mailer ``transport``,
``host``, ``port`` e ``encryption`` de acordo com as informações fornecidas no
`console SES`_. Crie suas credenciais SMTP individuais no console SES
e conclua a configuração com o ``username`` e ``password`` fornecidos:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            transport:  smtp
            host:       email-smtp.us-east-1.amazonaws.com
            port:       465 # different ports are available, see SES console
            encryption: tls # TLS encryption is required
            username:   AWS_ACCESS_KEY  # to be created in the SES console
            password:   AWS_SECRET_KEY  # to be created in the SES console

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <!-- ... -->
            <swiftmailer:config
                transport="smtp"
                host="email-smtp.us-east-1.amazonaws.com"
                port="465"
                encryption="tls"
                username="AWS_ACCESS_KEY"
                password="AWS_SECRET_KEY"
            />
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
            'transport'  => 'smtp',
            'host'       => 'email-smtp.us-east-1.amazonaws.com',
            'port'       => 465,
            'encryption' => 'tls',
            'username'   => 'AWS_ACCESS_KEY',
            'password'   => 'AWS_SECRET_KEY',
        ));

Por padrão, as chaves ``port`` e ``encryption`` não estão presentes na configuração da Edição Standard
do Symfony, mas você pode simplesmente adicioná-las conforme necessário.

E é isso, você está pronto para começar a enviar e-mails através da nuvem!

.. tip::

    Se você estiver usando a Edição Standard do Symfony, configure os parâmetros em
    ``parameters.yml`` e use-os em seus arquivos de configuração. Isso permite
    diferentes configurações do Swift Mailer para cada instalação da sua
    aplicação. Por exemplo, usar o Gmail durante o desenvolvimento e a nuvem em
    produção.

    .. code-block:: yaml

        # app/config/parameters.yml
        parameters:
            # ...
            mailer_transport:  smtp
            mailer_host:       email-smtp.us-east-1.amazonaws.com
            mailer_port:       465 # different ports are available, see SES console
            mailer_encryption: tls # TLS encryption is required
            mailer_user:       AWS_ACCESS_KEY # to be created in the SES console
            mailer_password:   AWS_SECRET_KEY # to be created in the SES console

.. note::

    Se você pretende usar o SES da Amazon, por favor, observe o seguinte:

    * Você tem que se inscrever no `Amazon Web Services (AWS)`_;

    * Cada endereço de remetente usado no cabeçalho ``From`` ou ``Return-Path`` 
      (endereço bounce) deve ser confirmado pelo proprietário. Você também pode
      confirmar um domínio inteiro;

    * Inicialmente você está em um modo sandbox restrito. Você precisa solicitar
      acesso de produção antes de ser autorizado a enviar e-mails para
      destinatários arbitrários;

    * O SES pode estar sujeito a uma taxa.

.. _`Simple Email Service (SES) da Amazon`: http://aws.amazon.com/ses
.. _`console SES`: https://console.aws.amazon.com/ses
.. _`Amazon Web Services (AWS)`: http://aws.amazon.com
