.. index::
   single: Config; Carregando recursos

Carregando recursos
=================

Localizando recursos
------------------

O carregamento da configuração normalmente se inicia com a busca por recursos - 
na maioria dos casos: arquivos. Isso pode ser feito com o :class:`Symfony\\Component\\Config\\FileLocator`::

    use Symfony\Component\Config\FileLocator;

    $configDirectories = array(__DIR__.'/app/config');

    $locator = new FileLocator($configDirectories);
    $yamlUserFiles = $locator->locate('users.yml', null, false);

O locator recebe uma coleção de locais onde ele deve buscar por arquivos.
O primeiro argumento do ``locate()`` é o nome do arquivo para se buscar. O segundo
argumento pode ser o caminho atual, e quando fornecido, o locator buscará nesse
diretório primeiro. O terceiro argumento indica quando ou não o locator deverá 
retornar o primeiro arquivo encontrado, ou um array contendo todas as correspondências.


Loaders de recursos
----------------

Para cada tipo de recurso (Yaml, XML, annotation, etc.) um loader deve ser definido.
Cada loader deve implementar :class:`Symfony\\Component\\Config\\Loader\\LoaderInterface`
ou extender a classe abstrata :class:`Symfony\\Component\\Config\\Loader\\FileLoader`
que permite a importação recursiva de outros recursos::

    use Symfony\Component\Config\Loader\FileLoader;
    use Symfony\Component\Yaml\Yaml;

    class YamlUserLoader extends FileLoader
    {
        public function load($resource, $type = null)
        {
            $configValues = Yaml::parse($resource);

            // ... handle the config values

            // maybe import some other resource:

            // $this->import('extra_users.yml');
        }

        public function supports($resource, $type = null)
        {
            return is_string($resource) && 'yml' === pathinfo(
                $resource,
                PATHINFO_EXTENSION
            );
        }
    }

Encontrando o loader correto
------------------------

O :class:`Symfony\\Component\\Config\\Loader\\LoaderResolver` recebe como
seu primeiro argumento construtor uma coleção de loaders. Quando um
recurso (por exemplo, um arquivo XML) deve ser carregado, ele faz um loop
nessa coleção de loaders e retorna o loader que suporta este tipo
particular de recurso.

O :class:`Symfony\\Component\\Config\\Loader\\DelegatingLoader` faz o uso
do :class:`Symfony\\Component\\Config\\Loader\\LoaderResolver`. Quando ele
ele é solicitado a carregar um recurso, ele delega esta questão ao
:class:`Symfony\\Component\\Config\\Loader\\LoaderResolver`. No caso do
resolver ter encontrado um loader adequado, este loader será questionado
a carregar o recurso::

    use Symfony\Component\Config\Loader\LoaderResolver;
    use Symfony\Component\Config\Loader\DelegatingLoader;

    $loaderResolver = new LoaderResolver(array(new YamlUserLoader($locator)));
    $delegatingLoader = new DelegatingLoader($loaderResolver);

    $delegatingLoader->load(__DIR__.'/users.yml');
    /*
    O YamlUserLoader será utilizado para carregar este recurso,
    já que ele suporta arquivos com a extensão "yml"
    */
