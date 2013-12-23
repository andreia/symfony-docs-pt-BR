.. index::
   single: Segurança; Provider de Usuário

Como criar um Provider de Usuário Personalizado
===============================================

Parte do processo de autenticação padrão do Symfony depende de "providers de usuário".
Quando um usuário submete um nome de usuário e senha, a camada de autenticação solicita ao provider
de usuário configurado para retornar um objeto de usuário para um determinado nome de usuário.
Em seguida, o Symfony verifica se a senha deste usuário está correta e gera
um token de segurança para que o usuário permaneça autenticado durante a sessão atual.
O Symfony vem com os providers de usuário "in_memory" e "entity" prontos para uso.
Neste artigo, você vai aprender como criar o seu próprio provider de usuário, que
pode ser útil se os usuários são acessados através de um banco de dados personalizado, um arquivo
ou - como mostrado neste exemplo - um serviço web.

Crie uma Classe de Usuário
--------------------------

Em primeiro lugar, independentemente de *onde* os seus dados de usuário estão vindo, você vai
precisar criar uma classe ``User`` que representa esses dados. No entando, a classe ``User`` pode
parecer com o que você quiser e conter quaisquer dados. O único requisito é que a
classe implemente :class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`.
Os métodos dessa interface devem ser definidos na classe de usuário 
personalizada: :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getRoles`,
:method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getPassword`,
:method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getSalt`,
:method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getUsername`,
:method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::eraseCredentials`.
Também pode ser útil implementar a interface
:class:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface`,
a qual define um método para verificar se o usuário é igual ao usuário atual. Essa
interface requer um método :method:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface::isEqualTo`
.

Vamos ver isso na prática::

    // src/Acme/WebserviceUserBundle/Security/User/WebserviceUser.php
    namespace Acme\WebserviceUserBundle\Security\User;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\User\EquatableInterface;

    class WebserviceUser implements UserInterface, EquatableInterface
    {
        private $username;
        private $password;
        private $salt;
        private $roles;

        public function __construct($username, $password, $salt, array $roles)
        {
            $this->username = $username;
            $this->password = $password;
            $this->salt = $salt;
            $this->roles = $roles;
        }

        public function getRoles()
        {
            return $this->roles;
        }

        public function getPassword()
        {
            return $this->password;
        }

        public function getSalt()
        {
            return $this->salt;
        }

        public function getUsername()
        {
            return $this->username;
        }

        public function eraseCredentials()
        {
        }

        public function isEqualTo(UserInterface $user)
        {
            if (!$user instanceof WebserviceUser) {
                return false;
            }

            if ($this->password !== $user->getPassword()) {
                return false;
            }

            if ($this->getSalt() !== $user->getSalt()) {
                return false;
            }

            if ($this->username !== $user->getUsername()) {
                return false;
            }

            return true;
        }
    }

.. versionadded:: 2.1
    A ``EquatableInterface`` foi adicionada no Symfony 2.1. Use o método
    ``equals()`` da ``UserInterface`` no Symfony 2.0.

Se você tiver mais informações sobre seus usuários - como um "primeiro nome" - então
você pode adicionar um campo ``firstName`` para guardar esse dado.

Criar um Provider de Usuário
----------------------------

Agora que você tem uma classe ``User``, você vai criar um provider de usuário, que irá
pegar informações de usuário de algum serviço web, criar um objeto ``WebserviceUser``
e popular ele com os dados.

O provider de usuário é apenas uma classe PHP que deve implementar a
:class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`,
que requer a definição de três métodos: ``loadUserByUsername($username)``,
``refreshUser(UserInterface $user)`` e ``supportsClass($class)``. Para
mais detalhes, consulte :class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`.

Aqui está um exemplo de como isso pode parecer::

    // src/Acme/WebserviceUserBundle/Security/User/WebserviceUserProvider.php
    namespace Acme\WebserviceUserBundle\Security\User;

    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Symfony\Component\Security\Core\Exception\UnsupportedUserException;

    class WebserviceUserProvider implements UserProviderInterface
    {
        public function loadUserByUsername($username)
        {
            // make a call to your webservice here
            $userData = ...
            // pretend it returns an array on success, false if there is no user

            if ($userData) {
                $password = '...';

                // ...

                return new WebserviceUser($username, $password, $salt, $roles);
            }

            throw new UsernameNotFoundException(sprintf('Username "%s" does not exist.', $username));
        }

        public function refreshUser(UserInterface $user)
        {
            if (!$user instanceof WebserviceUser) {
                throw new UnsupportedUserException(sprintf('Instances of "%s" are not supported.', get_class($user)));
            }

            return $this->loadUserByUsername($user->getUsername());
        }

        public function supportsClass($class)
        {
            return $class === 'Acme\WebserviceUserBundle\Security\User\WebserviceUser';
        }
    }

Crie um Serviço para o Provider de Usuário
------------------------------------------

