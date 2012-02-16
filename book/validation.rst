.. index::
   single: Validação

Validação
==========

Validação é uma tarefa muito comum em aplicações web. Dado inserido em formulário
precisa ser validado. Dado também precisa ser revalidado antes de ser escrito 
num banco de dados ou passado a um serviço web.

Symfony2 vem acompanhado com um componente `Validator`_ que torna essa tarefa fácil e transparente.
Esse componente é baseado na especificação `JSR303 Bean Validation`_. O quê ?
Uma especificação Java no PHP? Você ouviu corretamente, mas não é tão ruim quanto parece.
Vamos olhar como isso pode ser usado no PHP.

.. index:
   single: Validação; As bases

As bases da validação
------------------------

A melhor forma de entender validação é vê-la em ação. Para começar, suponha 
que você criou um bom e velho objeto PHP que você precisa usar em algum lugar da
sua aplicação:

.. code-block:: php

    // src/Acme/BlogBundle/Entity/Author.php
    namespace Acme\BlogBundle\Entity;

    class Author
    {
        public $name;
    }

Até agora, essa é somente uma classe comum que serve para alguns propósitos
dentro de sua aplicação. O objetivo da validação é avisar você se um dado de
um objeto é válido ou não. Para esse trabalho, você irá configura uma lista
de regras (chamada :ref:`constraints<validation-constraints>`) em que o objeto
deve seguir em ordem para ser validado. Essas regras podem ser especificadas por
um número de diferentes formatos (YAML, XML, annotations, ou PHP).

Por exemplo, para garantir que a propriedade ``$name`` não é vazia, adicione o
seguinte: 

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                name:
                    - NotBlank: ~

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank()
             */
            public $name;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="name">
                    <constraint name="NotBlank" />
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
            }
        }

