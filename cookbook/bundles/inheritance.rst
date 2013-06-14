.. index::
   single: Bundle; Inheritance

Como usar herança para substituir partes de um Bundle
=====================================================

Ao trabalhar com bundles de terceiros, você frequentemente precisará substituir
um arquivo dele por um próprio seu para personalizar seu comportarmento ou aparência.
Symfony possui uma maneira bem conveniente de personalizar controllers, templates,
Traduções e outros arquivos do diretório ``Resources/`` de um bundle.

Por exemplo, suponha que você está instalando `FOSUserBundle`_, mas você quer
que a template ``layout.html.twig`` o um dos seus controllers seja aqueles
que você personalizou e colocou no seu bundle. No exemplo seguinte, estamos assumindo
que você já tenha o bundle ``AcmeUserBundle`` e coloque os arquivos personalizados nele.
O primeiro passo é registrar o bundle ``FOSUserBundle`` como pai do seu bundle:

.. code-block:: php

    // src/Acme/UserBundle/AcmeUserBundle.php
    namespace Acme\UserBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class AcmeUserBundle extends Bundle
    {
        public function getParent()
        {
            return 'FOSUserBundle';
        }
    }

Esta simples alteração permitirá que substitua vários partes de ``FOSUserBundle``
simplesmente criando um arquivo com o mesmo nome.

Substituindo controladores
~~~~~~~~~~~~~~~~~~~~~~~~~~

Suponha que você queira adicionar alguma funcionalidade a ação ``registerAction``
do controlador ``RegistrationController`` que está dentro do bundle ``FOSUserBundle``.
Para fazê-lo, basta criar o seu próprio ``RegistrationController.php``, crie um método
que substitua o do bundle original e mude sua funcionalidade como mostrado a seguir.

.. code-block:: php

    // src/Acme/UserBundle/Controller/RegistrationController.php
    namespace Acme\UserBundle\Controller;

    use FOS\UserBundle\Controller\RegistrationController as BaseController;

    class RegistrationController extends BaseController
    {
        public function registerAction()
        {
            $response = parent::registerAction();
            
            // do custom stuff
            
            return $response;
        }
    }

.. tip::

    Dependendo do tipo de personalização que precisa fazer no controlador, você
    pode substituir completamente o método com lógica própria sem nem mesmo
    chamar ``parent::registerAction()``.

.. note::

    Substituir controladores desta maneira somente funciona se o bundle referencia
    o controlador utilizando sintaxe padrão ``FOSUserBundle:Registration:register``
    nas rotas e nos templates. Esta é a sintaxe recomendada.

Substituindo recursos: Templates, Rotas, Traduções, Validação, etc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A maioria dos recursos também podem ser substituídos, simplesmente criando um arquivo
no mesmo caminho relativo que estiver no bundle pai.

Por exemplo, é muito comum precisar substituir o arquivo de template ``layout.html.twig``
do bundle ``FOSUserBundle`` para utilizar o layout base de sua própria aplicação.
Uma vez que o arquivo fica no caminho ``Resources/views/layout.html.twig`` dentro do bundle
``FOSUserBundle`` você consegue criar seu próprio arquivo no mesmo lugar relativo dentro do seu bundler (por exemplo,
``Resources/views/layout.html.twig`` do bundle ``FOSUserBundle``). O Symfony vai
ignorar o arquivo dentro do ``FOSUserBundle`` e utilizar o seu no lugar.

O mesmo vale para arquivos de rotas, configuração de Validação e outros recursos.

.. note::

    A substituição de recursos só funciona quando você se refere a recursos utilizando
    a sintaxe recomendada ``@FosUserBundle/Resources/config/routing/security.xml``.
    Se você se referir a recursos sem o atalho @FosUserBundle, eles não serão substituídos.

.. note::

    Arquivos de traduções não funcionam da maneira descrita acima. Todos os ficheiros traduzidos serão adicionados em um conjunto de "pools" organizados por dominios. Symfony abrirá os ficheiros de tradução dos bundles primeiro na ordem que eles são inicializados e então do seu diretorio 'app/Resource'. Se houver dois ficheiros da mesma tradução estiver especificados por diferentes 'Resources', o ficheiro de tradução que for aberto por ultimo será o utilizado.

.. _`FOSUserBundle`: https://github.com/friendsofsymfony/fosuserbundle