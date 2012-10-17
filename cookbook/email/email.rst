.. index::
   single: Emails

Como enviar um e-mail
=====================

Enviar e-mails é uma tarefa clássica para qualquer aplicação web e, possui
complicações especiais e potenciais armadilhas. Em vez de recriar a roda,
uma solução para enviar e-mails é usar o ``SwiftmailerBundle``, que aproveita
o poder da biblioteca `Swiftmailer`_.

.. note::

    Não esqueça de ativar o bundle em seu kernel antes de usá-lo::

        public function registerBundles()
        {
            $bundles = array(
                // ...
                new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            );

            // ...
        }

.. _swift-mailer-configuration:

Configuração
------------

Antes de usar o Swiftmailer, não esqueça de incluir a sua configuração. O único
parâmetro de configuração obrigatório é o ``transport``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            transport:  smtp
            encryption: ssl
            auth_mode:  login
            host:       smtp.gmail.com
            username:   your_username
            password:   your_password

    .. code-block:: xml

        <!-- app/config/config.xml -->

        <!--
        xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
        http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            transport="smtp"
            encryption="ssl"
            auth-mode="login"
            host="smtp.gmail.com"
            username="your_username"
            password="your_password" />

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
            'transport'  => "smtp",
            'encryption' => "ssl",
            'auth_mode'  => "login",
            'host'       => "smtp.gmail.com",
            'username'   => "your_username",
            'password'   => "your_password",
        ));

A maioria das configurações do Swiftmailer lidam com a forma como as mensagens
devem ser entregues.

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
            ->setBody($this->renderView('HelloBundle:Hello:email.txt.twig', array('name' => $name)))
        ;
        $this->get('mailer')->send($message);

        return $this->render(...);
    }

Para manter as coisas desacopladas, o corpo do e-mail foi armazenado em um template e
renderizado através do método ``renderView()``.

O objeto ``$message`` suporta mais opções, como, a inclusão de anexos,
a adição de conteúdo HTML, e muito mais. Felizmente, o Swiftmailer cobre o tópico
`Criação de Mensagens`_ em grande detalhe na sua documentação.

.. tip::

    Vários outros artigos cookbook relacionados ao envio de e-mails estão disponíveis
    no Symfony2:

    * :doc:`gmail`
    * :doc:`dev_environment`
    * :doc:`spool`

.. _`Swiftmailer`: http://www.swiftmailer.org/
.. _`Criação de Mensagens`: http://swiftmailer.org/docs/messages
