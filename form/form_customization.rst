.. index::
   single: Formulário; Renderização personalizada de formulário

Como personalizar a Renderização de Formulários
===============================================

O Symfony dispõe de uma grande variedade de maneiras para personalizar como um formulário é renderizado.
Neste guia, você vai aprender a personalizar cada parte possível do seu
formulário com o menor esforço, se você usa o Twig ou PHP como sua
templating engine.

Noções Básicas sobre a Renderização de Formulários
--------------------------------------------------

Lembre-se que o widget HTML, a label e o erro de um campo do formulário podem ser facilmente
renderizados ​​usando a função Twig ``form_row`` ou o método helper PHP 
``row``:

.. configuration-block::

    .. code-block:: jinja

        {{ form_row(form.age) }}

    .. code-block:: php

        <?php echo $view['form']->row($form['age']) }} ?>

Você também pode renderizar cada uma das três partes do campo individualmente:

.. configuration-block::

    .. code-block:: html+jinja

        <div>
            {{ form_label(form.age) }}
            {{ form_errors(form.age) }}
            {{ form_widget(form.age) }}
        </div>

    .. code-block:: php

        <div>
            <?php echo $view['form']->label($form['age']) }} ?>
            <?php echo $view['form']->errors($form['age']) }} ?>
            <?php echo $view['form']->widget($form['age']) }} ?>
        </div>

Em ambos os casos, a label do formulário, os erros e o widget HTML são renderizados ​​usando
um conjunto de marcação que vem por padrão com o symfony. Por exemplo, ambos os
templates acima seriam renderizados da seguinte forma:

.. code-block:: html

    <div>
        <label for="form_age">Age</label>
        <ul>
            <li>This field is required</li>
        </ul>
        <input type="number" id="form_age" name="form[age]" />
    </div>

Para protótipos rápidos e para testar um formulário, você pode renderizar todo o formulário com
apenas uma linha:

.. configuration-block::

    .. code-block:: jinja

        {{ form_widget(form) }}

    .. code-block:: php

        <?php echo $view['form']->widget($form) }} ?>

O restante desta receita irá explicar como cada parte de marcação do formulário
pode ser modificada em vários níveis diferentes. Para mais informações sobre a renderização
do formulário em geral, consulte :ref:`form-rendering-template`.

.. _cookbook-form-customization-form-themes:

O que são Temas de Formulário?
------------------------------

O Symfony utiliza fragmentos de formulário - um pequeno pedaço de um template que renderiza apenas
uma parte de um formulário - para renderizar cada parte de um formulário - labels de campo, os erros,
campos de texto ``input``, tags ``select``, etc

Os fragmentos são definidos como blocos no Twig e como arquivos de template no PHP.

Um *tema* é nada mais do que um conjunto de fragmentos que você deseja usar quando está
renderizando um formulário. Em outras palavras, se você quiser personalizar uma parte de
como um formulário é processado, você vai importar um *tema* que contém uma personalização
dos fragmentos apropriados do formulário.

O Symfony vem com um tema padrão (`form_div_layout.html.twig`_ no Twig e
``FrameworkBundle:Form`` no PHP) que define cada fragmento necessário
para renderizar cada parte de um formulário.

Na seção seguinte, você vai aprender a personalizar um tema, sobrescrevendo
alguns ou todos os seus fragmentos.

Por exemplo, quando o widget de um campo do tipo ``integer`` é renderizado, um campo ``input``
``number`` é gerado

.. configuration-block::

    .. code-block:: html+jinja

        {{ form_widget(form.age) }}

    .. code-block:: php

        <?php echo $view['form']->widget($form['age']) ?>

renderiza:

.. code-block:: html

    <input type="number" id="form_age" name="form[age]" required="required" value="33" />

Internamente, o symfony usa o fragmento ``integer_widget`` para renderizar o campo.
Isso ocorre porque o tipo de campo é ``integer`` e você está renderizando seu ``widget``
(em oposição a sua ``label`` ou ``errors``).

No Twig ele seria por padrão o bloco ``integer_widget`` do template
`form_div_layout.html.twig`_.

No PHP ele seria o arquivo ``integer_widget.html.php`` localizado no diretório
``FrameworkBundle/Resources/views/Form``.

