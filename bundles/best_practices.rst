.. index::
   single: Bundle; Melhores práticas

Como usar Melhores Práticas para a Estruturação dos Bundles
===========================================================

Um Bundle é um diretório que tem uma estrutura bem definida e pode hospedar qualquer coisa
desde classes até controladores e recursos web. Mesmo os bundles sendo muito flexíveis, 
você deve seguir algumas das melhores práticas se deseja distribuí-los.

.. index::
   pair: Bundle; Convenções de Nomenclatura

.. _bundles-naming-conventions:

Nome do Bundle
--------------

Um bundle é também um namespace PHP. O namespace deve seguir os `padrões`_ técnicos 
de interoperabilidade para namespaces do PHP 5.3 e nomes de classes:
ele inicia com o segmento do vendor, seguido por zero ou mais segmentos de categoria, e
termina com o nome curto do namespace, que deve terminar com o sufixo
``Bundle``.

Um namespace torna-se um bundle assim que você adiciona uma classe bundle à ele. O
nome da classe do bundle deve seguir estas regras simples:

* Usar apenas caracteres alfanuméricos e sublinhados;
* Usar o nome em CamelCase;
* Usar um nome descritivo e curto (não mais que 2 palavras);
* Prefixar o nome com a concatenação do vendor (e, opcionalmente, os
  namespaces da categoria);
* Adicionar o sufixo ``Bundle`` ao nome.

Aqui estão alguns namespaces de bundle e nomes de classes válidos:

+-----------------------------------+--------------------------+
| Namespace                         | Nome da Classe do Bundle |
+===================================+==========================+
| ``Acme\Bundle\BlogBundle``        | ``AcmeBlogBundle``       |
+-----------------------------------+--------------------------+
| ``Acme\Bundle\Social\BlogBundle`` | ``AcmeSocialBlogBundle`` |
+-----------------------------------+--------------------------+
| ``Acme\BlogBundle``               | ``AcmeBlogBundle``       |
+-----------------------------------+--------------------------+

Por convenção, o método ``getName()`` da classe bundle deve retornar o
nome da classe.

.. note::

    Se você compartilhar publicamente seu bundle, você deve usar o nome da classe do 
    bundle como o nome do repositório (``AcmeBlogBundle`` e não ``BlogBundle``
    por exemplo).

.. note::

    Os Bundles do núcleo do Symfony2 não prefixam a classe Bundle com ``Symfony``
    e sempre adicionam um subnamespace ``Bundle``, por exemplo:
    :class:`Symfony\\Bundle\\FrameworkBundle\\FrameworkBundle`.

Cada bundle tem um alias, que é a versão curta e em letras minúsculas do nome do bundle
usando sublinhados (por exemplo, ``acme_hello`` para ``AcmeHelloBundle``, ou 
``acme_social_blog`` para ``Acme\Social\BlogBundle``). Estes alias
são usados para definir exclusividade dentro de um bundle (veja abaixo alguns 
exemplos de uso).

Estrutura de Diretórios
------------------------

A estrutura básica de diretório de um bundle ``HelloBundle`` deve parecer com 
a seguinte:

.. code-block:: text

    XXX/...
        HelloBundle/
            HelloBundle.php
            Controller/
            Resources/
                meta/
                    LICENSE
                config/
                doc/
                    index.rst
                translations/
                views/
                public/
            Tests/

O(s) diretório(s) ``XXX`` refletem a estrutura do namespace do bundle.

Os seguintes arquivos são obrigatórios:

* ``HelloBundle.php``;
* ``Resources/meta/LICENSE``: A licença completa para o código;
* ``Resources/doc/index.rst``: O arquivo raiz para a documentação do Bundle.

.. note::

    Estas convenções garantem que ferramentas automatizadas possam contar com esta estrutura
    padrão para trabalhar.

A profundidade dos sub-diretórios deve ser mantida ao mínimo para as classes e arquivos
mais utilizados (2 níveis, no máximo). Mais níveis podem ser definidos para arquivos
não-estratégicos ou menos utilizados.

O diretório bundle é somente leitura. Se você precisa gravar arquivos temporários, 
armazene-os sob o diretório ``cache/`` ou ``log/`` da aplicação host. Ferramentas
podem gerar arquivos na estrutura de diretório do bundle, mas apenas se os arquivos
gerados serão parte do repositório.

As classes e arquivos seguintes possuem um local específico:

+--------------------------------+-----------------------------+
| Tipo                           | Diretório                   |
+================================+=============================+
| Comandos                       | ``Command/``                |
+--------------------------------+-----------------------------+
| Controladores                  | ``Controller/``             |
+--------------------------------+-----------------------------+
| Extensões do Service Container | ``DependencyInjection/``    |
+--------------------------------+-----------------------------+
| Listeners de Evento            | ``EventListener/``          |
+--------------------------------+-----------------------------+
| Configuração                   | ``Resources/config/``       |
+--------------------------------+-----------------------------+
| Recursos Web                   | ``Resources/public/``       |
+--------------------------------+-----------------------------+
| Arquivos de Tradução           | ``Resources/translations/`` |
+--------------------------------+-----------------------------+
| Templates                      | ``Resources/views/``        |
+--------------------------------+-----------------------------+
| Testes Unitários e Funcionais  | ``Tests/``                  |
+--------------------------------+-----------------------------+

