.. index::
   pair: Autoloader; Configuração
   single: Componentes; ClassLoader

O Componente ClassLoader
========================

    O Componente ClassLoader carrega as classes do seu projeto automaticamente se
    elas seguirem algumas convenções PHP padrão.

Sempre que você usar uma classe indefinida, o PHP utiliza o mecanismo de autoload automático
para delegar o carregamento de um arquivo definindo a classe. O Symfony2 fornece um
autoloader "universal", que é capaz de carregar classes de arquivos que
implementam uma das seguintes convenções:

* Os `padrões`_ técnicos de interoperabilidade para namespaces do PHP 5.3 e para nomes de 
  classes;

* A convenção de nomenclatura de classes do `PEAR`_.

Se as suas classes e as bibliotecas de terceiros que você usa no seu projeto seguem
estes padrões, o autoloader do Symfony2 é o único autoloader que
você vai precisar.

Instalação
----------

Você pode instalar o componente de várias formas diferentes:

* Usando o repositório Git oficial (https://github.com/symfony/ClassLoader);
* Instalando via PEAR ( `pear.symfony.com/ClassLoader`);
* Instalando via Composer (`symfony/class-loader` no Packagist).

Uso
---

.. versionadded:: 2.1
   O método ``useIncludePath`` foi adicionado no Symfony 2.1.

Registrar o autoloader :class:`Symfony\\Component\\ClassLoader\\UniversalClassLoader`
é simples::

    require_once '/path/to/src/Symfony/Component/ClassLoader/UniversalClassLoader.php';

    use Symfony\Component\ClassLoader\UniversalClassLoader;

    $loader = new UniversalClassLoader();

    // You can search the include_path as a last resort.
    $loader->useIncludePath(true);

    // ... register namespaces and prefixes here - see below

    $loader->register();

Para ganhos de desempenho menores, os caminhos das classes podem ser armazenados em memória usando com 
o APC registrando o :class:`Symfony\\Component\\ClassLoader\\ApcUniversalClassLoader`::

    require_once '/path/to/src/Symfony/Component/ClassLoader/UniversalClassLoader.php';
    require_once '/path/to/src/Symfony/Component/ClassLoader/ApcUniversalClassLoader.php';

    use Symfony\Component\ClassLoader\ApcUniversalClassLoader;

    $loader = new ApcUniversalClassLoader('apc.prefix.');
    $loader->register();

O autoloader somente é útil se você adicionar algumas bibliotecas para autoload.

.. note::

    O autoloader é automaticamente registrado em uma aplicação Symfony2 (veja
    ``app/autoload.php``).

Se as classes para o autoload usam namespaces, utilize os métodos
:method:`Symfony\\Component\\ClassLoader\\UniversalClassLoader::registerNamespace`
ou
:method:`Symfony\\Component\\ClassLoader\\UniversalClassLoader::registerNamespaces`
::

    $loader->registerNamespace('Symfony', __DIR__.'/vendor/symfony/symfony/src');

    $loader->registerNamespaces(array(
        'Symfony' => __DIR__.'/../vendor/symfony/symfony/src',
        'Monolog' => __DIR__.'/../vendor/monolog/monolog/src',
    ));

    $loader->register();

Para as classes que seguem a convenção de nomenclatura do PEAR, utilize os métodos
:method:`Symfony\\Component\\ClassLoader\\UniversalClassLoader::registerPrefix`
ou
:method:`Symfony\\Component\\ClassLoader\\UniversalClassLoader::registerPrefixes`
::

    $loader->registerPrefix('Twig_', __DIR__.'/vendor/twig/twig/lib');

    $loader->registerPrefixes(array(
        'Swift_' => __DIR__.'/vendor/swiftmailer/swiftmailer/lib/classes',
        'Twig_'  => __DIR__.'/vendor/twig/twig/lib',
    ));

    $loader->register();

.. note::

    Algumas bibliotecas também exigem que seu caminho raiz seja registrado no include path do PHP
    (``set_include_path()``).

Classes de um sub-namespace ou uma sub-hierarquia das classes PEAR podem ser buscadas
em uma lista de localização para facilitar o vendoring de um sub-conjunto de classes 
para projetos grandes::

    $loader->registerNamespaces(array(
        'Doctrine\\Common'           => __DIR__.'/vendor/doctrine/common/lib',
        'Doctrine\\DBAL\\Migrations' => __DIR__.'/vendor/doctrine/migrations/lib',
        'Doctrine\\DBAL'             => __DIR__.'/vendor/doctrine/dbal/lib',
        'Doctrine'                   => __DIR__.'/vendor/doctrine/orm/lib',
    ));

    $loader->register();

Neste exemplo, se você tentar usar uma classe no namespace ``Doctrine\Common``
ou em um de seus filhos, o autoloader vai procurar primeiro pela classe sob o
diretório ``doctrine-common``, e vai, em seguida, retornar para o diretório padrão
``Doctrine`` (o último configurado) se não for encontrada, antes de desistir.
A ordem das inscrições é significativa neste caso.

.. _padrões: http://symfony.com/PSR0
.. _PEAR:    http://pear.php.net/manual/en/standards.php
