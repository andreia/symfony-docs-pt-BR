.. index::
   single: Traduções

Traduções
============

O termo "internacionalização" se refere ao processo de abstrair strings
e outras peças cim locales específicas para fora de sua aplicação e dentro de uma camada
onde eles podem ser traduzidos e convertidos baseados na localização do usuário (i.e.
idioma e páis). Para o texto, isso significa englobar cada um com uma função
capaz de traduzir o texto (ou "messagem") dentro do idioma do usuário::

    // text will *always* print out in English
    echo 'Hello World';

    // text can be translated into the end-user's language or default to English
    echo $translator->trans('Hello World');

.. note::

    O termo *locale* se refere rigorosamente ao idioma e país do usuário. Isto
    pode ser qualquer string que sua aplicação usa então para gerenciar traduções
    e outras diferenças de formato (e.g. formato de moeda). Nós recomendamos o código
    de *linguagem* ISO639-1 , um underscore (``_``), então o código de *país* 
    ISO3166 (e.g. ``fr_FR`` para Francês/França).

Nesse capítulo, nós iremos aprender como preparar uma aplicação para suportar múltiplos
locales e então como criar traduções para locale e então como criar traduções para múltiplos locales. No geral,
o processo tem vários passos comuns:

1. Abilitar e configurar o componente ``Translation`` do Symfony;

2. Abstrair strings (i.e. "messagens") por englobá-las em chamadas para o ``Translator``;

3. Criar translation resources para cada locale suportado que traduza
   cada mensagem na aplicação;

4. Determinar, fixar e gerenciar a locazle do usuário na sessão.

.. index::
   single: Traduções; Configuração

Configuração
-------------

Traduções são suportadas por um ``Translator`` :term:`service` que usa o
locale do usuário para observar e retornar mensagens traduzidas. Antes de usar isto,
abilite o ``Translator`` na sua configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            translator: { fallback: en }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:translator fallback="en" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'translator' => array('fallback' => 'en'),
        ));

A opção ``fallback`` define o locale alternativo quando uma tradução 
não existe no locale do usuário.

.. tip::
    
    Quando a tradução não existe para um locale, o tradutor primeiro tenta 
    encontrar a tradução para o idioma (``fr`` se o locale é
    ``fr_FR`` por exemplo). Se isto também falhar, procura uma tradução
    usando a locale alternativa.

O locale usado em traduções é o que está armazenado na sessão do usuário.

.. index::
   single: Traduções; Tradução básica
   
Tradução básica
-----------------

Tradução do texto é feita done através do serviço  ``translator`` 
(:class:`Symfony\\Component\\Translation\\Translator`). Para traduzir um bloco
de texto (chamado de *messagem*), use o
método :method:`Symfony\\Component\\Translation\\Translator::trans`. Suponhamos,
por exemplo, que estamos traduzindo uma simples mensagem de dentro do controller:

.. code-block:: php

    public function indexAction()
    {
        $t = $this->get('translator')->trans('Symfony2 is great');

        return new Response($t);
    }

Quando esse código é executado, Symfony2 irá tentar traduzir a mensagem
"Symfony2 is great" baseado no ``locale`` do usuário. Para isto funcionar,
nós precisamos informar o Symfony2 como traduzir a mensagem por um "translation resource", 
que é uma coleção de traduções de mensagens para um locale especificado.
Esse "dicionário" de traduções pode ser criado em diferentes formatos variados,
sendo XLIFF o formato recomendado:

.. configuration-block::

    .. code-block:: xml

        <!-- messages.fr.xliff -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
                        <source>Symfony2 is great</source>
                        <target>J'aime Symfony2</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // messages.fr.php
        return array(
            'Symfony2 is great' => 'J\'aime Symfony2',
        );

    .. code-block:: yaml

        # messages.fr.yml
        Symfony2 is great: J'aime Symfony2

Agora, se o idioma do locale do usuário é Francês (e.g. ``fr_FR`` ou ``fr_BE``),
a mensagem irá ser traduzida para ``J'aime Symfony2``.

O processo de tradução
~~~~~~~~~~~~~~~~~~~~~~~

