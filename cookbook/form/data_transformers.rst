.. index::
   single: Formulário; Transformadores de Dados

Como usar os Transformadores de Dados
=====================================

Você frenquentemente encontrará a necessidade de transformar os dados inseridos pelo usuário em um formulário 
para um outro formato para uso em seu programa. Isto pode ser feito manualmente no seu controlador 
facilmente, mas, e se você pretende utilizar este formulário específico em locais diferentes?

Digamos que você tenha uma relação um-para-um da `Task` para o `Issue`, por exemplo, uma `Task` opcionalmente 
tem um `issue` ligado à ela. Adicionando uma caixa de listagem com todos os possíveis `issues`, eventualmente, 
pode levar a uma caixa de listagem muito longa onde é impossível encontrar algo. Em vez disso, você 
poderia adicionar uma caixa de texto, onde o usuário pode simplesmente informar o número do `issue`.

Você poderia tentar fazer isso no seu controlador, mas não é a melhor solução.
Seria melhor se esse `issue` fosse automaticamente convertido em um objeto `Issue`.
É onde os transformadores de dados entram em jogo.

Criando o Transformador
-----------------------

Primeiro, crie uma classe `IssueToNumberTransformer` - esta classe será responsável
por converter de e para o número do `issue` e o objeto `Issue`::

    // src/Acme/TaskBundle/Form/DataTransformer/IssueToNumberTransformer.php
    namespace Acme\TaskBundle\Form\DataTransformer;

    use Symfony\Component\Form\DataTransformerInterface;
    use Symfony\Component\Form\Exception\TransformationFailedException;
    use Doctrine\Common\Persistence\ObjectManager;
    use Acme\TaskBundle\Entity\Issue;

    class IssueToNumberTransformer implements DataTransformerInterface
    {
        /**
         * @var ObjectManager
         */
        private $om;

        /**
         * @param ObjectManager $om
         */
        public function __construct(ObjectManager $om)
        {
            $this->om = $om;
        }

        /**
         * Transforms an object (issue) to a string (number).
         *
         * @param  Issue|null $issue
         * @return string
         */
        public function transform($issue)
        {
            if (null === $issue) {
                return "";
            }

            return $issue->getNumber();
        }

        /**
         * Transforms a string (number) to an object (issue).
         *
         * @param  string $number
         * @return Issue|null
         * @throws TransformationFailedException if object (issue) is not found.
         */
        public function reverseTransform($number)
        {
            if (!$number) {
                return null;
            }

            $issue = $this->om
                ->getRepository('AcmeTaskBundle:Issue')
                ->findOneBy(array('number' => $number))
            ;

            if (null === $issue) {
                throw new TransformationFailedException(sprintf(
                    'An issue with number "%s" does not exist!',
                    $number
                ));
            }

            return $issue;
        }
    }

.. tip::

    Se você deseja que um novo `issue` seja criado quando um número desconhecido é informado, 
    você pode instanciá-lo ao invés de gerar uma ``TransformationFailedException``.

Usando o Transformador
----------------------

Agora que você já tem o transformador construído, você só precisa adicioná-lo ao seu 
campo `issue` de alguma forma.

    Você também pode usar transformadores sem criar um novo tipo de formulário personalizado
    chamando ``addModelTransformer`` (ou ``addViewTransformer`` - ver
    `Transformadores de Modelo e Visão`_) ) em qualquer builder de campo::

        use Symfony\Component\Form\FormBuilderInterface;
        use Acme\TaskBundle\Form\DataTransformer\IssueToNumberTransformer;

        class TaskType extends AbstractType
        {
            public function buildForm(FormBuilderInterface $builder, array $options)
            {
                // ...

                // this assumes that the entity manager was passed in as an option
                $entityManager = $options['em'];
                $transformer = new IssueToNumberTransformer($entityManager);

                // add a normal text field, but add our transformer to it
                $builder->add(
                    $builder->create('issue', 'text')
                        ->addModelTransformer($transformer)
                );
            }

            // ...
        }

Este exemplo requer que você passe no gerenciador de entidade como uma opção
ao criar o seu formulário. Mais tarde, você vai aprender como criar um tipo de campo
``issue`` personalizado para evitar ter de fazer isso no seu controlador::

    $taskForm = $this->createForm(new TaskType(), $task, array(
        'em' => $this->getDoctrine()->getEntityManager(),
    ));

Legal, está feito! O usuário poderá informar um número de `issue` no campo 
texto e ele será transformado novamente em um objeto `Issue`. Isto significa
que, após um `bind` bem sucedido, o framework de Formulário passará um objeto 
`Issue` real para o ``Task::setIssue()`` em vez do número do `issue`.

Se o `issue` não for encontrado, um erro de formulário será criado para esse campo e
sua mensagem de erro pode ser controlada com a opção do campo ``invalid_message``.

.. caution::

    Note que a adição de um transformador exige a utilização de uma sintaxe um pouco mais 
    complicada ao adicionar o campo. O código seguinte está **errado**, já que o transformador
    será aplicado à todo o formulário, em vez de apenas este campo::

        // ISTO ESTÁ ERRADO - O TRANSFORMADOR SERÁ APLICADA A TODO O FORMULÁRIO
        // Veja o exemplo acima para o código correto
        $builder->add('issue', 'text')
            ->addModelTransformer($transformer);

