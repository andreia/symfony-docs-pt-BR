Como Trabalhar com E-mails Durante o Desenvolvimento
====================================================

Quando você estiver criando uma aplicação que envia e-mails, muitas vezes não vai
desejar enviar os e-mails ao destinatário especificado, durante o 
desenvolvimento. Se você estiver usando o ``SwiftmailerBundle`` com o Symfony2, 
poderá facilmente conseguir isso através de definições de configuração sem ter que
fazer quaisquer alterações no código da sua aplicação. Existem duas opções
principais quando se trata de manipulação de e-mails durante o desenvolvimento: (a) desativação do
envio de e-mails totalmente ou (b) o envio de todos os e-mails para um endereço 
especificado.

Desativando o Envio
-------------------

Você pode desativar o envio de e-mails, definindo a opção ``disable_delivery`` 
para ``true``. Este é o padrão para o ambiente ``test`` na distribuição
Standard. Se você fizer isso especificamente na configuração ``test``, então os emails
não será enviados quando você executar testes, mas continuarão a ser enviados nos
ambientes ``prod`` e ``dev``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_test.yml
        swiftmailer:
            disable_delivery:  true

    .. code-block:: xml

        <!-- app/config/config_test.xml -->

        <!--
        xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
        http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            disable-delivery="true" />

    .. code-block:: php

        // app/config/config_test.php
        $container->loadFromExtension('swiftmailer', array(
            'disable_delivery'  => "true",
        ));

Se você também gostaria de desativar a entrega no ambiente ``dev``, simplesmente
adicione esta configuração ao arquivo ``config_dev.yml``.

Enviando para um Endereço Especificado
--------------------------------------

Você também pode optar por enviar todos os emails para um endereço específico, em vez
do endereço atualmente especificado, ao enviar a mensagem. Isto pode ser feito
através da opção ``delivery_address``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        swiftmailer:
            delivery_address:  dev@example.com

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->

        <!--
        xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
        http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            delivery-address="dev@example.com" />

    .. code-block:: php

        // app/config/config_dev.php
        $container->loadFromExtension('swiftmailer', array(
            'delivery_address'  => "dev@example.com",
        ));

Agora, suponha que você está enviando um email para ``recipient@example.com``.

.. code-block:: php

    public function indexAction($name)
    {
        $message = \Swift_Message::newInstance()
            ->setSubject('Hello Email')
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody($this->renderView('HelloBundle:Hello:email.txt.twig', array('name' => $name)))
        ;
        $this->get('mailer')->send($message);

        return $this->render(...);
    }

No ambiente ``dev``, o e-mail será enviado para ``dev@example.com``.
O Swiftmailer irá adicionar um cabeçalho extra para o e-mail, ``X-Swift-To`` contendo
o endereço substituído, assim você ainda poderá visualizar para quem ele teria sido
enviado.

.. note::

    Além do endereço ``to``, ele também irá parar os e-mails sendo
    enviados para quaisquer endereços ``CC`` e ``BCC`` definidos. O SwiftMailer irá adicionar
    cabeçalhos adicionais para o e-mail com os endereços substituídos neles.
    Eles são ``X-Swift-Cc`` e ``X-Swift-Bcc`` para os endereços ``CC`` e ``BCC``, 
    respectivamente.

Visualização na Barra de Ferramentas de Debug Web
-------------------------------------------------

Você pode visualizar quaisquer e-mails enviados por uma página quando estiver no ambiente ``dev``
usando a Barra de Ferramentas para Debug Web. O ícone de e-mail na barra de ferramentas irá mostrar 
quantos e-mails foram enviados. Se você clicar nele, um relatório mostrando os detalhes dos
e-mails será aberto.

Se você estiver enviando um e-mail e imediatamente executar um redirecionamento, você
precisará definir a opção ``intercept_redirects`` para ``true`` no arquivo ``config_dev.yml``
para que possa ver o e-mail na barra de ferramentas de debug web antes de ser redirecionado.