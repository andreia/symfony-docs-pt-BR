.. index::
   single: Traduções

Traduções
=========

O termo "internacionalização" se refere ao processo de abstrair strings
e outras peças com localidades específicas para fora de sua aplicação e dentro de uma camada
onde eles podem ser traduzidos e convertidos baseados na localização do usuário (em outras palavras,
idioma e páis). Para o texto, isso significa englobar cada um com uma função
capaz de traduzir o texto (ou "messagem") dentro do idioma do usuário::

    // text will *always* print out in English
    echo 'Hello World';

    // text can be translated into the end-user's language or default to English
    echo $translator->trans('Hello World');

.. note::

    O termo *localidade* se refere rigorosamente ao idioma e país do usuário. Isto
    pode ser qualquer string que sua aplicação usa então para gerenciar traduções
    e outras diferenças de formato (ex: formato de moeda). Nós recomendamos o código
    de *linguagem* ISO639-1 , um underscore (``_``), então o código de *país* 
    ISO3166 (ex: ``fr_FR`` para Francês/França).

Nesse capítulo, nós iremos aprender como preparar uma aplicação para suportar múltiplas
localidades e então como criar traduções para localidade e então como criar traduções para múltiplas localidades. No geral,
o processo tem vários passos comuns:

1. Abilitar e configurar o componente ``Translation`` do Symfony;

2. Abstrair strings (em outras palavras, "messagens") por englobá-las em chamadas para o ``Translator``;

3. Criar translation resources para cada localidade suportada que traduza
   cada mensagem na aplicação;

4. Determinar, fixar e gerenciar a localidade do usuário na sessão.

.. index::
   single: Traduções; Configuração

Configuração
-------------

Traduções são suportadas por um ``Translator`` :term:`service` que usa o
localidadedo usuário para observar e retornar mensagens traduzidas. Antes de usar isto,
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

A opção ``fallback`` define a localidade alternativa quando uma tradução 
não existe no localidadedo usuário.

.. tip::
    
    Quando a tradução não existe para uma localidade, o tradutor primeiro tenta 
    encontrar a tradução para o idioma (``fr`` se o localidade é
    ``fr_FR`` por exemplo). Se isto também falhar, procura uma tradução
    usando a localidade alternativa.

O localidade usada em traduções é a que está armazenada na sessão do usuário.

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
"Symfony2 is great" baseada na ``localidade`` do usuário. Para isto funcionar,
nós precisamos informar o Symfony2 como traduzir a mensagem por um "translation resource", 
que é uma coleção de traduções de mensagens para um localidade especificada.
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

Agora, se o idioma do localidade do usuário é Francês (ex: ``fr_FR`` ou ``fr_BE``),
a mensagem irá ser traduzida para ``J'aime Symfony2``.

O processo de tradução
~~~~~~~~~~~~~~~~~~~~~~~

Para realmente traduzir a mensagem, Symfony2 usa um processo simples:

* A ``localidade`` do usuário atual, que é armazenada na sessão, é determinada;

* Um catálogo de mensagens traduzidas é carregado de translation resources definidos
  pelo ``locale`` (ex: ``fr_FR``). Messagens da localidade alternativa são
  também carregadas e adiconadas ao catalogo se elas realmente não existem. O resultado
  final é um grande "dicionário" de traduções. Veja `Catálogo de Mensagens`_
  para mais detalhes;

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
(ex: "Hello Ryan" ou "Hello Fabien"). Ao invés escrever uma tradução
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

2. Criar uma tradução para a mensagem em cada localidade que você escolha 
   dar suporte.

O segundo passo é feito mediante criar catálogos de mensagem que definem as traduções
para qualquer número de localidades diferentes.

.. index::
   single: Traduções; Catálogo de Mensagens

Catálogo de Mensagens
------------------

Quando uma mensagem é traduzida, Symfony2 compila um catálogo de mensagem para
a localidade do usuário e investiga por uma tradução da mensagem. Um catálogo 
de mensagens é como um dicionário de traduções para uma localidade específica. Por
exemplo, o catálogo para a localidade``fr_FR`` poderia conter a seguinte
tradução:

    Symfony2 is Great => J'aime Symfony2

