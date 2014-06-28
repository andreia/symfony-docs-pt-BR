.. index::
   single: Segurança; Voters

Como implementar seu próprio Voter para lista negra (blacklist) de endereços IP
===============================================================================

O componente de segurança do Symfony2 fornece várias camadas para autorizar usuários.
Uma das camadas é chamada `voter`. Um voter é uma classe dedicada que verifica
se o usuário possui direitos para estar conectado à aplicação.
Por exemplo, o Symfony2 fornece uma camada que verifica se o usuário
está totalmente autorizado ou se ele tem alguns papéis esperados.

Às vezes é útil criar um voter personalizado para lidar com um caso específico não
manipulado pelo framework. Nesta seção, você vai aprender como criar um voter
que permitirá criar uma blacklist de usuários por seus IPs.

A Interface Voter
-----------------

Um voter personalizado deve implementar
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`,
que exige os três métodos a seguir:

.. code-block:: php

    interface VoterInterface
    {
        public function supportsAttribute($attribute);
        public function supportsClass($class);
        public function vote(TokenInterface $token, $object, array $attributes);
    }

O método ``supportsAttribute()`` é usado para verificar se o voter suporta
o atributo de usuário fornecido (ou seja: um papel - role -, um ACL, etc.).

O método ``supportsClass()`` é usado para verificar se o voter suporta a
classe do token de usuário atual.

O método ``vote()`` deve implementar a lógica de negócio que verifica se
o usuário tem acesso. Esse método deve retornar um dos seguintes
valores:

* ``VoterInterface::ACCESS_GRANTED``: O usuário tem permissão para acessar a aplicação
* ``VoterInterface::ACCESS_ABSTAIN``: O voter não pode decidir se o usuário está ou não autorizado
* ``VoterInterface::ACCESS_DENIED``: O usuário não tem permissão para acessar a aplicação

Neste exemplo, você vai verificar se o endereço IP do usuário corresponde a uma lista de
endereços na blacklist. Se o IP do usuário está na lista negra, você vai retornar
``VoterInterface::ACCESS_DENIED``, caso contrário você vai retornar
``VoterInterface::ACCESS_ABSTAIN`` pois o propósito desse voter é somente negar
acesso, e não conceder.

Criando um Voter Personalizado
------------------------------

Para adicionar um usuário na lista negra com base em seu endereço IP, você pode usar o serviço ``request``
e comparar o endereço IP a um conjunto de endereços IP na lista negra:

.. code-block:: php

    // src/Acme/DemoBundle/Security/Authorization/Voter/ClientIpVoter.php
    namespace Acme\DemoBundle\Security\Authorization\Voter;

    use Symfony\Component\DependencyInjection\ContainerInterface;
    use Symfony\Component\Security\Core\Authorization\Voter\VoterInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;

    class ClientIpVoter implements VoterInterface
    {
        private $container;

        private $blacklistedIp;

        public function __construct(ContainerInterface $container, array $blacklistedIp = array())
        {
            $this->container     = $container;
            $this->blacklistedIp = $blacklistedIp;
        }

        public function supportsAttribute($attribute)
        {
            // you won't check against a user attribute, so return true
            return true;
        }

        public function supportsClass($class)
        {
            // your voter supports all type of token classes, so return true
            return true;
        }

        public function vote(TokenInterface $token, $object, array $attributes)
        {
            $request = $this->container->get('request');
            if (in_array($request->getClientIp(), $this->blacklistedIp)) {
                return VoterInterface::ACCESS_DENIED;
            }

            return VoterInterface::ACCESS_ABSTAIN;
        }
    }

É isso! O voter está pronto. O próximo passo é injetar o voter na
camada de segurança. Isso pode ser feito facilmente através do container de serviço.

.. tip::

    A sua implementação dos métodos
    :method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface::supportsAttribute`
    e :method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface::supportsClass`
    não está sendo chamada internamente pelo framework. Depois de ter registado o seu
    voter o método ``vote()`` sempre será chamado, independentemente desses dois
    métodos retornarem true ou não. Portanto, você precisa chamar os
    métodos na sua implementação do método ``vote()`` e retornar ``ACCESS_ABSTAIN``
    se o seu voter não suporta a classe ou atributo.

Declarando o Voter como um Serviço
----------------------------------

Para injetar o voter na camada de segurança, você deve declará-lo como um serviço,
e adicionar a tag ``security.voter``:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/AcmeBundle/Resources/config/services.yml
        services:
            security.access.blacklist_voter:
                class:     Acme\DemoBundle\Security\Authorization\Voter\ClientIpVoter
                arguments: ["@service_container", [123.123.123.123, 171.171.171.171]]
                public:    false
                tags:
                    - { name: security.voter }

    .. code-block:: xml

        <!-- src/Acme/AcmeBundle/Resources/config/services.xml -->
        <service id="security.access.blacklist_voter"
                 class="Acme\DemoBundle\Security\Authorization\Voter\ClientIpVoter" public="false">
            <argument type="service" id="service_container" strict="false" />
            <argument type="collection">
                <argument>123.123.123.123</argument>
                <argument>171.171.171.171</argument>
            </argument>
            <tag name="security.voter" />
        </service>

    .. code-block:: php

        // src/Acme/AcmeBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $definition = new Definition(
            'Acme\DemoBundle\Security\Authorization\Voter\ClientIpVoter',
            array(
                new Reference('service_container'),
                array('123.123.123.123', '171.171.171.171'),
            ),
        );
        $definition->addTag('security.voter');
        $definition->setPublic(false);

        $container->setDefinition('security.access.blacklist_voter', $definition);

.. tip::

   Certifique-se de importar esse arquivo de configuração em seu arquivo de configuração principal
   da aplicação (por exemplo, ``app/config/config.yml``). Para mais informações
   veja :ref:`service-container-imports-directive`. Para ler mais sobre a definição de
   serviços em geral, consulte o capítulo :doc:`/book/service_container` .

.. _security-voters-change-strategy:

Mudando a Estratégia de Decisão de Acesso
-----------------------------------------

Para que o novo voter tenha efeito, é necessário alterar a estratégia de decisão de
acesso padrão, que, concede acesso se *qualquer* voter conceder
acesso.

Neste caso, escolha a estratégia ``unanimous``. Ao contrário da estratégia
``affirmative`` (o padrão), com a estratégia ``unanimous``, se apenas um voter
negar o acesso (por exemplo, o ``ClientIpVoter``), o acesso não é concedido ao
o usuário final.

Para fazer isso, sobrescreva a seção ``access_decision_manager`` padrão de seu
arquivo de configuração da aplicação com o seguinte código.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            access_decision_manager:
                # strategy can be: affirmative, unanimous or consensus
                strategy: unanimous

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <!-- strategy can be: affirmative, unanimous or consensus -->
            <access-decision-manager strategy="unanimous">
        </config>

    .. code-block:: php

        // app/config/security.xml
        $container->loadFromExtension('security', array(
            // strategy can be: affirmative, unanimous or consensus
            'access_decision_manager' => array(
                'strategy' => 'unanimous',
            ),
        ));

É isso! Agora, no momento de decidir se um usuário deve ou não ter acesso,
o novo voter vai negar acesso a qualquer usuário na lista negra de IPs.

.. seealso::

    Para um uso mais avançado ver
    :ref:`components-security-access-decision-manager`.
