.. index::
   single: Formulário; Embutir uma coleção de formulários

Como embutir uma Coleção de Formulários
=======================================

Neste artigo, você vai aprender como criar um formulário que incorpora uma coleção
de muitos outros formulários. Isto pode ser útil, por exemplo, se você tem uma classe
``Task`` onde você deseja editar/criar/remover muitos objetos ``Tag`` relacionados a
Task, dentro do mesmo formulário.

.. note::

    Neste artigo, é livremente assumido que você está usando o Doctrine para armazenar em
    seu banco de dados. Mas se você não está usando o Doctrine (por exemplo, Propel ou apenas
    uma conexão de banco de dados), tudo é muito semelhante. Há apenas algumas partes
    deste tutorial que realmente se preocupam com "persistência".

    Se você *está* usando o Doctrine, você vai precisar adicionar os metadados do Doctrine, 
    incluindo a definição de mapeamento da associação ``ManyToMany`` na propriedade
    ``tags`` da Task.

Vamos começar: suponha que cada ``Task`` pertence a vários objetos
``Tags``. Comece criando uma classe simples ``Task``::

    // src/Acme/TaskBundle/Entity/Task.php
    namespace Acme\TaskBundle\Entity;

    use Doctrine\Common\Collections\ArrayCollection;

    class Task
    {
        protected $description;

        protected $tags;

        public function __construct()
        {
            $this->tags = new ArrayCollection();
        }

        public function getDescription()
        {
            return $this->description;
        }

        public function setDescription($description)
        {
            $this->description = $description;
        }

        public function getTags()
        {
            return $this->tags;
        }

        public function setTags(ArrayCollection $tags)
        {
            $this->tags = $tags;
        }
    }

.. note::

    O ``ArrayCollection`` é específico do Doctrine e é basicamente o
    mesmo que usar um ``array`` (mas deve ser um ``ArrayCollection`` se
    você está usando o Doctrine).

Agora, crie uma classe ``Tag``. Como você viu acima, uma ``Task`` pode ter muitos objetos
``Tag``::

    // src/Acme/TaskBundle/Entity/Tag.php
    namespace Acme\TaskBundle\Entity;

    class Tag
    {
        public $name;
    }

.. tip::

    A propriedade ``name`` é pública aqui, mas ela pode facilmente ser protegida
    ou privada (então seriam necessários os métodos ``getName`` e ``setName``).

Agora, vamos para os formulários. Crie uma classe de formulário para que um objeto ``Tag``
possa ser modificado pelo usuário::

    // src/Acme/TaskBundle/Form/Type/TagType.php
    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class TagType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('name');
        }

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'Acme\TaskBundle\Entity\Tag',
            ));
        }

        public function getName()
        {
            return 'tag';
        }
    }

Com isso, você tem o suficiente para renderizar um formulário tag. Mas, uma vez que o objetivo
final é permitir que as tags de uma ``Task`` sejam modificadas dentro do próprio formulário da
task, crie um formulário para a classe ``Task``.

Observe que você embutiu uma coleção de formulários ``TagType`` usando o
tipo de campo :doc:`collection</reference/forms/types/collection>`::

    // src/Acme/TaskBundle/Form/Type/TaskType.php
    namespace Acme\TaskBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('description');

            $builder->add('tags', 'collection', array('type' => new TagType()));
        }

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'Acme\TaskBundle\Entity\Task',
            ));
        }

        public function getName()
        {
            return 'task';
        }
    }

Em seu controlador, você irá agora inicializar uma nova instância do ``TaskType``::

    // src/Acme/TaskBundle/Controller/TaskController.php
    namespace Acme\TaskBundle\Controller;

    use Acme\TaskBundle\Entity\Task;
    use Acme\TaskBundle\Entity\Tag;
    use Acme\TaskBundle\Form\Type\TaskType;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class TaskController extends Controller
    {
        public function newAction(Request $request)
        {
            $task = new Task();

            // dummy code - this is here just so that the Task has some tags
            // otherwise, this isn't an interesting example
            $tag1 = new Tag();
            $tag1->name = 'tag1';
            $task->getTags()->add($tag1);
            $tag2 = new Tag();
            $tag2->name = 'tag2';
            $task->getTags()->add($tag2);
            // end dummy code

            $form = $this->createForm(new TaskType(), $task);

            // process the form on POST
            if ($request->isMethod('POST')) {
                $form->bind($request);
                if ($form->isValid()) {
                    // ... maybe do some form processing, like saving the Task and Tag objects
                }
            }

            return $this->render('AcmeTaskBundle:Task:new.html.twig', array(
                'form' => $form->createView(),
            ));
        }
    }