É responsabilidade do desenvolvedor (ou tradutor) de uma aplicação 
internacionalizada criar essas traduções. Traduções são armazenadas no sistema 
de arquivos e descoberta pelo Symfony, graças a algumas convenções.

.. tip::

    Cada vez que você criar um *novo* translation resource (ou instalar um pacote
    que inclua o translation resource), tenha certeza de limpar o cache então
    aquele Symfony poderá detectar o novo translation resource:
    
    .. code-block:: bash
    
        php app/console cache:clear

.. index::
   single: Traduções; Localizações de Translation resource 

Localização de Traduções e Convenções de Nomeamento
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony2 procura por arquivos de mensagem (em outras palavras, traduções) em duas localizações:

* Para mensagens encontradas no pacote,os arquivos de mensagem correspondente deveriam
  constar no diretório ``Resources/translations/`` do pacote;

* Para sobrepor qualquer tradução de pacote, coloque os arquivos de mensagem no
  diretório ``app/Resources/translations``.

O nome de arquivo das traduções é também importante, já que Symfony2 usa uma convenção
para determinar detalhes sobre as traduções. Cada arquivo de messagem deve ser nomeado
de acordo com o seguinte padrão: ``domínio.localidade.carregador``:

* **domínio**: Uma forma opcional de organizar mensagens em grupos (ex: ``admin``,
  ``navigation`` ou o padrão ``messages``) - veja `Usando Domínios de Mensagem`_;

* **localidade**: A localidade para a qual a tradução é feita (ex: ``en_GB``, ``en``, etc);

* **carregador**: Como Symfony2 deveria carregar e  analisar o arquivo (ex: ``xliff``,
  ``php`` or ``yml``).

O carregador poder ser o nome de qualquer carregador registrado. Por padrão, Symfony
providencia os seguintes carregadores:

* ``xliff``: arquivo XLIFF;
* ``php``:   arquivo PHP;
* ``yml``:  arquivo YAML.

A escolha de qual carregador é inteiramente tua e é uma questão de gosto.

.. note::
    
    Você também pode armazenar traduções em um banco de dados, ou outro armazenamento
    ao providenciar uma classe personalizada implementando a interface
    :class:`Symfony\\Component\\Translation\\Loader\\LoaderInterface`.
    Veja :doc:`Custom Translation Loaders </cookbook/translation/custom_loader>`
    abaixo para aprender como registrar carregadores personalizados.

.. index::
   single: Traduções; Criando translation resources

Criando traduções
~~~~~~~~~~~~~~~~~~~~~

Cada arquivo consiste de uma série de pares de tradução de id para um dado domínio e
locale. A id é o identificador para a tradução individual, e pode ser a mensagem da
localidade principal (ex: "Symfony is great") de sua aplicaçãp
ou um identificador único (ex: "symfony2.great" - veja a barra lateral abaixo):

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

Symfony2 irá descobrir esses arquivos e usá-los quando ou traduzir
"Symfony2 is great" ou "symfony2.great" no localidade do idioma Francês (ex:
``fr_FR`` ou ``fr_BE``).

.. sidebar:: Usando Mensagens Reais ou Palavras-Chave
    
    Esse exemplo ilustra duas diferentes filosofias quando criar 
    mensagens para serem traduzidas:

    .. code-block:: php

        $t = $translator->trans('Symfony2 is great');

        $t = $translator->trans('symfony2.great');

    No primeiro método, mensagens são escritas no idioma do localidade padrão
    (Inglês neste caso). Aquela mensagem é então usada como "id" quando criar
    traduções.
    
    No segundo método, mensagens são realmente "palavras-chave" que contém 
    a idéia da mensagem. A mensagem de palavras-chave é então usada como o "id"
    de qualquer tradução. Neste caso, traduções devem ser feitas para o locale
    padrão (em outras palavras, traduzir ``symfony2.great`` para ``Symfony2 is great``).

    O segundo método é prático porque a chave da mensagem não precisará ser mudada
    em cada arquivo de tradução se decidirmos que a mensagem realmente deveria
    ler "Symfony2 is really great" no localidade padrão.
    
    A escolha de qual método usar é inteiramente sua, mas o formato de "palavra-chave"
    é frequentemente recomendada.
    
    Adicionalmente, os formatos de arquivo ``php`` e ``yaml`` suportam ids encaixadas
    para que você evite repetições se você usar palavras-chave ao invés do texto real 
    para suas ids:

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
    
    Os níveis múltiplos são achatados em id unitária / pares de tradução ao
    adicionar um ponto (.) entre cada nível, portanto os exemplos acima 
    são equivalentes ao seguinte:

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
   single: Traduções; Domínios de mensagem

