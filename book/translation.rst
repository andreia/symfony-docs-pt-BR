.. index::
   single: Tradu��es

Tradu��es
============

O termo "internacionaliza��o" se refere ao processo de abstrair strings
e outras pe�as cim localidades espec�ficas para fora de sua aplica��o e dentro de uma camada
onde eles podem ser traduzidos e convertidos baseados na localiza��o do usu�rio (em outras palavras,
idioma e p�is). Para o texto, isso significa englobar cada um com uma fun��o
capaz de traduzir o texto (ou "messagem") dentro do idioma do usu�rio::

    // text will *always* print out in English
    echo 'Hello World';

    // text can be translated into the end-user's language or default to English
    echo $translator->trans('Hello World');

.. note::

    O termo *localidade* se refere rigorosamente ao idioma e pa�s do usu�rio. Isto
    pode ser qualquer string que sua aplica��o usa ent�o para gerenciar tradu��es
    e outras diferen�as de formato (ex: formato de moeda). N�s recomendamos o c�digo
    de *linguagem* ISO639-1 , um underscore (``_``), ent�o o c�digo de *pa�s* 
    ISO3166 (ex: ``fr_FR`` para Franc�s/Fran�a).

Nesse cap�tulo, n�s iremos aprender como preparar uma aplica��o para suportar m�ltiplas
localidades e ent�o como criar tradu��es para localidade e ent�o como criar tradu��es para m�ltiplas localidades. No geral,
o processo tem v�rios passos comuns:

1. Abilitar e configurar o componente ``Translation`` do Symfony;

2. Abstrair strings (em outras palavras, "messagens") por englob�-las em chamadas para o ``Translator``;

3. Criar translation resources para cada localidade suportada que traduza
   cada mensagem na aplica��o;

4. Determinar, fixar e gerenciar a locazle do usu�rio na sess�o.

.. index::
   single: Tradu��es; Configura��o

Configura��o
-------------

Tradu��es s�o suportadas por um ``Translator`` :term:`service` que usa o
localidadedo usu�rio para observar e retornar mensagens traduzidas. Antes de usar isto,
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

A op��o ``fallback`` define a localidade alternativa quando uma tradu��o 
n�o existe no localidadedo usu�rio.

.. tip::
    
    Quando a tradu��o n�o existe para uma localidade, o tradutor primeiro tenta 
    encontrar a tradu��o para o idioma (``fr`` se o localidade �
    ``fr_FR`` por exemplo). Se isto tamb�m falhar, procura uma tradu��o
    usando a localidade alternativa.

O localidade usada em tradu��es � a que est� armazenada na sess�o do usu�rio.

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
"Symfony2 is great" baseada na ``localidade`` do usu�rio. Para isto funcionar,
n�s precisamos informar o Symfony2 como traduzir a mensagem por um "translation resource", 
que � uma cole��o de tradu��es de mensagens para um localidade especificada.
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

Agora, se o idioma do localidade do usu�rio � Franc�s (ex: ``fr_FR`` ou ``fr_BE``),
a mensagem ir� ser traduzida para ``J'aime Symfony2``.

O processo de tradu��o
~~~~~~~~~~~~~~~~~~~~~~~

Para realmente traduzir a mensagem, Symfony2 usa um processo simples:

* A ``localidade`` do usu�rio atual, que � armazenada na sess�o, � determinada;

* Um cat�logo de mensagens traduzidas � carregado de translation resources definidos
  pelo ``locale`` (ex: ``fr_FR``). Messagens da localidade alternativa s�o
  tamb�m carregadas e adiconadas ao catalogo se elas realmente n�o existem. O resultado
  final � um grande "dicion�rio" de tradu��es. Veja `Cat�logo de Mensagens`_
  para mais detalhes;

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
(ex: "Hello Ryan" ou "Hello Fabien"). Ao inv�s escrever uma tradu��o
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

2. Criar uma tradu��o para a mensagem em cada localidade que voc� escolha 
   dar suporte.

O segundo passo � feito mediante criar cat�logos de mensagem que definem as tradu��es
para qualquer n�mero de localidades diferentes.

.. index::
   single: Tradu��es; Cat�logo de Mensagens

Cat�logo de Mensagens
------------------

Quando uma mensagem � traduzida, Symfony2 compila um cat�logo de mensagem para
a localidade do usu�rio e investiga por uma tradu��o da mensagem. Um cat�logo 
de mensagens � como um dicion�rio de tradu��es para uma localidade espec�fica. Por
exemplo, o cat�logo para a localidade``fr_FR`` poderia conter a seguinte
tradu��o:

    Symfony2 is Great => J'aime Symfony2

