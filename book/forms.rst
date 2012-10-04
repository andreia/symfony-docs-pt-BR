.. index::
   single: Formulários

Formulários
===========

Lidar com formulários HTML é uma das mais comuns - e desafiadoras - tarefas para um 
desenvolvedor web. O Symfony2 integra um componente de formulário que torna fácil a tarefa de lidar 
com formulários. Neste capítulo, você vai construir um formulário complexo a partir do zero,
aprendendo as características mais importantes da biblioteca de formulários ao longo do caminho.

.. note::

   O componente de formulário do Symfony é uma biblioteca independente que pode ser utilizada fora
   de projetos Symfony2. Para mais informações, consulte o `Componente de Formulário do Symfony2`_
   no Github.

.. index::
   single: Formulários; Criando um formulário simples

Criando um formulário simples
-----------------------------

Suponha que você está construindo uma aplicação simples de lista de tarefas que precisará
exibir "tarefas". Devido aos seus usuários terem que editar e criar tarefas, você precisará 
construir um formulário. Mas, antes de começar, primeiro vamos focar na classe genérica ``Task`` 
que representa e armazena os dados de uma única tarefa:

.. code-block:: php

    // src/Acme/TaskBundle/Entity/Task.php
    namespace Acme\TaskBundle\Entity;

    class Task
    {
        protected $task;

        protected $dueDate;

        public function getTask()
        {
            return $this->task;
        }
        public function setTask($task)
        {
            $this->task = $task;
        }

        public function getDueDate()
        {
            return $this->dueDate;
        }
        public function setDueDate(\DateTime $dueDate = null)
        {
            $this->dueDate = $dueDate;
        }
    }

.. note::

   Se você está codificando junto com este exemplo, crie o ``AcmeTaskBundle``
   primeiro, executando o seguinte comando (e aceite todas as opções
   padrão):

   .. code-block:: bash

        php app/console generate:bundle --namespace=Acme/TaskBundle

Essa classe é um "antigo objeto PHP simples", porque, até agora, não tem nada
a ver com Symfony ou qualquer outra biblioteca. É simplesmente um objeto PHP normal
que, diretamente resolve um problema no interior da *sua* aplicação (ou seja, a necessidade de
representar uma tarefa na sua aplicação). Claro, até o final deste capítulo,
você será capaz de enviar dados para uma instância ``Task`` (através de um formulário HTML), validar
os seus dados, e persisti-los para o banco de dados.

.. index::
   single: Formulários; Criando um formulário no controlador

Construindo o Formulário
~~~~~~~~~~~~~~~~~~~~~~~~

Agora que você já criou a classe ``Task``, o próximo passo é criar e renderizar 
o formulário HTML real. No Symfony2, isto é feito através da construção de um objeto 
de formulário e, em seguida, renderizando em um template. Por ora, tudo isso pode 
ser feito dentro de um controlador::

    // src/Acme/TaskBundle/Controller/DefaultController.php
    namespace Acme\TaskBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Acme\TaskBundle\Entity\Task;
    use Symfony\Component\HttpFoundation\Request;

    class DefaultController extends Controller
    {
        public function newAction(Request $request)
        {
            // create a task and give it some dummy data for this example
            $task = new Task();
            $task->setTask('Write a blog post');
            $task->setDueDate(new \DateTime('tomorrow'));

            $form = $this->createFormBuilder($task)
                ->add('task', 'text')
                ->add('dueDate', 'date')
                ->getForm();

            return $this->render('AcmeTaskBundle:Default:new.html.twig', array(
                'form' => $form->createView(),
            ));
        }
    }

.. tip::

   Este exemplo mostra como construir o seu formulário diretamente no controlador.
   Mais tarde, na seção ":ref:`book-form-creating-form-classes`", você aprenderá
   como construir o seu formulário em uma classe independente, que é o recomendado 
   pois torna o seu formulário reutilizável.

A criação de um formulário requer relativamente pouco código porque os objetos de formulário do Symfony2
são construídos com um "construtor de formulários". A finalidade do construtor de formulários é permitir 
que você escreva "receitas" simples de formulários, e ele fazer todo o trabalho pesado, de, realmente,
construir o formulário.

Neste exemplo, você acrescentou dois campos ao seu formulário - ``task`` e ``dueDate`` -
que correspondem as propriedades ``task`` e ``dueDate`` da classe ``Task``.
Você também atribuiu a cada um deles um "type" (exemplo: ``text``, ``date``), que, entre
outras coisas, determina qual(ais) tag(s) HTML de formulário serão renderizadas para esse campo.

O Symfony2 vem com muitos tipos embutidos, que serão discutidos em breve
(veja :ref:`book-forms-type-reference`).

.. index::
  single: Forms; Renderização básica do template

Renderizando o Formulário
~~~~~~~~~~~~~~~~~~~~~~~~~

Agora que o formulário foi criado, o próximo passo é renderizá-lo. Isto é
feito passando um objeto "view" especial para o seu template (note o
``$form->createView()`` no controlador acima) e usando um conjunto de funções helper
para o formulário:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/TaskBundle/Resources/views/Default/new.html.twig #}

        <form action="{{ path('task_new') }}" method="post" {{ form_enctype(form) }}>
            {{ form_widget(form) }}

            <input type="submit" />
        </form>

    .. code-block:: html+php

        <!-- src/Acme/TaskBundle/Resources/views/Default/new.html.php -->

        <form action="<?php echo $view['router']->generate('task_new') ?>" method="post" <?php echo $view['form']->enctype($form) ?> >
            <?php echo $view['form']->widget($form) ?>

            <input type="submit" />
        </form>

.. image:: /images/book/form-simple.png
    :align: center

.. note::

    Este exemplo assume que você criou uma rota chamada ``task_new``
    que aponta para o controlador ``AcmeTaskBundle:Default:new`` o qual 
    foi criado anteriormente.

É isso! Ao imprimir o ``form_widget(form)``, cada campo do formulário é
renderizado, juntamente com uma label e uma mensagem de erro (se houver). Fácil
assim, embora não muito flexível (ainda). Normalmente, você desejará renderizar cada
campo do formulário individualmente, pois poderá controlar como será a aparência do formulário. 
Você aprenderá como fazer isso na seção ":ref:`form-rendering-template`".

Antes de prosseguirmos, observe como o campo input ``task`` renderizado tem o valor
da propriedade ``task`` do objeto ``$task`` (Ex. "Write a blog post").
Este é o primeiro trabalho de um formulário: pegar os dados de um objeto e traduzi-lo 
em um formato que seja adequado para ser renderizado em um formulário HTML.

