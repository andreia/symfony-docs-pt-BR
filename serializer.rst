.. index::
   single: Serializer

Como usar o Componente Serializer
=================================

Serializar e desserializar de e para objetos e em formatos diferentes (por exemplo,
JSON ou XML) é um tema muito complexo. O Symfony vem com um
:doc:`Componente Serializer</components/serializer>`, que lhe fornece algumas
ferramentas que você pode aproveitar para sua solução.

Na verdade, antes de começar, familiarize-se com o serializer, normalizadores
e codificadores (encoders) lendo :doc:`Componente Serializer</components/serializer>`.
Você também deve verificar o `JMSSerializerBundle`_, que expande as
funcionalidades oferecidas pelo serializer do núcleo do Symfony.

Ativando o Serializer
---------------------

.. versionadded:: 2.3
    O Serializer sempre existiu no Symfony, mas antes do Symfony 2.3,
    você mesmo precisava construir o serviço ``serializer``.

O serviço ``serializer`` não está disponível por padrão. Para ativá-lo, habilite-o
em sua configuração:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            # ...
            serializer:
                enabled: true

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <!-- ... -->
            <framework:serializer enabled="true" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            // ...
            'serializer' => array(
                'enabled' => true
            ),
        ));

Adicionando Normalizadores e Codificadores (Encoders)
-----------------------------------------------------

Uma vez ativado, o serviço ``serializer`` estará disponível no container
e será carregado com dois :ref:`encoders<component-serializer-encoders>`
(:class:`Symfony\\Component\\Serializer\\Encoder\\JsonEncoder` e
:class:`Symfony\\Component\\Serializer\\Encoder\\XmlEncoder`)
mas não :ref:`normalizers<component-serializer-normalizers>`, ou seja, você
precisará carregar o seu próprio.

Você pode carregar normalizadores e/ou codificadores adicionando tags a eles como
:ref:`serializer.normalizer<reference-dic-tags-serializer-normalizer>` e
:ref:`serializer.encoder<reference-dic-tags-serializer-encoder>`. Também é
possível definir a prioridade da tag, a fim de decidir a ordem de correspondência.

Aqui está um exemplo de como carregar o
:class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer`:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            get_set_method_normalizer:
            class: Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer
                tags:
                    - { name: serializer.normalizer }

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <services>
            <service id="get_set_method_normalizer" class="Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer">
                <tag name="serializer.normalizer" />
            </service>
        </services>

    .. code-block:: php

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $definition = new Definition(
            'Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer'
        ));
        $definition->addTag('serializer.normalizer');
        $container->setDefinition('get_set_method_normalizer', $definition);

.. _JMSSerializerBundle: http://jmsyst.com/bundles/JMSSerializerBundle

Aprendendo mais sobre o Serializer
----------------------------------

`ApiPlatform`_ fornece um sistema de API com suporte aos formatos hypermedia `JSON-LD`_ e
`Hydra Core Vocabulary`_. Ele é construído com o Framework Symfony e seu componente
Serializer. Ele fornece normalizadores personalizados e um encoder personalizado, metadata
personalizados e um sistema de cache.

Se você deseja aproveitar todo o poder do componente Serializer do Symfony,
verifique como esse bundle funciona.

.. toctree::
    :maxdepth: 1
    :glob:

    serializer/*

.. _`APCu`: https://github.com/krakjoe/apcu
.. _`ApiPlatform`: https://github.com/api-platform/core
.. _`JSON-LD`: http://json-ld.org
.. _`Hydra Core Vocabulary`: http://hydra-cg.com
