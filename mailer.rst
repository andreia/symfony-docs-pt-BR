Enviando E-mails com o Mailer
=============================

.. versionadded :: 4.3

    O componente Mailer foi adicionado no Symfony 4.3 e atualmente é experimental.
    A solução anterior - Swift Mailer - ainda é válida: :doc:`Swift Mailer</email>`.

Instalação
----------

.. caution::

    O componente Mailer é experimental no Symfony 4.3: alguma quebra de compatibilidade com
    versões anteriores pode ocorrer antes da versão 4.4.

Os componentes Mailer e :doc:`Mime </components/mime>` do Symfony formam um sistema *poderoso*
para criar e enviar e-mails - completo com suporte para mensagens multipartes, integração
com o Twig, CSS inline, anexos de arquivos e muito mais. Instale-os com:

.. code-block:: terminal

    $ composer require symfony/mailer

Configuração do Transporte
--------------------------

Os e-mails são entregues através de um "transporte". E sem instalar mais nada, você
pode enviar emails usando ``smtp`` apenas configurando o seguinte em seu arquivo ``.env``:

.. code-block:: bash

    # .env
    MAILER_DSN=smtp://user:pass@smtp.example.com

Usando um Transporte de Terceiros
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mas uma opção mais fácil é enviar e-mails através de um provedor de terceiros. O Mailer suporta
vários - instale o que você desejar:

==================  =============================================
Serviço             Instale com
==================  =============================================
Amazon SES          ``composer require symfony/amazon-mailer``
Gmail               ``composer require symfony/google-mailer``
MailChimp           ``composer require symfony/mailchimp-mailer``
Mailgun             ``composer require symfony/mailgun-mailer``
Postmark            ``composer require symfony/postmark-mailer``
SendGrid            ``composer require symfony/sendgrid-mailer``
==================  =============================================

Cada biblioteca inclui uma :ref:`receita Flex <flex-recipe>` que adicionará configuração exemplo
em seu arquivo ``.env``. Por exemplo, supondo que você queira usar o SendGrid. Primeiro,
instale-o:

.. code-block:: terminal

    $ composer require symfony/sendgrid-mailer

Você agora terá uma nova linha no seu arquivo ``.env`` que você pode remover o seu comentário:

.. code-block:: bash

    # .env
    SENDGRID_KEY=
    MAILER_DSN=smtp://$SENDGRID_KEY@sendgrid

O ``MAILER_DSN`` não é um endereço SMTP *real*: é um formato simples que transfere
a maior parte do trabalho de configuração para o mailer. A parte ``@sendgrid`` do endereço
ativa a biblioteca de e-mail SendGrid que você acabou de instalar, que sabe tudo
sobre como entregar mensagens para o SendGrid.

A *única* parte que você precisa alterar é setar a sua chave em ``SENDGRID_KEY`` (no
arquivo ``.env`` ou `` .env.local``).

Cada transporte terá variáveis de ambiente diferentes que a biblioteca usará
para configurar o endereço *real* e a autenticação para entrega. Alguns também têm
opções que podem ser configuradas com parâmetros query no final do ``MAILER_DSN`` -
como ``?region = `` para o Amazon SES. Alguns transportes suportam o envio via ``http``
ou ``smtp`` - ambos funcionam da mesma forma, mas ``http`` é recomendado quando disponível.

Criando e Enviando Mensagens
----------------------------

Para enviar um email, faça o autowire do mailer usando
:class:`Symfony\\Component\\Mailer\\MailerInterface` (id de serviço ``mailer``)
e crie um objeto :class:`Symfony\\Component\\Mime\\Email`::

    // src/Controller/MailerController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Mailer\MailerInterface;
    use Symfony\Component\Mime\Email;

    class MailerController extends AbstractController
    {
        /**
         * @Route("/email")
         */
        public function sendEmail(MailerInterface $mailer)
        {
            $email = (new Email())
                ->from('hello@example.com')
                ->to('you@example.com')
                //->cc('cc@example.com')
                //->bcc('bcc@example.com')
                //->replyTo('fabien@example.com')
                //->priority(Email::PRIORITY_HIGH)
                ->subject('Time for Symfony Mailer!')
                ->text('Sending emails is fun again!')
                ->html('<p>See Twig integration for better HTML integration!</p>');

            $mailer->send($email);

            // ...
        }
    }

