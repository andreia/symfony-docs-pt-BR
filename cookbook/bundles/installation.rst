.. index::
   single: Bundle; Instalação

Como Instalar Bundles de Terceiros
==================================

A maioria dos bundles fornecem suas próprias instruções de instalação. No entanto, os
passos básicos para a instalação de um bundle são os mesmos:

* `A) Adicionar Dependências com o Composer`_
* `B) Habilitar o Bundle`_
* `C) Configurar o Bundle`_

A) Adicionar Dependências com o Composer
----------------------------------------

As dependências são gerenciadas com o Composer, por isso, se o Composer é novo para você, aprenda
algumas noções básicas em `sua documentação`_. Isso tem dois passos:

1) Descobrir o Nome do Bundle no Packagist
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

O arquivo README de um bundle (por exemplo, `FOSUserBundle`_) geralmente vai lhe dizer seu nome
(ex., ``friendsofsymfony/user-bundle``). Caso não informar, você pode procurar
pela biblioteca no site `Packagist.org`_.

.. note::

    Procurando por bundles? Tente pesquisar em `KnpBundles.com`_: o arquivo não-oficial
    de Bundles do Symfony.

2) Instalar o bundle via Composer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Agora que você sabe o nome do bundle, pode instalá-lo via Composer:

.. code-block:: bash

    $ composer require friendsofsymfony/user-bundle

Isso irá escolher a melhor versão para o seu projeto, adicionando-a ao ``composer.json``
e fazendo o download da biblioteca no diretório ``vendor/``. Se você precisa de uma versão específica
, adicione um ``:`` e a versão logo após o nome da biblioteca (ver
`composer require`_).

B) Habilitar o Bundle
---------------------

Neste ponto, o bundle está instalado no seu projeto (em
``vendor/friendsofsymfony/``) e o autoloader reconhece as classes dele.
A única coisa que você precisa fazer agora é registrar o bundle em ``AppKernel``::

    // app/AppKernel.php

    // ...
    class AppKernel extends Kernel
    {
        // ...

        public function registerBundles()
        {
            $bundles = array(
                // ...,
                new FOS\UserBundle\FOSUserBundle(),
            );

            // ...
        }
    }

C) Configurar o Bundle
----------------------

É muito comum um bundle precisar de alguma instalação ou configuração adicional
em ``app/config/config.yml``. A documentação do bundle vai falar sobre
a configuração, mas você também pode obter uma referência de configuração do bundle
através do comando ``config:dump-reference``.

Por exemplo, a fim de olhar a referência de configuração do ``assetic`` você
pode usar isto:

.. code-block:: bash

    $ app/console config:dump-reference AsseticBundle

ou isto:

.. code-block:: bash

    $ app/console config:dump-reference assetic

A saída será parecida com a seguinte:

.. code-block:: text

    assetic:
        debug:                %kernel.debug%
        use_controller:
            enabled:              %kernel.debug%
            profiler:             false
        read_from:            %kernel.root_dir%/../web
        write_to:             %assetic.read_from%
        java:                 /usr/bin/java
        node:                 /usr/local/bin/node
        node_paths:           []
        # ...

Outra Configuração
------------------

Neste ponto, verifique o arquivo ``README`` de seu novo bundle para ver
o que fazer em seguida. Divirta-se!

.. _sua documentação:    http://getcomposer.org/doc/00-intro.md
.. _Packagist.org:       https://packagist.org
.. _FOSUserBundle:       https://github.com/FriendsOfSymfony/FOSUserBundle
.. _KnpBundles.com:      http://knpbundles.com/
.. _`composer require`:  https://getcomposer.org/doc/03-cli.md#require