� responsabilidade do desenvolvedor (ou tradutor) de uma aplica��o 
internacionalizada criar essas tradu��es. Tradu��es s�o armazenadas no sistema 
de arquivos e descoberta pelo Symfony, gra�as a algumas conven��es.

.. tip::

    Cada vez que voc� criar um *novo* translation resource (ou instalar um pacote
    que inclua o translation resource), tenha certeza de limpar o cache ent�o
    aquele Symfony poder� detectar o novo translation resource:
    
    .. code-block:: bash
    
        php app/console cache:clear

.. index::
   single: Tradu��es; Localiza��es de Translation resource 

Localiza��o de Tradu��es e Conven��es de Nomeamento
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony2 procura por arquivos de mensagem (em outras palavras, tradu��es) em duas localiza��es:

* Para mensagens encontradas no pacote,os arquivos de mensagem correspondente deveriam
  constar no diret�rio ``Resources/translations/`` do pacote;

* Para sobrepor qualquer tradu��o de pacote, coloque os arquivos de mensagem no
  diret�rio ``app/Resources/translations``.

O nome de arquivo das tradu��es � tamb�m importante, j� que Symfony2 usa uma conven��o
para determinar detalhes sobre as tradu��es. Cada arquivo de messagem deve ser nomeado
de acordo com o seguinte padr�o: ``dom�nio.localidade.carregador``:

* **dom�nio**: Uma forma opcional de organizar mensagens em grupos (ex: ``admin``,
  ``navigation`` ou o padr�o ``messages``) - veja `Usando Dom�nios de Mensagem`_;

* **localidade**: A localidade para a qual a tradu��o � feita (ex: ``en_GB``, ``en``, etc);

* **carregador**: Como Symfony2 deveria carregar e  analisar o arquivo (ex: ``xliff``,
  ``php`` or ``yml``).

O carregador poder ser o nome de qualquer carregador registrado. Por padr�o, Symfony
providencia os seguintes carregadores:

* ``xliff``: arquivo XLIFF;
* ``php``:   arquivo PHP;
* ``yml``:  arquivo YAML.

A escolha de qual carregador � inteiramente tua e � uma quest�o de gosto.

.. note::
    
    Voc� tamb�m pode armazenar tradu��es em um banco de dados, ou outro armazenamento
    ao providenciar uma classe personalizada implementando a interface
    :class:`Symfony\\Component\\Translation\\Loader\\LoaderInterface`.
    Veja :doc:`Custom Translation Loaders </cookbook/translation/custom_loader>`
    abaixo para aprender como registrar carregadores personalizados.

.. index::
   single: Tradu��es; Criando translation resources

Criando tradu��es
~~~~~~~~~~~~~~~~~~~~~

Cada arquivo consiste de uma s�rie de pares de tradu��o de id para um dado dom�nio e
locale. A id � o identificador para a tradu��o individual, e pode ser a mensagem da
localidade principal (ex: "Symfony is great") de sua aplica��p
ou um identificador �nico (ex: "symfony2.great" - veja a barra lateral abaixo):

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

Symfony2 ir� descobrir esses arquivos e us�-los quando ou traduzir
"Symfony2 is great" ou "symfony2.great" no localidade do idioma Franc�s (ex:
``fr_FR`` ou ``fr_BE``).

.. sidebar:: Usando Mensagens Reais ou Palavras-Chave
    
    Esse exemplo ilustra duas diferentes filosofias quando criar 
    mensagens para serem traduzidas:

    .. code-block:: php

        $t = $translator->trans('Symfony2 is great');

        $t = $translator->trans('symfony2.great');

    No primeiro m�todo, mensagens s�o escritas no idioma do localidade padr�o
    (Ingl�s neste caso). Aquela mensagem � ent�o usada como "id" quando criar
    tradu��es.
    
    No segundo m�todo, mensagens s�o realmente "palavras-chave" que cont�m 
    a id�ia da mensagem. A mensagem de palavras-chave � ent�o usada como o "id"
    de qualquer tradu��o. Neste caso, tradu��es devem ser feitas para o locale
    padr�o (em outras palavras, traduzir ``symfony2.great`` para ``Symfony2 is great``).

    O segundo m�todo � pr�tico porque a chave da mensagem n�o precisar� ser mudada
    em cada arquivo de tradu��o se decidirmos que a mensagem realmente deveria
    ler "Symfony2 is really great" no localidade padr�o.
    
    A escolha de qual m�todo usar � inteiramente sua, mas o formato de "palavra-chave"
    � frequentemente recomendada.
    
    Adicionalmente, os formatos de arquivo ``php`` e ``yaml`` suportam ids encaixadas
    para que voc� evite repeti��es se voc� usar palavras-chave ao inv�s do texto real 
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
    
    Os n�veis m�ltiplos s�o achatados em id unit�ria / pares de tradu��o ao
    adicionar um ponto (.) entre cada n�vel, portanto os exemplos acima 
    s�o equivalentes ao seguinte:

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
   single: Tradu��es; Dom�nios de mensagem

