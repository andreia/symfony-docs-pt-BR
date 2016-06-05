.. index::
   single: Doctrine; Mapeamento das Classes de Modelo

Como Fornecer Classes de Modelo para várias Implementações Doctrine
===================================================================

Ao construir um bundle que pode ser utilizado não somente com o ORM Doctrine, mas
também o ODM CouchDB, o ODM MongoDB ou o ODM PHPCR, você ainda deve escrever
somente uma classe de modelo. Os bundles do Doctrine fornecem um compiler pass
para registrar os mapeamentos para as suas classes de modelo.

.. note::

    Para bundles não reutilizáveis, a opção mais fácil é colocar suas classes de modelo
    nos locais padrão: ``Entity`` para o ORM Doctrine ou ``Document``
    para os ODMs. Para bundles reutilizáveis, em vez de duplicar as classes de modelo
    apenas para obter o auto-mapping, use o compiler pass.

Em sua classe do bundle, escreva o código seguinte para registrar o compiler pass.
Esse foi escrito para o CmfRoutingBundle, por isso partes dele devem ser
adaptadas para o seu caso::

    use Doctrine\Bundle\DoctrineBundle\DependencyInjection\Compiler\DoctrineOrmMappingsPass;
    use Doctrine\Bundle\MongoDBBundle\DependencyInjection\Compiler\DoctrineMongoDBMappingsPass;
    use Doctrine\Bundle\CouchDBBundle\DependencyInjection\Compiler\DoctrineCouchDBMappingsPass;
    use Doctrine\Bundle\PHPCRBundle\DependencyInjection\Compiler\DoctrinePhpcrMappingsPass;

    class CmfRoutingBundle extends Bundle
    {
        public function build(ContainerBuilder $container)
        {
            parent::build($container);
            // ...

            $modelDir = realpath(__DIR__.'/Resources/config/doctrine/model');
            $mappings = array(
                $modelDir => 'Symfony\Cmf\RoutingBundle\Model',
            );

            $ormCompilerClass = 'Doctrine\Bundle\DoctrineBundle\DependencyInjection\Compiler\DoctrineOrmMappingsPass';
            if (class_exists($ormCompilerClass)) {
                $container->addCompilerPass(
                    DoctrineOrmMappingsPass::createXmlMappingDriver(
                        $mappings,
                        array('cmf_routing.model_manager_name'),
                        'cmf_routing.backend_type_orm',
                        array('CmfRoutingBundle' => 'Symfony\Cmf\RoutingBundle\Model')
                ));
            }

            $mongoCompilerClass = 'Doctrine\Bundle\MongoDBBundle\DependencyInjection\Compiler\DoctrineMongoDBMappingsPass';
            if (class_exists($mongoCompilerClass)) {
                $container->addCompilerPass(
                    DoctrineMongoDBMappingsPass::createXmlMappingDriver(
                        $mappings,
                        array('cmf_routing.model_manager_name'),
                        'cmf_routing.backend_type_mongodb',
                        array('CmfRoutingBundle' => 'Symfony\Cmf\RoutingBundle\Model')
                ));
            }

            $couchCompilerClass = 'Doctrine\Bundle\CouchDBBundle\DependencyInjection\Compiler\DoctrineCouchDBMappingsPass';
            if (class_exists($couchCompilerClass)) {
                $container->addCompilerPass(
                    DoctrineCouchDBMappingsPass::createXmlMappingDriver(
                        $mappings,
                        array('cmf_routing.model_manager_name'),
                        'cmf_routing.backend_type_couchdb',
                        array('CmfRoutingBundle' => 'Symfony\Cmf\RoutingBundle\Model')
                ));
            }

            $phpcrCompilerClass = 'Doctrine\Bundle\PHPCRBundle\DependencyInjection\Compiler\DoctrinePhpcrMappingsPass';
            if (class_exists($phpcrCompilerClass)) {
                $container->addCompilerPass(
                    DoctrinePhpcrMappingsPass::createXmlMappingDriver(
                        $mappings,
                        array('cmf_routing.model_manager_name'),
                        'cmf_routing.backend_type_phpcr',
                        array('CmfRoutingBundle' => 'Symfony\Cmf\RoutingBundle\Model')
                ));
            }
        }
    }