.. tip::

   O sistema de formulários é inteligente o suficiente para acessar o valor da propriedade protegida
   ``task`` através dos métodos ``getTask()`` e ``setTask()`` na classe ``Task``. A menos que a 
   propriedade seja pública, ela *deve* ter um método "getter" e "setter" para que o componente de 
   formulário possa obter e definir os dados na propriedade. Para uma propriedade Boolean, você pode 
   usar um método "isser" (por exemplo, ``isPublished()``) em vez de um getter 
   (Ex. ``getPublished()``).

.. index::
  single: Forms; Handling form submission

Manipulando o envio de formulários
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O segundo trabalho de um formulário é traduzir os dados enviados pelo usuário de volta as
propriedades de um objeto. Para que isso aconteça, os dados enviados pelo
usuário devem ser vinculados (bound) ao formulário. Adicione as seguintes funcionalidades 
no seu controlador::

    // ...

    public function newAction(Request $request)
    {
        // just setup a fresh $task object (remove the dummy data)
        $task = new Task();

        $form = $this->createFormBuilder($task)
            ->add('task', 'text')
            ->add('dueDate', 'date')
            ->getForm();

        if ($request->isMethod('POST')) {
            $form->bind($request);

            if ($form->isValid()) {
                // perform some action, such as saving the task to the database

                return $this->redirect($this->generateUrl('task_success'));
            }
        }

        // ...
    }

.. versionadded:: 2.1
    O método ``bind`` tornou-se mais flexível no Symfony 2.1. Ele aceita agora os 
    dados brutos do cliente (como antes) ou um objeto Request do Symfony. Ele é 
    preferido ao invés do método obsoleto ``bindRequest``.

Agora, quando enviar o formulário, o controlador vincula (bind) ao formulário os dados enviados, 
que traduz os dados de volta as propriedades ``task`` e ``dueDate``
do objeto ``$task``. Isso tudo acontece através do método ``bind()``.

.. note::

    Assim que o ``bind()`` é chamado, os dados enviados são transferidos
    imediatamente para o objeto implícito. Isso acontece independentemente de
    os dados implícitos serem realmente válidos.

Este controlador segue um padrão comum para a manipulação de formulários, e possui três
caminhos possíveis:

#. Inicialmente quando se carrega a página em um navegador, o método de solicitação (request) é 
   ``GET`` e o formulário é simplesmente criado e renderizado;

#. Quando o usuário envia o formulário (Ex., o método é ``POST``) mas os dados não são válidos
   (a validação será discutida na próxima seção), o formulário é vinculado (bound) e
   então processado, desta vez exibindo todos os erros de validação;

#. Quando o usuário envia o formulário com dados válidos, o formulário é vinculado (bound) e
   você tem a oportunidade de executar algumas ações usando o objeto ``$task``
   (por exemplo, persisti-lo para o banco de dados) antes de redirecionar o usuário
   para outra página (por exemplo, uma página de "obrigado" ou "sucesso").

.. note::

   Redirecionar o usuário após o envio bem sucedido do formulário impede que ele,
   ao clicar em "atualizar", reenvie os dados do formulário.

.. index::
   single: Formulários; Validação

Validação do formulário
-----------------------

Na seção anterior, você aprendeu como um formulário pode ser enviado com dados 
válidos ou inválidos. No Symfony2, a validação é aplicada ao objeto implícito
(Ex., ``Task``). Em outras palavras, a questão não é se o "formulário" é
válido, mas se o objeto ``$task`` é válido após a aplicação dos dados enviados pelo
formulário. A chamada ``$form->isValid()`` é um atalho
que pergunta ao objeto ``$task`` se ele possui ou não dados válidos.

A validação é feita adicionando um conjunto de regras (chamadas *constraints*) à uma classe. Para
ver isso em ação, adicione *constraints* de validação para que o campo ``task`` não deve 
ser vazio e o campo ``dueDate`` não deve ser vazio e deve ser um objeto \DateTime 
válido.

.. configuration-block::

    .. code-block:: yaml

        # Acme/TaskBundle/Resources/config/validation.yml
        Acme\TaskBundle\Entity\Task:
            properties:
                task:
                    - NotBlank: ~
                dueDate:
                    - NotBlank: ~
                    - Type: \DateTime

    .. code-block:: php-annotations

        // Acme/TaskBundle/Entity/Task.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Task
        {
            /**
             * @Assert\NotBlank()
             */
            public $task;

            /**
             * @Assert\NotBlank()
             * @Assert\Type("\DateTime")
             */
            protected $dueDate;
        }

    .. code-block:: xml

        <!-- Acme/TaskBundle/Resources/config/validation.xml -->
        <class name="Acme\TaskBundle\Entity\Task">
            <property name="task">
                <constraint name="NotBlank" />
            </property>
            <property name="dueDate">
                <constraint name="NotBlank" />
                <constraint name="Type">
                    <value>\DateTime</value>
                </constraint>
            </property>
        </class>

    .. code-block:: php

        // Acme/TaskBundle/Entity/Task.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\Type;

        class Task
        {
            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('task', new NotBlank());

                $metadata->addPropertyConstraint('dueDate', new NotBlank());
                $metadata->addPropertyConstraint('dueDate', new Type('\DateTime'));
            }
        }

É isso! Se você reenviar o formulário com dados inválidos, verá os
erros correspondentes exibidos com o formulário.

.. _book-forms-html5-validation-disable:

.. sidebar:: Validação HTML5

   Com o HTML5, muitos navegadores podem, nativamente, impor certas *constraints* de validação 
   no lado do cliente. A validação mais comum é ativada renderizando
   um atributo ``required`` em campos que são obrigatórios. Para navegadores que
   suportam HTML5, isso irá resultar em uma mensagem nativa do navegador sendo exibida
   se o usuário tentar enviar o formulário com o campo em branco.
   
   Os formulários gerados podem aproveitar ao máximo esta nova funcionalidade, adicionando 
   atributos HTML que disparam a validação. A validação ao lado do cliente,
   entretanto, pode ser desativada ao adicionar o atributo ``novalidate`` na
   tag ``form`` ou ``formnovalidate`` na tag submit. Isto é especialmente
   útil quando você quiser testar suas *constraints* de validação ao lado do servidor,
   mas estão sendo impedidas pelo seu navegador, por exemplo, ao enviar 
   campos em branco.

A validação é um recurso muito poderoso do Symfony2 e tem seu próprio
:doc:`capítulo dedicado</book/validation>`.

.. index::
   single: Formulários; Grupos de Validação

.. _book-forms-validation-groups:

Grupos de Validação
~~~~~~~~~~~~~~~~~~~

.. tip::

    Se você não estiver usando :ref:`grupos de validação<book-validation-validation-groups>`,
    então, você pode pular esta seção.

