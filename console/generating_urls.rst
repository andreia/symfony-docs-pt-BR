.. index::
   single: Console; Geração de URLs

Como gerar URLs com um Host personalizado em Comandos de Console
================================================================

Infelizmente, o contexto de linha de comando não sabe sobre o seu VirtualHost
ou nome de domínio. Isto significa que se você gerar URLs absolutas em um
comando de Console, você provavelmente vai acabar com algo como ``http://localhost/foo/bar``
o que não é muito útil.

Para corrigir isso, você precisa configurar o "contexto do pedido", que é uma
maneira elegante de dizer que você precisa configurar o seu ambiente para que ele saiba
qual URL ele deve usar ao gerar as URLs.

Há duas maneiras de configurar o contexto do pedido: a nível da aplicação
e por Comando.

Configurando o Contexto do Pedido globalmente
---------------------------------------------

Para configurar o Contexto do Pedido - que é usado pelo Gerador de URL - você pode
redefinir os parâmetros que ele usa como valores padrão para alterar o host padrão
(localhost) e o esquema (http). Note que isso não impacta nas URLs geradas
através de solicitações normais da web, uma vez que substituirá os padrões.

.. configuration-block::

    .. code-block:: yaml

        # app/config/parameters.yml
        parameters:
            router.request_context.host: example.org
            router.request_context.scheme: https

    .. code-block:: xml

        <!-- app/config/parameters.xml -->
        <?xml version="1.0" encoding="UTF-8"?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <parameters>
                <parameter key="router.request_context.host">example.org</parameter>
                <parameter key="router.request_context.scheme">https</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // app/config/config_test.php
        $container->setParameter('router.request_context.host', 'example.org');
        $container->setParameter('router.request_context.scheme', 'https');

Configurando o Contexto do Pedido por Comando
---------------------------------------------

Para alterá-lo em apenas um comando, você pode simplesmente chamar o serviço do Contexto de Pedido
e sobrescrever as suas configurações::

    // src/Acme/DemoBundle/Command/DemoCommand.php

    // ...
    class DemoCommand extends ContainerAwareCommand
    {
        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $context = $this->getContainer()->get('router')->getContext();
            $context->setHost('example.com');
            $context->setScheme('https');

            // ... your code here
        }
    }
