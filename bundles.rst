.. index::
   single: Bundles

.. _page-creation-bundles:

O Sistema de Bundles
====================

Um bundle é semelhante a um plug-in em outro software, mas ainda melhor. A principal
diferença é que *tudo* é um bundle no Symfony, incluindo tanto as
funcionalidades do núcleo do framework quanto o código escrito para a sua aplicação.
Bundles são cidadãos de primeira classe no Symfony. Isso lhe fornece a flexibilidade
para usar os recursos pré-construídos em `bundles de terceiros`_ ou para distribuir os
seus próprios bundles. Isso torna mais fácil escolher quais recursos devem ser habilitados
em seu aplicativo e otimizá-los da forma que quiser.

.. note::

    Enquanto você vai aprender o básico aqui, um artigo inteiro é dedicado à
    organização e melhores práticas dos :doc:`bundles </bundles/best_practices>`.

Um bundle é simplesmente um conjunto estruturado de arquivos dentro de um diretório que implementa
um único recurso. Você pode criar um BlogBundle, um ForumBundle ou
um bundle para gerenciamento de usuários (muitos deles já existem como bundles open
source). Cada diretório contém tudo relacionado ao recurso em questão, incluindo
arquivos PHP, templates, folhas de estilo, arquivos JavaScript, testes e qualquer outra coisa.
Cada aspecto de um recurso existe em um bundle e cada recurso reside em um
bundle.

Bundles usados ​​em seus aplicativos deve estar habilitados, registando-os no
método ``registerBundles()`` da classe ``AppKernel``::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            new Symfony\Bundle\MonologBundle\MonologBundle(),
            new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            new Symfony\Bundle\DoctrineBundle\DoctrineBundle(),
            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
            new AppBundle\AppBundle(),
        );

        if (in_array($this->getEnvironment(), array('dev', 'test'))) {
            $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            $bundles[] = new Sensio\Bundle\DistributionBundle\SensioDistributionBundle();
            $bundles[] = new Sensio\Bundle\GeneratorBundle\SensioGeneratorBundle();
        }

        return $bundles;
    }

Com o método ``registerBundles()``, você tem total controle sobre quais bundles
são usados ​​pela sua aplicação (incluindo os bundles do núcleo do symfony).

.. tip::

   Um pacote pode residir em *qualquer lugar* contanto que possa ser carregado automaticamente (através do
   autoloader configurado em ``app/autoload.php``).

Criando um Bundle
-----------------

A Edição Standard do Symfony vem com uma tarefa útil que cria um bundle totalmente
funcional para você. Claro, a criação de um bundle manualmente é muito fácil também.

Para mostrar como o sistema de bundle é simples, crie um novo bundle chamado
AcmeTestBundle e habilite-o.

.. tip::

    A porção ``Acme`` é apenas um nome fictício que deve ser substituído por
    algum nome "vendor" que representa você ou a sua organização (por exemplo,
    ABCTestBundle para alguma empresa chamada ``ABC``).

Comece criando um diretório ``src/Acme/TestBundle/`` e adicionando um novo arquivo
chamado ``AcmeTestBundle.php``::

    // src/Acme/TestBundle/AcmeTestBundle.php
    namespace Acme\TestBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class AcmeTestBundle extends Bundle
    {
    }

.. tip::

   O nome AcmeTestBundle segue o padrão de
   :ref:`convenções de nomenclatura para Bundle <bundles-naming-conventions>`. Você poderia
   também optar por encurtar o nome do bundle para simplesmente TestBundle nomeando
   essa classe TestBundle (e nomear o arquivo ``TestBundle.php``).

Essa classe vazia é a única parte que você precisa criar para o novo pacote. Embora
comumente vazia, essa classe é poderosa e pode ser utilizada para personalizar o comportamento
do bundle.

Agora que você criou o bundle, habilite-o através da classe ``AppKernel``::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            // ...

            // register your bundle
            new Acme\TestBundle\AcmeTestBundle(),
        );
        // ...

        return $bundles;
    }

Embora ele não faça nada ainda, AcmeTestBundle está agora pronto para ser usado.

E tão simples quanto isso, o Symfony também fornece uma interface de linha de comando
para gerar um esqueleto básico do bundle:

.. code-block:: terminal

    $ php bin/console generate:bundle --namespace=Acme/TestBundle

O esqueleto do bundle gera um controlador básico, template e recurso de roteamento
que podem ser personalizados. Você vai aprender mais sobre aa ferramentas de linha de comando
do Symfony mais tarde.

.. tip::

   Sempre que criar um novo bundle ou usar um bundle de terceiros, certifique-se
   que o bundle tenha sido habilitado em ``registerBundles()``. Ao utilizar
   o comando ``generate:bundle``, isso já é feito para você.

Estrutura de Diretório do Bundle
--------------------------------

A estrutura de diretório de um bundle é simples e flexível. Por padrão, o
sistema de bundle segue um conjunto de convenções que ajudam a manter o código consistente
entre todos os bundles do Symfony. Dê uma olhada no AcmeDemoBundle, pois ele contém alguns
dos elementos mais comuns de um bundle:

``Controller/``
    Contém os controladores do bundle (por exemplo, ``RandomController.php``).

``DependencyInjection/``
    Contém certas classes de Extensão de Injeção de Dependência, que podem importar
    configuração de serviço, registrar compiler passes ou mais (esse diretório não é
    necessário).

``Resources/config/``
    Armazena configurações, incluindo a configuração de roteamento (por exemplo, ``routing.yml``).

``Resources/views/``
    Armazena templates organizados pelo nome do controlador (por exemplo, ``Hello/index.html.twig``).

``Resources/public/``
    Contém ativos web (imagens, folhas de estilo, etc.) e é copiado ou criado um link simbólico
    ao diretório ``web/`` do projeto através do comando de console
    ``assets:install``.

``Tests/``
    Contém todos os testes para o bundle.

Um bundle pode ser tão pequeno ou grande quanto o recurso que ele implementa. Ele contém
apenas os arquivos que você precisa e nada mais.

Ao move-se através das guias, você vai aprender como fazer para persistir objetos em um
banco de dados, criar e validar formulários, criar traduções para a sua aplicação,
escrever testes e muito mais. Cada uma delas tem seu próprio lugar e papel dentro
de um bundle.

Aprenda mais
------------

.. toctree::
    :maxdepth: 1
    :glob:

    bundles/*

_`third-party bundles`: http://knpbundles.com