Transformadores de Modelo e Visão
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.1
    Os nomes e método de transformadores foram alterados no Symfony 2.1.
    ``prependNormTransformer`` tornou-se ``addModelTransformer`` e ``appendClientTransformer``
    tornou-se ``addViewTransformer``.

No exemplo acima, o transformador foi utilizado como um transformador de "modelo".
De fato, existem dois tipos diferentes de transformadores e três tipos 
diferentes de dados subjacentes.

Em qualquer formulário, os 3 tipos de dados possíveis são os seguintes:

1) **Dados do modelo** - Estes são os dados no formato usado em sua aplicação
(ex., um objeto ``Issue``). Se você chamar ``Form::getData`` ou ``Form::setData``, 
você está lidando com os dados do "modelo".

2) **Dados Normalizados** - Esta é uma versão normalizada de seus dados, e é comumente
o mesmo que os dados do "modelo" (apesar de não no nosso exemplo). Geralmente ele não é 
usado diretamente.

3) **Dados da Visão** - Este é o formato que é usado para preencher os campos do 
formulário. É também o formato no qual o usuário irá enviar os dados. Quando 
você chama ``Form::bind($data)``, o ``$data`` está no formato de dados da "visão".

Os 2 tipos diferentes de transformadores ajudam a converter de e para cada um destes
tipos de dados:

**Transformadores de Modelo**:
    - ``transform``: "model data" => "norm data"
    - ``reverseTransform``: "norm data" => "model data"

**Transformadores de Visão**:
    - ``transform``: "norm data" => "view data"
    - ``reverseTransform``: "view data" => "norm data"

O transformador que você vai precisar depende de sua situação.

Para utilizar o transformador de visão, chame ``addViewTransformer``.

Então, por que nós usamos o transformador de modelo?
----------------------------------------------------

No nosso exemplo, o campo é um campo ``text``, e nós sempre esperamos que um campo 
texto seja um formato escalar simples, nos formatos "normalizado" e "visão". Por 
esta razão, o transformador mais apropriado é o transformador de "modelo" (que 
converte de/para o formato *normalizado* - número de issue string - para o formato 
*modelo* - objeto `Issue`).

A diferença entre os transformadores é sutil e você deve sempre pensar
sobre o que o dado "normalizado" para um campo deve realmente ser. Por exemplo, o 
dado "normalizado" para um campo ``text`` é uma string, mas é um objeto ``DateTime`` 
para um campo ``date``.

Usando Transformadores em um tipo de campo personalizado
--------------------------------------------------------

No exemplo acima, você aplicou o transformador para um campo ``text`` normal.
Isto foi fácil, mas tem duas desvantagens:

1) Você precisa lembrar de aplicar o transformador sempre que você está adicionando
um campo para números de `issue`

2) Você precisa se preocupar em sempre passar a opção ``em`` quando você está criando
um formulário que usa o transformador.

Devido à isto, você pode optar por :doc:`criar um tipo de campo personalizado</cookbook/form/create_custom_field_type>`.
Primeiro, crie a classe do tipo de campo personalizado::

    // src/Acme/TaskBundle/Form/Type/IssueSelectorType.php
    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Acme\TaskBundle\Form\DataTransformer\IssueToNumberTransformer;
    use Doctrine\Common\Persistence\ObjectManager;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class IssueSelectorType extends AbstractType
    {
        /**
         * @var ObjectManager
         */
        private $om;

        /**
         * @param ObjectManager $om
         */
        public function __construct(ObjectManager $om)
        {
            $this->om = $om;
        }

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $transformer = new IssueToNumberTransformer($this->om);
            $builder->addModelTransformer($transformer);
        }

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'invalid_message' => 'The selected issue does not exist',
            ));
        }

        public function getParent()
        {
            return 'text';
        }

        public function getName()
        {
            return 'issue_selector';
        }
    }

Em seguida, registre o seu tipo como um serviço e use a tag ``form.type``, para que
ele seja reconhecido como um tipo de campo personalizado:

.. configuration-block::

    .. code-block:: yaml

        services:
            acme_demo.type.issue_selector:
                class: Acme\TaskBundle\Form\Type\IssueSelectorType
                arguments: ["@doctrine.orm.entity_manager"]
                tags:
                    - { name: form.type, alias: issue_selector }

    .. code-block:: xml

        <service id="acme_demo.type.issue_selector" class="Acme\TaskBundle\Form\Type\IssueSelectorType">
            <argument type="service" id="doctrine.orm.entity_manager"/>
            <tag name="form.type" alias="issue_selector" />
        </service>

Agora, sempre que você precisa usar o seu tipo de campo especial ``issue_selector``,
é muito fácil::

    // src/Acme/TaskBundle/Form/Type/TaskType.php
    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('task')
                ->add('dueDate', null, array('widget' => 'single_text'));
                ->add('issue', 'issue_selector');
        }

        public function getName()
        {
            return 'task';
        }
    }
