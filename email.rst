.. index::
   single: Emails

Como enviar um e-mail
=====================

Enviar e-mails é uma tarefa clássica para qualquer aplicação web e, possui
complicações especiais e potenciais armadilhas. Em vez de recriar a roda,
uma solução para enviar e-mails é usar o ``SwiftmailerBundle``, que aproveita
o poder da biblioteca `Swiftmailer`_. Esse bundle vem com a Edição
Standard do Symfony.

.. _swift-mailer-configuration:

Configuração
------------

Para usar o Swiftmailer, você precisa configurá-lo para seu servidor de e-mail:

.. tip::

    Ao invés de configurar/usar o seu próprio servidor de e-mail, você pode usar
    um provedor de e-mail como `Mandrill`_, `SendGrid`_, `Amazon SES`_
    ou outros. Esses lhe fornecem um servidor SMTP, nome de usuário e senha (algumas vezes
    chamada de chaves) que podem ser usados com a configuração do Swift Mailer.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            transport: '%mailer_transport%'
            host:      '%mailer_host%'
            username:  '%mailer_user%'
            password:  '%mailer_password%'

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config
                transport="%mailer_transport%"
                host="%mailer_host%"
                username="%mailer_user%"
                password="%mailer_password%"
            />
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
            'transport'  => "%mailer_transport%",
            'host'       => "%mailer_host%",
            'username'   => "%mailer_user%",
            'password'   => "%mailer_password%",
        ));

Esses valores (ex. ``%mailer_transport%``), são lidos dos parâmetros
que são setados no arquivo :ref:`parameters.yml <config-parameters.yml>`. Você
pode modificar os valores nesse arquivo, ou setar os valores diretamente aqui.

Os seguintes atributos de configuração estão disponíveis:

* ``transport``         (``smtp``, ``mail``, ``sendmail``, ou ``gmail``)
* ``username``
* ``password``
* ``host``
* ``port``
* ``encryption``        (``tls``, ou ``ssl``)
* ``auth_mode``         (``plain``, ``login``, ou ``cram-md5``)
* ``spool``

  * ``type`` (como será o queue de mensagens, são suportados ``file`` ou ``memory``, veja :doc:`/cookbook/email/spool`)
  * ``path`` (onde armazenar as mensagens)
* ``delivery_address``  (um endereço de email para onde serão enviados TODOS os e-mails)
* ``disable_delivery``  (defina como true para desabilitar completamente a entrega)

Enviando e-mails
----------------

A biblioteca Swiftmailer funciona através da criação, configuração e, então, o envio de 
objetos ``Swift_Message``. O "mailer" é responsável pela entrega da mensagem e é 
acessível através do serviço ``mailer``. No geral, o envio de
um e-mail é bastante simples::

    public function indexAction($name)
    {
        $message = \Swift_Message::newInstance()
            ->setSubject('Hello Email')
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody(
                $this->renderView(
                    // app/Resources/views/Emails/registration.html.twig
                    'Emails/registration.html.twig',
                    array('name' => $name)
                ),
                'text/html'
            )
            /*
             * If you also want to include a plaintext version of the message
            ->addPart(
                $this->renderView(
                    'Emails/registration.txt.twig',
                    array('name' => $name)
                ),
                'text/plain'
            )
            */
        ;
        $this->get('mailer')->send($message);

        return $this->render(...);
    }

Para manter as coisas desacopladas, o corpo do e-mail foi armazenado em um template e
renderizado através do método ``renderView()``. . O template ``registration.html.twig``
deve parecer com algo assim:

.. code-block:: html+jinja

    {# app/Resources/views/Emails/registration.html.twig #}
    <h3>You did it! You registered!</h3>

    Hi {{ name }}! You're successfully registered.

    {# example, assuming you have a route named "login" #}
    To login, go to: <a href="{{ url('login') }}">...</a>.

    Thanks!

    {# Makes an absolute URL to the /images/logo.png file #}
    <img src="{{ absolute_url(asset('images/logo.png')) }}">

O objeto ``$message`` suporta mais opções, como, a inclusão de anexos,
a adição de conteúdo HTML, e muito mais. Felizmente, o Swiftmailer cobre o tópico
`Criação de Mensagens`_ em grande detalhe na sua documentação.

Aprenda mais
------------

.. toctree::
    :maxdepth: 1
    :glob:

    email/*

.. _`Swift Mailer`: http://swiftmailer.org/
.. _`Criação de Mensagens`: http://swiftmailer.org/docs/messages.html
.. _`Mandrill`: https://mandrill.com/
.. _`SendGrid`: https://sendgrid.com/
.. _`Amazon SES`: http://aws.amazon.com/ses/
