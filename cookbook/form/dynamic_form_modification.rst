.. index::
   single: Formulários; Eventos

Como Modificar Formulários dinamicamente usando Eventos de Formulário
=====================================================================

Antes de saltar direitamente para a geração dinâmica de formulário, vamos fazer uma revisão rápida
do como uma classe de formulário parece::

    // src/Acme/DemoBundle/Form/Type/ProductType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('name');
            $builder->add('price');
        }

        public function getName()
        {
            return 'product';
        }
    }

.. note::

    Se esta parte de código em particular ainda não lhe é familiar, você
    provavelmente terá que dar um passo para trás e primeiro rever o :doc:`capítulo Formulários </book/forms>`
    antes de prosseguir.

Vamos assumir, por um momento, que este formulário utiliza uma classe imaginária "Product" 
que possui apenas duas propriedades relevantes ("name" e "price"). O formulário gerado
desta classe terá exatamente a mesma aparência, independentemente se um novo "Product" está sendo criado
ou se um produto já existente está sendo editado (por exemplo, um produto obtido a partir do banco de dados).

Suponha agora, que você não deseja que o usuário possa alterar o valor de ``name``
uma vez que o objeto foi criado. Para fazer isso, você pode contar com o sistema de
:doc:`Dispatcher de Evento </components/event_dispatcher/introduction>`
do Symfony para analisar os dados sobre o objeto e modificar o formulário com base nos
dados do objeto "Product". Neste artigo, você vai aprender como adicionar este nível de
flexibilidade aos seus formulários.

.. _`cookbook-forms-event-subscriber`:

Adicionando um Assinante (Subscriber) de evento à uma Classe de Formulário
--------------------------------------------------------------------------

Assim, em vez de adicionar diretamente o widget "name" através da sua classe de formulário
ProductType, vamos delegar a responsabilidade de criar esse campo específico
para um Assinante de Evento::

    // src/Acme/DemoBundle/Form/Type/ProductType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\AbstractType
    use Symfony\Component\Form\FormBuilderInterface;
    use Acme\DemoBundle\Form\EventListener\AddNameFieldSubscriber;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $subscriber = new AddNameFieldSubscriber($builder->getFormFactory());
            $builder->addEventSubscriber($subscriber);
            $builder->add('price');
        }

        public function getName()
        {
            return 'product';
        }
    }

É passado o objeto FormFactory ao construtor do assinante de evento para que
o seu novo assinante seja capaz de criar o widget de formulário, uma vez que ele é
notificado do evento despachado durante a criação do formulário.

.. _`cookbook-forms-inside-subscriber-class`:

Dentro da Classe do Assinante de Evento
---------------------------------------

O objetivo é criar um campo "name" *apenas* se o objeto Product subjacente
é novo (por exemplo, não tenha sido persistido no banco de dados). Com base nisso, o assinante
pode parecer com o seguinte::

    // src/Acme/DemoBundle/Form/EventListener/AddNameFieldSubscriber.php
    namespace Acme\DemoBundle\Form\EventListener;

    use Symfony\Component\Form\Event\DataEvent;
    use Symfony\Component\Form\FormFactoryInterface;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Form\FormEvents;

    class AddNameFieldSubscriber implements EventSubscriberInterface
    {
        private $factory;

        public function __construct(FormFactoryInterface $factory)
        {
            $this->factory = $factory;
        }

        public static function getSubscribedEvents()
        {
            // Tells the dispatcher that you want to listen on the form.pre_set_data
            // event and that the preSetData method should be called.
            return array(FormEvents::PRE_SET_DATA => 'preSetData');
        }

        public function preSetData(DataEvent $event)
        {
            $data = $event->getData();
            $form = $event->getForm();

            // During form creation setData() is called with null as an argument
            // by the FormBuilder constructor. You're only concerned with when
            // setData is called with an actual Entity object in it (whether new
            // or fetched with Doctrine). This if statement lets you skip right
            // over the null condition.
            if (null === $data) {
                return;
            }

            // check if the product object is "new"
            if (!$data->getId()) {
                $form->add($this->factory->createNamed('name', 'text'));
            }
        }
    }

.. caution::

    É fácil entender mal o propósito do segmento ``if (null === $data)``
    deste assinante de evento. Para entender plenamente o seu papel, você pode considerar
    também verificar a `classe Form`_ e prestar atenção especial
    onde o setData() é chamado no final do construtor, bem como o
    método setData() em si.

A linha ``FormEvents::PRE_SET_DATA`` resolve para a string ``form.pre_set_data``.
A `classe FormEvents`_ serve para propósito organizacional. É um local centralizado
em que você pode encontrar todos os vários eventos disponíveis.

Enquanto este exemplo poderia ter usado o evento ``form.set_data`` ou até mesmo o ``form.post_set_data``
com a mesma eficácia, usando o ``form.pre_set_data`` você garante que
os dados que estão sendo recuperados do objeto ``Event`` não foram de modo algum modificados
por quaisquer outros assinantes ou ouvintes. Isto é porque o ``form.pre_set_data``
passa um objeto `DataEvent`_ em vez do objcto `FilterDataEvent`_ passado
pelo evento ``form.set_data``. O `DataEvent`_, ao contrário de seu filho `FilterDataEvent`_,
não tem um método setData().

.. note::

    Você pode ver a lista completa de eventos de formulário através da `classe FormEvents`_,
    encontrada no bundle de formulário.

.. _`DataEvent`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Event/DataEvent.php
.. _`classe FormEvents`: https://github.com/symfony/Form/blob/master/FormEvents.php
.. _`classe Form`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Form.php
.. _`FilterDataEvent`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Event/FilterDataEvent.php
