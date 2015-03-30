.. index::
    single: Limite a Escrita dos Metadados; Sessão

Limite a Escrita dos Metadados de Sessão
========================================

O comportamento padrão da sessão PHP é persistir a sessão independentemente
se os dados da sessão foram alterados ou não. No Symfony, cada vez que a sessão
é acessada, os metadados são gravados (sessão criada/utilizada pela última vez) que pode ser usado
para determinar a idade da sessão e o tempo ocioso.

Se, por razões de desempenho, você deseja limitar a frequência com que a sessão
é persistida, esse recurso pode ajustar a granularidade das atualizações de metadados e
persistir a sessão com menos frequência, enquanto mantêm os metadados relativamente
precisos. Se outros dados de sessão forem alterados, a sessão irá sempre persistir.

Você pode dizer ao Symfony para não atualizar o metadado de tempo da "última atualização da sessão"
até que uma certa quantidade de tempo tenha passado, por definição
``framework.session.metadata_update_threshold`` para um valor em segundos maior
que zero:

.. configuration-block::

    .. code-block:: yaml

        framework:
            session:
                metadata_update_threshold: 120

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:session metadata-update-threshold="120" />
            </framework:config>

        </container>

    .. code-block:: php

        $container->loadFromExtension('framework', array(
            'session' => array(
                'metadata_update_threshold' => 120,
            ),
        ));

.. note::

    O comportamento padrão do PHP é salvar a sessão tanto se ela foi alterada ou
    não. Ao usar ``framework.session.metadata_update_threshold`` o Symfony
    envolverá o manipulador de sessão (configurado em
    ``framework.session.handler_id``) no WriteCheckSessionHandler. Isso
    impedirá que qualquer sessão grave se a sessão não foi modificada.

.. caution::

    Esteja ciente de que, se a sessão não é escrita em cada requisição, ela pode ser
    coletada pelo coletor de lixo (garbage collector), mais cedo do que o habitual. Isso significa que seus
    usuários podem ser deslogados mais cedo do que o esperado.