Se o seu objeto aproveita a :ref:`grupos de validação<book-validation-validation-groups>`,
você precisa especificar qual(ais) grupo(s) de validação seu formulário deve usar::

    $form = $this->createFormBuilder($users, array(
        'validation_groups' => array('registration'),
    ))->add(...)
    ;

Se você está criando :ref:`classes de formulário<book-form-creating-form-classes>` (uma
boa prática), então você precisa adicionar o seguinte ao método 
``setDefaultOptions()``::

    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    public function setDefaultOptions(OptionsResolverInterface $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => array('registration')
        ));
    }

Em ambos os casos, *apenas* o grupo de validação ``registration`` será
usado para validar o objeto implícito.

Grupos com base nos dados submetidos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 2.1
   A capacidade de especificar um callback ou Closure no ``validation_groups``
   é novo na versão 2.1

Se você precisar de alguma lógica avançada para determinar os grupos de validação (por exemplo,
com base nos dados submetidos), você pode definir a opção ``validation_groups``
para um ``array callback`` ou uma ``Closure``::

    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    public function setDefaultOptions(OptionsResolverInterface $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => array('Acme\\AcmeBundle\\Entity\\Client', 'determineValidationGroups'),
        ));
    }

Isso irá chamar o método estático ``determineValidationGroups()`` na
classe ``Client`` após o formulário ser vinculado (bound), mas antes da validação ser executada.
O objeto do formulário é passado como um argumento para esse método (veja o exemplo seguinte).
Você também pode definir toda a lógica inline usando uma Closure::

    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    public function setDefaultOptions(OptionsResolverInterface $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => function(FormInterface $form) {
                $data = $form->getData();
                if (Entity\Client::TYPE_PERSON == $data->getType()) {
                    return array('person')
                } else {
                    return array('company');
                }
            },
        ));
    }

.. index::
   single: Formulários; Tipos de campos integrados

.. _book-forms-type-reference:

Tipos de campos integrados (Built-in)
-------------------------------------

O Symfony vem, por padrão, com um grande grupo de tipos de campos que cobrem todos os
os campos comuns de formulário e tipos de dados que você vai encontrar:

.. include:: /reference/forms/types/map.rst.inc

Você também pode criar os seus próprios tipos de campo personalizados. Este tópico é abordado
no artigo ":doc:`/cookbook/form/create_custom_field_type`" do cookbook.

.. index::
   single: Formulários; Opções dos tipos de campos

Opções dos tipos de campos
~~~~~~~~~~~~~~~~~~~~~~~~~~

Cada tipo de campo possui um número de opções que podem ser usadas ​​para configurá-lo.
Por exemplo, o campo ``dueDate`` é atualmente processado como 3 select
boxes. No entanto, o :doc:`campo date</reference/forms/types/date>` pode ser
configurado para ser renderizado como uma caixa de texto simples (onde o usuário deve 
digitar a data como uma string na caixa)::

    ->add('dueDate', 'date', array('widget' => 'single_text'))

.. image:: /images/book/form-simple2.png
    :align: center

Cada tipo de campo tem um número de opções diferentes que podem ser passadas à ele.
Muitas delas são específicas para o tipo de campo e os detalhes podem ser encontrados 
na documentação de cada tipo.

.. sidebar:: A opção ``required`` 

    A opção mais comum é a opção ``required``, que pode ser aplicada à
    qualquer campo. Por padrão, a opção ``required`` é definida como ``true``, o que significa
    que os navegadores prontos para o HTML5 aplicarão a validação ao lado do cliente se o campo
    for deixado em branco. Se você não deseja esse comportamento, defina a opção ``required``
    em seu campo para ``false`` ou :ref:`desabilite a validação HTML5<book-forms-html5-validation-disable>`.
    
    Além disso, note que a configuração da opção ``required`` para ``true`` **não**
    resultará em validação aplicada ao lado do servidor. Em outras palavras, se um
    usuário enviar um valor em branco para o campo (ou usar um navegador antigo
    ou web service, por exemplo), ela será aceita como um valor válido, a menos
    que você utilize a constraint de validação do Symfony ``NotBlank`` ou ``NotNull``.
    
    Em outras palavras, a opção ``required`` é "agradável", mas a validação verdadeira 
    ao lado do servidor *sempre* deverá ser usada.

.. index::
   single: Formulários; Adivinhando o tipo do campo

.. _book-forms-field-guessing:

Adivinhando o tipo do campo
---------------------------

Agora que você adicionou metadados de validação na classe ``Task``, o Symfony
já sabe um pouco sobre os seus campos. Se você permitir, o Symfony pode "adivinhar"
o tipo do seu campo e configurá-lo para você. Neste exemplo, o Symfony pode
adivinhar a partir das regras de validação que o campo ``task`` é um campo ``texto`` 
normal e o campo ``dueDate`` é um campo ``data``::

    public function newAction()
    {
        $task = new Task();

        $form = $this->createFormBuilder($task)
            ->add('task')
            ->add('dueDate', null, array('widget' => 'single_text'))
            ->getForm();
    }

A "adivinhação" é ativada quando você omitir o segundo argumento do método ``add()``
(ou se você passar ``null`` para ele). Se você passar um array de opções como o
terceiro argumento (feito para o ``dueDate`` acima), estas opções são aplicadas ao
campo adivinhado.

.. caution::

    Se o formulário usa um grupo de validação específico, o adivinhador do tipo de campo
    ainda vai considerar *todas* as *constraints* de validação quando estiver adivinhando os seus 
    tipos de campos (incluindo as *constraints* que não fazem parte dos grupos de validação
    sendo utilizados).

.. index::
   single: Formulários; Adivinhando o tipo do campo

Adivinhando as opções dos tipos de campos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Além de adivinhar o "tipo" para um campo, o Symfony também pode tentar adivinhar
os valores corretos de uma série de opções do campo.

.. tip::

    Quando essas opções são definidas, o campo será renderizado com atributos HTML especiais
    que fornecem para a validação HTML5 ao lado do cliente. Entretanto, ele
    não gera as *constraints* equivalentes ao lado do servidor (Ex. ``Assert\MaxLength``).
    E, embora você precisará adicionar manualmente a validação ao lado do servidor, essas
    opções de tipo de campo podem, então, ser adivinhadas a partir dessa informação.

* ``required``: A opção ``required`` pode ser adivinhada com base nas regras de 
  validação (ou seja, o campo é ``NotBlank`` ou ``NotNull``) ou metadados do Doctrine
  (ou seja, é o campo é ``nullable``). Isto é muito útil, pois a sua validação ao lado
  do cliente irá corresponder automaticamente as suas regras de validação.

* ``min_length``: Se o campo é uma espécie de campo de texto, então, a opção ``min_length``
  pode ser adivinhada a partir das *constraints* de validação (se o ``MinLength``
  ou ``Min`` é usado) ou a partir dos metadados do Doctrine (através do tamanho do campo).

