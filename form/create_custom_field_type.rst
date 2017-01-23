.. index::
   single: Formulário; Tipo de Campo Personalizado

Como Criar um Tipo de Campo de Formulário Personalizado
=======================================================

O Symfony vem com vários tipos de campo disponíveis para a construção de formulários.
No entanto, existem situações em que você pode desejar criar um tipo de campo de formulário personalizado
para um propósito específico. Esta receita assume que você precisa de uma definição de campo
que possui o gênero de uma pessoa, com base no campo choice existente. Esta seção
explica como o campo é definido, como você pode personalizar o seu layout e, finalmente,
como registrá-lo para uso em sua aplicação.

Definindo o Tipo de Campo
-------------------------

A fim de criar o tipo de campo personalizado, primeiro você precisa criar a classe
que o representa. Nesta situação, a classe que contém o tipo de campo
será chamada `GenderType` e o arquivo será armazenado no local padrão
para campos de formulário, que é ``<BundleName>\Form\Type``. Verifique se o campo
estende a classe :class:`Symfony\\Component\\Form\\AbstractType`::

    // src/Acme/DemoBundle/Form/Type/GenderType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class GenderType extends AbstractType
    {
        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'choices' => array(
                    'm' => 'Male',
                    'f' => 'Female',
                )
            ));
        }

        public function getParent()
        {
            return 'choice';
        }

        public function getName()
        {
            return 'gender';
        }
    }

.. tip::

    A localização deste arquivo não é importante - o diretório ``Form\Type`` 
    é apenas uma convenção.

Aqui, o valor de retorno da função ``getParent`` indica que você está
estendendo o tipo de campo ``choice``. Isto significa que, por padrão, você herda
toda a lógica e renderização deste tipo de campo. Para ver um pouco da lógica,
confira a classe `ChoiceType`_. Existem três métodos que são particularmente
importantes:

* ``buildForm()`` - Cada tipo de campo possui um método ``buildForm``, que é onde
  você configura e constrói qualquer campo(s). Note que este é o mesmo método
  que você usa para configurar *seus* formulários, e ele funciona da mesma forma aqui.

* ``buildView()`` - Este método é usado para definir quaisquer variáveis extras que você
  precisa ao renderizar o seu campo em um template. Por exemplo, no `ChoiceType`_,
  uma variável ``multiple`` é definida e utilizada no template para setar (ou não)
  o atributo ``multiple`` no campo ``select``. Veja `Criando um template para o Campo`_
  para mais detalhes.

* ``setDefaultOptions()`` - Define opções para o seu tipo do formulário, que
  podem ser usadas no ``buildForm()`` e no ``buildView()``. Há várias
  opções comuns a todos os campos (veja :doc:`/reference/forms/types/form`),
  mas você pode criar quaisquer outras que você precisar aqui.

.. tip::

    Se você está criando um campo que consiste de muitos campos, então não se esqueça
    de definir o seu tipo "pai" como ``form`` ou algo que estenda ``form``.
    Além disso, se você precisar modificar a "visão" de qualquer um dos tipos filho
    a partir de seu tipo pai, use o método ``finishView()``.

O método ``getName()`` retorna um identificador que deve ser único na
sua aplicação. Isto é usado em vários lugares, como ao personalizar
a forma que o seu tipo de formulário será renderizado.

O objetivo deste campo foi estender o tipo choice para ativar a seleção de
um gênero. Isto é alcançado através do ajuste das ``choices`` para uma lista de gêneros
possíveis.

Criando um Template para o Campo
--------------------------------

Cada tipo de campo é renderizado por um fragmento de template, o qual é determinado, em
parte, pelo valor do seu método ``getName()``. Para maiores informações, visite
:ref:`cookbook-form-customization-form-themes`.

