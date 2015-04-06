.. index::
    single: Validação; Níveis de Erro
    single: Validação; Payload

Como Lidar com Diferentes Níveis de Erro
========================================

Às vezes, você pode querer exibir mensagens de erro da contraint de validação de forma diferente
com base em algumas regras. Por exemplo, você tem um formulário de inscrição para novos usuários
onde eles informam alguns dados pessoais e escolhem suas credenciais de
autenticação. Eles terão de escolher um nome de usuário e uma senha segura,
mas fornecer as informações da conta bancária será opcional. No entanto, você
quer ter certeza de que esses campos opcionais, se informados, são válidos,
porém exibir seus erros de forma diferente.

O processo para atingir esse comportamento consiste de dois passos:

#. Aplicar diferentes níveis de erro para as constraints de validação;
#. Personalizar suas mensagens de erro, dependendo do nível de erro configurado.

1. Atribuir o nível de erro
---------------------------

.. versionadded:: 2.6
    A opção `` payload`` foi introduzida no Symfony 2.6.

Use a opção ``payload`` para configurar o nível de erro para cada constraint:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            /**
             * @Assert\NotBlank(payload = {severity = "error"})
             */
            protected $username;

            /**
             * @Assert\NotBlank(payload = {severity = "error"})
             */
            protected $password;

            /**
             * @Assert\Iban(payload = {severity = "warning"})
             */
            protected $bankAccountNumber;
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\User:
            properties:
                username:
                    - NotBlank:
                        payload:
                            severity: error
                password:
                    - NotBlank:
                        payload:
                            severity: error
                bankAccountNumber:
                    - Iban:
                        payload:
                            severity: warning

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\User">
                <property name="username">
                    <constraint name="NotBlank">
                        <option name="payload">
                            <value key="severity">error</value>
                        </option>
                    </constraint>
                </property>
                <property name="password">
                    <constraint name="NotBlank">
                        <option name="payload">
                            <value key="severity">error</value>
                        </option>
                    </constraint>
                </property>
                <property name="bankAccountNumber">
                    <constraint name="Iban">
                        <option name="payload">
                            <value key="severity">warning</value>
                        </option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('username', new Assert\NotBlank(array(
                    'payload' => array('severity' => 'error'),
                )));
                $metadata->addPropertyConstraint('password', new Assert\NotBlank(array(
                    'payload' => array('severity' => 'error'),
                )));
                $metadata->addPropertyConstraint('bankAccountNumber', new Assert\Iban(array(
                    'payload' => array('severity' => 'warning'),
                )));
            }
        }

2. Personalizar o Template de Mensagem de Erro
----------------------------------------------

.. versionadded:: 2.6
    O método ``getConstraint()`` na classe ``ConstraintViolation`` foi
    introduzido no Symfony 2.6.

Quando a validação do objeto ``User`` falhar, você pode recuperar a constraint
que causou uma falha em particular usando o
método :method:`Symfony\\Component\\Validator\\ConstraintViolation::getConstraint`
. Cada constraint expõe o payload anexado como uma propriedade pública::

    // a constraint validation failure, instance of
    // Symfony\Component\Validator\ConstraintViolation
    $constraintViolation = ...;
    $constraint = $constraintViolation->getConstraint();
    $severity = isset($constraint->payload['severity']) ? $constraint->payload['severity'] : null;

Por exemplo, você pode aproveitar isso para personalizar o bloco ``form_errors``
de modo que severity é adicionada como uma classe HTML adicional:

.. code-block:: html+jinja

    {%- block form_errors -%}
        {%- if errors|length > 0 -%}
        <ul>
            {%- for error in errors -%}
                {% if error.cause.constraint.payload.severity is defined %}
                    {% set severity = error.cause.constraint.payload.severity %}
                {% endif %}
                <li{% if severity is defined %} class="{{ severity }}"{% endif %}>{{ error.message }}</li>
            {%- endfor -%}
        </ul>
        {%- endif -%}
    {%- endblock form_errors -%}

.. seealso::

    Para obter mais informações sobre como personalizar a renderização do formulário, consulte :doc:`/cookbook/form/form_customization`.