Usando Domínios de mensagem
---------------------

Como nós vimos, arquivos de mensagem são organizados em diferentes localidades que 
eles traduzem. Os arquivos de mensagem podem também ser organizados além de "domínios".
O domínio padrão é ``messages``. Por exemplo, suponha que, para organização, 
traduções foram divididas em três domínios diferentes: ``messages``, ``admin``
e ``navigation``. A tradução Francesa teria os seguintes arquivos de
mensagem:

* ``messages.fr.xliff``
* ``admin.fr.xliff``
* ``navigation.fr.xliff``

Quando traduzir strings que não estão no domínio padrão (``messages``),
você deve especificar o domínio como terceiro argumento de ``trans()``:

.. code-block:: php

    $this->get('translator')->trans('Symfony2 is great', array(), 'admin');

Symfony2 irá pesquisar pela mensagem no domínio``admin`` da localidade do
usuário.

.. index::
   single: Traduções; Localidade do usuário

Tratando a localidade do Usuário
--------------------------

A localidade do usuário atual é armazenada na sessão e é acessível
via serviço ``session`:

.. code-block:: php

    $locale= $this->get('session')->getLocale();

    $this->get('session')->setLocale('en_US');

.. index::
   single: Traduções; localidade padrão e alternativo

Localidade padrão e alternativa
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se a localidade não foi fixada explicitamente na sessão , o parâmetro de 
configuração ``fallback_locale`` será usada pelo ``Translator``. O parâmetro
é padronizado para ``en`` (veja `Configuração`_).

Alternativamente, você pode garantir que uma localidade é fixada na sessão do usuário
definindo um ``default_locale`` para o serviço de sessão:

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

a localidade and a URL
~~~~~~~~~~~~~~~~~~~~~~

Como a localidade do usuário é armazenada na sessão, pode ser tentador
usar a mesma URL para mostrar o recurso em muitos idiomas diferentes baseados
na localidade do usuário.Por exemplo, ``http://www.example.com/contact`` poderia
mostrar conteúdo em Inglês para um usuário e Francês para outro usuário. Infelizmente,
isso viola uma regra fundamental da Web: que um URL particular retorne 
o mesmo recurso independente do usuário. Para complicar ainda o problema, qual 
versão do conteúdo deveria ser indexado pelas ferramentas de pesquisa ?

Uma melhor política é incluir a localidade na URL. Isso é totalmente suportado
pelo sistema de roteamnto usando o parâmetro especial ``_locale``:

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

Quando usar o parâmetro especial `_locale` numa rota, a localidade encontrada
será *automaticamente estabelecida na sessão do usuário*. Em outras palavras, se um usuário
visita a URI ``/fr/contact``, a localidade``fr`` será automaticamente estabelecida
como a localidade para a sessão do usuário.

Você pode agora usar a localidade do usuário para criar rotas para outras páginas traduzidas
na sua aplicação.

.. index::
   single: Traduções; Pluralização

Pluralização
-------------

Pluralização de mensagem é um tópico difícil já que as regras podem ser bem complexas. Por
convenção, aqui está a representação matemática das regrad de pluralização Russa::

    (($number % 10 == 1) && ($number % 100 != 11)) ? 0 : ((($number % 10 >= 2) && ($number % 10 <= 4) && (($number % 100 < 10) || ($number % 100 >= 20))) ? 1 : 2);

Como você viu, em Russo, você pode ter três formas diferentes de plural, cada uma
com index de 0, 1 ou 2. Para cada forma, o plural é diferente, e então
a tradução também é diferente.

Quando uma tradução tem formas diferentes devido à pluralização, você pode providenciar
todas as formas como string separadas por barra vertical (``|``)::

    'There is one apple|There are %count% apples'

Para traduzir mensagens pluralizadas, use o método:
:method:`Symfony\\Component\\Translation\\Translator::transChoice`

.. code-block:: php

    $t = $this->get('translator')->transChoice(
        'There is one apple|There are %count% apples',
        10,
        array('%count%' => 10)
    );

O segundo argumento (``10`` neste exemplo), é o *número* de objetos sendo
descritos e é usado para determinar qual tradução usar e também para preencher
o espaço reservado ``%count%``.

Baseado em certo número, o tradutor escolhe a forma correta do plural.
Em Inglês, muitas palavras tem uma forma singular quando existe exatamente um objeto
e uma forma no plural para todos os outros números (0, 2, 3...). Então, se ``count`` é
``1``, o tradutor usará a primeira string (``There is one apple``)
como tradução. Senão irá usar ``There are %count% apples``.

Aqui está a tradução Francesa::

    'Il y a %count% pomme|Il y a %count% pommes'

Mesmo se a string parecer similar (é feita de duas substrings separadas por
barra vertical), as regras Francesas são diferentes: a primeira forma (sem plural) é usada quando
``count`` is ``0`` or ``1``. Então, o tradutor irá automaticamente usar a
primeira string (``Il y a %count% pomme``) quando ``count`` é ``0`` ou ``1``.

Cada localidade tem sua própria lista de regras, com algumas tendo tanto quanto seis formas
diferentes de plural com regras complexas por trás de quais números de mapas de quais formas no plural.
As regras são bem simples para Inglês e Francês, mas para Russo, você
poderia querer um palpite para conhecer quais regras combinam com qual string. Para ajudar tradutores,
você pode opcionalmente "atribuir uma tag" a cada string::

    'one: There is one apple|some: There are %count% apples'

    'none_or_one: Il y a %count% pomme|some: Il y a %count% pommes'

As tags são realmente as únicas pistas para tradutores e não afetam a lógica
usada para determinar qual forma plural usar. As tags podem ser qualquer string
descritiva que termine com dois pontos (``:``). As tags traduzidas também não são necessariamente a mesma
que as da mensagem original.

.. tip:

    Como tags são opcionais, o tradutor não usa elas (o tradutor irá 
    somente obter uma string baseada na posição do string).

Pluralização de Intervalo Explícito
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A maneira mais fácil de pluralizar uma mensagem é deixar o Symfony2 usar lógica interna
para escolher qual string usar, baseando em um número fornecido. Às vezes, você irá 
precisar de mais controle ou querer uma tradução diferente para casos específicos (para
``0``, ou quando o contador é negativo, por exemplo). Para certos casos, você pode
usar intervalos matemáticos explícitos::

    '{0} There are no apples|{1} There is one apple|]1,19] There are %count% apples|[20,Inf] There are many apples'

Os intervalos seguem a notação `ISO 31-11`_. A string acima especifica
quatro intervalos diferentes: exatamente ``0``, exatamente ``1``, ``2-19``, e ``20``
e mais altos.

Você também pode misturar regras matemáticas explícitas e regras padrão. Nesse caso, se
o contador não combinar com um intervalo específico, as regras padrão, terão
efeito após remover as regras explícitas::

    '{0} There are no apples|[20,Inf] There are many apples|There is one apple|a_few: There are %count% apples'

Por exemplo, para ``1`` maçã, a regra padrão ``There is one apple`` será
usada. Para ``2-19`` apples, a segunda regra padrão ``There are %count%
apples`` será selecionada.

Uma classe :class:`Symfony\\Component\\Translation\\Interval` pode representar um conjunto finito
de números::

    {1,2,3,4}

Ou números entre dois outros números::

    [1, +Inf[
    ]-1,2[

O delimitador esquerdo pode ser ``[`` (inclusivo) or ``]`` (exclusivo). O delimitador
direito pode ser ``[`` (exclusivo) or ``]`` (inclusivo). Além de números, você
pode usar ``-Inf`` e ``+Inf`` para infinito.

.. index::
   single: Tranduções; Em templates

Traduções em Templates
-------------------------

A maior parte do tempo, traduções ocorrem em templates. Symfony2 providencia
suporte nativo para ambos os templates PHP e Twig.

Templates Twig
~~~~~~~~~~~~~~

Symfony2 providencia tags Twig especializadas (``trans`` e ``transchoice``) para
ajudar com tradução de mensagem de *blocos estáticos de texto*:

.. code-block:: jinja

    {% trans %}Hello %name%{% endtrans %}

    {% transchoice count %}
        {0} There are no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

A tag ``transchoice`` automaticamente obtém a variável ``%count%`` do
contexto atual e a passa para o tradutor. Esse mecanismo só funciona
quando você usa um espaço reservado seguindo o padrão ``%var%``.

.. tip::
    
    Se você precisa usar o caractere de percentual (``%``) em uma string, escape dela ao
    dobrá-la: ``{% trans %}Percent: %percent%%%{% endtrans %}``

Você também pode especificar o domínio da mensagem e passar algumas variáveis adicionais:

.. code-block:: jinja

    {% trans with {'%name%': 'Fabien'} from "app" %}Hello %name%{% endtrans %}

    {% trans with {'%name%': 'Fabien'} from "app" into "fr" %}Hello %name%{% endtrans %}

    {% transchoice count with {'%name%': 'Fabien'} from "app" %}
        {0} There is no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

Os filtros ``trans`` e ``transchoice`` podem ser usados para traduzir *textos de 
variáveis* e expressões complexas:

.. code-block:: jinja

    {{ message | trans }}

    {{ message | transchoice(5) }}

    {{ message | trans({'%name%': 'Fabien'}, "app") }}

    {{ message | transchoice(5, {'%name%': 'Fabien'}, 'app') }}

.. tip::
    
    Usando as tags de tradução ou filtros que tenham o mesmo efeito, mas com
    uma diferença sutil: saída para escape automático só é aplicada para
    variáveis traduzidas por utilização de filtro. Em outras palavras, se você precisar
    estar certo que sua variável traduzida *não* é uma saída para escape, você precisa
    aplicar o filtro bruto após o filtro de tradução.
    
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

Templates PHP
~~~~~~~~~~~~~

O serviço tradutor é acessível em templates PHP através do 
helper ``translator``:

.. code-block:: html+php

    <?php echo $view['translator']->trans('Symfony2 is great') ?>

    <?php echo $view['translator']->transChoice(
        '{0} There is no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        array('%count%' => 10)
    ) ?>

Forçando a Localidade Tradutora
-----------------------------

Quando traduzir a mensagem, Symfony2 usa a localidade da sessão do usuário
ou a localidade ``alternativa`` se necessária. Você também pode especificar manualmente a
localidade a usar para a tradução:

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

Traduzindo Conteúdo de Banco de Dados
----------------------------

Quando a tradução do conteúdo do banco de dados deveria ser manipulada pelo Doctrine
através do `Translatable Extension`_. Para mais informações, veja a documentação
para aquela biblioteca.

Sumário
-------

Com o componente Translation do Symfony2, criar uma aplicação internacionalizada
não precisa mais ser um processo dolorido e desgastante para somente uns passos
básicos:

* Mensagens abstratas na sua aplicação ao englobar cada uma ou com métodos
  :method:`Symfony\\Component\\Translation\\Translator::trans` ou
  :method:`Symfony\\Component\\Translation\\Translator::transChoice`;

* Traduza cada mensagem em localidades múltiplas ao criar arquivos de tradução
  de mensagem. Symfony2 descobre e processa cada arquivo porque o nome dele segue
  uma convenção específica;

* Gerenciar a localidade do usuário, que é armazenada na sessão.

.. _`strtr function`: http://www.php.net/manual/en/function.strtr.php
.. _`ISO 31-11`: http://en.wikipedia.org/wiki/Interval_%28mathematics%29#The_ISO_notation
.. _`Translatable Extension`: https://github.com/l3pp4rd/DoctrineExtensions