* ``max_length``: Semelhante ao ``min_length``, o tamanho máximo também pode 
  ser adivinhado.

.. note::

  Estas opções de campo são adivinhadas *apenas* se você estiver usando o Symfony para adivinhar
  o tipo de campo (ou seja, omitir ou passar ``null`` como o segundo argumento para o ``add()``).

Se você desejar modificar um dos valores adivinhados, você pode sobrescrevê-lo
passando a opção no array de opções do campo::

    ->add('task', null, array('min_length' => 4))

.. index::
   single: Formulários; Renderizando em um Template

.. _form-rendering-template:

Renderizando um formulário em um Template
-----------------------------------------

Até agora, você viu como um formulário inteiro pode ser renderizado com apenas uma linha
de código. Claro, você geralmente precisará de muito mais flexibilidade quando estiver renderizando:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/TaskBundle/Resources/views/Default/new.html.twig #}

        <form action="{{ path('task_new') }}" method="post" {{ form_enctype(form) }}>
            {{ form_errors(form) }}

            {{ form_row(form.task) }}
            {{ form_row(form.dueDate) }}

            {{ form_rest(form) }}

            <input type="submit" />
        </form>

    .. code-block:: html+php

        <!-- // src/Acme/TaskBundle/Resources/views/Default/newAction.html.php -->

        <form action="<?php echo $view['router']->generate('task_new') ?>" method="post" <?php echo $view['form']->enctype($form) ?>>
            <?php echo $view['form']->errors($form) ?>

            <?php echo $view['form']->row($form['task']) ?>
            <?php echo $view['form']->row($form['dueDate']) ?>

            <?php echo $view['form']->rest($form) ?>

            <input type="submit" />
        </form>

Vamos dar uma olhada em cada parte:

* ``form_enctype(form)`` - Se pelo menos um campo for um campo para upload de arquivo, ele irá 
  renderizar o ``enctype="multipart/form-data"`` obrigatório;

* ``form_errors(form)`` - Renderiza quaisquer erros globais para todo o formulário
  (erros específicos de campos são exibidos ao lado de cada campo);

* ``form_row(form.dueDate)`` - Renderiza a label, qualquer erro, e o widget 
  HTML do formulário para o campo informado (Ex. ``dueDate``), por padrão, um
  elemento ``div``;

* ``form_rest(form)`` - Renderiza quaisquer campos que ainda não tenham sido renderizados.
  Geralmente é uma boa idéia fazer uma chamada deste helper na parte inferior de
  cada formulário (no caso de você ter esquecido algum campo ou não quer se preocupar
  em renderizar manualmente os campos ocultos). Este helper também é útil para aproveitar
  a :ref:`Proteção CSRF<forms-csrf>` automática.

A maioria do trabalho é feito pelo helper ``form_row``, que renderiza
a label, os erros e widgets HTML do formulário para cada campo dentro de uma tag ``div``
por padrão. Na seção :ref:`form-theming`, você aprenderá como a saída do ``form_row``
pode ser personalizada em muitos níveis diferentes.

.. tip::

    Você pode acessar os dados atuais do seu formulário via ``form.vars.value``:
    
    .. configuration-block::

        .. code-block:: jinja
        
            {{ form.vars.value.task }}

        .. code-block:: html+php

            <?php echo $view['form']->get('value')->getTask() ?>

.. index::
   single: Formulários; Renderizando cada campo manualmente

Renderizando cada campo manualmente
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O helper ``form_row`` é ótimo porque você pode renderizar rapidamente cada
campo de seu formulário (e também é possível personalizar a marcação utilizada para a "linha"
). Mas, como a vida nem sempre é tão simples, você também pode renderizar cada campo
inteiramente à mão. O produto final do que segue é o mesmo de quando você
usou o helper ``form_row``:

.. configuration-block::

    .. code-block:: html+jinja

        {{ form_errors(form) }}

        <div>
            {{ form_label(form.task) }}
            {{ form_errors(form.task) }}
            {{ form_widget(form.task) }}
        </div>

        <div>
            {{ form_label(form.dueDate) }}
            {{ form_errors(form.dueDate) }}
            {{ form_widget(form.dueDate) }}
        </div>

        {{ form_rest(form) }}

    .. code-block:: html+php

        <?php echo $view['form']->errors($form) ?>

        <div>
            <?php echo $view['form']->label($form['task']) ?>
            <?php echo $view['form']->errors($form['task']) ?>
            <?php echo $view['form']->widget($form['task']) ?>
        </div>

        <div>
            <?php echo $view['form']->label($form['dueDate']) ?>
            <?php echo $view['form']->errors($form['dueDate']) ?>
            <?php echo $view['form']->widget($form['dueDate']) ?>
        </div>

        <?php echo $view['form']->rest($form) ?>

Se a label auto-gerada para um campo não estiver correta, você pode especificá-la
explicitamente:

.. configuration-block::

    .. code-block:: html+jinja

        {{ form_label(form.task, 'Task Description') }}

    .. code-block:: html+php

        <?php echo $view['form']->label($form['task'], 'Task Description') ?>

Finalmente, alguns tipos de campos tem opções de renderização adicionais que podem ser passadas
para o widget. Estas opções estão documentadas com cada tipo, mas uma opção em comum
é o ``attr``, que permite modificar atributos no elemento do formulário.
O seguinte código adiciona a classe ``task_field`` para o campo texto de entrada
renderizado:

.. configuration-block::

    .. code-block:: html+jinja

        {{ form_widget(form.task, { 'attr': {'class': 'task_field'} }) }}

    .. code-block:: html+php

        <?php echo $view['form']->widget($form['task'], array(
            'attr' => array('class' => 'task_field'),
        )) ?>

Referência de funções dos templates Twig
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você está usando o Twig, uma referência completa das funções de renderização do formulário 
está disponível no :doc:`manual de referência</reference/forms/twig_reference>`.
Leia ele para saber tudo sobre os helpers disponíveis e as opções
que podem ser usadas ​​com cada um.

.. index::
   single: Formulários; Criando classes de formulário

.. _book-form-creating-form-classes:

Criando classes de formulário
-----------------------------

Como você viu, um formulário pode ser criado e usado diretamente em um controlador.
No entanto, uma prática melhor é construir o formulário separadamente, em uma classe PHP independente, 
que poderá, então, ser reutilizada em qualquer lugar na sua aplicação. Crie uma nova classe
que vai abrigar a lógica da construção do formulário de tarefas:

.. code-block:: php

    // src/Acme/TaskBundle/Form/Type/TaskType.php

    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('task');
            $builder->add('dueDate', null, array('widget' => 'single_text'));
        }

        public function getName()
        {
            return 'task';
        }
    }

