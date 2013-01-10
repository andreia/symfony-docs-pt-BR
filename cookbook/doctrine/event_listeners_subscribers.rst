.. index::
   single: Doctrine; Ouvintes de Eventos e Assinantes

.. _doctrine-event-config:

Como registrar Ouvintes de Eventos e Assinantes
===============================================

O Doctrine contém um valioso sistema de evento que dispara eventos quando quase tudo
acontece dentro do sistema. Para você, isso significa que poderá criar
:doc:`serviços</book/service_container>` arbitrários e dizer ao Doctrine para notificar os
objetos sempre que uma determinada ação (ex. ``prePersist``) acontecer.
Isto pode ser útil, por exemplo, para criar um índice de pesquisa independente
sempre que um objeto em seu banco de dados for salvo.

O Doctrine define dois tipos de objetos que podem ouvir eventos do Doctrine:
ouvintes e assinantes. Ambos são muito semelhantes, mas os ouvintes são um pouco
mais simples. Para saber mais, consulte `O Sistema de Eventos`_ no site do Doctrine.

Configurando o Ouvinte/Assinante
--------------------------------

Para registrar um serviço para agir como um ouvinte ou assinante de evento você só tem
que usar a :ref:`tag<book-service-container-tags>` com o nome apropriado. Dependendo
de seu caso de uso, você pode ligar um ouvinte em cada conexão DBAL e gerenciador de
entidade ORM ou apenas em uma conexão específica DBAL e todos os gerenciadores de
entidades que usam esta conexão.

.. configuration-block::

    .. code-block:: yaml

        doctrine:
            dbal:
                default_connection: default
                connections:
                    default:
                        driver: pdo_sqlite
                        memory: true

        services:
            my.listener:
                class: Acme\SearchBundle\EventListener\SearchIndexer
                tags:
                    - { name: doctrine.event_listener, event: postPersist }
            my.listener2:
                class: Acme\SearchBundle\EventListener\SearchIndexer2
                tags:
                    - { name: doctrine.event_listener, event: postPersist, connection: default }
            my.subscriber:
                class: Acme\SearchBundle\EventListener\SearchIndexerSubscriber
                tags:
                    - { name: doctrine.event_subscriber, connection: default }

    .. code-block:: xml

        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine">

            <doctrine:config>
                <doctrine:dbal default-connection="default">
                    <doctrine:connection driver="pdo_sqlite" memory="true" />
                </doctrine:dbal>
            </doctrine:config>

            <services>
                <service id="my.listener" class="Acme\SearchBundle\EventListener\SearchIndexer">
                    <tag name="doctrine.event_listener" event="postPersist" />
                </service>
                <service id="my.listener2" class="Acme\SearchBundle\EventListener\SearchIndexer2">
                    <tag name="doctrine.event_listener" event="postPersist" connection="default" />
                </service>
                <service id="my.subscriber" class="Acme\SearchBundle\EventListener\SearchIndexerSubscriber">
                    <tag name="doctrine.event_subscriber" connection="default" />
                </service>
            </services>
        </container>

Criando a Classe Ouvinte
------------------------

No exemplo anterior, um serviço ``my.listener`` foi configurado como um
ouvinte Doctrine no evento ``postPersist``. A classe atrás desse serviço deve ter
um método ``postPersist``, que será chamado quando o evento é lançado::

    // src/Acme/SearchBundle/EventListener/SearchIndexer.php
    namespace Acme\SearchBundle\EventListener;

    use Doctrine\ORM\Event\LifecycleEventArgs;
    use Acme\StoreBundle\Entity\Product;

    class SearchIndexer
    {
        public function postPersist(LifecycleEventArgs $args)
        {
            $entity = $args->getEntity();
            $entityManager = $args->getEntityManager();

            // perhaps you only want to act on some "Product" entity
            if ($entity instanceof Product) {
                // do something with the Product
            }
        }
    }

Em cada evento, você tem acesso a um objeto ``LifecycleEventArgs``, que
dá acesso tanto ao objeto entidade do evento quanto ao gerenciador de entidade
em si.

Algo importante a notar é que um ouvinte estará ouvindo *todas* as entidades
em sua aplicação. Então, se você está interessado apenas em lidar com um tipo
específico de entidade (por exemplo, uma entidade ``Product`, mas não uma entidade
``BlogPost``), você deve verificar o nome da classe da entidade em seu método
(como mostrado acima).

.. _`O Sistema de Eventos`: http://docs.doctrine-project.org/projects/doctrine-orm/en/2.1/reference/events.html
