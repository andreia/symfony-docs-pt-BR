.. index::
   single: Formulário; Dados Vazios

Como configurar Dados Vazios para uma Classe de Formulário
==========================================================

A opção ``empty_data`` permite que você especifique um conjunto de dados vazios para a sua
classe de formulário. Este conjunto de dados vazios será usado se você fez o bind do seu
formulário, mas não chamou o ``setData()`` nele ou não passou dados quando criou
ele. Por exemplo::

    public function indexAction()
    {
        $blog = // ...

        // $blog is passed in as the data, so the empty_data option is not needed
        $form = $this->createForm(new BlogType(), $blog);

        // no data is passed in, so empty_data is used to get the "starting data"
        $form = $this->createForm(new BlogType());
    }

Por padrão, o ``empty_data`` é setado como ``null``. Ou, se você especificou
a opção ``data_class`` para a sua classe de formulário, ele será, por padrão, uma nova instância
dessa classe. Essa instância será criada chamando o construtor
sem argumentos.

Se você quiser sobrescrever esse comportamento padrão, existem duas formas de fazer isso.

Opção 1: Instanciar uma nova Classe
-----------------------------------

Uma razão para usar esta opção é se você quer usar um construtor
que possui argumentos. Lembre-se, a opção ``data_class`` padrão chama
o construtor sem argumentos::

    // src/Acme/DemoBundle/Form/Type/BlogType.php
    // ...

    use Symfony\Component\Form\AbstractType;
    use Acme\DemoBundle\Entity\Blog;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class BlogType extends AbstractType
    {
        private $someDependency;

        public function __construct($someDependency)
        {
            $this->someDependency = $someDependency;
        }
        // ...

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'empty_data' => new Blog($this->someDependency),
            ));
        }
    }

Você pode instanciar sua classe como desejar. Neste exemplo, nós passamos
algumas dependências para o ``BlogType`` ao instanciá-lo, então, use isso
para instanciar o objeto ``Blog``. O ponto é, você pode setar o ``empty_data``
para o objeto "novo" que você deseja usar.

Opção 2: Fornecer uma Closure
-----------------------------

Usar uma closure é o método preferido, uma vez que irá criar o objeto
apenas se for necessário.

A closure deve aceitar uma instância ``FormInterface`` como seu primeiro argumento::

    use Symfony\Component\OptionsResolver\OptionsResolverInterface;
    use Symfony\Component\Form\FormInterface;
    // ...

    public function setDefaultOptions(OptionsResolverInterface $resolver)
    {
        $resolver->setDefaults(array(
            'empty_data' => function (FormInterface $form) {
                return new Blog($form->get('title')->getData());
            },
        ));
    }