.. tip::
   
    Propriedades protected e private podem também ser validadas, bem como os métodos
   "getter" (veja `validator-constraint-targets`

.. index::
   single: Validação; Usando o validator

Usando o serviço ``validator`` 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Próximo passo, para realmente validar um objeto``Author``, use o método ``validate``
no serviço ``validator`` (classe :class:`Symfony\\Component\\Validator\\Validator`).
A tarefa do ``validator`` é fácil: ler as restrições (i.e. regras) 
de uma classe e verificar se o dado no objeto satisfaz ou não aquelas restrições.
Se a validação falhar, retorna um array de erros. Observe esse
simples exemplo de dentro do controller:

.. code-block:: php

    use Symfony\Component\HttpFoundation\Response;
    use Acme\BlogBundle\Entity\Author;
    // ...

    public function indexAction()
    {
        $author = new Author();
        // ... do something to the $author object

        $validator = $this->get('validator');
        $errors = $validator->validate($author);

        if (count($errors) > 0) {
            return new Response(print_r($errors, true));
        } else {
            return new Response('The author is valid! Yes!');
        }
    }

Se a propriedade ``$name`` é vazia,  você verá a seguinte mensagem de
erro:

.. code-block:: text

    Acme\BlogBundle\Author.name:
        This value should not be blank

Se você inserir um valor na propriedade ``name``, aparecerá a feliz mensagem 
de sucesso.

.. tip::

    A maior parte do tempo, você não irá interagir diretamente com o serviço
    ``validator`` ou precisará se preocupar sobre imprimir os erros. A maior parte do tempo,
    você irá usar a validação indiretamente quando lidar com dados enviados do formulário.
    Para mais informações, veja: ref:`book-validation-forms`.

Você também poderia passar o conjunto de erros em um template.

.. code-block:: php

    if (count($errors) > 0) {
        return $this->render('AcmeBlogBundle:Author:validate.html.twig', array(
            'errors' => $errors,
        ));
    } else {
        // ...
    }

Dentro do template, você pode gerar a lista de erros exatamente necessária:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Author/validate.html.twig #}

        <h3>The author has the following errors</h3>
        <ul>
        {% for error in errors %}
            <li>{{ error.message }}</li>
        {% endfor %}
        </ul>

    .. code-block:: html+php

        <!-- src/Acme/BlogBundle/Resources/views/Author/validate.html.php -->

        <h3>The author has the following errors</h3>
        <ul>
        <?php foreach ($errors as $error): ?>
            <li><?php echo $error->getMessage() ?></li>
        <?php endforeach; ?>
        </ul>

.. note::

    Cada erro de validação (chamado de "constraint violation"), é representado por
    um objeto :class:`Symfony\\Component\\Validator\\ConstraintViolation` .

.. index::
   single: Validação; Validação com formulários

.. _book-validation-forms:

Validação e formulários
~~~~~~~~~~~~~~~~~~~~

O serviço ``validator`` pode ser usado a qualquer momento para validar qualquer objeto.
Na realidade, entretanto, você irá trabalhar frequentemente com o ``validator`` indiretamente
enquanto trabalhar com formulário. A biblioteca Symfony's form usa o serviço ``validator``
internamente para validar o objeto oculto após os valores terem sido enviados
e fixados. As violações de restrição no objeto são convertidas em objetos ``FieldError``
que podem ser facilmente exibidos com seu formulário. O tipico fluxo de envio do formulário
parece o seguinte dentro do controller::

    use Acme\BlogBundle\Entity\Author;
    use Acme\BlogBundle\Form\AuthorType;
    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function updateAction(Request $request)
    {
        $author = new Acme\BlogBundle\Entity\Author();
        $form = $this->createForm(new AuthorType(), $author);

        if ($request->getMethod() == 'POST') {
            $form->bindRequest($request);

            if ($form->isValid()) {
                // the validation passed, do something with the $author object

                $this->redirect($this->generateUrl('...'));
            }
        }

        return $this->render('BlogBundle:Author:form.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. note::

    Esse exemplo usa uma classe de formulários ``AuthorType`` , que não é mostrada aqui.

Para mais informações, veja: doc:`Forms</book/forms>` chapter.

.. index::
   pair: Validação; Configuração

.. _book-validation-configuration:

Configuração
-------------

O validador do Symfony2 é abilitado por padrão, mas você deve abilitar explicitamente anotações
se você usar o método de anotação para especificar suas restrições:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            validation: { enable_annotations: true }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:validation enable_annotations="true" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array('validation' => array(
            'enable_annotations' => true,
        )));

.. index::
   single: Validação; Restrições

.. _validation-constraints:

Restrições
-----------

O ``validator`` é designado para validar objtos perante *restrições* (i.e.
regras). Em ordem para validar um objeto, simplesmente mapeie uma ou mais restrições 
para aquela classe e então passe para o serviço ``validator``.

Por trás dos bastidores, uma restrição é simplesmente um objeto PHP que faz uma sentença
afirmativa. Na vida real, uma restrição poderia ser: "O bolo não deve queimar".
No Symfony2, restrições são similares: elas são afirmações que uma condição 
é verdadeira. Dado um valor, a restriçao irá indicar a você se o valor adere ou não às
regras da restrição.


Restições Suportadas
~~~~~~~~~~~~~~~~~~~~~

Symfony2 engloba um grande número de restrições mais frequentemente usadas:

.. include:: /reference/constraints/map.rst.inc

Você também pode criar sua própria restrição personalizada. Esse tópico é coberto
no artigo do cookbook ":doc:`/cookbook/validation/custom_constraint`"  .

.. index::
   single: Validação; Configuração de Restrições

.. _book-validation-constraint-configuration:

Configuração de restrições
~~~~~~~~~~~~~~~~~~~~~~~~

Algumas restrições, como :doc:`NotBlank</reference/constraints/NotBlank>`,
são simples como as outras, como a restrição :doc:`Choice</reference/constraints/Choice>`
, tem várias opções de configuração disponíveis. Suponha que a classe``Author`` 
tenha outra propriedade, ``gender`` que possa ser configurado como
"male" ou "female":

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                gender:
                    - Choice: { choices: [male, female], message: Choose a valid gender. }

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice(
             *     choices = { "male", "female" },
             *     message = "Choose a valid gender."
             * )
             */
            public $gender;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="gender">
                    <constraint name="Choice">
                        <option name="choices">
                            <value>male</value>
                            <value>female</value>
                        </option>
                        <option name="message">Choose a valid gender.</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $gender;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('gender', new Choice(array(
                    'choices' => array('male', 'female'),
                    'message' => 'Choose a valid gender.',
                )));
            }
        }

.. _validation-default-option:

A opção de uma restrição pode sempre ser passada como um array. Algumas restrições,
entretanto, também permitem a você passar o valor de uma opção "*default*" no lugar 
do array. No cado da restrição ``Choice`` , as opções ``choices``
podem ser espeficadas dessa forma.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                gender:
                    - Choice: [male, female]

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice({"male", "female"})
             */
            protected $gender;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="gender">
                    <constraint name="Choice">
                        <value>male</value>
                        <value>female</value>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\Choice;

        class Author
        {
            protected $gender;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('gender', new Choice(array('male', 'female')));
            }
        }