Para realmente traduzir a mensagem, Symfony2 usa um processo simples:

* O ``locale`` do usuário atual, que é armazenado na sessão, é determinado;

* Um catálogo de mensagens traduzidas é carregado de translation resources definidos
  pelo ``locale`` (e.g. ``fr_FR``). Messagens do locale alternativo são
  também carregados e adiconados ao catalogo se eles realmente não existem. The end
  result is a large "dictionary" of translations. See `Message Catalogues`_
  for more details;

* Se a mensagem é localizada no catálogo, retorna a tradução. Se
  não, o tradutor retorna a mensagem original.

Quando usa o método ``trans()``, Symfony2 procura pelo string exato dentro
do catálogo de mensagem apropriada e o retorna (se ele existir).

.. index::
   single: Traduções; Espaços reservados de mensagem

Espaços reservados de mensagem
~~~~~~~~~~~~~~~~~~~~

Às vezes, uma mensagem conteúdo uma variável precisa ser traduzida:

.. code-block:: php

    public function indexAction($name)
    {
        $t = $this->get('translator')->trans('Hello '.$name);

        return new Response($t);
    }

Entretanto criar uma tradução para este string é impossível visto que o tradutor
irá tentar achar a mensagem exata, incluindo porções da variável
(e.g. "Hello Ryan" ou "Hello Fabien"). Ao invés escrever uma tradução
para toda interação possível da mesma variável ``$name`` , podemos substituir a
variável com um "espaço reservado":

.. code-block:: php

    public function indexAction($name)
    {
        $t = $this->get('translator')->trans('Hello %name%', array('%name%' => $name));

        new Response($t);
    }

Symfony2 irá procurar uma tradução da mensagem pura (``Hello %name%``)
e *então* substitui o espaço reservado com os valores deles. Criar uma tradução
é exatamente como foi feito anteriormente:

.. configuration-block::

    .. code-block:: xml

        <!-- messages.fr.xliff -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
                        <source>Hello %name%</source>
                        <target>Bonjour %name%</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // messages.fr.php
        return array(
            'Hello %name%' => 'Bonjour %name%',
        );

    .. code-block:: yaml

        # messages.fr.yml
        'Hello %name%': Hello %name%

.. note::

    Os espaços reservados podem suportar qualquer outro forma já que a mensagem inteira é reconstruída
    usando a função PHP `strtr function`_. Entretanto, a notação ``%var%`` é
    requerida quando traduzir em templates Twig, e é no geral uma convenção
    sensata a seguit.
    
Como podemos ver, criar uma tradução é um processo de dois passos:

1. Abstrair a mensagem que precisa ser traduzida por processamento através
   do ``Translator``.

2. Criar uma tradução para a mensagem em cada locale que você escolha 
   dar suporte.

O segundo passo é feito mediante criar catálogos de mensagem que definem as traduções
para qualquer número de locales diferentes.

.. index::
   single: Traduções; Catálogo de Mensagens

Catálogo de Mensagens
------------------

Quando uma mensagem é traduzida, Symfony2 compila um catálogo de mensagem para
o locale do usuário e investiga por uma tradução da mensagem. Um catálogo 
de mensagens é como um dicionário de traduções para um locale específico.Por
exemplo, o catálogo para o locale ``fr_FR`` poderia conter a seguinte
tradução:

    Symfony2 is Great => J'aime Symfony2

É responsabilidade do desenvolvedor (ou tradutor) de uma aplicação 
internacionalizada criar essas traduções. Traduções são armazenadas no sistema 
de arquivos e descoberta pelo Symfony, graças a algumas convenções.

.. tip::

    Cada vez que você criar um *novo* translation resource (ou instalar um pacote
    que inclua o translation resource), tenha certeza de limpar o cache então
    aquele Symfony poderá detectar a o novo translation resource:
    
    .. code-block:: bash
    
        php app/console cache:clear

.. index::
   single: Translations; Translation resource locations

Localização de Traduções e Convenções de Nomeamento
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony2 procura por arquivos de mensagem (i.e. traduções) em duas localizações:

* Para mensagens encontradas no pacote,os arquivos de mensagem correspondente deveriam
  constar no diretório ``Resources/translations/`` do pacote;

* Para sobrepor qualquer tradução de pacote, coloque os arquivos de mensagem no
  diretório ``app/Resources/translations``.

O nome de arquivo das traduções é também importante já que Symfony2 usa uma convenção
para determinar detalhes sobre as traduções. Cada arquivo de messagem deve ser nomeado
de acordo com o seguinte padrão: ``domínio.locale.loader``:

* **domínio**: Uma forma opcional de organizar mensagens em grupos (e.g. ``admin``,
  ``navigation`` ou o padrão ``messages``) - veja `Usando Domínios de Mensagem`_;

* **locale**: O locale para a qual a tradução é feita (e.g. ``en_GB``, ``en``, etc);

* **loader**: Como Symfony2 deveria carregar e  analisar o arquivo (e.g. ``xliff``,
  ``php`` or ``yml``).

O loader poder ser o nome de qualquer loader registrado. Por padrão, Symfony
providencia os seguintes loaders:

* ``xliff``: arquivo XLIFF;
* ``php``:   arquivo PHP;
* ``yml``:  arquivo YAML.

A escolha de qual loader é inteiramente tua e é uma questão de gosto.

.. note::
    
    Você também pode armazenar traduções em um banco de dados, ou outro armazenamento
    ao providenciar uma classe personalizada implementando a interface
    :class:`Symfony\\Component\\Translation\\Loader\\LoaderInterface`.
    Veja :doc:`Custom Translation Loaders </cookbook/translation/custom_loader>`
    abaixo para aprender como registrar loaders personalizados.

.. index::
   single: Traduções; Criando translation resources

Criando traduções
~~~~~~~~~~~~~~~~~~~~~

Cada arquivo consiste de uma série de pares de tradução de id para um dado domínio e
locale. A id é o identificador para a tradução individual, e pode ser a mensagem do
locale principal (e.g. "Symfony is great") de sua aplicaçãp
ou um identificador único (e.g. "symfony2.great" - veja a barra lateral abaixo):

.. configuration-block::

    .. code-block:: xml

        <!-- src/Acme/DemoBundle/Resources/translations/messages.fr.xliff -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
                        <source>Symfony2 is great</source>
                        <target>J'aime Symfony2</target>
                    </trans-unit>
                    <trans-unit id="2">
                        <source>symfony2.great</source>
                        <target>J'aime Symfony2</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // src/Acme/DemoBundle/Resources/translations/messages.fr.php
        return array(
            'Symfony2 is great' => 'J\'aime Symfony2',
            'symfony2.great'    => 'J\'aime Symfony2',
        );

    .. code-block:: yaml

        # src/Acme/DemoBundle/Resources/translations/messages.fr.yml
        Symfony2 is great: J'aime Symfony2
        symfony2.great:    J'aime Symfony2

Symfony2 will discover these files and use them when translating either
"Symfony2 is great" or "symfony2.great" into a French language locale (e.g.
``fr_FR`` or ``fr_BE``).