O template correspondente agora é capaz de renderizar tanto o campo ``description`` para
o formulário da task, quanto todos os formulários ``TagType`` para quaisquer tags
que já estão relacionadas com esta ``Task``. No controlador acima, foi adicionado
algum código fictício para que você possa ver isso em ação (uma vez que uma ``Task`` não tem
nenhuma tag quando ela é criada pela primeira vez).

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/TaskBundle/Resources/views/Task/new.html.twig #}

        {# ... #}

        <form action="..." method="POST" {{ form_enctype(form) }}>
            {# render the task's only field: description #}
            {{ form_row(form.description) }}

            <h3>Tags</h3>
            <ul class="tags">
                {# iterate over each existing tag and render its only field: name #}
                {% for tag in form.tags %}
                    <li>{{ form_row(tag.name) }}</li>
                {% endfor %}
            </ul>

            {{ form_rest(form) }}
            {# ... #}
        </form>

    .. code-block:: html+php

        <!-- src/Acme/TaskBundle/Resources/views/Task/new.html.php -->

        <!-- ... -->

        <form action="..." method="POST" ...>
            <h3>Tags</h3>
            <ul class="tags">
                <?php foreach($form['tags'] as $tag): ?>
                    <li><?php echo $view['form']->row($tag['name']) ?></li>
                <?php endforeach; ?>
            </ul>

            <?php echo $view['form']->rest($form) ?>
        </form>

        <!-- ... -->

Quando o usuário submeter o formulário, os dados submetidos para os campos ``Tags``
são usados ​​para construir um ArrayCollection de objetos ``Tag``, o qual é então
definido no campo ``tag`` da instância ``Task``.

A coleção ``Tags`` é acessível naturalmente via ``$task->getTags()``
e pode ser persistida no banco de dados ou utilizada da forma que você precisar.

Até agora, isso funciona muito bem, mas não permite que você adicione dinamicamente novas
tags ou exclua as tags existentes. Então, enquanto a edição de tags existentes irá funcionar
perfeitamente, o usuário não pode, ainda, adicionar quaisquer tags novas.

.. caution::

    Neste artigo, você embutiu apenas uma coleção, mas você não está limitado
    a apenas isto. Você também pode incorporar coleção aninhada com a quantidade de níveis abaixo
    que desejar. Mas, se você usar o Xdebug em sua configuração de desenvolvimento, você pode receber
    erro ``Maximum function nesting level of '100' reached, aborting!``.
    Isto ocorre devido a configuração do PHP ``xdebug.max_nesting_level``, que tem como padrão
    ``100``.

    Esta diretiva limita recursão para 100 chamadas, o que pode não ser o suficiente para
    renderizar o formulário no template se você renderizar todo o formulário de
    uma vez (por exemplo, usando ``form_widget(form)``). Para corrigir isso, você pode definir
    esta diretiva para um valor maior (através do arquivo ini do PHP ou via :phpfunction:`ini_set`,
    por exemplo em ``app/autoload.php``) ou renderizar cada campo do formulário manualmente
    usando ``form_row``.

.. _cookbook-form-collections-new-prototype:

Permitindo "novas" tags com o "prototype"
-----------------------------------------

Permitir ao usuário adicionar dinamicamente novas tags significa que você vai precisar
usar algum JavaScript. Anteriormente, você adicionou duas tags ao seu formulário no controlador.
Agora, para permitir ao usuário adicionar a quantidade de formulários tag que precisar diretamente no
navegador, vamos utilizar um pouco de JavaScript.

A primeira coisa que você precisa fazer é tornar a coleção de formulário ciente de que ela vai
receber um número desconhecido de tags. Até agora, você adicionou duas tags e o tipo formulário
espera receber exatamente duas, caso contrário, um erro será lançado:
``Este formulário não deve conter campos extras``. Para tornar isto flexível,
adicione a opção ``allow_add`` no seu campo de coleção::

    // src/Acme/TaskBundle/Form/Type/TaskType.php

    // ...

    use Symfony\Component\Form\FormBuilderInterface;

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder->add('description');

        $builder->add('tags', 'collection', array(
            'type'         => new TagType(),
            'allow_add'    => true,
            'by_reference' => false,
        ));
    }

Note que ``'by_reference' => false`` também foi adicionado. Normalmente, o framework de formulário
irá modificar as tags em um objeto `Task` *sem* realmente
nunca chamar `setTags`. Definindo :ref:`by_reference<reference-form-types-by-reference>`
para `false`, o `setTags` será chamado. Você verá que isto será importante
mais tarde.

Além de dizer ao campo para aceitar qualquer número de objetos submetidos, o
``allow_add`` também disponibiliza para você uma variável "prototype". Este "prototype"
é um "template" que contém todo o HTML para poder renderizar quaisquer
formulários "tag" novos. Para renderizá-lo, faça a seguinte alteração no seu template:

.. configuration-block::

    .. code-block:: html+jinja

        <ul class="tags" data-prototype="{{ form_widget(form.tags.vars.prototype)|e }}">
            ...
        </ul>

    .. code-block:: html+php

        <ul class="tags" data-prototype="<?php echo $view->escape($view['form']->row($form['tags']->getVar('prototype'))) ?>">
            ...
        </ul>

.. note::

    Se você renderizar todo o seu sub-formulário "tags" de uma vez (por exemplo ``form_row(form.tags)``),
    então o prototype está automaticamente disponível na ``div`` externa, no
    atributo ``data-prototype``, semelhante ao que você vê acima.

.. tip::

    O ``form.tags.vars.prototype`` é um elemento de formulário com o aspecto semelhante
    aos elementos individuais ``form_widget(tag)`` dentro do seu laço ``for``.
    Isso significa que você pode chamar ``form_widget``, ``form_row`` ou ``form_label``
    nele. Você pode até mesmo optar por renderizar apenas um de seus campos (por exemplo, o
    campo ``name``):

    .. code-block:: html+jinja

        {{ form_widget(form.tags.vars.prototype.name)|e }}

Na página renderizada, o resultado será algo parecido com o seguinte:

.. code-block:: html

    <ul class="tags" data-prototype="&lt;div&gt;&lt;label class=&quot; required&quot;&gt;__name__&lt;/label&gt;&lt;div id=&quot;task_tags___name__&quot;&gt;&lt;div&gt;&lt;label for=&quot;task_tags___name___name&quot; class=&quot; required&quot;&gt;Name&lt;/label&gt;&lt;input type=&quot;text&quot; id=&quot;task_tags___name___name&quot; name=&quot;task[tags][__name__][name]&quot; required=&quot;required&quot; maxlength=&quot;255&quot; /&gt;&lt;/div&gt;&lt;/div&gt;&lt;/div&gt;">

O objetivo desta seção será usar JavaScript para ler este atributo
e dinamicamente adicionar novos formulários tag quando o usuário clicar no link "Adicionar uma tag".
Para tornar as coisas simples, este exemplo usa jQuery e assume que você o incluiu
em algum lugar na sua página.

Adicione uma tag ``script`` em algum lugar na sua página para que você possa começar a escrever um pouco de JavaScript.

Primeiro, adicione um link no final da lista "tags" via JavaScript. Segundo, faça o bind do evento 
"click" desse link para que você possa adicionar um novo formulário de tag (``addTagForm``
será exibido em seguida):

.. code-block:: javascript

    // Get the ul that holds the collection of tags
    var collectionHolder = $('ul.tags');

    // setup an "add a tag" link
    var $addTagLink = $('<a href="#" class="add_tag_link">Add a tag</a>');
    var $newLinkLi = $('<li></li>').append($addTagLink);

    jQuery(document).ready(function() {
        // add the "add a tag" anchor and li to the tags ul
        collectionHolder.append($newLinkLi);

        // count the current form inputs we have (e.g. 2), use that as the new
        // index when inserting a new item (e.g. 2)
        collectionHolder.data('index', collectionHolder.find(':input').length);

        $addTagLink.on('click', function(e) {
            // prevent the link from creating a "#" on the URL
            e.preventDefault();

            // add a new tag form (see next code block)
            addTagForm(collectionHolder, $newLinkLi);
        });
    });

O trabalho da função ``addTagForm`` será usar o atributo ``data-prototype``
para adicionar dinamicamente um novo formulário quando é clicado neste link. O HTML ``data-prototype``
contém o elemento de entrada ``text`` com um nome de ``task[tags][__name__][name]``
e com o id ``task_tags___name___name``. O nome ``__name__`` é um pequeno "placeholder",
que você vai substituir por um número único, incrementado (por exemplo: ``task[tags][3][name]``).

.. versionadded:: 2.1
    O placeholder foi alterado de ``$$name$$`` para ``__name__`` no Symfony 2.1

O código real necessário para fazer todo este trabalho pode variar um pouco, mas aqui está
um exemplo:

.. code-block:: javascript

    function addTagForm(collectionHolder, $newLinkLi) {
        // Get the data-prototype explained earlier
        var prototype = collectionHolder.data('prototype');

        // get the new index
        var index = collectionHolder.data('index');

        // Replace '__name__' in the prototype's HTML to
        // instead be a number based on the current collection's length.
        var newForm = prototype.replace(/__name__/g, collectionHolder.children().length);

        // increase the index with one for the next item
        collectionHolder.data('index', index + 1);

        // Display the form in the page in an li, before the "Add a tag" link li
        var $newFormLi = $('<li></li>').append(newForm);
        $newLinkLi.before($newFormLi);
    }

.. note::

    É melhor separar o seu javascript em arquivos JavaScript do que
    escrevê-lo dentro do HTML como foi feito aqui.

Agora, cada vez que um usuário clicar no link ``Adicionar uma tag``, um novo sub-formulário vai
aparecer na página. Quando o formulário é submetido, todos os novos formulários de tag serão convertidos
em novos objetos ``Tag`` e adicionados à propriedade ``tags`` do objeto ``Task``.

.. sidebar:: Doctrine: Relações em Cascata e salvando o lado "Inverso"

    Para obter as novas tags para salvar no Doctrine, é preciso considerar algumas
    coisas a mais. Em primeiro lugar, a menos que você iterar sobre todos os novos objetos ``Tag``
    e chamar ``$em->persist($tag)`` em cada um, você receberá um erro do
    Doctrine:

        Uma nova entidade foi encontrada através da relação `Acme\TaskBundle\Entity\Task#tags`
        que não foi configurada para operações de persistir em cascata para a entidade...

    Para corrigir isso, você pode optar pela operação de persistir em "cascata" automaticamente
    a partir do objeto ``Task`` para todas as tags relacionadas. Para fazer isso, adicione a opção ``cascade``
    em seu metadado ``ManyToMany``:

    .. configuration-block::

        .. code-block:: php-annotations

            // src/Acme/TaskBundle/Entity/Task.php

            // ...

            /**
             * @ORM\ManyToMany(targetEntity="Tag", cascade={"persist"})
             */
            protected $tags;

        .. code-block:: yaml

            # src/Acme/TaskBundle/Resources/config/doctrine/Task.orm.yml
            Acme\TaskBundle\Entity\Task:
                type: entity
                # ...
                oneToMany:
                    tags:
                        targetEntity: Tag
                        cascade:      [persist]

        .. code-block:: xml

            <!-- src/Acme/TaskBundle/Resources/config/doctrine/Task.orm.xml -->
            <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

                <entity name="Acme\TaskBundle\Entity\Task" ...>
                    <!-- ... -->
                    <one-to-many field="tags" target-entity="Tag">
                        <cascade>
                            <cascade-persist />
                        </cascade>
                    </one-to-many>
                </entity>
            </doctrine-mapping>

    Um segundo problema potencial aborda o `Lado Proprietário e Lado Inverso`_
    dos relacionamentos do Doctrine. Neste exemplo, se o lado "proprietário" da
    relação é "Task", então a persistência irá funcionar bem pois as tags são
    devidamente adicionadas à Task. No entanto, se o lado proprietário é a "Tag", então
    você vai ter um pouco mais de trabalho para garantir que o lado correto
    da relação será modificado.

    O truque é ter certeza de que uma única "Task" é definida em cada "Tag".
    Uma maneira fácil de fazer isso é adicionar alguma lógica extra ao ``setTags()``,
    que é chamada pelo framework de formulário desde que :ref:`by_reference<reference-form-types-by-reference>`
    esteja definido como ``false``::

        // src/Acme/TaskBundle/Entity/Task.php

        // ...

        public function setTags(ArrayCollection $tags)
        {
            foreach ($tags as $tag) {
                $tag->addTask($this);
            }

            $this->tags = $tags;
        }

    Dentro da ``Tag``, apenas certifique-se que você tem um método ``addTask``::

        // src/Acme/TaskBundle/Entity/Tag.php

        // ...
        use Symfony\Component\Form\FormBuilderInterface;

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            if (!$this->tasks->contains($task)) {
                $this->tasks->add($task);
            }
        }

    Se você tem uma relação ``OneToMany``, então a solução é semelhante,
    exceto que você pode simplesmente chamar ``setTask`` de dentro do ``setTags``.

.. _cookbook-form-collections-remove:

Permitindo que as tags sejam removidas
--------------------------------------

O passo seguinte é permitir a remoção de um item em particular na coleção.
A solução é similar a que permite que as tags sejam adicionadas.

Comece adicionando a opção ``allow_delete`` no tipo do formulário::

    // src/Acme/TaskBundle/Form/Type/TaskType.php

    // ...

    public function buildForm(FormBuilder $builder, array $options)
    {
        $builder->add('description');

        $builder->add('tags', 'collection', array(
            'type'         => new TagType(),
            'allow_add'    => true,
            'allow_delete' => true,
            'by_reference' => false,
        ));
    }

Modificações nos Templates
~~~~~~~~~~~~~~~~~~~~~~~~~~

A opção ``allow_delete`` tem uma consequência: se um item de uma coleção
não for enviado na submissão, o dado relacionado é removido da coleção
no servidor. A solução é, portanto, remover o elemento de formulário do DOM.

Primeiro, adicione um link "excluir esta tag" para cada formulário de tag:

.. code-block:: javascript

    jQuery(document).ready(function() {
        // add a delete link to all of the existing tag form li elements
        collectionHolder.find('li').each(function() {
            addTagFormDeleteLink($(this));
        });

        // ... the rest of the block from above
    });

    function addTagForm() {
        // ...

        // add a delete link to the new form
        addTagFormDeleteLink($newFormLi);
    }

A função ``addTagFormDeleteLink`` será parecida com a seguinte:

.. code-block:: javascript

    function addTagFormDeleteLink($tagFormLi) {
        var $removeFormA = $('<a href="#">delete this tag</a>');
        $tagFormLi.append($removeFormA);

        $removeFormA.on('click', function(e) {
            // prevent the link from creating a "#" on the URL
            e.preventDefault();

            // remove the li for the tag form
            $tagFormLi.remove();
        });
    }

Quando um formulário de tag é removido do DOM e submetido, o objeto ``Tag`` removido
não será incluído na coleção passada ao ``setTags``. Dependendo de
sua camada de persistência, isto pode ou não ser o suficiente para remover efetivamente
a relação entre os objetos ``Tag`` e ``Task``.

.. sidebar:: Doctrine: Garantir a persistência de dados

    Ao remover objetos dessa forma, você pode precisar fazer um pouco mais
    de trabalho para garantir que a relação entre a Task e Tag seja removida
    adequadamente.

    No Doctrine, você tem dois lados da relação: o lado proprietário e o
    lado inverso. Normalmente, neste caso, você vai ter uma relação ManyToMany
    e as tags excluídas desaparecerão e será persistido corretamente (a adição de novas
    tags também funciona sem esforço).

    Mas, se você tem uma relação ``OneToMany`` ou uma ``ManyToMany`` com um
    ``mappedBy`` na entidade Task (significando que Task é o lado "inverso"),
    você vai ter mais trabalho para que as tags removidas persistam corretamente.

    Neste caso, você pode modificar o controlador para remover a relação
    na tag removida. Isso pressupõe que você tenha algum ``editAction`` que
    está lidando com a "atualização" da sua Task::

        // src/Acme/TaskBundle/Controller/TaskController.php

        // ...

        public function editAction($id, Request $request)
        {
            $em = $this->getDoctrine()->getManager();
            $task = $em->getRepository('AcmeTaskBundle:Task')->find($id);
    
            if (!$task) {
                throw $this->createNotFoundException('No task found for is '.$id);
            }

            $originalTags = array();

            // Create an array of the current Tag objects in the database
            foreach ($task->getTags() as $tag) $originalTags[] = $tag;
          
            $editForm = $this->createForm(new TaskType(), $task);

            if ($request->isMethod('POST')) {
                $editForm->bind($this->getRequest());

                if ($editForm->isValid()) {

                    // filter $originalTags to contain tags no longer present
                    foreach ($task->getTags() as $tag) {
                        foreach ($originalTags as $key => $toDel) {
                            if ($toDel->getId() === $tag->getId()) {
                                unset($originalTags[$key]);
                            }
                        }
                    }

                    // remove the relationship between the tag and the Task
                    foreach ($originalTags as $tag) {
                        // remove the Task from the Tag
                        $tag->getTasks()->removeElement($task);

                        // if it were a ManyToOne relationship, remove the relationship like this
                        // $tag->setTask(null);

                        $em->persist($tag);

                        // if you wanted to delete the Tag entirely, you can also do that
                        // $em->remove($tag);
                    }

                    $em->persist($task);
                    $em->flush();

                    // redirect back to some edit page
                    return $this->redirect($this->generateUrl('task_edit', array('id' => $id)));
                }
            }

            // render some form template
        }

    Como você pode ver, adicionar e remover os elementos corretamente pode ser complicado.
    A menos que você tenha um relacionamento ManyToMany onde Task é o lado "proprietário",
    você terá que fazer trabalho extra para certificar-se de que o relacionamento será propriamente
    atualizado (se você está adicionando novas tags ou removendo tags existentes) em
    cada objeto Tag.


.. _`Lado Proprietário e Lado Inverso`: http://docs.doctrine-project.org/en/latest/reference/unitofwork-associations.html