Agora você tornará o provider de usuário disponível como um serviço:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/WebserviceUserBundle/Resources/config/services.yml
        parameters:
            webservice_user_provider.class: Acme\WebserviceUserBundle\Security\User\WebserviceUserProvider

        services:
            webservice_user_provider:
                class: "%webservice_user_provider.class%"

    .. code-block:: xml

        <!-- src/Acme/WebserviceUserBundle/Resources/config/services.xml -->
        <parameters>
            <parameter key="webservice_user_provider.class">Acme\WebserviceUserBundle\Security\User\WebserviceUserProvider</parameter>
        </parameters>

        <services>
            <service id="webservice_user_provider" class="%webservice_user_provider.class%"></service>
        </services>

    .. code-block:: php

        // src/Acme/WebserviceUserBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setParameter('webservice_user_provider.class', 'Acme\WebserviceUserBundle\Security\User\WebserviceUserProvider');

        $container->setDefinition('webservice_user_provider', new Definition('%webservice_user_provider.class%');

.. tip::

    A verdadeira implementação do provider de usuário provavelmente terá algumas
    dependências, opções de configuração ou outros serviços. Adicione eles como
    argumentos na definição de serviço.

.. note ::

    Certifique-se que o arquivo de serviços está sendo importado. Veja :ref:`service-container-imports-directive`
    para mais detalhes.

Modifique o ``security.yml``
----------------------------

Tudo será combinado em sua configuração de segurança. Adicione o provider de usuário
na lista de providers na seção "security". Escolha um nome para o provider de usuário
(por exemplo, "webservice") e mencione o id do serviço que você acabou de definir.

.. configuration-block::

    .. code-block:: yaml

        // app/config/security.yml
        security:
            providers:
                webservice:
                    id: webservice_user_provider

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <provider name="webservice" id="webservice_user_provider" />
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'webservice' => array(
                    'id' => 'webservice_user_provider',
                ),
            ),
        ));

O Symfony também precisa saber como codificar as senhas que são fornecidas no site pelos
usuários, por exemplo, através do preenchimento de um formulário de login. Você pode fazer
isso adicionando uma linha na seção "encoders" da sua configuração de segurança:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            encoders:
                Acme\WebserviceUserBundle\Security\User\WebserviceUser: sha512

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <config>
            <encoder class="Acme\WebserviceUserBundle\Security\User\WebserviceUser">sha512</encoder>
        </config>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'encoders' => array(
                'Acme\WebserviceUserBundle\Security\User\WebserviceUser' => 'sha512',
            ),
        ));

O valor aqui deve corresponder porém com as senhas que foram originalmente
codificadas ao criar os seus usuários (no entanto os usuários foram criados). Quando
um usuário submete a sua senha, a senha é acrescentada ao valor salt e então são
codificados usando este algoritmo antes de ser comparada com o hash da senha retornado
pelo seu método ``getPassword()``. Além disso, dependendo das suas opções,
a senha pode ser codificada várias vezes e codificada para base64.

.. sidebar:: Detalhes sobre como as senhas são codificadas

    O Symfony utiliza um método específico para combinar o salt e codificar a senha
    antes de compará-la com a senha codificada. Se o ``getSalt()`` não retornar
    nada, então a senha submetida é simplesmente codificada utilizando o algoritmo
    que você especificou no ``security.yml``. Se um salt *é* especificado, então o valor
    seguinte é criado e *então* feito o hash através do algoritmo:

        ``$password.'{'.$salt.'}';``, ``

    Caso os usuários externos tenham nas suas senhas um salt através de um método diferente,
    então você terá um pouco mais de trabalho para que o Symfony codifique corretamente
    a senha. Isso está além do escopo deste artigo, mas incluiria
    estender a classe ``MessageDigestPasswordEncoder`` e sobrescrever o método
    ``mergePasswordAndSalt``.

    Além disso, o hash, por padrão, é codificado várias vezes e codificado
    para base64. Para obter detalhes específicos, consulte `MessageDigestPasswordEncoder`_.
    Para evitar isso, configure ele no seu arquivo de configuração:

    .. configuration-block::

        .. code-block:: yaml

            # app/config/security.yml
            security:
                encoders:
                    Acme\WebserviceUserBundle\Security\User\WebserviceUser:
                        algorithm: sha512
                        encode_as_base64: false
                        iterations: 1

        .. code-block:: xml

            <!-- app/config/security.xml -->
            <config>
                <encoder class="Acme\WebserviceUserBundle\Security\User\WebserviceUser"
                    algorithm="sha512"
                    encode-as-base64="false"
                    iterations="1"
                />
            </config>

        .. code-block:: php

            // app/config/security.php
            $container->loadFromExtension('security', array(
                'encoders' => array(
                    'Acme\WebserviceUserBundle\Security\User\WebserviceUser' => array(
                        'algorithm'         => 'sha512',
                        'encode_as_base64'  => false,
                        'iterations'        => 1,
                    ),
                ),
            ));

.. _MessageDigestPasswordEncoder: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Security/Core/Encoder/MessageDigestPasswordEncoder.php
