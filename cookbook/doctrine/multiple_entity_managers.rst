.. index::
   single: Doctrine; Múltiplos Gerenciadores de Entidade

Como trabalhar com Múltiplos Gerenciadores de Entidade
======================================================

Você pode usar múltiplos gerenciadores de entidades em uma aplicação Symfony2. Isto é
necessário se você está usando bancos de dados diferentes ou algum vendor com conjuntos de
entidades completamente diferentes. Em outras palavras, um gerenciador de entidades que
conecta em um banco de dados manipulará algumas entidades enquanto um outro gerenciador de
entidades que conecta a um outro banco de dados irá manupular as entidades restantes.

.. note::

    Usar múltiplos gerenciadores de entidade é muito fácil, mas mais avançado e
    geralmente não necessário. Certifique se você realmente precisa de múltiplos
    gerenciadores de entidades antes de adicionar esta camada de complexibilidade.

O código de configuração seguinte mostra como configurar dois gerenciadores de entidade:

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            orm:
                default_entity_manager:   default
                entity_managers:
                    default:
                        connection:       default
                        mappings:
                            AcmeDemoBundle: ~
                            AcmeStoreBundle: ~
                    customer:
                        connection:       customer
                        mappings:
                            AcmeCustomerBundle: ~

Neste caso, você deve definir dois gerenciadores de entidade e chamá-los de
``default`` e ``customer``. O gerenciador de entidade ``default`` manipula as
entidades em ``AcmeDemoBundle`` e ``AcmeStoreBundle``, enquanto o gerenciador
de entidades ``customer`` manipula as entidades ``AcmeCustomerBundle``.

Quando estiver trabalhando com múltiplos gerenciadores de entidade, você deve ser explícito
sobre qual gerenciador de entidade você quer. Se você *omitir* o nome do gerenciador de 
entidade quando você atualizar o seu schema, será usado o padrão (ou seja, ``default``)::

    # Play only with "default" mappings
    php app/console doctrine:schema:update --force

    # Play only with "customer" mappings
    php app/console doctrine:schema:update --force --em=customer

Se você *omitir* o nome do gerenciador de entidade ao solicitar ele,
o gerenciador de entidade padrão (ou seja, ``default``) é retornado::

    class UserController extends Controller
    {
        public function indexAction()
        {
            // both return the "default" em
            $em = $this->get('doctrine')->getEntityManager();
            $em = $this->get('doctrine')->getEntityManager('default');

            $customerEm =  $this->get('doctrine')->getEntityManager('customer');
        }
    }

Agora você pode usar Doctrine exatamente da mesma forma que você fez antes -
usando o gerenciador de entidade ``default`` para persistir e buscar as
entidades que ele gerencia, e o gerenciador de entidade ``customer`` para
persistir e buscar suas entidades.

O mesmo se aplica para chamadas de repositório::

    class UserController extends Controller
    {
        public function indexAction()
        {
            // Retrieves a repository managed by the "default" em
            $products = $this->get('doctrine')
                             ->getRepository('AcmeStoreBundle:Product')
                             ->findAll();

            // Explicit way to deal with the "default" em
            $products = $this->get('doctrine')
                             ->getRepository('AcmeStoreBundle:Product', 'default')
                             ->findAll();

            // Retrieves a repository managed by the "customer" em
            $customers = $this->get('doctrine')
                              ->getRepository('AcmeCustomerBundle:Customer', 'customer')
                              ->findAll();
        }
    }
