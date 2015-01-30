.. index::
    single: Bundle; Removendo o AcmeDemoBundle

Como remover o AcmeDemoBundle
=============================

A Edição Standard do Symfony vem com uma demonstração completa que reside dentro de um
bundle chamado AcmeDemoBundle. É um ótimo boilerplate como referência ao
iniciar um projeto, mas, provavelmente, você vai querer eventualmente removê-lo.

.. tip::

    Este artigo usa o AcmeDemoBundle como um exemplo, mas você pode usar
    estes passos para remover qualquer bundle.

1. Cancelar o registro do Bundle no ``AppKernel``
-------------------------------------------------

Para desconectar o bundle do framework, você deve remover o bundle do
método ``AppKernel::registerBundles()``. O bundle é normalmente encontrado no
array ``bundles`` mas o AcmeDemoBundle somente é registrado no
ambiente de desenvolvimento e você pode encontrá-lo dentro do if abaixo::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        public function registerBundles()
        {
            $bundles = array(...);

            if (in_array($this->getEnvironment(), array('dev', 'test'))) {
                // comment or remove this line:
                // $bundles[] = new Acme\DemoBundle\AcmeDemoBundle();
                // ...
            }
        }
    }

2. Remover a Configuração do Bundle
-----------------------------------

Agora que o Symfony não sabe sobre o bundle, você precisa remover qualquer
configuração e configuração de roteamento dentro do diretório ``app/config``
que refere-se ao pacote.

2.1 Remover Rotas do Bundle
~~~~~~~~~~~~~~~~~~~~~~~~~~~

As rotas para o AcmeDemoBundle podem ser encontradas em ``app/config/routing_dev.yml``.
Remova a entrada ``_acme_demo`` na parte inferior deste arquivo.

2.2 Remover a Configuração do Bundle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alguns bundles contêm configuração em um dos arquivos ``app/config/config*.yml``
. Certifique-se de remover a configuração relacionada nesses arquivos. Você pode
rapidamente detectar a configuração do bundle, procurando por uma string ``acme_demo`` (ou qualquer
seja o nome do bundle, por exemplo, ``fos_user`` para o FOSUserBundle) nos
arquivos de configuração.

O AcmeDemoBundle não têm configuração. No entanto, o bundle é
utilizado na configuração para o arquivo ``app/config/security.yml``. Você pode
usá-lo como um boilerplate para a sua própria segurança, mas você também **pode** remover
tudo: não importa para o Symfony se você removeu ele ou não.

3. Remover o Bundle do Sistema de Arquivos
------------------------------------------

Agora que você removeu todas as referências ao bundle em sua aplicação, você
deve remover o bundle do sistema de arquivos. O bundle está localizado no diretório
``src/Acme/DemoBundle``. Você deve remover esse diretório e
pode remover o diretório ``Acme`` também.

.. tip::

    Se você não sabe a localização de um bundle, pode usar o
    método :method:`Symfony\\Component\\HttpKernel\\Bundle\\BundleInterface::getPath`
    para obter o caminho do bundle::

        echo $this->container->get('kernel')->getBundle('AcmeDemoBundle')->getPath();

3.1 Remover Assets do Bundle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Remover os assets do bundle no diretório web/ (ex.,
``web/bundles/acmedemo`` para o AcmeDemoBundle).

4. Remover Integração em outros Bundles
---------------------------------------

.. note::

    Isto não se aplica ao AcmeDemoBundle - não há outros bundles que dependem
    dele, de modo que você pode pular esta etapa.

Alguns bundles dependem de outros bundles, e, se você remover um dos dois, o outro
provavelmente não vai funcionar. Certifique-se de que nenhum outro bundle, terceiros ou self-made,
dependam do bundle que você está prestes a remover.

.. tip::

    Se um bundle depende de um outro, na maioria dos casos, significa que ele usa
    alguns serviços do bundle. Pesquisando pelo alias do bundle pode
    ajudá-lo a identificá-los (ex., ``acme_demo`` para bundles que dependem do AcmeDemoBundle).

.. tip::

    Se um bundle de terceiro depende de um outro bundle, você pode encontrar esse bundle
    mencionado no arquivo ``composer.json`` incluído no diretório do bundle.
