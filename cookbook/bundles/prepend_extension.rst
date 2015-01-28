.. index::
   single: Configuração; Semântica
   single: Bundle; Configuração de Extensão

Como Simplificar a Configuração de múltiplos Bundles
====================================================

Ao construir aplicações reutilizáveis ​​e extensíveis, os desenvolvedores são muitas vezes
confrontados com uma escolha: criar um único grande bundle ou múltiplos bundles menores
. A criação de um único bundle tem o inconveniente de que é impossível para os
usuários optarem por remover as funcionalidades que eles não estão usando. A criação de múltiplos
bundles tem a desvantagem de que a configuração se torna mais tediosa e as definições de configuração,
muitas vezes, precisam ser repetidas para vários bundles.

Utilizando a abordagem abaixo, é possível remover a desvantagem da
abordagem de múltiplos bundles ao habilitar uma única extensão para prefixar as configurações
para qualquer bundle. Ele pode usar as configurações definidas no ``app/config/config.yml``
para prefixar as definições de configuração como se elas tivessem sido escritas explicitamente
pelo usuário na configuração do aplicativo.

Por exemplo, isso pode ser utilizado para configurar o nome do gerenciador de entidades para usar em
múltiplos bundles. Ou ele pode ser usado para permitir que um recurso opcional que depende
de outro bundle seja carregado.

Para permitir a uma extensão o poder para fazer isso, ela precisa implementar
:class:`Symfony\\Component\\DependencyInjection\\Extension\\PrependExtensionInterface`::

    // src/Acme/HelloBundle/DependencyInjection/AcmeHelloExtension.php
    namespace Acme\HelloBundle\DependencyInjection;

    use Symfony\Component\HttpKernel\DependencyInjection\Extension;
    use Symfony\Component\DependencyInjection\Extension\PrependExtensionInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;

    class AcmeHelloExtension extends Extension implements PrependExtensionInterface
    {
        // ...

        public function prepend(ContainerBuilder $container)
        {
            // ...
        }
    }

Dentro do método :method:`Symfony\\Component\\DependencyInjection\\Extension\\PrependExtensionInterface::prepend`
, os desenvolvedores têm acesso total à instância :class:`Symfony\\Component\\DependencyInjection\\ContainerBuilder`
pouco antes do método :method:`Symfony\\Component\\DependencyInjection\\Extension\\ExtensionInterface::load`
ser chamado em cada uma das extensões de bundle registradas. A fim de
prefixar configurações para uma extensão de bundle, os desenvolvedores podem usar o
método :method:`Symfony\\Component\\DependencyInjection\\ContainerBuilder::prependExtensionConfig`
na instância :class:`Symfony\\Component\\DependencyInjection\\ContainerBuilder`
. Como esse método só prefixa configurações, quaisquer outras configurações feitas explicitamente
dentro de ``app/config/config.yml`` iriam sobrescrever essas configurações prefixadas.

O exemplo a seguir ilustra como prefixar
uma definição de configuração em vários bundles, bem como desativar uma flag em vários bundles
no caso de um outro bundle específico não estar registrado::

    public function prepend(ContainerBuilder $container)
    {
        // get all bundles
        $bundles = $container->getParameter('kernel.bundles');
        // determine if AcmeGoodbyeBundle is registered
        if (!isset($bundles['AcmeGoodbyeBundle'])) {
            // disable AcmeGoodbyeBundle in bundles
            $config = array('use_acme_goodbye' => false);
            foreach ($container->getExtensions() as $name => $extension) {
                switch ($name) {
                    case 'acme_something':
                    case 'acme_other':
                        // set use_acme_goodbye to false in the config of
                        // acme_something and acme_other note that if the user manually
                        // configured use_acme_goodbye to true in the app/config/config.yml
                        // then the setting would in the end be true and not false
                        $container->prependExtensionConfig($name, $config);
                        break;
                }
            }
        }

        // process the configuration of AcmeHelloExtension
        $configs = $container->getExtensionConfig($this->getAlias());
        // use the Configuration class to generate a config array with
        // the settings "acme_hello"
        $config = $this->processConfiguration(new Configuration(), $configs);

        // check if entity_manager_name is set in the "acme_hello" configuration
        if (isset($config['entity_manager_name'])) {
            // prepend the acme_something settings with the entity_manager_name
            $config = array('entity_manager_name' => $config['entity_manager_name']);
            $container->prependExtensionConfig('acme_something', $config);
        }
    }

O código acima seria o equivalente a escrever o seguinte em
``app/config/config.yml`` no caso de AcmeGoodbyeBundle não estar registado e a
definição ``entity_manager_name`` para ``acme_hello`` estar setada para ``non_default``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        acme_something:
            # ...
            use_acme_goodbye: false
            entity_manager_name: non_default

        acme_other:
            # ...
            use_acme_goodbye: false

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <acme-something:config use-acme-goodbye="false">
            <acme-something:entity-manager-name>non_default</acme-something:entity-manager-name>
        </acme-something:config>

        <acme-other:config use-acme-goodbye="false" />

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('acme_something', array(
            // ...
            'use_acme_goodbye' => false,
            'entity_manager_name' => 'non_default',
        ));
        $container->loadFromExtension('acme_other', array(
            // ...
            'use_acme_goodbye' => false,
        ));
