.. index::
   single: Debug
   single: Error
   single: Exception
   single: Components; ErrorHandler

O Componente ErrorHandler
=========================

    O componente ErrorHandler disponibiliza ferramentas para gerenciar erros e facilitar
    a depuração de código PHP.

Instalação
----------

.. code-block:: terminal

    $ composer require symfony/error-handler

.. include:: /components/require_autoload.rst.inc

Uso
---

O componente ErrorHandler fornece várias ferramentas para ajudá-lo a depurar código PHP:

* Um **manipulador de erros** que transforma erros PHP em exceções;
* Um **manipulador de exceções** que transforma exceções não capturadas do PHP em respostas PHP melhoradas;
* Um **carregador de classes de depuração** que fornece erros melhores quando uma classe não é encontrada.

Chame o método abaixo (por exemplo, no seu :ref:`front controller <architecture-front-controller>`)
para habilitar todos esses recursos em sua aplicação::

    // public/index.php
    use Symfony\Component\ErrorHandler\Debug;

    if ($_SERVER['APP_DEBUG']) {
        Debug::enable();
    }

    // ...

Continue lendo este artigo para saber mais sobre esses recursos, incluindo como
habilitar cada um deles separadamente.

.. caution::

    Você nunca deve habilitar as ferramentas de depuração, exceto o manipulador de erros, em um
    ambiente de produção, pois eles podem exibir informações confidenciais ao usuário.

Transformando Erros PHP em Exceções
-----------------------------------

A classe :class:`Symfony\\Component\\ErrorHandler\\ErrorHandler` captura os erros PHP
e os transforma em objetos :phpclass:`ErrorException` do PHP, com exceção dos
erros fatais do PHP, que são transformados em objetos
:class:`Symfony\\Component\\ErrorHandler\\Exception\\FatalErrorException` do Symfony.

Se a aplicação usar o FrameworkBundle, esse manipulador de erros será ativado por
padrão no :ref:`ambiente de produção <configuration-environments>`
pois gera melhores logs de erro.

Use o seguinte código (por exemplo, no seu :ref:`front controller <architecture-front-controller>`)
para habilitar esse manipulador de erros::

    use Symfony\Component\ErrorHandler\ErrorHandler;

    ErrorHandler::register();

.. tip::

    Se você deseja obter páginas de exceção ainda melhores, instale também o
    :doc:`componente ErrorRenderer </components/error_renderer>`.

Capturando Erros de Função PHP e Transformando-os em Exceções
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Considere o seguinte exemplo::

    $data = json_decode(file_get_contents($filename), true);
    $data['read_at'] = date($datetimeFormat);
    file_put_contents($filename, json_encode($data));

A maioria das principais funções do PHP foram escritas antes da introdução do tratamento de exceções,
então, elas retornam ``false`` ou ``null`` em caso de erro, em vez de lançar uma
exceção. É por isso que você precisa adicionar algo assim para verificar se há erros::

    $content = @file_get_contents($filename);
    if (false === $content) {
        throw new \RuntimeException('Could not load file.');
    }

    // since PHP 7.3 json_decode() defines an option to throw JSON_THROW_ON_ERROR
    // but you need to enable that option explicitly
    $data = @json_decode($content, true);
    if (null === $data) {
        throw new \RuntimeException('File does not contain valid JSON.');
    }

    $datetime = @date($datetimeFormat);
    if (false === $datetime) {
        throw new \RuntimeException('Invalid datetime format.');
    }

Para simplificar esse código, a classe :class:`Symfony\\Component\\ErrorHandler\\ErrorHandler`
fornece um método :method:`Symfony\\Component\\ErrorHandler\\ErrorHandler::call`
que lança uma exceção automaticamente quando ocorre um erro PHP::

    $content = ErrorHandler::call('file_get_contents', $filename);

O primeiro argumento de ``call()`` é o nome da função PHP que será executada e
o restante dos argumentos são passados para a função PHP. O resultado da função PHP
é retornado como resultado de ``call()``.

É possível passar qualquer callable PHP como primeiro argumento de ``call()``, para que você possa
agrupar várias chamadas de função dentro de uma função anônima::

    $data = ErrorHandler::call(static function () use ($filename, $datetimeFormat) {
        // if any code executed inside this anonymous function fails, a PHP exception
        // will be thrown, even if the code uses the '@' PHP silence operator
        $data = json_decode(file_get_contents($filename), true);
        $data['read_at'] = date($datetimeFormat);
        file_put_contents($filename, json_encode($data));

        return $data;
    });

Depurando Exceções do PHP Não Capturadas
----------------------------------------

A classe :class:`Symfony\\Component\\ErrorHandler\\ExceptionHandler` captura
exceções não detectadas do PHP e as transforma em uma resposta melhorada, para que você possa
depurará-las. É conveniente no :ref:`modo debug <debug-mode>` para substituir a saída padrão
do PHP/XDebug por algo mais bonito e útil.

Se o :doc:`componente HttpFoundation </components/http_foundation>` estiver
disponível, o manipulador retorna um objeto
:class:`Symfony\\Component\\HttpFoundation\\Response` do Symfony. Caso contrário,
ele retorna uma resposta PHP genérica.

Use o seguinte código (por exemplo, no seu :ref:`front controller <architecture-front-controller>`)
para habilitar esse manipulador de exceção::

    use Symfony\Component\ErrorHandler\ExceptionHandler;

    ExceptionHandler::register();

.. tip::

    Se você deseja obter páginas de exceção ainda melhores, instale o
    :doc:`componente ErrorRenderer </components/error_renderer>` também.

.. _component-debug-class-loader:

Depurador de Carregamento de Classe
-----------------------------------

A classe :class:`Symfony\\Component\\ErrorHandler\\DebugClassLoader` lança
exceções mais úteis quando uma classe não é encontrada pelos carregadores automáticos registrados
(por exemplo, procura erros de digitação nos nomes das classes e sugere o nome correto da classe).

Na prática, esse depurador procura todos os carregadores automáticos registrados que implementam um
método ``findFile()`` e os substitui por seu próprio método para encontrar arquivos de classe.

Use o seguinte código (por exemplo, no seu :ref:`front controller <architecture-front-controller>`)
para habilitar esse depurador de carregamento de classe::

    use Symfony\Component\ErrorHandler\DebugClassLoader;

    DebugClassLoader::enable();