A implementação padrão do fragmento ``integer_widget`` seria parecida com esta:

.. configuration-block::

    .. code-block:: jinja

        {# form_div_layout.html.twig #}
        {% block integer_widget %}
            {% set type = type|default('number') %}
            {{ block('field_widget') }}
        {% endblock integer_widget %}

    .. code-block:: html+php

        <!-- integer_widget.html.php -->
        <?php echo $view['form']->renderBlock('field_widget', array('type' => isset($type) ? $type : "number")) ?>

Como você pode ver, este próprio fragmento renderiza outro fragmento - ``field_widget``:

.. configuration-block::

    .. code-block:: html+jinja

        {# form_div_layout.html.twig #}
        {% block field_widget %}
            {% set type = type|default('text') %}
            <input type="{{ type }}" {{ block('widget_attributes') }} value="{{ value }}" />
        {% endblock field_widget %}

    .. code-block:: html+php

        <!-- FrameworkBundle/Resources/views/Form/field_widget.html.php -->
        <input
            type="<?php echo isset($type) ? $view->escape($type) : "text" ?>"
            value="<?php echo $view->escape($value) ?>"
            <?php echo $view['form']->renderBlock('attributes') ?>
        />

O ponto é, os fragmentos ditam a saída HTML de cada parte de um formulário. Para
personalizar a saída do formulário, você só precisa identificar e substituir o fragmento
apropriado. O conjunto dessas personalizações de fragmentos de formulário é conhecida como "tema" de formulário.
Ao renderizar um formulário, você pode escolher qual(ais) tema(s) de formulário deseja aplicar.

No Twig, um tema é um único arquivo de template e os fragmentos são os blocos definidos
neste arquivo.

No PHP um tema é um diretório e os fragmentos são os arquivos de template individuais neste
diretório.

.. _cookbook-form-customization-sidebar:

.. sidebar:: Sabendo qual bloco personalizar

    Neste exemplo, o nome do fragmento personalizado é ``integer_widget`` porque
    você quis sobrescrever o ``widget`` HTML para todos os tipos de campo ``integer``. Se
    você precisa personalizar campos textarea, você iria personalizar o ``textarea_widget``.

    Como você pode ver, o nome do fragmento é uma combinação do tipo do campo e de
    qual parte do campo está sendo renderizada (ex.: ``widget``, ``label``,
    ``errors``, ``row``). Como tal, para personalizar a forma como os erros são renderizados 
    apenas para campos input ``text``, você deve personalizar o fragmento ``text_errors``.

    Mais comumente, no entanto, você vai querer personalizar a forma como os erros são exibidos
    através de *todos* os campos. Você pode fazer isso personalizando o fragmento
    ``field_errors``. Isso aproveita a herança do tipo de campo. Especificamente,
    uma vez que o tipo ``text`` estende o tipo ``field``, o componente de formulário
    vai procurar primeiro pelo fragmento de tipo específico (ex., ``text_errors``) antes
    de voltar ao seu nome de fragmento pai, no caso dele não existir (ex., ``field_errors``).

    Para mais informações sobre este tópico, consulte :ref:`form-template-blocks`.

.. _cookbook-form-theming-methods:

Tematizando os Formulários
--------------------------

Para ver o poder da tematização de formulários, suponha que você queira envolver cada campo input ``number``
com uma tag ``div``. A chave para fazer isso é personalizar o
fragmento ``integer_widget``.

Tematização de Formulários no Twig
----------------------------------

Ao personalizar o bloco de campo de formulário no Twig, você tem duas opções de *onde*
o bloco de formulário personalizado pode residir:

+-------------------------------------------+-------------------------------------------+----------------------------------------------+
| Método                                    | Prós                                      | Contras                                      |
+===========================================+===========================================+==============================================+
| Dentro do mesmo template que o formulário | Rápido e fácil                            | Não pode ser reutilizado em outros templates |
+-------------------------------------------+-------------------------------------------+----------------------------------------------+
| Dentro de um template separado            | Pode ser reutilizado por muitos templates | Requer que um template extra seja criado     |
+-------------------------------------------+-------------------------------------------+----------------------------------------------+

Ambos os métodos têm o mesmo efeito, mas são melhores em situações diferentes.

.. _cookbook-form-twig-theming-self:

Método 1: Dentro do mesmo Template que o Formulário
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A maneira mais fácil de personalizar o bloco ``integer_widget`` é personalizá-lo
diretamente no mesmo template que está renderizando o formulário.

.. code-block:: html+jinja

    {% extends '::base.html.twig' %}

    {% form_theme form _self %}

    {% block integer_widget %}
        <div class="integer_widget">
            {% set type = type|default('number') %}
            {{ block('field_widget') }}
        </div>
    {% endblock %}

    {% block content %}
        {# ... render the form #}

        {{ form_row(form.age) }}
    {% endblock %}

Ao usar a tag especial ``{% form_theme form _self %}``, o Twig procura dentro
do mesmo template por qualquer blocos de formulários sobrescritos. Assumindo que o campo
``form.age`` é um tipo de campo ``integer``, quando o widget é renderizado, o bloco ``integer_widget``
personalizado irá ser utilizado.

A desvantagem deste método é que o bloco de formulário personalizado não pode ser
reutilizado ao renderizar outros formulários em outros templates. Em outras palavras, este método
é mais útil ao fazer personalizações de formulários que são específicas para um único
formulário em sua aplicação. Se você quiser reutilizar uma personalização em
vários (ou todos) os formulários de sua aplicação, leia a próxima seção.

.. _cookbook-form-twig-separate-template:

Método 2: Dentro de um Template Separado
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode optar por colocar o bloco de formulário ``integer_widget`` personalizado em um
template totalmente separado. O código e o resultado final é o mesmo, mas
agora você pode reutilizar a personalização de formulário em muitos templates:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Form/fields.html.twig #}
    {% block integer_widget %}
        <div class="integer_widget">
            {% set type = type|default('number') %}
            {{ block('field_widget') }}
        </div>
    {% endblock %}

Agora que você já criou o bloco de formulário personalizado, você precisa dizer ao Symfony
para usá-lo. Dentro do template onde você está renderizando o seu formulário,
diga ao Symfony para usar o template através da tag ``form_theme``:

.. _cookbook-form-twig-theme-import-template:

.. code-block:: html+jinja

    {% form_theme form 'AcmeDemoBundle:Form:fields.html.twig' %}

    {{ form_widget(form.age) }}

Quando o widget ``form.age`` é renderizado, o Symfony usará o bloco
``integer_widget`` do novo template e a tag ``input`` vai ser envolvida no
elemento ``div`` especificado no bloco personalizado.

.. _cookbook-form-php-theming:

Tematizando Formulários em PHP
------------------------------

Ao usar o PHP como templating engine, o único método de personalizar um fragmento
é criar um novo arquivo template - é semelhante ao segundo método utilizado pelo
Twig.

O arquivo de template deve ser nomeado após o fragmento. Você deve criar um arquivo ``integer_widget.html.php``
para personalizar o fragmento ``integer_widget``.

.. code-block:: html+php

    <!-- src/Acme/DemoBundle/Resources/views/Form/integer_widget.html.php -->
    <div class="integer_widget">
        <?php echo $view['form']->renderBlock('field_widget', array('type' => isset($type) ? $type : "number")) ?>
    </div>

Agora que você criou o template de formulário personalizado, você precisa dizer ao Symfony
para usá-lo. Dentro do template onde você está renderizando o seu formulário,
diga ao Symfony para usar o tema através do método helper ``setTheme``:

.. _cookbook-form-php-theme-import-template:

.. code-block:: php

    <?php $view['form']->setTheme($form, array('AcmeDemoBundle:Form')) ;?>

    <?php $view['form']->widget($form['age']) ?>

Quando o widget ``form.age`` é renderizado, o Symfony vai usar o template
``integer_widget.html.php`` personalizado e a tag ``input`` será envolvida pelo
elemento ``div``.

.. _cookbook-form-twig-import-base-blocks:

Referenciando Blocos de Formulário Base (específico para o Twig)
----------------------------------------------------------------

Até agora, para sobrescrever um bloco de formulário em particular, o melhor método é copiar
o bloco padrão de `form_div_layout.html.twig`_, colá-lo em um template diferente,
e, em seguida, personalizá-lo. Em muitos casos, você pode evitar ter que fazer isso através da referência
ao bloco base quando for personalizá-lo.

Isso é fácil de fazer, mas varia um pouco dependendo se as personalizações de seu bloco de formulário
estão no mesmo template que o formulário ou em um template separado.

Referenciando Blocos no interior do mesmo Template que o Formulário
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Importe os blocos adicionando uma tag ``use`` no template onde você está renderizando
o formulário:

.. code-block:: jinja

    {% use 'form_div_layout.html.twig' with integer_widget as base_integer_widget %}

Agora, quando os blocos do `form_div_layout.html.twig`_ são importados, o
bloco ``integer_widget`` é chamado ``base_integer_widget``. Isto significa que quando
você redefinir o bloco ``integer_widget``, você pode fazer referência a marcação padrão
através do ``base_integer_widget``:

.. code-block:: html+jinja

    {% block integer_widget %}
        <div class="integer_widget">
            {{ block('base_integer_widget') }}
        </div>
    {% endblock %}

Referenciando Blocos Base a partir de um Template Externo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se as personalizações do formulário residirem dentro de um template externo, você pode fazer referência
ao bloco base usando a função ``parent()`` do Twig:

.. code-block:: html+jinja

    {# src/Acme/DemoBundle/Resources/views/Form/fields.html.twig #}
    {% extends 'form_div_layout.html.twig' %}

    {% block integer_widget %}
        <div class="integer_widget">
            {{ parent() }}
        </div>
    {% endblock %}

.. note::

    Não é possível fazer referência ao bloco base quando se utiliza o PHP
    como template engine. Você tem que copiar manualmente o conteúdo do bloco base
    para o seu novo arquivo de template.

.. _cookbook-form-global-theming:

Fazendo Personalizações para toda a Aplicação
---------------------------------------------

Se você deseja que uma certa personalização de formulário seja global para a sua aplicação,
você pode conseguir isso fazendo as personalizações de formulário em um template
externo e depois importá-lo dentro da sua configuração da aplicação:

Twig
~~~~

Usando a seguinte configuração, quaisquer blocos de formulários personalizados dentro do
template ``AcmeDemoBundle:Form:fields.html.twig`` serão usados globalmente quando um
formulário é renderizado.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        twig:
            form:
                resources:
                    - 'AcmeDemoBundle:Form:fields.html.twig'
            # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <twig:config ...>
                <twig:form>
                    <resource>AcmeDemoBundle:Form:fields.html.twig</resource>
                </twig:form>
                <!-- ... -->
        </twig:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('twig', array(
            'form' => array(
                'resources' => array(
                    'AcmeDemoBundle:Form:fields.html.twig',
                ),
            ),

            // ...
        ));

Por padrão, o Twig usa um layout *div* ao renderizar os formulários. Algumas pessoas, no entanto,
podem preferir renderizar formulários em um layout de *tabela*. Para isso use o recurso
``form_table_layout.html.twig`` como layout:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        twig:
            form:
                resources: ['form_table_layout.html.twig']
            # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <twig:config ...>
                <twig:form>
                    <resource>form_table_layout.html.twig</resource>
                </twig:form>
                <!-- ... -->
        </twig:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('twig', array(
            'form' => array(
                'resources' => array(
                    'form_table_layout.html.twig',
                ),
            ),

            // ...
        ));

Se você quer fazer a alteração somente em um template, adicione a seguinte linha
em seu arquivo de template em vez de adicionar o template como um recurso:

.. code-block:: html+jinja

    {% form_theme form 'form_table_layout.html.twig' %}

Note que a variável ``form`` no código acima é a variável de visão do formulário
que você passou para o seu template.

PHP
~~~

Usando a seguinte configuração, quaisquer fragmentos de formulários personalizados no interior do
diretório ``src/Acme/DemoBundle/Resources/views/Form`` serão usados globalmente quando um
formulário é renderizado.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            templating:
                form:
                    resources:
                        - 'AcmeDemoBundle:Form'
            # ...


    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config ...>
            <framework:templating>
                <framework:form>
                    <resource>AcmeDemoBundle:Form</resource>
                </framework:form>
            </framework:templating>
            <!-- ... -->
        </framework:config>


    .. code-block:: php

        // app/config/config.php
        // PHP
        $container->loadFromExtension('framework', array(
            'templating' => array(
                'form' => array(
                    'resources' => array(
                        'AcmeDemoBundle:Form',
                    ),
                ),
             ),

             // ...
        ));

Por padrão, a engine PHP usa um layout *div* ao renderizar formulários. Algumas pessoas,
no entanto, podem preferir renderizar formulários em um layout de *tabela*. Para isso use o recurso ``FrameworkBundle:FormTable``
como layout:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            templating:
                form:
                    resources:
                        - 'FrameworkBundle:FormTable'

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config ...>
            <framework:templating>
                <framework:form>
                    <resource>FrameworkBundle:FormTable</resource>
                </framework:form>
            </framework:templating>
            <!-- ... -->
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'templating' => array(
                'form' => array(
                    'resources' => array(
                        'FrameworkBundle:FormTable',
                    ),
                ),
            ),

             // ...
        ));

Se você quer fazer a alteração somente em um template, adicione a seguinte linha
em seu arquivo de template em vez de adicionar o template como um recurso:

.. code-block:: html+php

    <?php $view['form']->setTheme($form, array('FrameworkBundle:FormTable')); ?>

Note que a variável ``$form`` no código acima é a variável de visão do formulário
que você passou para o seu template.

Como personalizar um campo individual
-------------------------------------

Até agora, você viu as diferentes maneiras que pode personalizar a saída dos widgets
de todos os tipos de campo texto. Você também pode personalizar campos individuais. Por exemplo,
supondo que você tenha dois campos ``text`` - ``first_name`` e ``last_name`` - mas
você só quer personalizar um dos campos. Isto pode ser feito pela
personalização de um fragmento cujo nome é uma combinação do atributo id do campo e
qual parte do campo está sendo personalizada. Por exemplo:

.. configuration-block::

    .. code-block:: html+jinja

        {% form_theme form _self %}

        {% block _product_name_widget %}
            <div class="text_widget">
                {{ block('field_widget') }}
            </div>
        {% endblock %}

        {{ form_widget(form.name) }}

    .. code-block:: html+php

        <!-- Main template -->
        <?php echo $view['form']->setTheme($form, array('AcmeDemoBundle:Form')); ?>

        <?php echo $view['form']->widget($form['name']); ?>

        <!-- src/Acme/DemoBundle/Resources/views/Form/_product_name_widget.html.php -->

        <div class="text_widget">
              echo $view['form']->renderBlock('field_widget') ?>
        </div>

Aqui, o fragmento ``_product_name_widget`` define o template a ser usado para o
campo cujo *id* é ``product_name`` (e o nome é ``product[name]``).

.. tip::

   A parte ``product`` do campo é o nome do formulário, que pode ser definido
   manualmente ou gerado automaticamente com base no nome do tipo de formulário (por exemplo,
   ``ProductType`` equivale ao ``product``). Se você não tem certeza de qual é o nome de seu
   formulário, apenas visualize o código fonte do formulário gerado.

Você também pode sobrescrever a marcação de uma linha inteira de campo utilizando o mesmo método:

.. configuration-block::

    .. code-block:: html+jinja

        {# _product_name_row.html.twig #}
        {% form_theme form _self %}

        {% block _product_name_row %}
            <div class="name_row">
                {{ form_label(form) }}
                {{ form_errors(form) }}
                {{ form_widget(form) }}
            </div>
        {% endblock %}

    .. code-block:: html+php

        <!-- _product_name_row.html.php -->

        <div class="name_row">
            <?php echo $view['form']->label($form) ?>
            <?php echo $view['form']->errors($form) ?>
            <?php echo $view['form']->widget($form) ?>
        </div>

Outras Personalizações Comuns
-----------------------------

Até agora, esta receita tem demonstrado diversas maneiras de personalizar um único
pedaço de como um formulário é renderizado. A chave é personalizar um fragmento específico que
corresponde à parte do formulário que você deseja controlar (veja
:ref:`naming form blocks<cookbook-form-customization-sidebar>`).

Nas próximas seções, você vai ver como é possível fazer várias personalizações comuns de formulário.
Para aplicar essas personalizações, utilize um dos métodos descritos na
seção :ref:`cookbook-form-theming-methods`.

Personalizando Saída de Erro
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::
   O componente de formulário só lida com *como* os erros de validação são renderizados,
   e não com as mensagens de erro de validação. As mensagens de erro em si
   são determinadas pelas restrições de validação que você aplicou aos seus objetos.
   Para mais informações, consulte o capítulo sobre :doc:`validation</book/validation>`.

Há muitas formas diferentes para personalizar como os erros são renderizados quando um
formulário é enviado com erros. As mensagens de erro para um campo são renderizadas
quando você usa o helper ``form_errors``:

.. configuration-block::

    .. code-block:: jinja

        {{ form_errors(form.age) }}

    .. code-block:: php

        <?php echo $view['form']->errors($form['age']); ?>

Por padrão, os erros são renderizados dentro de uma lista não ordenada:

.. code-block:: html

    <ul>
        <li>This field is required</li>
    </ul>

Para sobrecrever como os erros são renderizados para *todos* os campos, basta copiar, colar
e personalizar o fragmento ``field_errors``.

.. configuration-block::

    .. code-block:: html+jinja
        
        {# fields_errors.html.twig #}
        {% block field_errors %}
            {% spaceless %}
                {% if errors|length > 0 %}
                <ul class="error_list">
                    {% for error in errors %}
                        <li>{{ error.messageTemplate|trans(error.messageParameters, 'validators') }}</li>
                    {% endfor %}
                </ul>
                {% endif %}
            {% endspaceless %}
        {% endblock field_errors %}

    .. code-block:: html+php

        <!-- fields_errors.html.php -->
        <?php if ($errors): ?>
            <ul class="error_list">
                <?php foreach ($errors as $error): ?>
                    <li><?php echo $view['translator']->trans(
                        $error->getMessageTemplate(),
                        $error->getMessageParameters(),
                        'validators'
                    ) ?></li>
                <?php endforeach; ?>
            </ul>
        <?php endif ?>

.. tip::

    Veja :ref:`cookbook-form-theming-methods` para saber como aplicar essa personalização.

Você também pode personalizar a saída de erro de apenas um tipo de campo específico.
Por exemplo, certos erros que são mais globais para o seu formulário (ou seja, não específico
para apenas um campo) são renderizados separadamente, geralmente no topo do seu formulário:

.. configuration-block::

    .. code-block:: jinja

        {{ form_errors(form) }}

    .. code-block:: php

        <?php echo $view['form']->render($form); ?>

Para personalizar *apenas* a marcação usada para esses erros, siga as mesmas instruções
acima, mas agora chame o bloco ``form_errors`` (Twig) / o arquivo ``form_errors.html.php``
(PHP). Agora, quando os erros para o tipo ``form`` são renderizados, o seu fragmento
personalizado será usado em vez do padrão ``field_errors``.

Personalizando a "Linha de Formulário"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quando você pode gerenciá-lo, a maneira mais fácil de renderizar um campo de formulário é através da
função ``form_row``, que renderiza a label, os erros e o widget HTML de
um campo. Para personalizar a marcação usada para renderizar *todas* as linhas de campo do formulário,
sobrescreva o fragmento ``field_row``. Por exemplo, suponha que você deseja adicionar uma
classe para o elemento ``div`` que envolve cada linha:

.. configuration-block::

    .. code-block:: html+jinja

        {# field_row.html.twig #}
        {% block field_row %}
            <div class="form_row">
                {{ form_label(form) }}
                {{ form_errors(form) }}
                {{ form_widget(form) }}
            </div>
        {% endblock field_row %}

    .. code-block:: html+php

        <!-- field_row.html.php -->
        <div class="form_row">
            <?php echo $view['form']->label($form) ?>
            <?php echo $view['form']->errors($form) ?>
            <?php echo $view['form']->widget($form) ?>
        </div>

.. tip::

    Veja :ref:`cookbook-form-theming-methods` para saber como aplicar essa personalização.

Adicionando um Asterisco para as Labels de Campo "Obrigatórias"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você quiser indicar todos os seus campos obrigatórios com um asterisco (``*``),
você pode fazer isso personalizando o fragmento ``field_label``.

No Twig, se você estiver fazendo a personalização de formulário dentro do mesmo template que o seu
formulário, modifique a tag ``use`` e adicione o seguinte:

.. code-block:: html+jinja

    {% use 'form_div_layout.html.twig' with field_label as base_field_label %}

    {% block field_label %}
        {{ block('base_field_label') }}

        {% if required %}
            <span class="required" title="This field is required">*</span>
        {% endif %}
    {% endblock %}

No Twig, se você estiver fazendo a personalização do formulário dentro de um template separado, use
o seguinte:

.. code-block:: html+jinja

    {% extends 'form_div_layout.html.twig' %}

    {% block field_label %}
        {{ parent() }}

        {% if required %}
            <span class="required" title="This field is required">*</span>
        {% endif %}
    {% endblock %}

Ao usar o PHP como template engine você tem que copiar o conteúdo do
template original:

.. code-block:: html+php

    <!-- field_label.html.php -->

    <!-- original content -->
    <label for="<?php echo $view->escape($id) ?>" <?php foreach($attr as $k => $v) { printf('%s="%s" ', $view->escape($k), $view->escape($v)); } ?>><?php echo $view->escape($view['translator']->trans($label)) ?></label>

    <!-- customization -->
    <?php if ($required) : ?>
        <span class="required" title="This field is required">*</span>
    <?php endif ?>

.. tip::

    Veja :ref:`cookbook-form-theming-methods` para saber como aplicar essa personalização.

Adicionando mensagens de "ajuda" 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Você também pode personalizar os widgets do formulário para ter uma mensagem opcional de "ajuda".

No Twig, se você está fazendo a personalização do formulário dentro do mesmo template que o seu
formulário, modifique a tag ``use`` e adicione o seguinte:

.. code-block:: html+jinja

    {% use 'form_div_layout.html.twig' with field_widget as base_field_widget %}

    {% block field_widget %}
        {{ block('base_field_widget') }}

        {% if help is defined %}
            <span class="help">{{ help }}</span>
        {% endif %}
    {% endblock %}

No Twig, se você está fazendo a personalização do formulário dentro de um template separado, use
o seguinte:

.. code-block:: html+jinja

    {% extends 'form_div_layout.html.twig' %}

    {% block field_widget %}
        {{ parent() }}

        {% if help is defined %}
            <span class="help">{{ help }}</span>
        {% endif %}
    {% endblock %}

Ao usar o PHP como template engine você tem que copiar o conteúdo do
template original:

.. code-block:: html+php

    <!-- field_widget.html.php -->

    <!-- Original content -->
    <input
        type="<?php echo isset($type) ? $view->escape($type) : "text" ?>"
        value="<?php echo $view->escape($value) ?>"
        <?php echo $view['form']->renderBlock('attributes') ?>
    />

    <!-- Customization -->
    <?php if (isset($help)) : ?>
        <span class="help"><?php echo $view->escape($help) ?></span>
    <?php endif ?>

Para renderizar uma mensagem de ajuda abaixo de um campo, passe em uma variável ``help``:

.. configuration-block::

    .. code-block:: jinja

        {{ form_widget(form.title, {'help': 'foobar'}) }}

    .. code-block:: php

        <?php echo $view['form']->widget($form['title'], array('help' => 'foobar')) ?>

.. tip::

    Veja:ref:`cookbook-form-theming-methods` para saber como aplicar essa personalização.

Usando Variáveis de ​​Formulário
------------------------------

A maioria das funções disponíveis para renderizar diferentes partes de um formulário (por exemplo,
o widget de formulário, a label do formulário, os erros de formulário, etc) também permitem que você faça certas
personalizações diretamente. Veja o exemplo a seguir:

.. configuration-block::

    .. code-block:: jinja

        {# render a widget, but add a "foo" class to it #}
        {{ form_widget(form.name, { 'attr': {'class': 'foo'} }) }}

    .. code-block:: php

        <!-- render a widget, but add a "foo" class to it -->
        <?php echo $view['form']->widget($form['name'], array(
            'attr' => array(
                'class' => 'foo',
            ),
        )) ?>

O array passado como segundo argumento contém "variáveis" de formulário. Para
mais detalhes sobre este conceito no Twig, veja :ref:`twig-reference-form-variables`.

.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/2.0/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