Note a verificação :phpfunction:`class_exists`. Isso é crucial, pois você não quer que seu
bundle tenha uma dependência em todos os bundles do Doctrine, mas sim permitir que o usuário
decida qual usar.

O compiler pass fornece métodos factory para todos os drivers fornecidos pelo Doctrine:
Anotações, XML, YAML, PHP e StaticPHP. Os argumentos são:

* Um mapa/hash do caminho de diretório absoluto para namespace;
* Um array de parâmetros do container que seu bundle utiliza para especificar o nome do
  gerenciador do Doctrine que ele está usando. No exemplo acima, o CmfRoutingBundle
  armazena o nome do gerenciador que está sendo usado sob o parâmetro
  ``cmf_routing.model_manager_name``. O compiler pass irá acrescentar o parâmetro Doctrine que está
  usando para especificar o nome do gerenciador padrão. O primeiro parâmetro encontrado é
  utilizado e os mapeamentos são registrados com aquele gerenciador;
* Um nome opcional de parâmetro do container que será usado pelo compiler
  pass para determinar se esse tipo Doctrine está sendo usado. Isso é relevante se
  o usuário tem mais de um tipo do bundle Doctrine instalado, mas o seu
  bundle só é usado com um tipo do Doctrine;
* Um mapa/hash de alias para namespace. Essa deve ser a mesma convenção usada
  pelo auto-mapping do Doctrine. No exemplo acima, isso permite ao usuário chamar
  ``$om->getRepository('CmfRoutingBundle:Route')``.

.. note::

    O método factory está usando o ``SymfonyFileLocator`` do Doctrine, significando
    que ele só vai ver os arquivos de mapeamento XML e YML se eles não contêm o
    namespace completo como nome do arquivo. Isso ocorre por concepção: o ``SymfonyFileLocator``
    simplifica as coisas, assumindo que os arquivos são apenas a versão "curta"
    da classe como o seu nome de arquivo (por exemplo, ``BlogPost.orm.xml``)

    Se você também precisa mapear uma classe base, você pode registrar um compiler pass
    com o ``DefaultFileLocator`` como abaixo. Este código é retirado do
    ``DoctrineOrmMappingsPass`` e adaptado para usar o ``DefaultFileLocator``
    ao invés do ``SymfonyFileLocator``::

        private function buildMappingCompilerPass()
        {
            $arguments = array(array(realpath(__DIR__ . '/Resources/config/doctrine-base')), '.orm.xml');
            $locator = new Definition('Doctrine\Common\Persistence\Mapping\Driver\DefaultFileLocator', $arguments);
            $driver = new Definition('Doctrine\ORM\Mapping\Driver\XmlDriver', array($locator));

            return new DoctrineOrmMappingsPass(
                $driver,
                array('Full\Namespace'),
                array('your_bundle.manager_name'),
                'your_bundle.orm_enabled'
            );
        }

    Note que você não precisa fornecer um alias de namespace a menos que espera-se que seus usuários
    perguntem ao Doctrine pelas classes base.

    Agora coloque o seu arquivo de mapeamento em ``/Resources/config/doctrine-base`` com o
    nome completo da classe, separado por ``.`` ao invés de ``\``, por exemplo
    ``Other.Namespace.Model.Name.orm.xml``. Você não pode misturar os dois pois
    o ``SymfonyFileLocator`` ficará confuso.

    Ajuste em conformidade com as outras implementações Doctrine.

.. _`CouchDB Mapping Compiler Pass pull request`: https://github.com/doctrine/DoctrineCouchDBBundle/pull/27