É isso! A mensagem será enviada através do transporte que você configurou.

Endereços de E-mail
~~~~~~~~~~~~~~~~~~~

Todos os métodos que requerem endereços de e-mail (``from()``, ``to()``, etc.) aceitam
tanto strings quanto objetos de endereço::

    // ...
    use Symfony\Component\Mime\Address;
    use Symfony\Component\Mime\NamedAddress;

    $email = (new Email())
        // email address as a simple string
        ->from('fabien@example.com')

        // email address as an object
        ->from(new Address('fabien@example.com'))

        // email address as an object (email clients will display the name
        // instead of the email address)
        ->from(new NamedAddress('fabien@example.com', 'Fabien'))

        // ...
    ;

Múltiplos endereços são definidos com os métodos ``addXXX()``::

    $email = (new Email())
        ->to('foo@example.com')
        ->addTo('bar@example.com')
        ->addTo('baz@example.com')

        // ...
    ;

Alternativamente, você pode passar vários endereços para cada método::

    $toAddresses = ['foo@example.com', new Address('bar@example.com')];

    $email = (new Email())
        ->to(...$toAddresses)
        ->cc('cc1@example.com', 'cc2@example.com')

        // ...
    ;

Conteúdo da Mensagem
~~~~~~~~~~~~~~~~~~~~

O texto e o conteúdo HTML das mensagens de e-mail podem ser strings (geralmente
resultado da renderização de algum template) ou recursos PHP::

    $email = (new Email())
        // ...
        // simple contents defined as a string
        ->text('Lorem ipsum...')
        ->html('<p>Lorem ipsum...</p>')

        // attach a file stream
        ->text(fopen('/path/to/emails/user_signup.txt', 'r'))
        ->html(fopen('/path/to/emails/user_signup.html', 'r'))
    ;

.. tip::

    Você também pode usar templates Twig para renderizar o conteúdo HTML e de texto. Leia
    a seção `Twig: HTML e CSS`_ mais adiante neste artigo para
    saber mais.

Arquivos Anexo
~~~~~~~~~~~~~~

Use o método ``attachFromPath()`` para anexar arquivos que existem no seu sistema de arquivos::

    $email = (new Email())
        // ...
        ->attachFromPath('/path/to/documents/terms-of-use.pdf')
        // optionally you can tell email clients to display a custom name for the file
        ->attachFromPath('/path/to/documents/privacy.pdf', 'Privacy Policy')
        // optionally you can provide an explicit MIME type (otherwise it's guessed)
        ->attachFromPath('/path/to/documents/contract.doc', 'Contract', 'application/msword')
    ;

Como alternativa, você pode usar o método ``attach()`` para anexar conteúdo a partir de um stream::

    $email = (new Email())
        // ...
        ->attach(fopen('/path/to/documents/contract.doc', 'r'))
    ;

Incorporando Imagens
~~~~~~~~~~~~~~~~~~~~

Se você quiser exibir imagens dentro do seu e-mail, você deve incorporá-las
em vez de adicioná-las como anexos. Ao usar o Twig para renderizar o conteúdo
do e-mail, como explicado `mais adiante neste artigo <Embedding Images>`_,
as imagens são incorporadas automaticamente. Caso contrário, você precisa incorporá-las manualmente.

Primeiro, use o método ``embed()`` ou ``embedFromPath()`` para adicionar uma imagem a partir de um
arquivo ou stream::

    $email = (new Email())
        // ...
        // get the image contents from a PHP resource
        ->embed(fopen('/path/to/images/logo.png', 'r'), 'logo')
        // get the image contents from an existing file
        ->embedFromPath('/path/to/images/signature.gif', 'footer-signature')
    ;

O segundo argumento opcional de ambos os métodos é o nome da imagem ("Content-ID" no
padrão MIME). Seu valor é uma string arbitrária usada posteriormente para referenciar
imagens dentro do conteúdo HTML::

    $email = (new Email())
        // ...
        ->embed(fopen('/path/to/images/logo.png', 'r'), 'logo')
        ->embedFromPath('/path/to/images/signature.gif', 'footer-signature')
        // reference images using the syntax 'cid:' + "image embed name"
        ->html('<img src="cid:logo"> ... <img src="cid:footer-signature"> ...')
    ;

