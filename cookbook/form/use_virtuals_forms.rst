.. index::
   single: Formulário; Formulários Virtuais

Como usar a opção de campo de formulário ``Virtual``
====================================================

A opção de campo de formulário ``virtual`` pode ser muito útil quando você possui alguns
campos duplicados em entidades diferentes.

Por exemplo, imagine que você tem duas entidades:``Company`` e ``Customer``::

    // src/Acme/HelloBundle/Entity/Company.php
    namespace Acme\HelloBundle\Entity;

    class Company
    {
        private $name;
        private $website;

        private $address;
        private $zipcode;
        private $city;
        private $country;
    }

.. code-block:: php

    // src/Acme/HelloBundle/Entity/Customer.php
    namespace Acme\HelloBundle\Entity;

    class Customer
    {
        private $firstName;
        private $lastName;

        private $address;
        private $zipcode;
        private $city;
        private $country;
    }

Como pode-se ver, as entidades possuem alguns campos iguais: ``address``,
``zipcode``, ``city`` e ``country``.

Agora, você deseja construir dois formulários: um para ``Company`` e outro para 
``Customer``.

Comece criando classes simples de tipo de formulário para ``CompanyType`` e ``CustomerType``::

    // src/Acme/HelloBundle/Form/Type/CompanyType.php
    namespace Acme\HelloBundle\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;

    class CompanyType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('name', 'text')
                ->add('website', 'text');
        }
    }

.. code-block:: php

    // src/Acme/HelloBundle/Form/Type/CustomerType.php
    namespace Acme\HelloBundle\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;

    class CustomerType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('firstName', 'text')
                ->add('lastName', 'text');
        }
    }

Agora, temos que lidar com os quatro campos duplicados. Aqui está um formulário (simples)
para localidade (``Location``)::

    // src/Acme/HelloBundle/Form/Type/LocationType.php
    namespace Acme\HelloBundle\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class LocationType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('address', 'textarea')
                ->add('zipcode', 'text')
                ->add('city', 'text')
                ->add('country', 'text');
        }

        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'virtual' => true
            ));
        }

        public function getName()
        {
            return 'location';
        }
    }

Nós não temos *realmente* um campo de localidade em cada uma das nossas entidades, de modo que
não podemos ligar diretamente ``LocationType`` ao nosso ``CompanyType`` ou ``CustomerType``.
Mas, com certeza, queremos um tipo de formulário próprio para lidar com a localidade (lembre-se, DRY!).

A opção de campo de formulário ``virtual`` é a solução.

Podemos definir a opção ``'virtual' => true`` no método ``setDefaultOptions()``
da ``LocationType`` e começar a usá-lo diretamente nos dois tipos de formulários originais.

Verifique o resultado::

    // CompanyType
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder->add('foo', new LocationType(), array(
            'data_class' => 'Acme\HelloBundle\Entity\Company'
        ));
    }

.. code-block:: php

    // CustomerType
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder->add('bar', new LocationType(), array(
            'data_class' => 'Acme\HelloBundle\Entity\Customer'
        ));
    }

Com a opção virtual definida para false (comportamento padrão) , o Componente de Formulário
espera que cada objeto subjacente tenha uma propriedade ``foo`` (ou ``bar``) que
é algum objeto ou array que contém os quatro campos da localidade.
Claro, não temos este objeto/array em nossas entidades e nós não queremos isso!

Com a opção virtual definida como true, o componente de Formulário ignora a propriedade ``foo`` (ou ``bar``),
e, em vez disso, aplica "gets" e "sets" aos quatro campos de localidade diretamente
no objeto subjacente!

.. note::

    Ao invés de definir a opção ``virtual`` dentro de ``LocationType``, você
    pode (assim como com todas as outras opções) também passá-la como uma opção de array 
    no terceiro argumento de ``$builder->add()``.