Isso significa simplesmente fazer a configuração da opção mais comum de uma restrição
mais curta e rápida.

Se você está incerto de como especificar uma opção, ou verifique a documentação da API
para a restrição ou faça de forma segura sempre passando um array de opções 
(o primeiro método mostrado acima).

.. index::
   single: Validação; Escopo da restrição

.. _validator-constraint-targets:

Escopos da restrição
------------------

Restrições podem ser aplicadas a uma propriedade de classe (e.g. ``name``) ou
um método getter público (e.g. ``getFullName``). O primeiro é mais comum e fácil
de usar, mas o segundo permite você especificar regras de validação mais complexas.

.. index::
   single: Validação; Restrições de propriedades

.. _validation-property-target:

Propriedads
~~~~~~~~~~

Validar as propriedades de uma classe é a técnica de validação mais básica.Symfony2
permite a você validar propriedades private, protected ou public. A próxima listagem
mostra a você como configurar a propriedade ``$firstName`` da classe ``Author``
para ter ao menos 3 caracteres.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                firstName:
                    - NotBlank: ~
                    - MinLength: 3

    .. code-block:: php-annotations

        // Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank()
             * @Assert\MinLength(3)
             */
            private $firstName;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\BlogBundle\Entity\Author">
            <property name="firstName">
                <constraint name="NotBlank" />
                <constraint name="MinLength">3</constraint>
            </property>
        </class>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\MinLength;

        class Author
        {
            private $firstName;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('firstName', new NotBlank());
                $metadata->addPropertyConstraint('firstName', new MinLength(3));
            }
        }

.. index::
   single: Validação; Restrições getter

Getters
~~~~~~~

Restrições podem também ser aplicadas no método de retorno de um valor.Symfony2
permite a você adicionar uma restrição para qualquer método public cujo nome comec com
"get" ou "is". Nesse guia, ambos os tipos de métodos são referidos
como "getters".

O benefício dessa técnica é que permite a você validar seu objeto 
dinamicamente. Por exemplo, suponha que você queira ter certeza que um campo de senha
não coincida com o primeiro nome do usuário (por motivos de segurança). Você pode
fazer isso criando um método ``isPasswordLegal``, e então afirmando que
esse método deva retornar ''true'':

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            getters:
                passwordLegal:
                    - "True": { message: "The password cannot match your first name" }

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\True(message = "The password cannot match your first name")
             */
            public function isPasswordLegal()
            {
                // return true or false
            }
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\BlogBundle\Entity\Author">
            <getter property="passwordLegal">
                <constraint name="True">
                    <option name="message">The password cannot match your first name</option>
                </constraint>
            </getter>
        </class>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\True;

        class Author
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addGetterConstraint('passwordLegal', new True(array(
                    'message' => 'The password cannot match your first name',
                )));
            }
        }

Agora, crie o método ``isPasswordLegal()`` , e inclua a lógica que você precisa::

    public function isPasswordLegal()
    {
        return ($this->firstName != $this->password);
    }

.. note::
   
    Com uma visão apurada, você irá perceber que o prefixo do getter
    ("get" ou "is") é omitido no mapeamento. Isso permite você mover a 
    restrição para uma propriedade com o mesmo nome mais tarde (ou vice-versa) sem
    mudar sua lógica de validação.

.. _validation-class-target:

Classes
~~~~~~~

Algumas restrições aplicam para a classe inteira ser validada. Por exemplo,
a restrição :doc:`Callback</reference/constraints/Callback>` é uma restrição
genérica que é aplicada para a própria classe. Quando a classe é validada,
métodos especificados por aquela restrição são simplesmente executadas então cada um pode
prover uma validação mais personalizada.

.. _book-validation-validation-groups:

Grupos de validação
-----------------

Até agora, você foi capaz de adicionar restrições a uma classe e perguntar se aquela 
classe passa ou não por todas as restrições definidas. Em alguns casos, entretanto,
você precisará validar um objeto a somente *algumas* das restrições 
naqula classe. Para fazer isso, você pode organizar cada restrição dentro de um ou mais
"grupos de validação", e então aplicar validação a apenas um grupo de restrições.

