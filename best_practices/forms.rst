Formulários
===========

Os formulários são um dos componentes mais mal utilizados do Symfony devido à sua grande abrangência e
interminável lista de recursos. Neste capítulo vamos mostrar-lhe algumas das melhores
práticas para que você possa aproveitar os formulários, mas também ter o trabalho feito rapidamente.

Construindo Formulários
-----------------------

.. best-practice::

    Defina seus formulários como classes PHP.

O componente Form permite que você crie formulários diretamente dentro do código de seu
controlador. Honestamente, a menos que você necessite reutilizar o formulário em outro lugar,
isso é totalmente bom. Mas, para organização e reutilização, recomendamos que você defina cada
formulário em sua própria classe PHP:

.. code-block:: php

    namespace AppBundle\Form;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class PostType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('title')
                ->add('summary', 'textarea')
                ->add('content', 'textarea')
                ->add('authorEmail', 'email')
                ->add('publishedAt', 'datetime')
            ;
        }

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'AppBundle\Entity\Post'
            ));
        }

        public function getName()
        {
            return 'post';
        }
    }

Para utilizar a classe, use ``createForm`` e instancie a nova classe:

.. code-block:: php

    use AppBundle\Form\PostType;
    // ...

    public function newAction(Request $request)
    {
        $post = new Post();
        $form = $this->createForm(new PostType(), $post);

        // ...
    }

Registrando Formulários como Serviços
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode `registrar o seu tipo de formulário como um serviço`_. Mas isso *não* é recomendado
a menos que você planeje reutilizar o novo tipo de formulário em muitos lugares ou incorporá-lo
diretamente em outros formulários ou através do `tipo coleção`_.

Para a maioria dos formulários que são usados apenas para editar ou criar algo, registrar
o formulário como um serviço é exagero, e torna mais difícil descobrir
exatamente qual classe de formulário está sendo usada em um controlador.

Configuração do Botão de Formulário
-----------------------------------

Classes de formulário devem tentar ser agnósticas para *onde* serão utilizadas. Isso
as torna mais fáceis para uma posterior reutilização.

.. best-practice::

    Adicione botões nos templates, não nas classes de formulário ou nos controladores.

Desde o Symfony 2.5, você pode adicionar botões como campos no seu formulário. Essa é uma boa
maneira de simplificar o template que renderiza o formulário. Mas, se você adicionar os botões
diretamente em sua classe de formulário, isso irá efetivamente limitar o alcance desse formulário:

.. code-block:: php

    class PostType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('save', 'submit', array('label' => 'Create Post'))
            ;
        }

        // ...
    }

Esse formulário *pode* ter sido projetado para criar posts, mas se você gostaria
de reutilizá-lo para a edição de posts, a label do botão estaria errada. Ao invés disso,
alguns desenvolvedores configuram botões de formulário no controlador:

.. code-block:: php

    namespace AppBundle\Controller\Admin;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use AppBundle\Entity\Post;
    use AppBundle\Form\PostType;

    class PostController extends Controller
    {
        // ...

        public function newAction(Request $request)
        {
            $post = new Post();
            $form = $this->createForm(new PostType(), $post);
            $form->add('submit', 'submit', array(
                'label' => 'Create',
                'attr'  => array('class' => 'btn btn-default pull-right')
            ));

            // ...
        }
    }

Esse também é um erro importante, porque você está misturando marcações de apresentação
(labels, classes CSS, etc.) com código PHP puro. Separação de responsabilidades é
sempre uma boa prática a seguir, então, adicione tudo o que for relacionado com visão na
camada de visão:

.. code-block:: html+jinja

    <form method="POST" {{ form_enctype(form) }}>
        {{ form_widget(form) }}

        <input type="submit" value="Create"
               class="btn btn-default pull-right" />
    </form>

Renderizando o Formulário
-------------------------

Há uma série de maneiras para renderizar o seu formulário, que vão desde a renderização
de tudo em uma linha até renderizar cada parte de cada campo de forma independente. A
melhor maneira depende da quantidade de personalização que você precisa.

A forma mais simples - que é especialmente útil durante o desenvolvimento - é renderizar
as tags de formulário manualmente e então usar ``form_widget()`` para renderizar todos os campos:

.. code-block:: html+jinja

    <form method="POST" {{ form_enctype(form) }}>
        {{ form_widget(form) }}
    </form>

.. best-practice::

    Não use as funções ``form()`` ou ``form_start()`` para renderizar
    as tags de início e fim do formulário.

Desenvolvedores Symfony experientes irão reconhecer que estamos renderizando as tags ``<form>``
manualmente em vez de usar as funções ``form_start()`` ou ``form()``.
Enquanto são convenientes, elas tiram alguma clareza com pouco
benefício.

.. tip::

    A exceção é o formulário de exclusão porque ele é realmente apenas um botão e
    assim se beneficia de alguns desses atalhos extras.

Se você precisa de mais controle sobre como seus campos são renderizados, então deve
remover a função ``form_widget(form)`` e renderizar seus campos individualmente.
Veja `Como Personalizar a Renderização do Formulário`_ para mais informações sobre isso e como
você pode controlar *como* o formulário renderiza a nível global, utilizando a tematização (theming) de formulário.

Manipulação da Submissão do Formulário
--------------------------------------

A manipulação da submissão de um formulário geralmente segue um template semelhante:

.. code-block:: php

    public function newAction(Request $request)
    {
        // build the form ...

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            $em = $this->getDoctrine()->getManager();
            $em->persist($post);
            $em->flush();

            return $this->redirect($this->generateUrl(
                'admin_post_show',
                array('id' => $post->getId())
            ));
        }

        // render the template
    }

Há realmente apenas duas coisas notáveis ​​aqui. Primeiro, recomendamos que você
utilize uma única ação para renderizar o formulário e manusear a submissão do formulário.
Por exemplo, você *poderia* ter uma ``newAction`` que *apenas* renderiza o formulário
e uma ``createAction`` que *apenas* processa a submissão do formulário. Ambas as
ações serão quase idênticas. Por isso, é muito mais simples deixar a ``newAction``
lidar com tudo.

Segundo, recomendamos o uso de ``$form->isSubmitted()`` na declaração ``if``
para maior clareza. Isso não é tecnicamente necessário, uma vez que ``isValid()`` primeiro chama
``isSubmitted()``. Mas, sem isso, o fluxo não lê bem uma vez que *parece*
que o formulário é *sempre* processado (mesmo no pedido GET).

.. _`registrar o seu tipo de formulário como um serviço`: http://symfony.com/doc/current/cookbook/form/create_custom_field_type.html#creating-your-field-type-as-a-service
.. _`tipo coleção`: http://symfony.com/doc/current/reference/forms/types/collection.html
.. _`Como Personalizar a Renderização do Formulário`: http://symfony.com/doc/current/cookbook/form/form_customization.html
.. _`form event system`: http://symfony.com/doc/current/cookbook/form/dynamic_form_modification.html