Usando Dom�nios de mensagem
---------------------

Como n�s vimos, arquivos de mensagem s�o organizados em diferentes localidades que 
eles traduzem. Os arquivos de mensagem podem tamb�m ser organizados al�m de "dom�nios".
O dom�nio padr�o � ``messages``. Por exemplo, suponha que, para organiza��o, 
tradu��es foram divididas em tr�s dom�nios diferentes: ``messages``, ``admin``
e ``navigation``. A tradu��o Francesa teria os seguintes arquivos de
mensagem:

* ``messages.fr.xliff``
* ``admin.fr.xliff``
* ``navigation.fr.xliff``

Quando traduzir strings que n�o est�o no dom�nio padr�o (``messages``),
voc� deve especificar o dom�nio como terceiro argumento de ``trans()``:

.. code-block:: php

    $this->get('translator')->trans('Symfony2 is great', array(), 'admin');

Symfony2 ir� pesquisar pela mensagem no dom�nio``admin`` da localidade do
usu�rio.

.. index::
   single: Tradu��es; Localidade do usu�rio

Tratando a localidade do Usu�rio
--------------------------

A localidade do usu�rio atual � armazenada na sess�o e � acess�vel
via servi�o ``session`:

.. code-block:: php

    $locale= $this->get('session')->getLocale();

    $this->get('session')->setLocale('en_US');

.. index::
   single: Tradu��es; localidade padr�o e alternativo

Localidade padr�o e alternativa
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se a localidade n�o foi fixada explicitamente na sess�o , o par�metro de 
configura��o ``fallback_locale`` ser� usada pelo ``Translator``. O par�metro
� padronizado para ``en`` (veja `Configura��o`_).

Alternativamente, voc� pode garantir que uma localidade � fixada na sess�o do usu�rio
definindo um ``default_locale`` para o servi�o de sess�o:

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

Como a localidade do usu�rio � armazenada na sess�o, pode ser tentador
usar a mesma URL para mostrar o recurso em muitos idiomas diferentes baseados
na localidade do usu�rio.Por exemplo, ``http://www.example.com/contact`` poderia
mostrar conte�do em Ingl�s para um usu�rio e Franc�s para outro usu�rio. Infelizmente,
isso viola uma regra fundamental da Web: que um URL particular retorne 
o mesmo recurso independente do usu�rio. Para complicar ainda o problema, qual 
vers�o do conte�do deveria ser indexado pelas ferramentas de pesquisa ?

Uma melhor pol�tica � incluir a localidade na URL. Isso � totalmente suportado
pelo sistema de roteamnto usando o par�metro especial ``_locale``:

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

Quando usar o par�metro especial `_locale` numa rota, a localidade encontrada
ser� *automaticamente estabelecida na sess�o do usu�rio*. Em outras palavras, se um usu�rio
visita a URI ``/fr/contact``, a localidade``fr`` ser� automaticamente estabelecida
como a localidade para a sess�o do usu�rio.

Voc� pode agora usar a localidade do usu�rio para criar rotas para outras p�ginas traduzidas
na sua aplica��o.

.. index::
   single: Tradu��es; Pluraliza��o

Pluraliza��o
-------------

Pluraliza��o de mensagem � um t�pico dif�cil j� que as regras podem ser bem complexas. Por
conven��o, aqui est� a representa��o matem�tica das regrad de pluraliza��o Russa::

    (($number % 10 == 1) && ($number % 100 != 11)) ? 0 : ((($number % 10 >= 2) && ($number % 10 <= 4) && (($number % 100 < 10) || ($number % 100 >= 20))) ? 1 : 2);

Como voc� viu, em Russo, voc� pode ter tr�s formas diferentes de plural, cada uma
com index de 0, 1 ou 2. Para cada forma, o plural � diferente, e ent�o
a tradu��o tamb�m � diferente.

Quando uma tradu��o tem formas diferentes devido � pluraliza��o, voc� pode providenciar
todas as formas como string separadas por barra vertical (``|``)::

    'There is one apple|There are %count% apples'

Para traduzir mensagens pluralizadas, use o m�todo:
:method:`Symfony\\Component\\Translation\\Translator::transChoice`

.. code-block:: php

    $t = $this->get('translator')->transChoice(
        'There is one apple|There are %count% apples',
        10,
        array('%count%' => 10)
    );

O segundo argumento (``10`` neste exemplo), � o *n�mero* de objetos sendo
descritos e � usado para determinar qual tradu��o usar e tamb�m para preencher
o espa�o reservado ``%count%``.

Baseado em certo n�mero, o tradutor escolhe a forma correta do plural.
Em Ingl�s, muitas palavras tem uma forma singular quando existe exatamente um objeto
e uma forma no plural para todos os outros n�meros (0, 2, 3...). Ent�o, se ``count`` �
``1``, o tradutor usar� a primeira string (``There is one apple``)
como tradu��o. Sen�o ir� usar ``There are %count% apples``.

Aqui est� a tradu��o Francesa::

    'Il y a %count% pomme|Il y a %count% pommes'

Mesmo se a string parecer similar (� feita de duas substrings separadas por
barra vertical), as regras Francesas s�o diferentes: a primeira forma (sem plural) � usada quando
``count`` is ``0`` or ``1``. Ent�o, o tradutor ir� automaticamente usar a
primeira string (``Il y a %count% pomme``) quando ``count`` � ``0`` ou ``1``.

Cada localidade tem sua pr�pria lista de regras, com algumas tendo tanto quanto seis formas
diferentes de plural com regras complexas por tr�s de quais n�meros de mapas de quais formas no plural.
As regras s�o bem simples para Ingl�s e Franc�s, mas para Russo, voc�
poderia querer um palpite para conhecer quais regras combinam com qual string. Para ajudar tradutores,
voc� pode opcionalmente "atribuir uma tag" a cada string::

    'one: There is one apple|some: There are %count% apples'

    'none_or_one: Il y a %count% pomme|some: Il y a %count% pommes'

As tags s�o realmente as �nicas pistas para tradutores e n�o afetam a l�gica
usada para determinar qual forma plural usar. As tags podem ser qualquer string
descritiva que termine com dois pontos (``:``). As tags traduzidas tamb�m n�o s�o necessariamente a mesma
que as da mensagem original.

.. tip:

    Como tags s�o opcionais, o tradutor n�o usa elas (o tradutor ir� 
    somente obter uma string baseada na posi��o do string).

Pluraliza��o de Intervalo Expl�cito
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A maneira mais f�cil de pluralizar uma mensagem � deixar o Symfony2 usar l�gica interna
para escolher qual string usar, baseando em um n�mero fornecido. �s vezes, voc� ir� 
precisar de mais controle ou querer uma tradu��o diferente para casos espec�ficos (para
``0``, ou quando o contador � negativo, por exemplo). Para certos casos, voc� pode
usar intervalos matem�ticos expl�citos::

    '{0} There are no apples|{1} There is one apple|]1,19] There are %count% apples|[20,Inf] There are many apples'

Os intervalos seguem a nota��o `ISO 31-11`_. A string acima especifica
quatro intervalos diferentes: exatamente ``0``, exatamente ``1``, ``2-19``, e ``20``
e mais altos.

Voc� tamb�m pode misturar regras matem�ticas expl�citas e regras padr�o. Nesse caso, se
o contador n�o combinar com um intervalo espec�fico, as regras padr�o, ter�o
efeito ap�s remover as regras expl�citas::

    '{0} There are no apples|[20,Inf] There are many apples|There is one apple|a_few: There are %count% apples'

Por exemplo, para ``1`` ma��, a regra padr�o ``There is one apple`` ser�
usada. Para ``2-19`` apples, a segunda regra padr�o ``There are %count%
apples`` ser� selecionada.

Uma classe :class:`Symfony\\Component\\Translation\\Interval` pode representar um conjunto finito
de n�meros::

    {1,2,3,4}

Ou n�meros entre dois outros n�meros::

    [1, +Inf[
    ]-1,2[

O delimitador esquerdo pode ser ``[`` (inclusivo) or ``]`` (exclusivo). O delimitador
direito pode ser ``[`` (exclusivo) or ``]`` (inclusivo). Al�m de n�meros, voc�
pode usar ``-Inf`` e ``+Inf`` para infinito.

.. index::
   single: Trandu��es; Em templates

Tradu��es em Templates
-------------------------

A maior parte do tempo, tradu��es ocorrem em templates. Symfony2 providencia
suporte nativo para ambos os templates PHP e Twig.

Templates Twig
~~~~~~~~~~~~~~

Symfony2 providencia tags Twig especializadas (``trans`` e ``transchoice``) para
ajudar com tradu��o de mensagem de *blocos est�ticos de texto*:

.. code-block:: jinja

    {% trans %}Hello %name%{% endtrans %}

    {% transchoice count %}
        {0} There are no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

A tag ``transchoice`` automaticamente obt�m a vari�vel ``%count%`` do
contexto atual e a passa para o tradutor. Esse mecanismo s� funciona
quando voc� usa um espa�o reservado seguindo o padr�o ``%var%``.

.. tip::
    
    Se voc� precisa usar o caractere de percentual (``%``) em uma string, escape dela ao
    dobr�-la: ``{% trans %}Percent: %percent%%%{% endtrans %}``

Voc� tamb�m pode especificar o dom�nio da mensagem e passar algumas vari�veis adicionais:

.. code-block:: jinja

    {% trans with {'%name%': 'Fabien'} from "app" %}Hello %name%{% endtrans %}

    {% trans with {'%name%': 'Fabien'} from "app" into "fr" %}Hello %name%{% endtrans %}

    {% transchoice count with {'%name%': 'Fabien'} from "app" %}
        {0} There is no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

Os filtros ``trans`` e ``transchoice`` podem ser usados para traduzir *textos de 
vari�veis* e express�es complexas:

.. code-block:: jinja

    {{ message | trans }}

    {{ message | transchoice(5) }}

    {{ message | trans({'%name%': 'Fabien'}, "app") }}

    {{ message | transchoice(5, {'%name%': 'Fabien'}, 'app') }}

.. tip::
    
    Usando as tags de tradu��o ou filtros que tenham o mesmo efeito, mas com
    uma diferen�a sutil: sa�da para escape autom�tico s� � aplicada para
    vari�veis traduzidas por utiliza��o de filtro. Em outras palavras, se voc� precisar
    estar certo que sua vari�vel traduzida *n�o* � uma sa�da para escape, voc� precisa
    aplicar o filtro bruto ap�s o filtro de tradu��o.
    
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

O servi�o tradutor � acess�vel em templates PHP atrav�s do 
helper ``translator``:

.. code-block:: html+php

    <?php echo $view['translator']->trans('Symfony2 is great') ?>

    <?php echo $view['translator']->transChoice(
        '{0} There is no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        array('%count%' => 10)
    ) ?>

For�ando a Localidade Tradutora
-----------------------------

Quando traduzir a mensagem, Symfony2 usa a localidade da sess�o do usu�rio
ou a localidade ``alternativa`` se necess�ria. Voc� tamb�m pode especificar manualmente a
localidade a usar para a tradu��o:

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

Traduzindo Conte�do de Banco de Dados
----------------------------

Quando a tradu��o do conte�do do banco de dados deveria ser manipulada pelo Doctrine
atrav�s do `Translatable Extension`_. Para mais informa��es, veja a documenta��o
para aquela biblioteca.

Sum�rio
-------

Com o componente Translation do Symfony2, criar uma aplica��o internacionalizada
n�o precisa mais ser um processo dolorido e desgastante para somente uns passos
b�sicos:

* Mensagens abstratas na sua aplica��o ao englobar cada uma ou com m�todos
  :method:`Symfony\\Component\\Translation\\Translator::trans` ou
  :method:`Symfony\\Component\\Translation\\Translator::transChoice`;

* Traduza cada mensagem em localidades m�ltiplas ao criar arquivos de tradu��o
  de mensagem. Symfony2 descobre e processa cada arquivo porque o nome dele segue
  uma conven��o espec�fica;

* Gerenciar a localidade do usu�rio, que � armazenada na sess�o.

.. _`strtr function`: http://www.php.net/manual/en/function.strtr.php
.. _`ISO 31-11`: http://en.wikipedia.org/wiki/Interval_%28mathematics%29#The_ISO_notation
.. _`Translatable Extension`: https://github.com/l3pp4rd/DoctrineExtensions
