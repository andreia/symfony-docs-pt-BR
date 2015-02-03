.. index::
   single: Console; Enviar e-mails
   single: Console; Gerar URLs

Como gerar URLs e Enviar E-mails a partir do Console
====================================================

Infelizmente, o contexto de linha de comando não sabe sobre seu VirtualHost
ou nome de domínio. Isso significa que, se você gerar URLs absolutas dentro de um
Comando de Console, provavelmente vai acabar com algo como ``http://localhost/foo/bar``
o que não é muito útil.

Para corrigir isso, você precisa configurar o "contexto da requisição", que é uma
maneira de dizer que você precisa configurar seu ambiente para que ele saiba
qual URL deve usar ao gerar URLs.

Há duas maneiras de configurar o contexto da requisição: no nível da aplicação
e por comando.

Configurando o Contexto da Requisição Globalmente
-------------------------------------------------

Para configurar o Contexto da Requisição - que é utilizado pelo Gerador de URL - você pode
redefinir os parâmetros que ele usa como valores padrão para alterar o host padrão
(localhost) e esquema (http). Você também pode configurar o caminho base se o Symfony
não está em executando no diretório raiz.

Note que isso não afeta as URLs geradas via requisições da web normais, uma vez que essas
irão sobrescrever os padrões.

.. configuration-block::

    .. code-block:: yaml

        # app/config/parameters.yml
        parameters:
            router.request_context.host: example.org
            router.request_context.scheme: https
            router.request_context.base_url: my/path

    .. code-block:: xml

        <!-- app/config/parameters.xml -->
        <?xml version="1.0" encoding="UTF-8"?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <parameters>
                <parameter key="router.request_context.host">example.org</parameter>
                <parameter key="router.request_context.scheme">https</parameter>
                <parameter key="router.request_context.base_url">my/path</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // app/config/config_test.php
        $container->setParameter('router.request_context.host', 'example.org');
        $container->setParameter('router.request_context.scheme', 'https');
        $container->setParameter('router.request_context.base_url', 'my/path');

Configurando o Contexto da Requisição por Comando
-------------------------------------------------

Para alterar em apenas um comando, você pode simplesmente buscar o Contexto da Requisição
do serviço ``router`` e sobrescrever suas configurações::

   // src/AppBundle/Command/DemoCommand.php

   // ...
   class DemoCommand extends ContainerAwareCommand
   {
       protected function execute(InputInterface $input, OutputInterface $output)
       {
           $context = $this->getContainer()->get('router')->getContext();
           $context->setHost('example.com');
           $context->setScheme('https');
           $context->setBaseUrl('my/path');

           // ... your code here
       }
   }

Usando Spool em Memória
-----------------------

.. versionadded:: 2.3
    Ao usar o Symfony 2.3+ e o SwiftmailerBundle 2.3.5+, o spool em memória é agora
    manuseado automaticamente na CLI e o código abaixo não é mais necessário.

O envio de e-mails em um comando de console funciona da mesma maneira como descrito no
no cookbook :doc:`/cookbook/email/email` exceto se o spool em memória for usado.

Quando usar o spool de memória (ver o cookbook :doc:`/cookbook/email/spool` para mais informações),
você deve estar ciente de que, por causa de forma como o Symfony lida com comandos do console
, os e-mails não são enviados automaticamente. Você mesmo deve cuidar de realizar o flush
da fila. Use o seguinte código para enviar e-mails dentro de seu
comando de console::

    $message = new \Swift_Message();

    // ... prepare the message

    $container = $this->getContainer();
    $mailer = $container->get('mailer');

    $mailer->send($message);

    // now manually flush the queue
    $spool = $mailer->getTransport()->getSpool();
    $transport = $container->get('swiftmailer.transport.real');

    $spool->flushQueue($transport);

Outra opção é criar um ambiente que é usado somente por comandos de console
e usar um método diferente de spool.

.. note::

    Cuidar do spool somente é necessário quando o spool em memória é usado.
    Se você estiver usando spool em arquivo (ou nenhum spool), não há necessidade
    de realizar o flush da fila manualmente dentro do comando.