Endereço from Global
--------------------

Em vez de chamar ``->from()`` *cada* vez que você cria um novo e-mail, você pode
criar um subscriber de evento para configurá-lo automaticamente::

    // src/EventListener/MailerFromListener.php
    namespace App\EventListener;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Mailer\Event\MessageEvent;
    use Symfony\Component\Mime\Email;

    class MailerFromListener implements EventSubscriberInterface
    {
        public function onMessageSend(MessageEvent $event)
        {
            $message = $event->getMessage();

            // make sure it's an Email object
            if (!$message instanceof Email) {
                return;
            }

            // always set the from address
            $message->from('fabien@example.com');
        }

        public static function getSubscribedEvents()
        {
            return [MessageEvent::class => 'onMessageSend'];
        }
    }

.. _mailer-twig:

Twig: HTML e CSS
----------------

O componente Mime se integra com a :doc:`engine de template Twig </templating>`
para fornecer recursos avançados, como CSS inline e suporte para frameworks HTML/CSS
para criar mensagens de e-mail complexas em HTML. Primeiro, verifique se o Twig está instalado:

.. code-block:: terminal

    $ composer require symfony/twig-bundle

Conteúdo HTML
~~~~~~~~~~~~~

Para definir o conteúdo do seu e-mail com o Twig, use a
classe :class:`Symfony\\Bridge\\Twig\\Mime\\TemplatedEmail`. Essa classe estende
a classe normal :class:`Symfony\\Component\\Mime\\Email` mas adiciona alguns métodos novos
para templates Twig::

    use Symfony\Bridge\Twig\Mime\TemplatedEmail;

    $email = (new TemplatedEmail())
        ->from('fabien@example.com')
        ->to(new NamedAddress('ryan@example.com', 'Ryan'))
        ->subject('Thanks for signing up!')

        // path of the Twig template to render
        ->htmlTemplate('emails/signup.html.twig')

        // pass variables (name => value) to the template
        ->context([
            'expiration_date' => new \DateTime('+7 days'),
            'username' => 'foo',
        ])
    ;

Em seguida, crie o template:

