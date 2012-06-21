.. index::
   single: Validation; Custom constraints

Como criar uma Regra de Validação Personalizada
--------------------------------------------

Você pode criar uma regra de validação extendendo uma classe de restrição
básica, :class:`Symfony\\Component\\Validator\\Constraint`. Opções para a regra
são representadas como propriedades públicas na classe de restrição. Por
exemplo, a regra :doc:`Url</reference/constraints/Url>` inclui as propriedades
``message`` e ``protocols``:

.. code-block:: php

    namespace Symfony\Component\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;

    /**
     * @Annotation
     */
    class Url extends Constraint
    {
        public $message = 'This value is not a valid URL';
        public $protocols = array('http', 'https', 'ftp', 'ftps');
    }

.. note::

    A annotation ``@Annotation`` é necessária para esta nova regra para
    torná-la disponível para uso em classes via annotations.

Como você pode ver, uma classe de restrição é muito curta. A validação real é
realizada por uma outra classe "validadora de restrição". A classe validadora
de restrição é especificada pelo método de restrição``validatedBy()``, que
inclui alguma lógica lógica padrão simples:

.. code-block:: php

    // in the base Symfony\Component\Validator\Constraint class
    public function validatedBy()
    {
        return get_class($this).'Validator';
    }

Em outras palavras, se você criar uma ``Constraint`` personalizada
(e.g. ``MyConstraint``), o Symfony2 automaticamente irá procurar a outra
classe ``MyConstraintValidator`` quando realmente executar a validação.

A classe validadora também é simples, e só tem um método necessário:
``isValid``. Pegue o ``NotBlankValidator`` como exemplo.

.. code-block:: php

    class NotBlankValidator extends ConstraintValidator
    {
        public function isValid($value, Constraint $constraint)
        {
            if (null === $value || '' === $value) {
                $this->setMessage($constraint->message);

                return false;
            }

            return true;
        }
    }

Validadores de Restrição com Dependências
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se seu validador de restrição possui dependências, como uma conexão de banco
de dados, ela terá que ser configurada como um serviço no recipiente de injeção
de dependência. Este serviço deve incluir a tag
``validator.constraint_validator``e um atributo ``alias``:

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
            ->addTag('validator.constraint_validator', array('alias' => 'alias_name'))
        ;

Sua classe de restrição pode agora usar estes alias para referenciar o
validador apropriado::

    public function validatedBy()
    {
        return 'alias_name';
    }