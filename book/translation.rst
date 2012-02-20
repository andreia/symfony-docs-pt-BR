.. index::
   single: Tradu��es

Tradu��es
============

O termo "internacionaliza��o" se refere ao processo de abstrair strings
e outras pe�as cim locales espec�ficas para fora de sua aplica��o e dentro de uma camada
onde eles podem ser traduzidos e convertidos baseados na localiza��o do usu�rio (i.e.
idioma e p�is). Para o texto, isso significa englobar cada um com uma fun��o
capaz de traduzir o texto (ou "messagem") dentro do idioma do usu�rio::

    // text will *always* print out in English
    echo 'Hello World';

    // text can be translated into the end-user's language or default to English
    echo $translator->trans('Hello World');

.. note::

    O termo *locale* se refere rigorosamente ao idioma e pa�s do usu�rio. Isto
    pode ser qualquer string que sua aplica��o usa ent�o para gerenciar tradu��es
    e outras diferen�as de formato (e.g. formato de moeda). N�s recomendamos o c�digo
    de *linguagem* ISO639-1 , um underscore (``_``), ent�o o c�digo de *pa�s* 
    ISO3166 (e.g. ``fr_FR`` para Franc�s/Fran�a).

Nesse cap�tulo, n�s iremos aprender como preparar uma aplica��o para suportar m�ltiplos
locales e ent�o como criar tradu��es para locale e ent�o como criar tradu��es para m�ltiplos locales. No geral,
o processo tem v�rios passos comuns:

1. Abilitar e configurar o componente ``Translation`` do Symfony;

2. Abstrair strings (i.e. "messagens") por englob�-las em chamadas para o ``Translator``;

3. Criar translation resources para cada locale suportado que traduza
   cada mensagem na aplica��o;

4. Determinar, fixar e gerenciar a locazle do usu�rio na sess�o.

.. index::
   single: Tradu��es; Configura��o

Configura��o
-------------

Tradu��es s�o suportadas por um ``Translator`` :term:`service` que usa o
locale do usu�rio para observar e retornar mensagens traduzidas. Antes de usar isto,
abilite o ``Translator`` na sua configura��o:

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

A op��o ``fallback`` define o locale alternativo quando uma tradu��o 
n�o existe no locale do usu�rio.

.. tip::
    
    Quando a tradu��o n�o existe para um locale, o tradutor primeiro tenta 
    encontrar a tradu��o para o idioma (``fr`` se o locale �
    ``fr_FR`` por exemplo). Se isto tamb�m falhar, procura uma tradu��o
    usando a locale alternativa.

O locale usado em tradu��es � o que est� armazenado na sess�o do usu�rio.

.. index::
   single: Tradu��es; Tradu��o b�sica
   
Tradu��o b�sica
-----------------

Tradu��o do texto � feita done atrav�s do servi�o  ``translator`` 
(:class:`Symfony\\Component\\Translation\\Translator`). Para traduzir um bloco
de texto (chamado de *messagem*), use o
m�todo :method:`Symfony\\Component\\Translation\\Translator::trans`. Suponhamos,
por exemplo, que estamos traduzindo uma simples mensagem de dentro do controller:

.. code-block:: php

    public function indexAction()
    {
        $t = $this->get('translator')->trans('Symfony2 is great');

        return new Response($t);
    }

Quando esse c�digo � executado, Symfony2 ir� tentar traduzir a mensagem
"Symfony2 is great" baseado no ``locale`` do usu�rio. Para isto funcionar,
n�s precisamos informar o Symfony2 como traduzir a mensagem por um "translation resource", 
que � uma cole��o de tradu��es de mensagens para um locale especificado.
Esse "dicion�rio" de tradu��es pode ser criado em diferentes formatos variados,
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

Agora, se o idioma do locale do usu�rio � Franc�s (e.g. ``fr_FR`` ou ``fr_BE``),
a mensagem ir� ser traduzida para ``J'aime Symfony2``.

O processo de tradu��o
~~~~~~~~~~~~~~~~~~~~~~~

Para realmente traduzir a mensagem, Symfony2 usa um processo simples:

* O ``locale`` do usu�rio atual, que � armazenado na sess�o, � determinado;

* Um cat�logo de mensagens traduzidas � carregado de translation resources definidos
  pelo ``locale`` (e.g. ``fr_FR``). Messagens do locale alternativo s�o
  tamb�m carregados e adiconados ao catalogo se eles realmente n�o existem. The end
  result is a large "dictionary" of translations. See `Message Catalogues`_
  for more details;

* Se a mensagem � localizada no cat�logo, retorna a tradu��o. Se
  n�o, o tradutor retorna a mensagem original.

Quando usa o m�todo ``trans()``, Symfony2 procura pelo string exato dentro
do cat�logo de mensagem apropriada e o retorna (se ele existir).

.. index::
   single: Tradu��es; Espa�os reservados de mensagem

Espa�os reservados de mensagem
~~~~~~~~~~~~~~~~~~~~

�s vezes, uma mensagem conte�do uma vari�vel precisa ser traduzida:

.. code-block:: php

    public function indexAction($name)
    {
        $t = $this->get('translator')->trans('Hello '.$name);

        return new Response($t);
    }

Entretanto criar uma tradu��o para este string � imposs�vel visto que o tradutor
ir� tentar achar a mensagem exata, incluindo por��es da vari�vel
(e.g. "Hello Ryan" ou "Hello Fabien"). Ao inv�s escrever uma tradu��o
para toda intera��o poss�vel da mesma vari�vel ``$name`` , podemos substituir a
vari�vel com um "espa�o reservado":

.. code-block:: php

    public function indexAction($name)
    {
        $t = $this->get('translator')->trans('Hello %name%', array('%name%' => $name));

        new Response($t);
    }

Symfony2 ir� procurar uma tradu��o da mensagem pura (``Hello %name%``)
e *ent�o* substitui o espa�o reservado com os valores deles. Criar uma tradu��o
� exatamente como foi feito anteriormente:

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

    Os espa�os reservados podem suportar qualquer outro forma j� que a mensagem inteira � reconstru�da
    usando a fun��o PHP `strtr function`_. Entretanto, a nota��o ``%var%`` �
    requerida quando traduzir em templates Twig, e � no geral uma conven��o
    sensata a seguit.
    
Como podemos ver, criar uma tradu��o � um processo de dois passos:

1. Abstrair a mensagem que precisa ser traduzida por processamento atrav�s
   do ``Translator``.

2. Criar uma tradu��o para a mensagem em cada locale que voc� escolha 
   dar suporte.

O segundo passo � feito mediante criar cat�logos de mensagem que definem as tradu��es
para qualquer n�mero de locales diferentes.

.. index::
   single: Tradu��es; Cat�logo de Mensagens

Cat�logo de Mensagens
------------------

Quando uma mensagem � traduzida, Symfony2 compila um cat�logo de mensagem para
o locale do usu�rio e investiga por uma tradu��o da mensagem. Um cat�logo 
de mensagens � como um dicion�rio de tradu��es para um locale espec�fico.Por
exemplo, o cat�logo para o locale ``fr_FR`` poderia conter a seguinte
tradu��o:

    Symfony2 is Great => J'aime Symfony2

� responsabilidade do desenvolvedor (ou tradutor) de uma aplica��o 
internacionalizada criar essas tradu��es. Tradu��es s�o armazenadas no sistema 
de arquivos e descoberta pelo Symfony, gra�as a algumas conven��es.

.. tip::

    Cada vez que voc� criar um *novo* translation resource (ou instalar um pacote
    que inclua o translation resource), tenha certeza de limpar o cache ent�o
    aquele Symfony poder� detectar a o novo translation resource:
    
    .. code-block:: bash
    
        php app/console cache:clear

.. index::
   single: Translations; Translation resource locations

Localiza��o de Tradu��es e Conven��es de Nomeamento
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony2 procura por arquivos de mensagem (i.e. tradu��es) em duas localiza��es:

* Para mensagens encontradas no pacote,os arquivos de mensagem correspondente deveriam
  constar no diret�rio ``Resources/translations/`` do pacote;

* Para sobrepor qualquer tradu��o de pacote, coloque os arquivos de mensagem no
  diret�rio ``app/Resources/translations``.

O nome de arquivo das tradu��es � tamb�m importante j� que Symfony2 usa uma conven��o
para determinar detalhes sobre as tradu��es. Cada arquivo de messagem deve ser nomeado
de acordo com o seguinte padr�o: ``dom�nio.locale.loader``:

* **dom�nio**: Uma forma opcional de organizar mensagens em grupos (e.g. ``admin``,
  ``navigation`` ou o padr�o ``messages``) - veja `Usando Dom�nios de Mensagem`_;

* **locale**: O locale para a qual a tradu��o � feita (e.g. ``en_GB``, ``en``, etc);

* **loader**: Como Symfony2 deveria carregar e  analisar o arquivo (e.g. ``xliff``,
  ``php`` or ``yml``).

O loader poder ser o nome de qualquer loader registrado. Por padr�o, Symfony
providencia os seguintes loaders:

* ``xliff``: arquivo XLIFF;
* ``php``:   arquivo PHP;
* ``yml``:  arquivo YAML.

A escolha de qual loader � inteiramente tua e � uma quest�o de gosto.

.. note::
    
    Voc� tamb�m pode armazenar tradu��es em um banco de dados, ou outro armazenamento
    ao providenciar uma classe personalizada implementando a interface
    :class:`Symfony\\Component\\Translation\\Loader\\LoaderInterface`.
    Veja :doc:`Custom Translation Loaders </cookbook/translation/custom_loader>`
    abaixo para aprender como registrar loaders personalizados.

.. index::
   single: Tradu��es; Criando translation resources

Criando tradu��es
~~~~~~~~~~~~~~~~~~~~~

Cada arquivo consiste de uma s�rie de pares de tradu��o de id para um dado dom�nio e
locale. A id � o identificador para a tradu��o individual, e pode ser a mensagem do
locale principal (e.g. "Symfony is great") de sua aplica��p
ou um identificador �nico (e.g. "symfony2.great" - veja a barra lateral abaixo):

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