.. code-block:: html+twig

    {# templates/emails/signup.html.twig #}
    <h1>Welcome {{ email.toName }}!</h1>

    <p>
        You signed up as {{ username }} the following email:
    </p>
    <p><code>{{ email.to[0].address }}</code></p>

    <p>
        <a href="#">Click here to activate your account</a>
        (this link is valid until {{ expiration_date|date('F jS') }})
    </p>

O template Twig tem acesso a qualquer um dos parâmetros passados no método
``context()`` da classe ``TemplatedEmail`` e também para uma variável especial
chamada `` email``, que é uma instância de
:class:`Symfony\\Bridge\\Twig\\Mime\\WrappedTemplatedEmail`.

Conteúdo de Texto
~~~~~~~~~~~~~~~~~

Quando o conteúdo de texto de um ``TemplatedEmail`` não é explicitamente definido, o mailer
irá gerá-lo automaticamente, convertendo o conteúdo HTML em texto. Se você
possui o `league/html-to-markdown`_ instalado em sua aplicação,
ele irá usá-lo para transformar HTML em Markdown (para que o e-mail de texto tenha algum apelo visual).
Caso contrário, aplica-se a função PHP :phpfunction:`strip_tags` ao
conteúdo HTML original.

Se você quiser definir o conteúdo do texto, use o método ``text()``
explicado nas seções anteriores ou o método ``textTemplate()`` fornecido pela
classe ``TemplatedEmail``:

.. code-block:: diff

    + use Symfony\Bridge\Twig\Mime\TemplatedEmail;

    $email = (new TemplatedEmail())
        // ...

        ->htmlTemplate('emails/signup.html.twig')
    +     ->textTemplate('emails/signup.txt.twig')
        // ...
    ;

Incorporando Imagens
~~~~~~~~~~~~~~~~~~~~

Em vez de lidar com a sintaxe ``<img src="cid: ...">`` explicada nas
seções anteriores, ao usar o Twig para gerar conteúdo de e-mail você pode referenciar
os arquivos de imagem como de costume. Primeiro, para simplificar as coisas, defina um namespace Twig chamado
``images`` que aponta para o diretório onde suas imagens estão armazenadas:

.. code-block:: yaml

    # config/packages/twig.yaml
    twig:
        # ...

        paths:
            # point this wherever your images live
            '%kernel.project_dir%/assets/images': images

Agora, use o helper especial ``email.image()`` do Twig para incorporar as imagens dentro
do conteúdo do email:

.. code-block:: html+twig

    {# '@images/' refers to the Twig namespace defined earlier #}
    <img src="{{ email.image('@images/logo.png') }}" alt="Logo">

    <h1>Welcome {{ email.toName }}!</h1>
    {# ... #}

.. _mailer-inline-css:

Estilos CSS Inline
~~~~~~~~~~~~~~~~~~

Projetar o conteúdo HTML de um email é muito diferente de projetar uma
página HTML normal. Para começar, a maioria dos clientes de email suporta apenas um subconjunto
dos recursos CSS. Além disso, os clientes de e-mail populares, como o Gmail, não suportam
definir estilos dentro de seções ``<style> ... </ style>`` e você deve usar **todos os 
estilos CSS inline**.

CSS inline significa que cada tag HTML deve definir um atributo ``style`` com
todos os seus estilos CSS. Isso pode tornar a organização de seu CSS uma bagunça. É por isso que
o Twig fornece uma ``CssInlinerExtension`` que automatiza tudo para você. Instale
com:

.. code-block:: terminal

    $ composer require twig/cssinliner-extension

A extensão é ativada automaticamente. Para usá-la, envolva o template inteiro
com o filtro ``inline_css``:

.. code-block:: html+twig

    {% apply inline_css %}
        <style>
            {# here, define your CSS styles as usual #}
            h1 {
                color: #333;
            }
        </style>

        <h1>Welcome {{ email.toName }}!</h1>
        {# ... #}
    {% endapply %}

Usando Arquivos CSS Externos
............................

Você também pode definir estilos CSS em arquivos externos e passá-los como
argumentos para o filtro:

.. code-block:: html+twig

    {% apply inline_css(source('@css/email.css')) %}
        <h1>Welcome {{ username }}!</h1>
        {# ... #}
    {% endapply %}

Você pode passar um número ilimitado de argumentos em ``inline_css()`` para carregar múltiplos
arquivos CSS. Para esse exemplo funcionar, você também precisa definir um novo namespace Twig
chamado ``css`` que aponta para o diretório onde ``email.css`` encontra-se:

.. _mailer-css-namespace:

.. code-block:: yaml

    # config/packages/twig.yaml
    twig:
        # ...

        paths:
            # point this wherever your css files live
            '%kernel.project_dir%/assets/css': css

.. _mailer-markdown:

Renderizando Conteúdo Markdown
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O Twig fornece outra extensão chamada ``MarkdownExtension`` que permite 
definir o conteúdo do email usando a `sintaxa Markdown`_. Para usá-la, instale
a extensão e uma biblioteca de conversão Markdown (a extensão é compatível com
várias bibliotecas populares):

.. code-block:: terminal

    # instead of league/commonmark, you can also use erusev/parsedown or michelf/php-markdown
    $ composer require twig/markdown-extension league/commonmark

A extensão adiciona um filtro ``markdown``, que você pode usar para converter partes ou
todo o conteúdo do e-mail de Markdown para HTML:

.. code-block:: twig

    {% apply markdown %}
        Welcome {{ email.toName }}!
        ===========================

        You signed up to our site using the following email:
        `{{ email.to[0].address }}`

        [Click here to activate your account]({{ url('...') }})
    {% endapply %}

.. _mailer-inky:

Linguagem de Template de E-mail Inky
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Criar e-mails bem projetados que funcionam em todos os clientes de e-mail é tão
complexo que existem frameworks HTML/CSS dedicados a isso. Um dos frameworks
mais populares é chamado 'Inky`_. Ele define uma sintaxe baseada em algumas
tags que depois são transformadas no código HTML real enviado aos usuários:

.. code-block:: html

    <!-- a simplified example of the Inky syntax -->
    <container>
        <row>
            <columns>This is a column.</columns>
        </row>
    </container>

O Twig fornece integração com o Inky através da ``InkyExtension``. Primeiro, instale
a extensão em sua aplicação:

.. code-block:: terminal

    $ composer require twig/inky-extension

A extensão adiciona um filtro ``inky``, que pode ser usado para converter partes ou
todo o conteúdo do e-mail de Inky para HTML:

.. code-block:: html+twig

    {% apply inky %}
        <container>
            <row class="header">
                <columns>
                    <spacer size="16"></spacer>
                    <h1 class="text-center">Welcome {{ email.toName }}!</h1>
                </columns>

                {# ... #}
            </row>
        </container>
    {% endapply %}

Você pode combinar todos os filtros para criar mensagens de e-mail complexas:

.. code-block:: twig

    {% apply inky|inline_css(source('@css/foundation-emails.css')) %}
        {# ... #}
    {% endapply %}

Isso faz uso do :ref:`namespace css Twig <mailer-css-namespace>` que criamos
anteriormente. Você poderia, por exemplo, `baixar o arquivo foundation-emails.css`_
diretamente do GitHub e salvá-o em ``assets/css``.

Enviando Mensagens Assíncronas
------------------------------

Quando você chama ``$mailer->send($email)``, o e-mail é enviado para o transporte imediatamente.
Para melhorar o desempenho, você pode aproveitar o :doc:`Messenger </messenger>` para enviar
as mensagens depois, através de um transporte do Messenger.

Comece seguindo a documentação do :doc:`Messenger </messenger>` e configurando
um transporte. Quando tudo estiver configurado, ao você chamar ``$mailer->send()``, uma
mensagem :class:`Symfony\\Component\\Mailer\\Messenger\\SendEmailMessage`
será despachada através do bus de mensagens padrão (``messenger.default_bus``). Assumindo
que você tem um transporte chamado ``async``, você pode rotear a mensagem lá:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async: "%env(MESSENGER_TRANSPORT_DSN)%"

                routing:
                    'Symfony\Component\Mailer\Messenger\SendEmailMessage':  async

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="Symfony\Component\Mailer\Messenger\SendEmailMessage">
                        <framework:sender service="async"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'routing' => [
                    'Symfony\Component\Mailer\Messenger\SendEmailMessage' => 'async',
                ],
            ],
        ]);

Graças a isso, ao invés de serem entregues imediatamente, as mensagens serão enviadas para
o transporte para serem tratadas mais tarde (veja :ref:`messenger-worker`).

Desenvolvimento e Depuração
---------------------------

Desabilitando a Entrega
~~~~~~~~~~~~~~~~~~~~~~~

Durante o desenvolvimento (ou teste), você pode desabilitar a entrega de mensagens completamente.
Você pode fazer isso forçando o Mailer a usar o ``NullTransport`` somente no ambiente
``dev``:

.. code-block:: yaml

    # config/packages/dev/mailer.yaml
    framework:
        mailer:
            dsn: 'smtp://null'

.. note::

    Se você estiver usando o Messenger e roteando para um transporte, a mensagem *ainda* será
    enviada para esse transporte.

Enviar Sempre para o Mesmo Endereço
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Em vez de desabilitar totalmente a entrega, você pode querer *sempre* enviar e-mails para
um endereço específico, em vez do endereço *real*. Para fazer isso, você pode
aproveitar o ``EnvelopeListener`` e registrá-lo *somente* para o ambiente
``dev``:

.. code-block:: yaml

    # config/services_dev.yaml
    services:
        mailer.dev.set_recipients:
            class: Symfony\Component\Mailer\EventListener\EnvelopeListener
            tags: ['kernel.event_subscriber']
            arguments:
                $sender: null
                $recipients: ['youremail@example.com']

.. _`baixar o arquivo foundation-emails.css`: https://github.com/zurb/foundation-emails/blob/develop/dist/foundation-emails.css
.. _`league/html-to-markdown`: https://github.com/thephpleague/html-to-markdown
.. _`sintaxe Markdown`: https://commonmark.org/
.. _`Inky`: https://foundation.zurb.com/emails.html