Classes
-------

A estrutura de diretórios do bundle é usada como hierarquia de namespace. Por
exemplo, um controlador ``HelloController`` é armazenado em
``Bundle/HelloBundle/Controller/HelloController.php`` e o nome totalmente qualificado 
da classe é ``Bundle\HelloBundle\Controller\HelloController``.

Todas as classes e arquivos devem seguir os :doc:`padrões </contributing/code/standards>` 
de codificação do Symfony2.

Algumas classes deve ser vistas como facades e devem ser o mais curtas possível, como
Commands, Helpers, Listeners e Controllers.

As classes que se conectam ao Dispatcher de Eventos devem ter o sufixo
``Listener``.

Classes de exceções (Exceptions) devem ser armazenadas em um sub-namespace ``Exception``.

Vendors
-------

Um bundle não deve incorporar bibliotecas PHP de terceiros. Em vez disso, ele deve contar com o
autoloading padrão do Symfony2.

Um bundle não deve incorporar bibliotecas de terceiros escritas em JavaScript, CSS ou
qualquer outra linguagem.

Testes
------

Um bundle deve vir com um conjunto de testes escritos com o PHPUnit e armazenados sob
o diretório ``Tests/``. Os testes devem seguir os seguintes princípios:

* O conjunto de testes deve ser executável com um simples comando ``phpunit``, executado a 
  partir de uma aplicação de exemplo;
* Os testes funcionais só devem ser usados para testar a resposta de saída e
  algumas informações de perfis, caso você tiver;
* A cobertura de código deve cobrir pelo menos 95% da base de código.

.. note::
   Um conjunto de testes não deve conter scripts ``AllTests.php``, mas deve contar com a
   existência de um arquivo ``phpunit.xml.dist``.

Documentação
------------

Todas as classes e funções devem vir com PHPDoc completo.

Documentação extensa também deve ser fornecida no formato
:doc:`reStructuredText </contributing/documentation/format>`, sob o diretório 
``Resources/doc/``; o arquivo ``Resources/doc/index.rst`` é o único arquivo 
obrigatório e deve ser o ponto de entrada para a documentação.

Controladores
-------------

Como melhor prática, os controladores em um bundle que se destina a ser 
distribuído não deve estender a classe base 
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`.
Ao invés disso, eles podem implementar
:class:`Symfony\\Component\\DependencyInjection\\ContainerAwareInterface` ou 
estender :class:`Symfony\\Component\\DependencyInjection\\ContainerAware`
.

.. note::

    Se você verificar os métodos do 
    :class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`,
    vai ver que eles são apenas atalhos para facilitar a curva de aprendizado.

Roteamento
----------

Se o bundle oferece rotas, elas devem ser prefixadas com o alias do bundle.
Por exemplo, para o AcmeBlogBundle, todas as rotas devem ser prefixadas com
``acme_blog_``.

Templates
---------

Se um bundle fornece templates, eles devem usar o Twig. Um bundle não deve fornecer
um layout principal, exceto se ele fornece uma aplicação completa.

Arquivos de Tradução
--------------------

Se um bundle fornece mensagens de tradução, elas devem ser definidas no formato XLIFF;
o domínio deve ser nomeado após o nome do bundle (``bundle.hello``).

Um bundle não deve sobrescrever as mensagens existentes de outro bundle.

Configuração
------------

Para proporcionar maior flexibilidade, um bundle pode fornecer definições configuráveis ​​
usando os mecanismos embutidos do Symfony2.

Definições de configuração simples contam com a entrada padrão ``parameters`` da
configuração do Symfony2. Os parâmetros do Symfony2 são simples pares chave/valor, um
valor pode ser qualquer valor válido em PHP. Cada nome de parâmetro deve começar com o
alias do bundle, embora esta seja apenas uma sugestão de melhor prática. O resto do
nome do parâmetro usará um ponto (``.``) para separar partes diferentes (por exemplo,
``acme_hello.email.from``).

O usuário final pode fornecer valores em qualquer arquivo de configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            acme_hello.email.from: fabien@example.com

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="acme_hello.email.from">fabien@example.com</parameter>
        </parameters>

    .. code-block:: php

        // app/config/config.php
        $container->setParameter('acme_hello.email.from', 'fabien@example.com');

    .. code-block:: ini

        ; app/config/config.ini
        [parameters]
        acme_hello.email.from = fabien@example.com

Recupere os parâmetros de configuração no seu código a partir do container::

    $container->getParameter('acme_hello.email.from');

Mesmo esse mecanismo sendo bastante simples, você é altamente encorajado a usar a
configuração semântica descrita no cookbook.

.. note::

    Se você estiver definindo serviços, eles também devem ser prefixados com o alias 
    do bundle.

Aprenda mais no Cookbook
------------------------

* :doc:`/cookbook/bundles/extension`

.. _padrões: http://symfony.com/PSR0