.. sidebar:: Using Real or Keyword Messages

    This example illustrates the two different philosophies when creating
    messages to be translated:

    .. code-block:: php

        $t = $translator->trans('Symfony2 is great');

        $t = $translator->trans('symfony2.great');

    In the first method, messages are written in the language of the default
    locale (English in this case). That message is then used as the "id"
    when creating translations.

    In the second method, messages are actually "keywords" that convey the
    idea of the message. The keyword message is then used as the "id" for
    any translations. In this case, translations must be made for the default
    locale (i.e. to translate ``symfony2.great`` to ``Symfony2 is great``).

    The second method is handy because the message key won't need to be changed
    in every translation file if we decide that the message should actually
    read "Symfony2 is really great" in the default locale.

    The choice of which method to use is entirely up to you, but the "keyword"
    format is often recommended. 

    Additionally, the ``php`` and ``yaml`` file formats support nested ids to
    avoid repeating yourself if you use keywords instead of real text for your
    ids:

    .. configuration-block::

        .. code-block:: yaml

            symfony2:
                is:
                    great: Symfony2 is great
                    amazing: Symfony2 is amazing
                has:
                    bundles: Symfony2 has bundles
            user:
                login: Login

        .. code-block:: php

            return array(
                'symfony2' => array(
                    'is' => array(
                        'great' => 'Symfony2 is great',
                        'amazing' => 'Symfony2 is amazing',
                    ),
                    'has' => array(
                        'bundles' => 'Symfony2 has bundles',
                    ),
                ),
                'user' => array(
                    'login' => 'Login',
                ),
            );

    The multiple levels are flattened into single id/translation pairs by
    adding a dot (.) between every level, therefore the above examples are
    equivalent to the following:

    .. configuration-block::

        .. code-block:: yaml

            symfony2.is.great: Symfony2 is great
            symfony2.is.amazing: Symfony2 is amazing
            symfony2.has.bundles: Symfony2 has bundles
            user.login: Login

        .. code-block:: php

            return array(
                'symfony2.is.great' => 'Symfony2 is great',
                'symfony2.is.amazing' => 'Symfony2 is amazing',
                'symfony2.has.bundles' => 'Symfony2 has bundles',
                'user.login' => 'Login',
            );

.. index::
   single: Translations; Message domains

Using Message Domains
---------------------

As we've seen, message files are organized into the different locales that
they translate. The message files can also be organized further into "domains".
When creating message files, the domain is the first portion of the filename.
The default domain is ``messages``. For example, suppose that, for organization,
translations were split into three different domains: ``messages``, ``admin``
and ``navigation``. The French translation would have the following message
files:

* ``messages.fr.xliff``
* ``admin.fr.xliff``
* ``navigation.fr.xliff``

When translating strings that are not in the default domain (``messages``),
you must specify the domain as the third argument of ``trans()``:

.. code-block:: php

    $this->get('translator')->trans('Symfony2 is great', array(), 'admin');

Symfony2 will now look for the message in the ``admin`` domain of the user's
locale.

.. index::
   single: Translations; User's locale

Handling the User's Locale
--------------------------

The locale of the current user is stored in the session and is accessible
via the ``session`` service:

.. code-block:: php

    $locale = $this->get('session')->getLocale();

    $this->get('session')->setLocale('en_US');

.. index::
   single: Translations; Fallback and default locale

Fallback and Default Locale
~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the locale hasn't been set explicitly in the session, the ``fallback_locale``
configuration parameter will be used by the ``Translator``. The parameter
defaults to ``en`` (see `Configuration`_).

Alternatively, you can guarantee that a locale is set on the user's session
by defining a ``default_locale`` for the session service:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            session: { default_locale: en }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:session default-locale="en" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'session' => array('default_locale' => 'en'),
        ));

.. _book-translation-locale-url:

The Locale and the URL
~~~~~~~~~~~~~~~~~~~~~~

Since the locale of the user is stored in the session, it may be tempting
to use the same URL to display a resource in many different languages based
on the user's locale. For example, ``http://www.example.com/contact`` could
show content in English for one user and French for another user. Unfortunately,
this violates a fundamental rule of the Web: that a particular URL returns
the same resource regardless of the user. To further muddy the problem, which
version of the content would be indexed by search engines?

A better policy is to include the locale in the URL. This is fully-supported
by the routing system using the special ``_locale`` parameter:

.. configuration-block::

    .. code-block:: yaml

        contact:
            pattern:   /{_locale}/contact
            defaults:  { _controller: AcmeDemoBundle:Contact:index, _locale: en }
            requirements:
                _locale: en|fr|de

    .. code-block:: xml

        <route id="contact" pattern="/{_locale}/contact">
            <default key="_controller">AcmeDemoBundle:Contact:index</default>
            <default key="_locale">en</default>
            <requirement key="_locale">en|fr|de</requirement>
        </route>

    .. code-block:: php

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('contact', new Route('/{_locale}/contact', array(
            '_controller' => 'AcmeDemoBundle:Contact:index',
            '_locale'     => 'en',
        ), array(
            '_locale'     => 'en|fr|de'
        )));

        return $collection;

