.. index::
   single: Formulário; A opção "inherit_data"

Como reduzir a duplicação de código com "inherit_data"
======================================================

.. versionadded:: 2.3
    Esta opção ``inherit_data` foi introduzida no Symfony 2.3. Antes, ela
    era conhecida como ``virtual``.

A opção de campo de formulário ``inherit_data`` pode ser muito útil quando você possui alguns
campos duplicados em diferentes entidades. Por exemplo, imagine que você tem duas
entidades, uma ``Company`` e uma ``Customer``::

    // src/AppBundle/Entity/Company.php
    namespace AppBundle\Entity;

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

    // src/AppBundle/Entity/Customer.php
    namespace AppBundle\Entity;

    class Customer
    {
        private $firstName;
        private $lastName;

        private $address;
        private $zipcode;
        private $city;
        private $country;
    }

Como você pode ver, cada entidade compartilha alguns campos iguais: ``address``,
``zipcode``, ``city``, ``country``.

Comece com a construção de dois formulários para essas entidades, ``CompanyType`` e ``CustomerType``::

    // src/AppBundle/Form/Type/CompanyType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
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

    // src/AppBundle/Form/Type/CustomerType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\AbstractType;

    class CustomerType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('firstName', 'text')
                ->add('lastName', 'text');
        }
    }

Em vez de incluir os campos duplicados ``address``, ``zipcode``, ``city``
e ``country`` em ambos os fomulários, crie um terceiro formulário chamado ``LocationType``
para isso::

    // src/AppBundle/Form/Type/LocationType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
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
                'inherit_data' => true
            ));
        }

        public function getName()
        {
            return 'location';
        }
    }

O formulário location tem uma opção de configuração interessante, chamada ``inherit_data``. Essa
opção permite ao formulário herdar os dados de seu formulário pai. Se incorporado
no formulário company, os campos do formulário location irão acessar as propriedades da
instância ``Company``. Se incorporado no formulário customer, os campos irão
acessar as propriedades da instância de ``Customer``. Fácil, não é?

.. note::

    Em vez de definir a opção ``inherit_data`` dentro de `` LocationType``, você
    também pode (assim como com qualquer opção) passá-la no terceiro argumento de
    ``$builder->add()``.

Finalmente, faça isso funcionar adicionando o formulário location em seus dois fomrulários originais::

    // src/AppBundle/Form/Type/CompanyType.php
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('foo', new LocationType(), array(
            'data_class' => 'AppBundle\Entity\Company'
        ));
    }

.. code-block:: php

    // src/AppBundle/Form/Type/CustomerType.php
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('bar', new LocationType(), array(
            'data_class' => 'AppBundle\Entity\Customer'
        ));
    }

É isso! Você extraiu as definições de campo duplicadas para um formulário location separado
que você pode reutilizar sempre que precisar.

.. caution::

    Formulários com a opção ``inherit_data`` não podem ter ouvintes de eventos ``*_SET_DATA``.
