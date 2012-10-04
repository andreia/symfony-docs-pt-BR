.. index::
   single: Testes

Performance
===========

O Symfony2 é rápido, logo após a sua instalação. Claro, se você realmente precisa de mais velocidade,
há muitas maneiras para tornar o Symfony ainda mais rápido. Neste capítulo, 
você vai explorar muitas das formas mais comuns e poderosas para tornar a sua aplicação 
Symfony ainda mais rápida.

.. index::
   single: Performance; Cache de Código Byte

Use um Cache de Código Byte (ex.: APC)
--------------------------------------

Uma das melhores (e mais fáceis) coisas que você deve fazer para melhorar a sua performance
é usar um "cache de código byte". A idéia de um cache de código byte é remover
a necessidade de constantemente recompilar o código fonte PHP. Há uma série de
`Caches de código byte` _ disponíveis, alguns dos quais são de código aberto. O cache de código byte 
mais amplamente usado é, provavelmente, o `APC`_

Usar um cache de código byte não tem um lado negativo, e o Symfony2 foi arquitetado
para executar realmente bem neste tipo de ambiente.

Mais otimizações 
~~~~~~~~~~~~~~~~

Caches de código byte normalmente monitoram as alterações nos arquivos fonte. Isso garante
que, se o fonte de um arquivo for alterado, o código byte é recompilado automaticamente.
Isto é realmente conveniente, mas, obviamente, adiciona uma sobrecarga.

Por este motivo, alguns caches de código byte oferecem uma opção para desabilitar estas verificações.
Obviamente, quando desabilitar estas verificações, caberá ao administrador do servidor
garantir que o cache seja limpo sempre que houver qualquer alteração nos arquivos fonte. Caso contrário,
as atualizações que você fizer nunca serão vistas.

Por exemplo, para desativar estas verificações no APC, basta adicionar ``apc.stat=0`` ao seu
arquivo de configuração php.ini.

.. index::
   single: Performance; Autoloader

Utilize um Autoloader com cache (ex.: ``ApcUniversalClassLoader``)
------------------------------------------------------------------

Por padrão, a edição standard do Symfony2 usa o ``UniversalClassLoader``
no arquivo `autoloader.php`_ . Este autoloader é fácil de usar, pois ele 
encontra automaticamente as novas classes que você colocou nos diretórios
registrados.

Infelizmente, isso tem um custo, devido ao carregador iterar sobre todos os namespaces 
configurados para encontrar um determinado arquivo, fazendo chamadas ``file_exists`` até que,
finalmente, encontre o arquivo que estiver procurando.

A solução mais simples é armazenar em cache a localização de cada classe após ter sido localizada
pela primeira vez. O Symfony vem com um carregador de classe - :class:`Symfony\\Component\\ClassLoader\\ApcClassLoader` -
que faz exatamente isso. Para usá-lo, basta adaptar seu arquivo front controller.
Se você estiver usando a Distribuição Standard, este código já deve estar disponível com 
comentários neste arquivo::

    // app.php
    // ...

    $loader = require_once __DIR__.'/../app/bootstrap.php.cache';

    // Use APC for autoloading to improve performance
    // Change 'sf2' by the prefix you want in order to prevent key conflict with another application
    /*
    $loader = new ApcClassLoader('sf2', $loader);
    $loader->register(true);
    */

    // ...

.. note::

    Ao usar o autoloader APC, se você adicionar novas classes, elas serão encontradas
    automaticamente e tudo vai funcionar da mesma forma como antes (ou seja, sem
    razão para "limpar" o cache). No entanto, se você alterar a localização de um
    namespace ou prefixo em particular, você vai precisar limpar o cache do APC. Caso contrário,
    o autoloader ainda estará procurando pelo local antigo para todas as classes
    dentro desse namespace.

.. index::
   single: Performance; Arquivos de inicialização

Utilize arquivos de inicialização (bootstrap)
---------------------------------------------

Para garantir a máxima flexibilidade e reutilização de código, as aplicações do Symfony2 aproveitam 
uma variedade de classes e componentes de terceiros. Mas, carregar todas essas classes
de arquivos separados em cada requisição pode resultar em alguma sobrecarga. Para reduzir
essa sobrecarga, a Edição Standard do Symfony2 fornece um script para gerar
o chamado `arquivo de inicialização`_, que consiste em múltiplas definições de classes
em um único arquivo. Ao incluir este arquivo (que contém uma cópia de muitas das
classes core), o Symfony não precisa incluir nenhum dos arquivos fonte 
contendo essas classes. Isto reduzirá bastante a E/S no disco.

Se você estiver usando a Edição Standard do Symfony2, então, você provavelmente já
está usando o arquivo de inicialização. Para ter certeza, abra o seu ``front controller`` (geralmente
``app.php``) e, certifique-se que existe a seguinte linha::

    require_once __DIR__.'/../app/bootstrap.php.cache';

Note que existem duas desvantagens ao usar um arquivo de inicialização:

* O arquivo precisa ser regerado, sempre que houver qualquer mudança nos fontes originais 
  (ex.: quando você atualizar o fonte do Symfony2 ou as bibliotecas vendor);

* Quando estiver debugando, é necessário colocar ``break points`` dentro do arquivo de inicialização.

Se você estiver usando a Edição Standard do Symfony2, o arquivo de inicialização é automaticamente
reconstruído após a atualização das bibliotecas vendor através do comando ``php composer.phar install``
.

Arquivos de inicialização e caches de código byte
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mesmo quando se utiliza um cache de código byte, o desempenho irá melhorar quando se utiliza um 
arquivo de inicialização, pois, haverá menos arquivos para monitorar as mudanças. Claro, se este
recurso está desativado no cache de código byte (ex.: ``apc.stat=0`` no APC), não há
mais motivo para usar um arquivo de inicialização.

.. _`Caches de código byte`: http://en.wikipedia.org/wiki/List_of_PHP_accelerators
.. _`APC`: http://php.net/manual/en/book.apc.php
.. _`autoloader.php`: https://github.com/symfony/symfony-standard/blob/master/app/autoload.php
.. _`arquivo de inicialização`: https://github.com/sensio/SensioDistributionBundle/blob/master/Composer/ScriptHandler.php