When using the special `_locale` parameter in a route, the matched locale
will *automatically be set on the user's session*. In other words, if a user
visits the URI ``/fr/contact``, the locale ``fr`` will automatically be set
as the locale for the user's session.

You can now use the user's locale to create routes to other translated pages
in your application.

.. index::
   single: Translations; Pluralization

Pluralization
-------------

Message pluralization is a tough topic as the rules can be quite complex. For
instance, here is the mathematic representation of the Russian pluralization
rules::

    (($number % 10 == 1) && ($number % 100 != 11)) ? 0 : ((($number % 10 >= 2) && ($number % 10 <= 4) && (($number % 100 < 10) || ($number % 100 >= 20))) ? 1 : 2);

As you can see, in Russian, you can have three different plural forms, each
given an index of 0, 1 or 2. For each form, the plural is different, and
so the translation is also different.

When a translation has different forms due to pluralization, you can provide
all the forms as a string separated by a pipe (``|``)::

    'There is one apple|There are %count% apples'

To translate pluralized messages, use the
:method:`Symfony\\Component\\Translation\\Translator::transChoice` method:

.. code-block:: php

    $t = $this->get('translator')->transChoice(
        'There is one apple|There are %count% apples',
        10,
        array('%count%' => 10)
    );

The second argument (``10`` in this example), is the *number* of objects being
described and is used to determine which translation to use and also to populate
the ``%count%`` placeholder.

Based on the given number, the translator chooses the right plural form.
In English, most words have a singular form when there is exactly one object
and a plural form for all other numbers (0, 2, 3...). So, if ``count`` is
``1``, the translator will use the first string (``There is one apple``)
as the translation. Otherwise it will use ``There are %count% apples``.

Here is the French translation::

    'Il y a %count% pomme|Il y a %count% pommes'

Even if the string looks similar (it is made of two sub-strings separated by a
pipe), the French rules are different: the first form (no plural) is used when
``count`` is ``0`` or ``1``. So, the translator will automatically use the
first string (``Il y a %count% pomme``) when ``count`` is ``0`` or ``1``.

Each locale has its own set of rules, with some having as many as six different
plural forms with complex rules behind which numbers map to which plural form.
The rules are quite simple for English and French, but for Russian, you'd
may want a hint to know which rule matches which string. To help translators,
you can optionally "tag" each string::

    'one: There is one apple|some: There are %count% apples'

    'none_or_one: Il y a %count% pomme|some: Il y a %count% pommes'

The tags are really only hints for translators and don't affect the logic
used to determine which plural form to use. The tags can be any descriptive
string that ends with a colon (``:``). The tags also do not need to be the
same in the original message as in the translated one.

.. tip:

    As tags are optional, the translator doesn't use them (the translator will
    only get a string based on its position in the string).

Explicit Interval Pluralization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The easiest way to pluralize a message is to let Symfony2 use internal logic
to choose which string to use based on a given number. Sometimes, you'll
need more control or want a different translation for specific cases (for
``0``, or when the count is negative, for example). For such cases, you can
use explicit math intervals::

    '{0} There are no apples|{1} There is one apple|]1,19] There are %count% apples|[20,Inf] There are many apples'

The intervals follow the `ISO 31-11`_ notation. The above string specifies
four different intervals: exactly ``0``, exactly ``1``, ``2-19``, and ``20``
and higher.

You can also mix explicit math rules and standard rules. In this case, if
the count is not matched by a specific interval, the standard rules take
effect after removing the explicit rules::

    '{0} There are no apples|[20,Inf] There are many apples|There is one apple|a_few: There are %count% apples'

For example, for ``1`` apple, the standard rule ``There is one apple`` will
be used. For ``2-19`` apples, the second standard rule ``There are %count%
apples`` will be selected.

An :class:`Symfony\\Component\\Translation\\Interval` can represent a finite set
of numbers::

    {1,2,3,4}