Por exemplo, suponha que você tenha uma classe ``User`` , que é usada tanto quando um
usuário registra e quando um usuário atualiza sua informações de contato posteriormente:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\User:
            properties:
                email:
                    - Email: { groups: [registration] }
                password:
                    - NotBlank: { groups: [registration] }
                    - MinLength: { limit: 7, groups: [registration] }
                city:
                    - MinLength: 2

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/User.php
        namespace Acme\BlogBundle\Entity;

        use Symfony\Component\Security\Core\User\UserInterface;
        use Symfony\Component\Validator\Constraints as Assert;

        class User implements UserInterface
        {
            /**
            * @Assert\Email(groups={"registration"})
            */
            private $email;

            /**
            * @Assert\NotBlank(groups={"registration"})
            * @Assert\MinLength(limit=7, groups={"registration"})
            */
            private $password;

            /**
            * @Assert\MinLength(2)
            */
            private $city;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\BlogBundle\Entity\User">
            <property name="email">
                <constraint name="Email">
                    <option name="groups">
                        <value>registration</value>
                    </option>
                </constraint>
            </property>
            <property name="password">
                <constraint name="NotBlank">
                    <option name="groups">
                        <value>registration</value>
                    </option>
                </constraint>
                <constraint name="MinLength">
                    <option name="limit">7</option>
                    <option name="groups">
                        <value>registration</value>
                    </option>
                </constraint>
            </property>
            <property name="city">
                <constraint name="MinLength">7</constraint>
            </property>
        </class>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/User.php
        namespace Acme\BlogBundle\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\Email;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\MinLength;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('email', new Email(array(
                    'groups' => array('registration')
                )));

                $metadata->addPropertyConstraint('password', new NotBlank(array(
                    'groups' => array('registration')
                )));
                $metadata->addPropertyConstraint('password', new MinLength(array(
                    'limit'  => 7,
                    'groups' => array('registration')
                )));

                $metadata->addPropertyConstraint('city', new MinLength(3));
            }
        }

Com essa configuração, existem dois grupos de validação:

* ``Default`` - contém as restrições não atribuidas a qualquer outro grupo;

* ``registration`` - Contém as restrições somente nos campos ``email`` e ``password``.

Para avisar o validador a usar um grupo específico, passe um ou mais nomes de trupos
como um segundo argumento para o método``validate()``


    $errors = $validator->validate($author, array('registration'));

Claro, você irá frequentemente trabalhar com validação indiretamnte por meio
da biblioteca do formulário. Para informações em como usar grupos de validação dentro de formulários, 
veja :ref:`book-forms-validation-groups`.

.. index::
   single: Validation; Validating raw values

.. _book-validation-raw-values:

Validando Valores e Arrays
----------------------------

Até agora, você viu como pode validar objetos inteiros. Mas às vezes, você
somente quer validar um valor simples - como verificar se uma string é um endereço
de e-mail válido. Isso é realmente muito fácil de fazer. De dentro do controller,
parece com isso:

    // add this to the top of your class
    use Symfony\Component\Validator\Constraints\Email;
    
    public function addEmailAction($email)
    {
        $emailConstraint = new Email();
        // all constraint "options" can be set this way
        $emailConstraint->message = 'Invalid email address';

        // use the validator to validate the value
        $errorList = $this->get('validator')->validateValue($email, $emailConstraint);

        if (count($errorList) == 0) {
            // this IS a valid email address, do something
        } else {
            // this is *not* a valid email address
            $errorMessage = $errorList[0]->getMessage()
            
            // do somethign with the error
        }
        
        // ...
    }

Ao chamar ``validateValue`` no validador, você pode passar um valor bruto e 
o objeto de restrição que você com o qual você quer validar aquele valor. Uma lista
completa de restrições disponíveis - bem como o nome inteiro da classe para cada
restrição - está disponível em :doc:`constraints reference</reference/constraints>`
section .

O método ``validateValule`` retorna um objeto :class:`Symfony\\Component\\Validator\\ConstraintViolationList`
, que age como um array de erros. Cada erro na coleção é um objeto :
class:`Symfony\\Component\\Validator\\ConstraintViolation` ,
que contém a mensagem de erro no método `getMessage` dele.

Considerações Finais
--------------

O Symfony2 ``validator`` é uma ferramenta poderosa que pode ser multiplicada para
garantir que o dado de qualquer objeto seja "válido". O poder por trás da validação
rside em "restrições", que seão regras que você pode aplicar a propriedades ou métodos
getter de seus objetos. E enquanto você irá usar mais frequentemente usar a validação
do framework indiretamente quando usar formulários, lembre que isso pode ser usado
em qualquer lugar para validar qualquer objeto.


Aprenda mais do Cookbook
----------------------------

* :doc:`/cookbook/validation/custom_constraint`

.. _Validator: https://github.com/symfony/Validator
.. _JSR303 Bean Validation specification: http://jcp.org/en/jsr/detail?id=303
