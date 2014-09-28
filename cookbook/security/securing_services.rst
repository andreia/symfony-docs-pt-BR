.. index::
   single: Segurança; Protegendo qualquer serviço
   single: Segurança; Protegendo qualquer método

Como proteger qualquer serviço ou método em sua aplicação
=========================================================

No capítulo de segurança, você pode ver como :ref:`proteger um controlador <book-security-securing-controller>`
solicitando o serviço ``security.context`` do Container de Serviço
e verificando o papel (role) do usuário atual::

    // ...
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    public function helloAction($name)
    {
        if (false === $this->get('security.context')->isGranted('ROLE_ADMIN')) {
            throw new AccessDeniedException();
        }

        // ...
    }

Você também pode proteger *qualquer* serviço de forma semelhante ao injetar o serviço ``security.context``
nele. Para uma introdução geral sobre como injetar dependências em
serviços veja o capítulo do livro :doc:`/book/service_container`. Por
exemplo, supondo que você tenha uma classe ``NewsletterManager`` que envia e-mails
e você quer restringir o seu uso apenas aos usuários que possuam o papel
``ROLE_NEWSLETTER_ADMIN``. Antes de adicionar a segurança, a classe parece com o seguinte:

.. code-block:: php

    // src/Acme/HelloBundle/Newsletter/NewsletterManager.php
    namespace Acme\HelloBundle\Newsletter;

    class NewsletterManager
    {

        public function sendNewsletter()
        {
            // ... where you actually do the work
        }

        // ...
    }

Seu objetivo é verificar o papel do usuário quando o método ``sendNewsletter()`` é
chamado. O primeiro passo para isso é injetar o serviço ``security.context``
no objeto. Uma vez que não fará sentido *não* realizar a verificação de
segurança, esse é um candidato ideal para injeção de construtor, que garante
que o objeto do contexto de segurança (security context) estará disponível dentro da classe
``NewsletterManager``::

    namespace Acme\HelloBundle\Newsletter;

    use Symfony\Component\Security\Core\SecurityContextInterface;

    class NewsletterManager
    {
        protected $securityContext;

        public function __construct(SecurityContextInterface $securityContext)
        {
            $this->securityContext = $securityContext;
        }

        // ...
    }

Então, em sua configuração de serviço, você pode injetar o serviço:

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/services.yml
        parameters:
            newsletter_manager.class: Acme\HelloBundle\Newsletter\NewsletterManager

        services:
            newsletter_manager:
                class:     "%newsletter_manager.class%"
                arguments: ["@security.context"]

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
        <parameters>
            <parameter key="newsletter_manager.class">Acme\HelloBundle\Newsletter\NewsletterManager</parameter>
        </parameters>

        <services>
            <service id="newsletter_manager" class="%newsletter_manager.class%">
                <argument type="service" id="security.context"/>
            </service>
        </services>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->setParameter('newsletter_manager.class', 'Acme\HelloBundle\Newsletter\NewsletterManager');

        $container->setDefinition('newsletter_manager', new Definition(
            '%newsletter_manager.class%',
            array(new Reference('security.context'))
        ));

O serviço injetado pode agora ser utilizado para realizar a verificação de segurança quando o
método ``sendNewsletter()`` é chamado::

    namespace Acme\HelloBundle\Newsletter;

    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    use Symfony\Component\Security\Core\SecurityContextInterface;
    // ...

    class NewsletterManager
    {
        protected $securityContext;

        public function __construct(SecurityContextInterface $securityContext)
        {
            $this->securityContext = $securityContext;
        }

        public function sendNewsletter()
        {
            if (false === $this->securityContext->isGranted('ROLE_NEWSLETTER_ADMIN')) {
                throw new AccessDeniedException();
            }

            // ...
        }

        // ...
    }

Se o usuário atual não possui o ``ROLE_NEWSLETTER_ADMIN``, ele
será solicitado a efetuar o login.

Protegendo Métodos usando Anotações
-----------------------------------

Você também pode proteger chamadas de métodos em qualquer serviço com anotações usando o bundle
opcional `JMSSecurityExtraBundle`_. Esse bundle está incluído na
Distribuição Standard do Symfony.

Para habilitar a funcionalidade de anotações, :ref:`tag <book-service-container-tags>`
o serviço que você deseja proteger com a tag ``security.secure_service``
(você também pode ativar automaticamente essa funcionalidade para todos os serviços, consulte a
:ref:`sidebar <securing-services-annotations-sidebar>` abaixo):

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/HelloBundle/Resources/config/services.yml

        # ...
        services:
            newsletter_manager:
                # ...
                tags:
                    -  { name: security.secure_service }

    .. code-block:: xml

        <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
        <!-- ... -->

        <services>
            <service id="newsletter_manager" class="%newsletter_manager.class%">
                <!-- ... -->
                <tag name="security.secure_service" />
            </service>
        </services>

    .. code-block:: php

        // src/Acme/HelloBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $definition = new Definition(
            '%newsletter_manager.class%',
            array(new Reference('security.context'))
        ));
        $definition->addTag('security.secure_service');
        $container->setDefinition('newsletter_manager', $definition);

Você pode então alcançar os mesmos resultados descritos acima utilizando uma anotação::

    namespace Acme\HelloBundle\Newsletter;

    use JMS\SecurityExtraBundle\Annotation\Secure;
    // ...

    class NewsletterManager
    {

        /**
         * @Secure(roles="ROLE_NEWSLETTER_ADMIN")
         */
        public function sendNewsletter()
        {
            // ...
        }

        // ...
    }

.. note::

    As anotações funcionam porque uma classe proxy é criada para a sua classe
    que executa as verificações de segurança. Isso significa que, enquanto você pode usar
    anotações em métodos públicos e protegidos, você não pode usá-las em
    métodos privados ou em métodos marcados como final.

O JMSSecurityExtraBundle também permite que você proteja os parâmetros e valores de
retorno dos métodos. Para mais informações, consulte a documentação
`JMSSecurityExtraBundle`_.

.. _securing-services-annotations-sidebar:

.. sidebar:: Ativando a funcionalidade de anotações para todos os Serviços

    Ao proteger o método de um serviço (como mostrado acima), você pode
    aplicar a tag em cada serviço individualmente, ou ativar a funcionalidade para *todos*
    serviços de uma só vez. Para fazer isso, defina a opção de configuração ``secure_all_services``
    como true:

    .. configuration-block::

        .. code-block:: yaml

            # app/config/config.yml
            jms_security_extra:
                # ...
                secure_all_services: true

        .. code-block:: xml

            <?xml version="1.0" ?>

            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:acme_hello="http://www.example.com/symfony/schema/"
                xsi:schemaLocation="http://www.example.com/symfony/schema/ http://www.example.com/symfony/schema/hello-1.0.xsd">

                <!-- app/config/config.xml -->

                <jms_security_extra secure_controllers="true" secure_all_services="true" />

            </srv:container>

        .. code-block:: php

            // app/config/config.php
            $container->loadFromExtension('jms_security_extra', array(
                // ...

                'secure_all_services' => true,
            ));

    A desvantagem desse método é que, se ativado, o carregamento da página inicial
    pode ser muito lento, dependendo de quantos serviços você definiu.

.. _`JMSSecurityExtraBundle`: https://github.com/schmittjoh/JMSSecurityExtraBundle
