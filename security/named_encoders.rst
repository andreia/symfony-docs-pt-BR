.. index::
    single: Codificadores Nomeados

Como Escolher Dinamicamente o Algoritmo Codificador de Senha
============================================================

Normalmente, o mesmo codificador de senha é usado para todos os usuários configurando-o
para aplicar a todas as instâncias de uma classe específica:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            encoders:
                Symfony\Component\Security\Core\User\User: sha512

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >
            <config>
                <!-- ... -->
                <encoder class="Symfony\Component\Security\Core\User\User"
                    algorithm="sha512"
                />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => array(
                    'algorithm' => 'sha512',
                ),
            ),
        ));

Outra opção é usar um codificador "nomeado" e então selecionar qual codificador
pretende utilizar de forma dinâmica.

No exemplo anterior, foi definido o algoritmo ``sha512`` para ``Acme\UserBundle\Entity\User``.
Isso pode ser seguro o suficiente para um usuário comum, mas e se você deseja que seus administradores
tenham um algoritmo mais forte, por exemplo `` bcrypt``. Isso pode ser feito com
codificadores nomeados:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...
            encoders:
                harsh:
                    algorithm: bcrypt
                    cost: 15

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >

            <config>
                <!-- ... -->
                <encoder class="harsh"
                    algorithm="bcrypt"
                    cost="15" />
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'encoders' => array(
                'harsh' => array(
                    'algorithm' => 'bcrypt',
                    'cost'      => '15'
                ),
            ),
        ));

Isso cria um codificador nomeado ``harsh``. Para que uma instância ``User``
utilize ele, a classe deve implementar
:class:`Symfony\\Component\\Security\\Core\\Encoder\\EncoderAwareInterface`.
A interface requer um método - ``getEncoderName`` - que deve retornar
o nome do codificador que será utilizado::

    // src/Acme/UserBundle/Entity/User.php
    namespace Acme\UserBundle\Entity;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Encoder\EncoderAwareInterface;

    class User implements UserInterface, EncoderAwareInterface
    {
        public function getEncoderName()
        {
            if ($this->isAdmin()) {
                return 'harsh';
            }

            return null; // use the default encoder
        }
    }
