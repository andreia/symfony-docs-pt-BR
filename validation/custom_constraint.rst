.. index::
   single: Validação; Constraints Personalizadas

Como criar uma Constraint de Validação Personalizada
----------------------------------------------------

Você pode criar uma constraint personalizada estendendo uma classe base de constraint
:class:`Symfony\\Component\\Validator\\Constraint`.
Como exemplo vamos criar um validador simples que verifica se uma string
contém apenas caracteres alfanuméricos.

Criando a Classe Constraint
---------------------------

Primeiro você precisa criar uma classe de Constraint e estender :class:`Symfony\\Component\\Validator\\Constraint`::

    // src/Acme/DemoBundle/Validator/constraints/ContainsAlphanumeric.php
    namespace Acme\DemoBundle\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;

    /**
     * @Annotation
     */
    class ContainsAlphanumeric extends Constraint
    {
        public $message = 'The string "%string%" contains an illegal character: it can only contain letters or numbers.';
    }

.. note::

    A annotation ``@Annotation`` é necessária para esta nova constraint para
    torná-la disponível para uso em classes através de annotations.
    Opções para a sua constraint são representadas como propriedades públicas na
    classe constraint.

Criando o Validador em si
-------------------------

Como você pode ver, uma classe de constraint é muito curta. A validação real é
realizada por uma outra classe "validadora de constraint". A classe validadora
de constraint é especificada pelo método de constraint ``validatedBy()``, que
inclui alguma lógica padrão simples::

    // in the base Symfony\Component\Validator\Constraint class
    public function validatedBy()
    {
        return get_class($this).'Validator';
    }

Em outras palavras, se você criar uma ``Constraint`` personalizada
(Ex. ``MyConstraint``), o Symfony2 automaticamente irá procurar a outra
classe ``MyConstraintValidator`` quando realmente executar a validação.

A classe validadora também é simples, e só contém um método necessário: ``validate``::

    // src/Acme/DemoBundle/Validator/Constraints/ContainsAlphanumericValidator.php
    namespace Acme\DemoBundle\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;
    use Symfony\Component\Validator\ConstraintValidator;

    class ContainsAlphanumericValidator extends ConstraintValidator
    {
        public function validate($value, Constraint $constraint)
        {
            if (!preg_match('/^[a-zA-Za0-9]+$/', $value, $matches)) {
                $this->context->addViolation($constraint->message, array('%string%' => $value));
            }
        }
    }

.. note::

    O método ``validate`` não retorna um valor, em vez disso, ele acrescenta violações
    à propriedade ``context`` do validador com uma chamada do método ``addViolation``
    se existem falhas de validação. Portanto, um valor pode ser considerado como sendo 
    válido, desde que não cause violações adicionadas ao contexto. O primeiro 
    parâmetro da chamada ``addViolation`` é a mensagem de erro para usar para 
    aquela violação.

.. versionadded:: 2.1
    O método ``isValid`` foi renomeado para ``validate`` no Symfony 2.1. O 
    método ``setMessage`` também ficou obsoleto, em favor da chamada ``addViolation``
    do contexto.

Usando o novo Validador
-----------------------

Usar validadores personalizados é muito fácil, assim como os fornecidos pelo Symfony2 em si:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\DemoBundle\Entity\AcmeEntity:
            properties:
                name:
                    - NotBlank: ~
                    - Acme\DemoBundle\Validator\Constraints\ContainsAlphanumeric: ~

    .. code-block:: php-annotations

        // src/Acme/DemoBundle/Entity/AcmeEntity.php
        use Symfony\Component\Validator\Constraints as Assert;
        use Acme\DemoBundle\Validator\Constraints as AcmeAssert;

        class AcmeEntity
        {
            // ...

            /**
             * @Assert\NotBlank
             * @AcmeAssert\ContainsAlphanumeric
             */
            protected $name;

            // ...
        }

    .. code-block:: xml

        <!-- src/Acme/DemoBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\DemoBundle\Entity\AcmeEntity">
                <property name="name">
                    <constraint name="NotBlank" />
                    <constraint name="Acme\DemoBundle\Validator\Constraints\ContainsAlphanumeric" />
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Acme/DemoBundle/Entity/AcmeEntity.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Acme\DemoBundle\Validator\Constraints\ContainsAlphanumeric;

        class AcmeEntity
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
                $metadata->addPropertyConstraint('name', new ContainsAlphanumeric());
            }
        }

Se a sua constraint contém opções, então elas devem ser propriedades públicas na 
classe Constraint personalizada que você criou anteriormente. Essas opções podem 
ser configuradas como opções nas constraints do núcleo do Symfony.

Validadores de Constraints com Dependências
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se o seu validador de restrição possui dependências, como uma conexão de banco
de dados, ela terá que ser configurada como um serviço no container de injeção
de dependência. Este serviço deve incluir a tag
``validator.constraint_validator`` e um atributo ``alias``:

.. configuration-block::

    .. code-block:: yaml

        services:
            validator.unique.your_validator_name:
                class: Fully\Qualified\Validator\Class\Name
                tags:
                    - { name: validator.constraint_validator, alias: alias_name }

    .. code-block:: xml

        <service id="validator.unique.your_validator_name" class="Fully\Qualified\Validator\Class\Name">
            <argument type="service" id="doctrine.orm.default_entity_manager" />
            <tag name="validator.constraint_validator" alias="alias_name" />
        </service>

    .. code-block:: php

        $container
            ->register('validator.unique.your_validator_name', 'Fully\Qualified\Validator\Class\Name')
            ->addTag('validator.constraint_validator', array('alias' => 'alias_name'));

Sua classe de constraint pode agora usar este alias para referenciar o
validador apropriado::

    public function validatedBy()
    {
        return 'alias_name';
    }

Como mencionado acima, o Symfony2 irá procurar automaticamente por uma classe chamada após
a constraint, com ``Validator`` acrescentado. Se o seu validador constraint 
está definido como um serviço, é importante que você sobrescreva o método ``validatedBy()``
para retornar o alias utilizado na definição de seu serviço, caso contrário, o Symfony2
não vai usar o serviço do validador de constraint, e, em vez disso, irá instanciar a classe, 
sem quaisquer dependências injetadas.

Classe Constraint Validadora
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Junto da validação de uma propriedade de classe, uma constraint pode ter um escopo de 
classe, fornecendo um alvo::

    public function getTargets()
    {
        return self::CLASS_CONSTRAINT;
    }

Com isso, o método validador ``validate()`` obtém um objeto como seu primeiro argumento::

    class ProtocolClassValidator extends ConstraintValidator
    {
        public function validate($protocol, Constraint $constraint)
        {
            if ($protocol->getFoo() != $protocol->getBar()) {
                $this->context->addViolationAtSubPath('foo', $constraint->message, array(), null);
            }
        }
    }

Note que a classe constraint validadora é aplicada na classe em si, e
não à propriedade:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\DemoBundle\Entity\AcmeEntity:
            constraints:
                - ContainsAlphanumeric

    .. code-block:: php-annotations

        /**
         * @AcmeAssert\ContainsAlphanumeric
         */
        class AcmeEntity
        {
            // ...
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\DemoBundle\Entity\AcmeEntity">
            <constraint name="ContainsAlphanumeric" />
        </class>