Or numbers between two other numbers::

    [1, +Inf[
    ]-1,2[

The left delimiter can be ``[`` (inclusive) or ``]`` (exclusive). The right
delimiter can be ``[`` (exclusive) or ``]`` (inclusive). Beside numbers, you
can use ``-Inf`` and ``+Inf`` for the infinite.

.. index::
   single: Translations; In templates

Translations in Templates
-------------------------

Most of the time, translation occurs in templates. Symfony2 provides native
support for both Twig and PHP templates.

Twig Templates
~~~~~~~~~~~~~~

Symfony2 provides specialized Twig tags (``trans`` and ``transchoice``) to
help with message translation of *static blocks of text*:

.. code-block:: jinja

    {% trans %}Hello %name%{% endtrans %}

    {% transchoice count %}
        {0} There are no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

The ``transchoice`` tag automatically gets the ``%count%`` variable from
the current context and passes it to the translator. This mechanism only
works when you use a placeholder following the ``%var%`` pattern.

.. tip::

    If you need to use the percent character (``%``) in a string, escape it by
    doubling it: ``{% trans %}Percent: %percent%%%{% endtrans %}``

You can also specify the message domain and pass some additional variables:

.. code-block:: jinja

    {% trans with {'%name%': 'Fabien'} from "app" %}Hello %name%{% endtrans %}

    {% trans with {'%name%': 'Fabien'} from "app" into "fr" %}Hello %name%{% endtrans %}

    {% transchoice count with {'%name%': 'Fabien'} from "app" %}
        {0} There is no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

The ``trans`` and ``transchoice`` filters can be used to translate *variable
texts* and complex expressions:

.. code-block:: jinja

    {{ message | trans }}

    {{ message | transchoice(5) }}

    {{ message | trans({'%name%': 'Fabien'}, "app") }}

    {{ message | transchoice(5, {'%name%': 'Fabien'}, 'app') }}

.. tip::

    Using the translation tags or filters have the same effect, but with
    one subtle difference: automatic output escaping is only applied to
    variables translated using a filter. In other words, if you need to
    be sure that your translated variable is *not* output escaped, you must
    apply the raw filter after the translation filter:

    .. code-block:: jinja

            {# text translated between tags is never escaped #}
            {% trans %}
                <h3>foo</h3>
            {% endtrans %}

            {% set message = '<h3>foo</h3>' %}

            {# a variable translated via a filter is escaped by default #}
            {{ message | trans | raw }}

            {# but static strings are never escaped #}
            {{ '<h3>foo</h3>' | trans }}

PHP Templates
~~~~~~~~~~~~~

The translator service is accessible in PHP templates through the
``translator`` helper:

.. code-block:: html+php

    <?php echo $view['translator']->trans('Symfony2 is great') ?>

    <?php echo $view['translator']->transChoice(
        '{0} There is no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        array('%count%' => 10)
    ) ?>

Forcing the Translator Locale
-----------------------------

When translating a message, Symfony2 uses the locale from the user's session
or the ``fallback`` locale if necessary. You can also manually specify the
locale to use for translation:

.. code-block:: php

    $this->get('translator')->trans(
        'Symfony2 is great',
        array(),
        'messages',
        'fr_FR',
    );

    $this->get('translator')->trans(
        '{0} There are no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        array('%count%' => 10),
        'messages',
        'fr_FR',
    );

Translating Database Content
----------------------------

The translation of database content should be handled by Doctrine through
the `Translatable Extension`_. For more information, see the documentation
for that library.

Summary
-------

With the Symfony2 Translation component, creating an internationalized application
no longer needs to be a painful process and boils down to just a few basic
steps:

* Abstract messages in your application by wrapping each in either the
  :method:`Symfony\\Component\\Translation\\Translator::trans` or
  :method:`Symfony\\Component\\Translation\\Translator::transChoice` methods;

* Translate each message into multiple locales by creating translation message
  files. Symfony2 discovers and processes each file because its name follows
  a specific convention;

* Manage the user's locale, which is stored in the session.

.. _`strtr function`: http://www.php.net/manual/en/function.strtr.php
.. _`ISO 31-11`: http://en.wikipedia.org/wiki/Interval_%28mathematics%29#The_ISO_notation
.. _`Translatable Extension`: https://github.com/l3pp4rd/DoctrineExtensions
