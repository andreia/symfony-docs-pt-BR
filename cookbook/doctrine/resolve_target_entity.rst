.. index::
   single: Doctrine; Resolvendo entidades alvo
   single: Doctrine; Definir relações com classes abstratas e interfaces

Como Definir Relações com Classes Abstratas e Interfaces
========================================================

Um dos objetivos dos bundles é criar bundles discretos de funcionalidade
que não tem muitas (se houver) dependências, permitindo que você use a
funcionalidade em outras aplicações sem incluir itens desnecessários.

O Doctrine 2.2 inclui um novo utilitário chamado ``ResolveTargetEntityListener``,
que funciona interceptando certas chamadas dentro do Doctrine e reescrevendo
parâmetros ``targetEntity`` no seu mapeamento de metadados em tempo de execução. Isso significa que
você pode usar em seu bundle uma interface ou uma classe abstrata nos
mapeamentos e esperar o mapeamento correto para uma entidade concreta em tempo de execução.

Essa funcionalidade permite que você defina as relações entre diferentes entidades
sem torná-las dependências.

Contexto
--------

Suponha que você tenha um `InvoiceBundle` que fornece a funcionalidade de faturamento
e um `CustomerBundle` que contém ferramentas de gestão de clientes. Você quer
mantê-los separados, porque eles podem ser utilizados em outros sistemas separadamente
, mas na sua aplicação você deseja usá-los juntos.

Nesse caso, você tem uma entidade ``Invoice`` com uma relação a um
objeto inexistente, uma ``InvoiceSubjectInterface``. O objetivo é fazer com que
o ``ResolveTargetEntityListener`` substitua qualquer menção da interface
com um objeto real que implementa essa interface.

Configuração
------------

Este artigo utiliza as duas seguintes entidades básicas (que estão incompletas para
brevidade) para explicar como configurar e usar o ``ResolveTargetEntityListener``.

Uma entidade Customer::

    // src/Acme/AppBundle/Entity/Customer.php

    namespace Acme\AppBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Acme\CustomerBundle\Entity\Customer as BaseCustomer;
    use Acme\InvoiceBundle\Model\InvoiceSubjectInterface;

    /**
     * @ORM\Entity
     * @ORM\Table(name="customer")
     */
    class Customer extends BaseCustomer implements InvoiceSubjectInterface
    {
        // In this example, any methods defined in the InvoiceSubjectInterface
        // are already implemented in the BaseCustomer
    }

Uma entidade Invoice::

    // src/Acme/InvoiceBundle/Entity/Invoice.php

    namespace Acme\InvoiceBundle\Entity;

    use Doctrine\ORM\Mapping AS ORM;
    use Acme\InvoiceBundle\Model\InvoiceSubjectInterface;

    /**
     * Represents an Invoice.
     *
     * @ORM\Entity
     * @ORM\Table(name="invoice")
     */
    class Invoice
    {
        /**
         * @ORM\ManyToOne(targetEntity="Acme\InvoiceBundle\Model\InvoiceSubjectInterface")
         * @var InvoiceSubjectInterface
         */
        protected $subject;
    }

Uma InvoiceSubjectInterface::

    // src/Acme/InvoiceBundle/Model/InvoiceSubjectInterface.php

    namespace Acme\InvoiceBundle\Model;

    /**
     * An interface that the invoice Subject object should implement.
     * In most circumstances, only a single object should implement
     * this interface as the ResolveTargetEntityListener can only
     * change the target to a single object.
     */
    interface InvoiceSubjectInterface
    {
        // List any additional methods that your InvoiceBundle
        // will need to access on the subject so that you can
        // be sure that you have access to those methods.

        /**
         * @return string
         */
        public function getName();
    }

Em seguida, você precisa configurar o listener, que informa ao DoctrineBundle
sobre a substituição:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        doctrine:
            # ...
            orm:
                # ...
                resolve_target_entities:
                    Acme\InvoiceBundle\Model\InvoiceSubjectInterface: Acme\AppBundle\Entity\Customer

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:orm>
                    <!-- ... -->
                    <doctrine:resolve-target-entity interface="Acme\InvoiceBundle\Model\InvoiceSubjectInterface">Acme\AppBundle\Entity\Customer</doctrine:resolve-target-entity>
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('doctrine', array(
            'orm' => array(
                // ...
                'resolve_target_entities' => array(
                    'Acme\InvoiceBundle\Model\InvoiceSubjectInterface' => 'Acme\AppBundle\Entity\Customer',
                ),
            ),
        ));

Considerações Finais
--------------------

Com o ``ResolveTargetEntityListener``, você pode desacoplar seus
bundles, mantendo-os utilizáveis ​​por si mesmos, mas ainda podendo
definir relações entre objetos diferentes. Ao utilizar esse método,
os seus bundles serão mais fáceis de manter de forma independente.
