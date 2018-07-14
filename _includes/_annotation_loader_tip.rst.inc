.. note::

    Para usar o loader de anotações, você deve ter instalado os pacotes
    ``doctrine/annotations`` e ``doctrine/cache`` com o Composer.

.. tip::

    Classes Annotation não são carregadas automaticamente, então você deve carregá-las
    usando um class loader como este::

        use Composer\Autoload\ClassLoader;
        use Doctrine\Common\Annotations\AnnotationRegistry;

        /** @var ClassLoader $loader */
        $loader = require __DIR__.'/../vendor/autoload.php';

        AnnotationRegistry::registerLoader([$loader, 'loadClass']);

        return $loader;