Neste caso, uma vez que o campo pai é ``choice``, você não *precisa* fazer
qualquer trabalho pois o tipo de campo personalizado será automaticamente renderizado como um tipo
``choice``. Mas, para o propósito deste exemplo, vamos supor que, quando o seu campo
é "expandido" (ou seja, botões de opção ou caixas de seleção em vez de um campo de seleção),
você quer sempre renderizá-lo em um elemento ``ul``. Em seu template tema de formulário
(veja o link acima para mais detalhes), crie um bloco ``gender_widget`` para lidar com isso:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Form/fields.html.twig #}
    {% block gender_widget %}
        {% spaceless %}
            {% if expanded %}
                <ul {{ block('widget_container_attributes') }}>
                {% for child in form %}
                    <li>
                        {{ form_widget(child) }}
                        {{ form_label(child) }}
                    </li>
                {% endfor %}
                </ul>
            {% else %}
                {# just let the choice widget render the select tag #}
                {{ block('choice_widget') }}
            {% endif %}
        {% endspaceless %}
    {% endblock %}

.. note::

    Certifique-se que é usado o prefixo widget correto. Neste exemplo, o nome deve
    ser ``gender_widget``, de acordo com o valor retornado pelo ``getName``.
    Além disso, o arquivo de configuração principal deve apontar para o template de formulário
    personalizado, assim, ele será usado ao renderizar todos os formulários.

    .. code-block:: yaml

        # app/config/config.yml
        twig:
            form:
                resources:
                    - 'AcmeDemoBundle:Form:fields.html.twig'

Usando o Tipo de Campo
----------------------

Agora você pode usar o seu tipo de campo personalizado imediatamente, simplesmente criando uma
nova instância do tipo em um de seus formulários::

    // src/Acme/DemoBundle/Form/Type/AuthorType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class AuthorType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('gender_code', new GenderType(), array(
                'empty_value' => 'Choose a gender',
            ));
        }
    }

Mas isso só funciona porque o ``GenderType()`` é muito simples. E se 
os códigos do gênero foram armazenados em configuração ou num banco de dados? A próxima
seção explica como os tipos de campos mais complexos resolvem este problema.

Criando o seu Tipo de Campo como um Serviço
-------------------------------------------

Até agora, este artigo assumiu que você tem um tipo de campo personalizado bem simples.
Mas se você precisar acessar a configuração, uma conexão de banco de dados ou algum outro
serviço, então, você vai querer registrar o seu tipo personalizado como um serviço. Por
exemplo, suponha que você está armazenando os parâmetros de gênero em configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            genders:
                m: Male
                f: Female

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="genders" type="collection">
                <parameter key="m">Male</parameter>
                <parameter key="f">Female</parameter>
            </parameter>
        </parameters>

Para usar o parâmetro, defina o seu tipo de campo personalizado como um serviço, injetando
o valor do parâmetro ``genders`` como o primeiro argumento para a sua função recém-criada
``__construct``:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/DemoBundle/Resources/config/services.yml
        services:
            acme_demo.form.type.gender:
                class: Acme\DemoBundle\Form\Type\GenderType
                arguments:
                    - "%genders%"
                tags:
                    - { name: form.type, alias: gender }

    .. code-block:: xml

        <!-- src/Acme/DemoBundle/Resources/config/services.xml -->
        <service id="acme_demo.form.type.gender" class="Acme\DemoBundle\Form\Type\GenderType">
            <argument>%genders%</argument>
            <tag name="form.type" alias="gender" />
        </service>

.. tip::

    Certifique-se que o arquivo de serviços está sendo importado. Para mais detalhes consulte
    :ref:`service-container-imports-directive`.

Certifique-se também que o atributo ``alias`` da tag corresponde ao valor
retornado pelo método ``getName`` definido anteriormente. Você vai ver a importância
disto logo que usar o tipo de campo personalizado. Mas, primeiro, adicione um método
``__construct`` para o ``GenderType``, o qual recebe a configuração do gênero::

    // src/Acme/DemoBundle/Form/Type/GenderType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    // ...

    class GenderType extends AbstractType
    {
        private $genderChoices;
        
        public function __construct(array $genderChoices)
        {
            $this->genderChoices = $genderChoices;
        }
    
        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setDefaults(array(
                'choices' => $this->genderChoices,
            ));
        }

        // ...
    }

Ótimo! O ``GenderType`` é alimentado agora por parâmetros de configuração e
registrado como um serviço. Além disso, devido a você ter usado o alias ``form.type`` na sua
configuração, a utilização do campo é muito mais fácil agora::

    // src/Acme/DemoBundle/Form/Type/AuthorType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;

    // ...

    class AuthorType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('gender_code', 'gender', array(
                'empty_value' => 'Choose a gender',
            ));
        }
    }

Observe que em vez de criar uma nova instância, você pode apenas referir-se à
ela pelo alias usado na sua configuração do serviço, ``gender``. Divirta-se! 

.. _`ChoiceType`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Extension/Core/Type/ChoiceType.php
.. _`FieldType`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Extension/Core/Type/FieldType.php
