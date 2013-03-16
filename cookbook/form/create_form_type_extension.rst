.. index::
   single: Formulário; Extensão de Tipo de Formulário

Como criar uma Extensão do Tipo de Formulário
=============================================

:doc:`Tipos de campo de formulário personalizados <create_custom_field_type>` são ótimos quando
você precisa de tipos de campo para um propósito específico, por exemplo para selecionar o gênero
ou uma entrada de número de IVA.

Mas, às vezes, você realmente não precisa adicionar novos tipos de campo - você quer
adicionar funcionalidades aos tipos existentes. É onde o tipo de formulário
é usado.

As extensões do tipo de formulário têm dois casos de uso principais:

#. Você quer adicionar **uma funcionalidade genérica para vários tipos** (como
   a adição de um texto de "ajuda" para cada tipo de campo);
#. Você quer adicionar **uma funcionalidade específica para um único tipo** (tal
   como a adição de uma funcionalidade "download" para o tipo de campo "file").

Em ambos os casos, é possível alcançar o seu objetivo com a renderização
personalizada de formulário. Mas, usar extensões do tipo de formulário
pode ser mais limpo (limitando a quantidade de lógica de negócios nos templates)
e mais flexível (você pode adicionar várias extensões do tipo a um único tipo
de formulário.

Extensões do tipo de formulário podem alcançar mais do que os tipos de campos personalizados
podem fazer, mas, em vez de serem tipos de campos próprios, **elas conectam em tipos existentes**.

Imagine que você gerencia uma entidade ``Media``, e que cada mídia está associada
a um arquivo. Seu formulário ``Media`` usa um tipo file, mas, ao editar a entidade,
você gostaria de ver sua imagem processada automaticamente ao lado do campo
arquivo.

Você poderia, naturalmente, fazer isso personalizando a forma como este campo é renderizado em um
template. Mas as extensões do tipo de formulário permitem que você faça isso de uma forma DRY agradável.

Definindo a Extensão do Tipo de Formulário
------------------------------------------

Sua primeira tarefa será criar a classe da extensão do tipo de formulário. Vamos
chamá-la ``ImageTypeExtension``. Por padrão, as extensões de formulário geralmente
residem no diretório ``Form\Extension`` de um de seus bundles.

Ao criar uma extensão de tipo de formulário, você pode implementar a
interface :class:`Symfony\\Component\\Form\\FormTypeExtensionInterface`
ou estender a classe :class:`Symfony\\Component\\Form\\AbstractTypeExtension`.
Na maioria dos casos, é mais fácil estender a classe abstrata::

    // src/Acme/DemoBundle/Form/Extension/ImageTypeExtension.php
    namespace Acme\DemoBundle\Form\Extension;

    use Symfony\Component\Form\AbstractTypeExtension;

    class ImageTypeExtension extends AbstractTypeExtension
    {
        /**
         * Returns the name of the type being extended.
         *
         * @return string The name of the type being extended
         */
        public function getExtendedType()
        {
            return 'file';
        }
    }

O único método que você **deve** implementar é o ``getExtendedType``.
Ele é usado para indicar o nome do tipo de formulário que será estendido
pela sua extensão.

.. tip::

    O valor que você retorna no método ``getExtendedType`` corresponde
    ao valor retornado pelo método ``getName`` na classe do tipo de formulário
    que você deseja estender.

Além da função``getExtendedType``, provavelmente você vai querer
sobrescrever um dos seguintes métodos:

* ``buildForm()``

* ``buildView()``

* ``setDefaultOptions()``

* ``finishView()``

Para mais informações sobre o que esses métodos fazem, você pode consultar o
artigo do cookbook
:doc:`Criando Tipos de Campo Personalizados</cookbook/form/create_custom_field_type>`.

Registrando a sua Extensão do Tipo de Formulário como um Serviço
----------------------------------------------------------------

O próximo passo tornar o Symfony ciente da sua extensão. Tudo o que você
precisa fazer é declará-la como um serviço usando a tag
``form.type_extension``.

.. configuration-block::

    .. code-block:: yaml

        services:
            acme_demo_bundle.image_type_extension:
                class: Acme\DemoBundle\Form\Extension\ImageTypeExtension
                tags:
                    - { name: form.type_extension, alias: file }

    .. code-block:: xml

        <service id="acme_demo_bundle.image_type_extension"
            class="Acme\DemoBundle\Form\Extension\ImageTypeExtension"
        >
            <tag name="form.type_extension" alias="file" />
        </service>

    .. code-block:: php

        $container
            ->register(
                'acme_demo_bundle.image_type_extension',
                'Acme\DemoBundle\Form\Extension\ImageTypeExtension'
            )
            ->addTag('form.type_extension', array('alias' => 'file'));

A chave ``alias`` da tag é o tipo de campo que essa extensão deve
ser aplicada. No seu caso, como você deseja estender o tipo de ``file``,
você vai usar o ``file`` como um alias.

Adicionando a lógica de negócio da extensão
-------------------------------------------

O objetivo da sua extensão é exibir imagens agradáveis ​​ao lado de campos arquivo
(quando o modelo subjacente contém imagens). Para esta finalidade, vamos supor
que você usa uma abordagem semelhante à descrita em 
:doc:`Como manusear o upload de arquivos com o Doctrine</cookbook/doctrine/file_uploads>`:
você tem um modelo Media com uma propriedade file (correspondente ao campo de arquivo,
no formulário) e uma propriedade path (correspondendo ao caminho da imagem, no
banco de dados)::

    // src/Acme/DemoBundle/Entity/Media.php
    namespace Acme\DemoBundle\Entity;

    use Symfony\Component\Validator\Constraints as Assert;

    class Media
    {
        // ...

        /**
         * @var string The path - typically stored in the database
         */
        private $path;

        /**
         * @var \Symfony\Component\HttpFoundation\File\UploadedFile
         * @Assert\File(maxSize="2M")
         */
        public $file;

        // ...

        /**
         * Get the image url
         *
         * @return null|string
         */
        public function getWebPath()
        {
            // ... $webPath being the full image url, to be used in templates

            return $webPath;
        }
    }

Sua classe da extensão do tipo de formulário terá que fazer duas coisas, a fim de estender
o tipo de formulário ``file``:

#. Sobrescrever o método ``setDefaultOptions``, a fim de adicionar uma opção
   image_path;
#. Sobrescrever os métodos ``buildForm`` e ``buildView`` a fim de passar a url da imagem
   para a visão.

A lógica é a seguinte: quando adicionar um campo de formulário do tipo ``file``,
você poderá especificar uma nova opção: `` image_path ``. Esta opção irá
dizer ao campo arquivo como obter o endereço real da imagem, a fim de exibí-la
na visão::

    // src/Acme/DemoBundle/Form/Extension/ImageTypeExtension.php
    namespace Acme\DemoBundle\Form\Extension;

    use Symfony\Component\Form\AbstractTypeExtension;
    use Symfony\Component\Form\FormView;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\Form\Util\PropertyPath;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class ImageTypeExtension extends AbstractTypeExtension
    {
        /**
         * Returns the name of the type being extended.
         *
         * @return string The name of the type being extended
         */
        public function getExtendedType()
        {
            return 'file';
        }

        /**
         * Add the image_path option
         *
         * @param OptionsResolverInterface $resolver
         */
        public function setDefaultOptions(OptionsResolverInterface $resolver)
        {
            $resolver->setOptional(array('image_path'));
        }

        /**
         * Pass the image url to the view
         *
         * @param FormView $view
         * @param FormInterface $form
         * @param array $options
         */
        public function buildView(FormView $view, FormInterface $form, array $options)
        {
            if (array_key_exists('image_path', $options)) {
                $parentData = $form->getParent()->getData();

                if (null !== $parentData) {
                    $propertyPath = new PropertyPath($options['image_path']);
                    $imageUrl = $propertyPath->getValue($parentData);
                } else {
                     $imageUrl = null;
                }

                // set an "image_url" variable that will be available when rendering this field
                $view->set('image_url', $imageUrl);
            }
        }

    }

Sobrescrevendo o Fragmento de Template do Widget File
-----------------------------------------------------

Cada tipo de campo é renderizado por um fragmento de template. Esses fragmentos de template
podem ser sobrescritos, para personalizar a renderização formulário. Para mais informações
você pode consultar o artigo :ref:`cookbook-form-customization-form-themes`.

Em sua classe de extensão, você adicionou uma nova variável (``image_url``), mas
você ainda precisa aproveitar esta nova variável em seus templates.
Especificamente, você precisa sobrescrever o bloco ``file_widget``:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/DemoBundle/Resources/views/Form/fields.html.twig #}
        {% extends 'form_div_layout.html.twig' %}

        {% block file_widget %}
            {% spaceless %}

            {{ block('form_widget') }}
            {% if image_url is not null %}
                <img src="{{ asset(image_url) }}"/>
            {% endif %}

            {% endspaceless %}
        {% endblock %}

    .. code-block:: html+php

        <!-- src/Acme/DemoBundle/Resources/views/Form/file_widget.html.php -->
        <?php echo $view['form']->widget($form) ?>
        <?php if (null !== $image_url): ?>
            <img src="<?php echo $view['assets']->getUrl($image_url) ?>"/>
        <?php endif ?>

.. note::

    Você precisará mudar o seu arquivo de configuração ou especificar explicitamente como
    você quer o tema do seu formulário, para que o Symfony utilize o seu bloco
    sobrecrito. Veja :ref:`cookbook-form-customization-form-themes` para mais
    informações.

Usando a Extensão do Tipo de Formulário
---------------------------------------

A partir de agora, ao adicionar um campo do tipo ``file`` no seu formulário, você pode
especificar uma opção ``image_path`` que será usada para exibir uma imagem
próxima ao campo arquivo. Por exemplo::

    // src/Acme/DemoBundle/Form/Type/MediaType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class MediaType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('name', 'text')
                ->add('file', 'file', array('image_path' => 'webPath'));
        }

        public function getName()
        {
            return 'media';
        }
    }

Ao exibir o formulário, se o modelo subjacente já foi associado
com uma imagem, você vai vê-la ao lado do campo arquivo.