Esta nova classe contém todas as orientações necessárias para criar o formulário de tarefas
(Note que o método ``getName()`` deve retornar um identificador exclusivo para esse
"tipo" do formulário). Ele pode ser usado para construir rapidamente um objeto de formulário no controlador:

.. code-block:: php

    // src/Acme/TaskBundle/Controller/DefaultController.php

    // add this new use statement at the top of the class
    use Acme\TaskBundle\Form\Type\TaskType;

    public function newAction()
    {
        $task = // ...
        $form = $this->createForm(new TaskType(), $task);

        // ...
    }

Colocando a lógica do formulário em sua própria classe significa que o formulário pode ser facilmente
reutilizado em outros lugares no seu projeto. Esta é a melhor forma de criar formulários, mas,
a decisão final depende de você.

.. _book-forms-data-class:

.. sidebar:: Setando o ``data_class``

    Todo formulário precisa saber o nome da classe que contém os dados implícitos
    (Ex. ``Acme\TaskBundle\Entity\Task``). Normalmente, ele é apenas adivinhado
    com base no objeto passado no segundo argumento para o ``createForm``
    (Ex. ``$task``). Mais tarde, quando você iniciar nos formulários embutidos, isto 
    não será suficiente. Então, embora nem sempre necessário, é geralmente uma
    boa idéia especificar explicitamente a opção ``data_class`` adicionando
    o seguinte à sua classe type de formulário::

        use Symfony\Component\OptionsResolver\OptionsResolverInterface;

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'Acme\TaskBundle\Entity\Task',
            ));
        }

.. tip::

    Ao mapear formulários para objetos, todos os campos são mapeados. Qualquer  
    campo do formulário que não existe no objeto mapeado irá fazer com que uma 
    exceção seja gerada.

    In cases where you need extra fields in the form (for example: a "do you
    agree with these terms" checkbox) that will not be mapped to the underlying
    object, you need to set the property_path option to ``false``::

    Nos casos em que você precisa de campos extras na formulário (por exemplo: um
    checkbox "você concorda com os termos") que não será mapeado para o objeto implícito,
    você precisa definir a opção property_path como ``false``::

        use Symfony\Component\Form\FormBuilderInterface;

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('task');
            $builder->add('dueDate', null, array('property_path' => false));
        }

    Além disso, se houver quaiquer campos do formulário que não estão incluídos nos 
    dados submetidos, esses campos serão definidos explicitamente como ``null``.

    Os dados do campo podem ser acessados em um controlador com::

        $form->get('dueDate')->getData();


.. index::
   pair: Formulários; Doctrine

Formulários e o Doctrine
------------------------

O objetivo de um formulário é traduzir os dados de um objeto (Ex. ``Task``) para um 
formulário HTML e, em seguida, traduzir os dados enviados pelo usuário de volta ao objeto original. 
Como tal, o tópico da persistência do objeto ``Task`` no banco de dados é totalmente
não relacionado ao tópico de formulários. Mas, se você configurou a classe ``Task``
para ser persistida através do Doctrine (ou seja, você adicionou
:ref:`metadados de mapeamento<book-doctrine-adding-mapping>` à ele), então, a persistência
após a submissão do formulário pode ser feita quando o formulário é válido::

    if ($form->isValid()) {
        $em = $this->getDoctrine()->getManager();
        $em->persist($task);
        $em->flush();

        return $this->redirect($this->generateUrl('task_success'));
    }

Se, por algum motivo, você não tem acesso ao seu objeto ``$task`` original,
você pode buscá-lo a partir do formulário::

    $task = $form->getData();

Para mais informações, consulte o :doc:`capítulo Doctrine ORM</book/doctrine>`.

A chave para entender é que, quando o formulário é vinculado (bound), os dados submetidos
são transferidos imediatamente para o objeto implícito. Se você quiser
persistir esses dados, basta persistir o objeto em si (que já
contém os dados submetidos).

.. index::
   single: Formulários; Formulários embutidos

Formulários embutidos
---------------------

Muitas vezes, você desejará criar um formulário que vai incluir campos de vários objetos 
diferentes. Por exemplo, um formulário de inscrição pode conter dados que pertencem a
um objeto ``User``, bem como, muitos objetos ``Address``. Felizmente, isto
é fácil e natural com o componente de formulário.

Embutindo um único objeto
~~~~~~~~~~~~~~~~~~~~~~~~~

Suponha que cada ``Task`` pertence a um simples objeto ``Category``. Inicie, 
é claro, criando o objeto ``Category``::

    // src/Acme/TaskBundle/Entity/Category.php
    namespace Acme\TaskBundle\Entity;

    use Symfony\Component\Validator\Constraints as Assert;

    class Category
    {
        /**
         * @Assert\NotBlank()
         */
        public $name;
    }

Em seguida, adicione uma nova propriedade ``category`` na classe ``Task``::

    // ...

    class Task
    {
        // ...

        /**
         * @Assert\Type(type="Acme\TaskBundle\Entity\Category")
         */
        protected $category;

        // ...

        public function getCategory()
        {
            return $this->category;
        }

        public function setCategory(Category $category = null)
        {
            $this->category = $category;
        }
    }

Agora que a sua aplicação foi atualizada para refletir as novas exigências,
crie uma classe de formulário para que o objeto ``Category`` possa ser modificado pelo usuário::

    // src/Acme/TaskBundle/Form/Type/CategoryType.php
    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class CategoryType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('name');
        }

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'Acme\TaskBundle\Entity\Category',
            ));
        }

        public function getName()
        {
            return 'category';
        }
    }

O objetivo final é permitir que a ``Category`` de uma ``Task`` possa ser modificada direitamente
dentro do próprio formulário da tarefa. Para fazer isso, adicione um campo ``category`` 
ao objeto ``TaskType`` cujo tipo é uma instância da nova classe 
``CategoryType``:

.. code-block:: php

    use Symfony\Component\Form\FormBuilderInterface;

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('category', new CategoryType());
    }

Os campos do ``CategoryType`` podem agora ser renderizados juntamente com os campos da
classe ``TaskType``. Para ativar a validação no CategoryType, adicione
a opção ``cascade_validation``::

    public function setDefaultOptions(OptionsResolverInterface $resolver)
    {
        $resolver->setDefaults(array(
            'data_class' => 'Acme\TaskBundle\Entity\Category',
            'cascade_validation' => true,
        ));
    }

Renderize os campos ``Category`` da mesma forma
que os campos originais da ``Task``:

.. configuration-block::

    .. code-block:: html+jinja

        {# ... #}

        <h3>Category</h3>
        <div class="category">
            {{ form_row(form.category.name) }}
        </div>

        {{ form_rest(form) }}
        {# ... #}

    .. code-block:: html+php

        <!-- ... -->

        <h3>Category</h3>
        <div class="category">
            <?php echo $view['form']->row($form['category']['name']) ?>
        </div>

        <?php echo $view['form']->rest($form) ?>
        <!-- ... -->

Quando o usuário enviar o formulário, os dados submetidos para os campos ``Category``
são usados ​​para construir uma instância de ``Category``, que é então definida
no campo ``Category`` da instância ``Task``.

A instância ``Category`` é acessível naturalmente via ``$task->getCategory()``
e pode ser persistida no banco de dados ou usada como você precisar.

Embutindo uma coleção de formulários
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode embutir uma coleção de formulários em um formulário (imagine um formulário
``Category`` com muitos sub-formulários ``Product``). Isto é feito usando o tipo de 
campo ``collection``.

Para mais informações consulte no ":doc:`/cookbook/form/form_collections`" 
e na :ref:`collection</reference/forms/types/collection>` referência dos tipos de campo.

.. index::
   single: Formulários; Tematizando
   single: Formulários; Personalizando os campos

.. _form-theming:

Tematizando os formulários
--------------------------

Cada parte de como um formulário é renderizado pode ser personalizada. Você está livre para mudar
como cada "linha" do formulário é renderizada, alterar a marcação usada para renderizar os erros, ou
até mesmo, personalizar como uma tag ``textarea` deve ser renderizada. Nada está fora dos limites,
e é possível utilizar diferentes personalizações ​​em diferentes lugares.

O Symfony utiliza templates para renderizar todas e cada uma das partes de um formulário, tais como
tags ``label``, tags ``input``, mensagens de erro e tudo mais.

No Twig, cada "fragmento" do formulário é representado por um bloco Twig. Para personalizar
qualquer parte de como um formulário é renderizado, você só precisa substituir o bloco apropriado.

No PHP, cada "fragmento" do formulário é renderizado por um arquivo de template individual.
Para personalizar qualquer parte de como um formulário é renderizado, você só precisa sobrescrever o
template já existente, criando um novo.

Para entender como isso funciona, vamos personalizar o fragmento ``form_row`` e
adicionar um atributo class para o elemento ``div`` que envolve cada linha. Para
fazer isso, crie um novo arquivo template que irá armazenar a marcação nova:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/TaskBundle/Resources/views/Form/fields.html.twig #}

        {% block field_row %}
        {% spaceless %}
            <div class="form_row">
                {{ form_label(form) }}
                {{ form_errors(form) }}
                {{ form_widget(form) }}
            </div>
        {% endspaceless %}
        {% endblock field_row %}

    .. code-block:: html+php

        <!-- src/Acme/TaskBundle/Resources/views/Form/field_row.html.php -->

        <div class="form_row">
            <?php echo $view['form']->label($form, $label) ?>
            <?php echo $view['form']->errors($form) ?>
            <?php echo $view['form']->widget($form, $parameters) ?>
        </div>

O fragmento ``field_row`` do formulário é utilizado para renderizar a maioria dos campos através da
da função ``form_row``. Para dizer ao componente de formulário para utilizar o seu novo fragmento
``field_row`` definido acima, adicione o seguinte no topo do template que
renderiza o formulário:

.. configuration-block:: php

    .. code-block:: html+jinja

        {# src/Acme/TaskBundle/Resources/views/Default/new.html.twig #}

        {% form_theme form 'AcmeTaskBundle:Form:fields.html.twig' %}

        <form ...>

    .. code-block:: html+php

        <!-- src/Acme/TaskBundle/Resources/views/Default/new.html.php -->

        <?php $view['form']->setTheme($form, array('AcmeTaskBundle:Form')) ?>

        <form ...>

A tag ``form_theme`` (no Twig) "importa" os fragmentos definidos no 
template informado e utiliza-os quando renderiza o formulário. Em outras palavras, 
quando a função ``form_row`` é chamada mais tarde neste template, ela usará o bloco
``field_row`` de seu tema personalizado (ao invés do bloco padrão ``field_row``
que vem com o Symfony).

Para personalizar qualquer parte de um formulário, você só precisa substituir o fragmento
apropriado. Saber exatamente qual bloco ou arquivo deve-se substituir é o tema da
próxima seção.

.. versionadded:: 2.1
   Foi introduzida uma sintaxe alternativa do Twig para ``form_theme`` no 2.1. Ela aceita 
   qualquer expressão Twig válida (a diferença mais notável está no uso de um array quando 
   utilizar vários temas).

   .. code-block:: html+jinja

       {# src/Acme/TaskBundle/Resources/views/Default/new.html.twig #}

       {% form_theme form with 'AcmeTaskBundle:Form:fields.html.twig' %}

       {% form_theme form with ['AcmeTaskBundle:Form:fields.html.twig', 'AcmeTaskBundle:Form:fields2.html.twig'] %}

Para uma discussão mais extensiva, consulte :doc:`/cookbook/form/form_customization`.

.. index::
   single: Formulários; Nomeando os fragmentos do formulário

.. _form-template-blocks:

Nomeando os fragmentos do formulário
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No Symfony, cada parte de um formulário que é renderizada - elementos de formulário HTML, erros,
labels, etc - é definida em um tema base, que é uma coleção de blocos
no Twig e uma coleção de arquivos de template no PHP.

No Twig, cada bloco necessário é definido em um único arquivo de template (`form_div_layout.html.twig`_)
que encontra-se no interior do `Twig Bridge`_. Dentro desse arquivo, você pode ver todos os blocos
necessários para renderizar um formulário e todo o tipo de campo padrão.

No PHP, os fragmentos são arquivos de template individuais. Por padrão, eles estão localizados no
diretório `Resources/views/Form` do framework bundle (`veja no GitHub`_).

Cada nome de fragmento segue o mesmo padrão básico e é dividido em duas partes,
separadas por um único caractere de sublinhado (``_``). Alguns exemplos são:

* ``field_row`` - usado pelo ``form_row`` para renderizar a maioria dos campos;
* ``textarea_widget`` - usado pelo ``form_widget`` para renderizar um campo do tipo 
  ``textarea``;
* ``field_errors`` - usado pelo ``form_errors`` para renderizar os erros para um campo;

Cada fragmento segue o mesmo padrão básico: ``type_part``. A porção ``type``
corresponde ao *tipo* do campo sendo renderizado (Ex. ``textarea``, ``checkbox``,
``date``, etc) enquanto a porção ``part`` corresponde a *o que* está sendo
renderizado (Ex., ``label``, ``widget``, ``errors``, etc). Por padrão, existem
4 *partes* possíveis de um formulário que podem ser renderizadas:

+-------------+--------------------------+------------------------------------------------------------+
| ``label``   | (Ex. ``field_label``)    | renderiza label do campo                                   |
+-------------+--------------------------+------------------------------------------------------------+
| ``widget``  | (Ex. ``field_widget``)   | renderiza a representação HTML do campo                    |
+-------------+--------------------------+------------------------------------------------------------+
| ``errors``  | (Ex. ``field_errors``)   | renderiza os errors do campo                               |
+-------------+--------------------------+------------------------------------------------------------+
| ``row``     | (Ex. ``field_row``)      | renderiza a linha inteira do campo (label, widget e erros) |
+-------------+--------------------------+------------------------------------------------------------+

.. note::

    Na verdade, existem outras três *partes* - ``rows``, ``rest`` e ``enctype`` -
    mas você raramente ou nunca vai precisar se preocupar em sobrescrevê-las

Ao conhecer o tipo do campo (Ex. `` textarea``) e qual parte você deseja
personalizar (Ex. ``widget``), você pode construir o nome do fragmento que precisa
ser sobrescrito (Ex. ``textarea_widget``).

.. index::
   single: Formulários; Herança dos fragmentos de template

Herança dos fragmentos de template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Em alguns casos, o fragmento que você deseja personalizar parecerá estar faltando.
Por exemplo, não existe um fragmento ``textarea_errors`` nos temas padrão
fornecidos com o Symfony. Então, como são renderizados os erros de um campo textarea?

A resposta é: através do fragmento ``field_errors``. Quando o Symfony renderiza os erros
para um tipo textarea, ele procura primeiro por um fragmento ``textarea_errors`` antes
de voltar para o fragmento ``field_errors``. Cada tipo de campo tem um tipo 
*pai* (o tipo pai do ``textarea`` é ``field``), e o Symfony usa
o fragmento para o tipo pai se o fragmento base não existir.

Então, para substituir os erros para *apenas* os campos ``textarea``, copie o
fragmento ``field_errors``, renomeie para ``textarea_errors`` e personalize-o. Para
sobrescrever a renderização de erro padrão para *todos* os campos, copie e personalize 
diretamente o fragmento ``field_errors``.

.. tip::

    O tipo "pai" de cada tipo de campo está disponível na 
    :doc:`referência de tipos do formulário</reference/forms/types>` para cada tipo de campo.

.. index::
   single: Formulários; Temas Globais

Tematizando os formulários globalmente
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No exemplo acima, você usou o helper ``form_theme`` (no Twig) para "importar"
os fragmentos personalizados *somente* para este formulário. Você também pode dizer 
ao Symfony para importar as personalizações do formulário para todo o seu projeto.

Twig
....

Para incluir automaticamente os blocos personalizados do template ``fields.html.twig`` 
criado anteriormente em *todos* os templates, modifique o seu arquivo de configuração 
da aplicação:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml

        twig:
            form:
                resources:
                    - 'AcmeTaskBundle:Form:fields.html.twig'
            # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->

        <twig:config ...>
                <twig:form>
                    <resource>AcmeTaskBundle:Form:fields.html.twig</resource>
                </twig:form>
                <!-- ... -->
        </twig:config>

    .. code-block:: php

        // app/config/config.php

        $container->loadFromExtension('twig', array(
            'form' => array('resources' => array(
                'AcmeTaskBundle:Form:fields.html.twig',
             ))
            // ...
        ));

Quaisquer blocos dentro do template ``fields.html.twig`` agora são usados ​​globalmente
para definir a saída do formulário.

.. sidebar:: Personalizando toda a saída do formulário em um único arquivo com o Twig

    No Twig, você também pode personalizar um bloco de formulário diretamente dentro do template
    onde a personalização é necessária:

    .. code-block:: html+jinja

        {% extends '::base.html.twig' %}

        {# import "_self" as the form theme #}
        {% form_theme form _self %}

        {# make the form fragment customization #}
        {% block field_row %}
            {# custom field row output #}
        {% endblock field_row %}

        {% block content %}
            {# ... #}

            {{ form_row(form.task) }}
        {% endblock %}

    A tag ``{% form_theme form _self %}`` permite que blocos de formulário sejam personalizados
    diretamente dentro do template que usará essas personalizações. Utilize
    este método para fazer personalizações de saída do formulário rapidamente, que, somente
    serão necessárias em um único template.

PHP
...

Para incluir automaticamente os templates personalizados do diretório ``Acme/TaskBundle/Resources/views/Form``
criado anteriormente em *todos* os templates, modifique o seu arquivo de configuração da
aplicação:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml

        framework:
            templating:
                form:
                    resources:
                        - 'AcmeTaskBundle:Form'
        # ...


    .. code-block:: xml

        <!-- app/config/config.xml -->

        <framework:config ...>
            <framework:templating>
                <framework:form>
                    <resource>AcmeTaskBundle:Form</resource>
                </framework:form>
            </framework:templating>
            <!-- ... -->
        </framework:config>

    .. code-block:: php

        // app/config/config.php

        $container->loadFromExtension('framework', array(
            'templating' => array('form' =>
                array('resources' => array(
                    'AcmeTaskBundle:Form',
             )))
            // ...
        ));

Qualquer fragmento dentro do diretório ``Acme/TaskBundle/Resources/views/Form``
agora será usado ​​globalmente para definir a saída do formulário.

.. index::
   single: Formulários; Proteção CSRF

.. _forms-csrf:

Proteção CSRF
-------------

CSRF - ou `Cross-site request forgery`_ - é um método pelo qual um usuário mal-intencionado
tenta fazer com que os seus usuários legítimos, sem saber, enviem dados que
eles não pretendem enviar. Felizmente, os ataques CSRF podem ser prevenidos 
usando um token CSRF dentro do seu formulário.

A boa notícia é que o Symfony, por padrão, incorpora e valida os tokens CSRF
automaticamente para você. Isso significa que você pode aproveitar a proteção CSRF
sem precisar fazer nada. Na verdade, todo formulário neste capítulo
aproveitou a proteção CSRF!

A proteção CSRF funciona adicionando um campo oculto ao seu formulário - chamado ``_token``
por padrão - que contém um valor que só você e seu usuário sabem. Isto
garante que o usuário - e não alguma outra entidade - está enviando os dados.
O Symfony automaticamente valida a presença e exatidão deste token.

O campo ``_token`` é um campo oculto e será automaticamente renderizado
se você incluir a função ``form_rest()`` em seu template, que garante
a saída de todos os campos não-renderizados.

O token CSRF pode ser personalizado formulário por formulário. Por exemplo::

    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class TaskType extends AbstractType
    {
        // ...

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'data_class'      => 'Acme\TaskBundle\Entity\Task',
                'csrf_protection' => true,
                'csrf_field_name' => '_token',
                // a unique key to help generate the secret token
                'intention'       => 'task_item',
            ));
        }

        // ...
    }

Para desativar a proteção CSRF, defina a opção ``csrf_protection`` para false.
As personalizações também podem ser feitas globalmente em seu projeto. Para mais informações
veja a seção :ref:`referência de configuração do formulário </reference-frameworkbundle-forms>`
.

.. note::

    A opção ``intention`` é opcional, mas aumenta muito a segurança do
    token gerado, tornando-o diferente para cada formulário.

.. index:
   single: Formulários; Sem uma classe

Utilizando um formulário sem uma classe
---------------------------------------

Na maioria dos casos, um formulário é vinculado a um objeto, e os campos do formulário obtêm
e armazenam seus dados nas propriedades desse objeto. Isto foi exatamente o que
você viu até agora neste capítulo com a classe `Task`.

Mas, às vezes, você pode desejar apenas utilizar um formulário sem uma classe, e receber
um array dos dados submetidos. Isso é realmente muito fácil::

    // Certifique-se que você importou o namespace Request acima da classe
    use Symfony\Component\HttpFoundation\Request
    // ...

    public function contactAction(Request $request)
    {
        $defaultData = array('message' => 'Type your message here');
        $form = $this->createFormBuilder($defaultData)
            ->add('name', 'text')
            ->add('email', 'email')
            ->add('message', 'textarea')
            ->getForm();
        
            if ($request->isMethod('POST')) {
                $form->bind($request);

                // data is an array with "name", "email", and "message" keys
                $data = $form->getData();
            }
        
        // ... render the form
    }

Por padrão, um formulário assume que você deseja trabalhar com arrays de
dados, em vez de um objeto. Há exatamente duas maneiras em que você pode mudar
esse comportamento e amarrar o formulário à um objeto:

1. Passar um objeto ao criar o formulário (como o primeiro argumento para ``createFormBuilder``
   ou o segundo argumento para ``createForm``);

2. Declarar a opção ``data_class`` no seu formulário.

Se você *não* fizer qualquer uma destas, então o formulário irá retornar os dados como
um array. Neste exemplo, uma vez que ``$defaultData`` não é um objeto (e
não foi definida a opção ``data_class``), o ``$form->getData()`` retorna
um array.

.. tip::

    Você também pode acessar os valores POST (neste caso, "name") diretamente através 
    do objeto do pedido (``request``), desta forma:

    .. code-block:: php

        $this->get('request')->request->get('name');

    Esteja ciente, no entanto, que, na maioria dos casos, usar o método getData() é 
    uma melhor escolha, já que retorna os dados (geralmente um objeto), após
    ele ser transformado pelo framework de formulário.

Adicionando a Validação
~~~~~~~~~~~~~~~~~~~~~~~

A peça que falta é a validação. Normalmente, quando você chama ``$form->isValid()``,
o objeto é validado através da leitura das *constraints* que você aplicou à
classe. Mas, sem uma classe, como você pode adicionar *constraints* para os dados do seu
formulário?

A resposta é configurar as *constraints* você mesmo, e passá-las para o seu
formulário. A abordagem global é explicada um pouco mais no :ref:`validation chapter<book-validation-raw-values>`,
mas aqui está um pequeno exemplo::

    // import the namespaces above your controller class
    use Symfony\Component\Validator\Constraints\Email;
    use Symfony\Component\Validator\Constraints\MinLength;
    use Symfony\Component\Validator\Constraints\Collection;

    $collectionConstraint = new Collection(array(
        'name' => new MinLength(5),
        'email' => new Email(array('message' => 'Invalid email address')),
    ));

    // create a form, no default values, pass in the constraint option
    $form = $this->createFormBuilder(null, array(
        'validation_constraint' => $collectionConstraint,
    ))->add('email', 'email')
        // ...
    ;

Agora, quando você chamar `$form->bind($request)`, a configuração de *constraints* aqui será executada
em relação aos dados do seu formulário. Se você estiver usando uma classe de formulário, sobrescreva 
o método ``setDefaultOptions`` para especificar a opção::

    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilder;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;
    use Symfony\Component\Validator\Constraints\Email;
    use Symfony\Component\Validator\Constraints\MinLength;
    use Symfony\Component\Validator\Constraints\Collection;

    class ContactType extends AbstractType
    {
        // ...

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $collectionConstraint = new Collection(array(
                'name' => new MinLength(5),
                'email' => new Email(array('message' => 'Invalid email address')),
            ));

            $resolver->setDefaults(array(
                'validation_constraint' => $collectionConstraint
            ));
        }
    }

Agora, você tem a flexibilidade de criar formulários - com validação - que retorna 
um array de dados, em vez de um objeto. Na maioria dos casos, é melhor - e
certamente mais robusto - ligar (bind) o seu formulário à um objeto. Mas, para formulários simples,
esta é uma excelente abordagem. 

Considerações finais
--------------------

Você já conhece todos os blocos de construção necessários para construir formulários complexos e
funcionais para a sua aplicação. Ao construir formulários, tenha em mente que
a primeira meta de um formulário é traduzir os dados de um objeto (``Task``) para um 
formulário HTML, para que o usuário possa modificar os dados. O segundo objetivo de um formulário é 
pegar os dados enviados pelo usuário e reaplicá-los ao objeto.

Ainda há muito mais para aprender sobre o mundo poderoso das formulários, tais como
como lidar com :doc:`uploads de arquivos com o Doctrine </cookbook/doctrine/file_uploads>`
ou como criar um formulário onde um número dinâmico de sub-formulários podem ser adicionados 
(por exemplo, uma lista de tarefas onde você pode continuar a adicionar
mais campos antes de enviar via Javascript). Veja estes tópicos no
cookbook. Além disso, certifique-se de apoiar-se na 
:doc:`documentação de referência de tipos de campo </reference/forms/types>`, que
inclui exemplos de como usar cada tipo de campo e suas opções.

Aprenda mais no Cookbook
------------------------

* :doc:`/cookbook/doctrine/file_uploads`
* :doc:`File Field Reference </reference/forms/types/file>`
* :doc:`Creating Custom Field Types </cookbook/form/create_custom_field_type>`
* :doc:`/cookbook/form/form_customization`
* :doc:`/cookbook/form/dynamic_form_generation`
* :doc:`/cookbook/form/data_transformers`

.. _`Componente de Formulário do Symfony2`: https://github.com/symfony/Form
.. _`DateTime`: http://php.net/manual/en/class.datetime.php
.. _`Twig Bridge`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bridge/Twig
.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
.. _`Cross-site request forgery`: http://en.wikipedia.org/wiki/Cross-site_request_forgery
.. _`veja no GitHub`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bundle/FrameworkBundle/Resources/views/Form
