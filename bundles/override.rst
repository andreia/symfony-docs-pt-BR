.. index::
   single: Bundle; Herança

Como Sobrescrever qualquer parte de um Bundle
=============================================

Este documento é uma referência rápida sobre como sobrescrever diferentes partes
de bundles de terceiros.

Templates
---------

Para obter informações sobre como sobrescrever templates, consulte
* :ref:`overriding-bundle-templates`.
* :doc:`/cookbook/bundles/inheritance`

Roteamento
----------

O roteamento nunca é automaticamente importado no Symfony2. Se você quiser incluir
as rotas de qualquer bundle, elas devem ser manualmente importadas em algum lugar
na sua aplicação (ex.: ``app/config/routing.yml``).

A maneira mais fácil para "sobrescrever" o roteamento de um bundle é nunca importá-lo 
. Em vez de importar o roteamento de um bundle de terceiros, simplesmente copie
o arquivo de roteamento em sua aplicação, modifique-o e importe-o no lugar.

Controladores
-------------

Assumindo que o bundle de terceiro envolvido usa controladores não-serviços (que
é quase sempre o caso), você pode facilmente sobrescrever os controladores através de herança
do bundle: Para mais informações, consulte :doc:`/cookbook/bundles/inheritance`.

Serviços e Configuração
-----------------------

Existem duas opções para sobrescrever/estender um serviço. Primeiro, você pode
definir o parâmetro que contêm o nome do serviço da classe para a sua própria classe, definindo
ele em ``app/config/config.yml``. Isto, naturalmente, só é possível se o nome da classe está
definido como um parâmetro na configuração de serviço do bundle que contém o
serviço. Por exemplo, para sobrescrever a classe usada pelo serviço ``translator`` do 
Symfony, você poderia sobrescrever o parâmetro ``translator.class``. Para saber exatamente
qual parâmetro deve-se sobrescrever, poderá ser necessária alguma pesquisa. Para o tradutor, o
parâmetro é definido e usado no arquivo ``Resources/config/translation.xml``
do FrameworkBundle:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            translator.class:      Acme\HelloBundle\Translation\Translator

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="translator.class">Acme\HelloBundle\Translation\Translator</parameter>
        </parameters>

    .. code-block:: php

        // app/config/config.php
        $container->setParameter('translator.class', 'Acme\HelloBundle\Translation\Translator');

Em segundo lugar, se a classe não está disponível como um parâmetro, você quer ter a certeza que a
classe será sempre sobrescrita quando seu bundle for utilizado, ou quando você precisa modificar
algo além do nome da classe, você deve usar um compiler pass::

    // src/Acme/FooBundle/DependencyInjection/Compiler/OverrideServiceCompilerPass.php
    namespace Acme\DemoBundle\DependencyInjection\Compiler;

    use Symfony\Component\DependencyInjection\Compiler\CompilerPassInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;

    class OverrideServiceCompilerPass implements CompilerPassInterface
    {
        public function process(ContainerBuilder $container)
        {
            $definition = $container->getDefinition('original-service-id');
            $definition->setClass('Acme\DemoBundle\YourService');
        }
    }

Neste exemplo, buscamos a definição de serviço do serviço original, e definimos
seu nome de classe para a nossa própria classe.

Veja :doc:`/cookbook/service_container/compiler_passes` para obter informações sobre como usar
compiler passes. Se você quer fazer algo além de apenas sobrescrever a classe -
como adicionar uma chamada de método - você só pode usar o método compiler pass.

Entidades e Mapeamento de Entidade
----------------------------------

Em andamento...

Formulários
-----------

A fim de sobrescrever um tipo de formulário (form type), ele tem que ser registrado como um serviço (o que significa
que tem a tag definida como "form.type"). Você pode, então, sobrescrevê-lo como faria com qualquer
serviço, como foi explicado em `Serviços e Configuração`_. Isto, é claro, somente
funcionará se o tipo é referido por seu alias, em vez de ser instanciado,
ex.::

    $builder->add('name', 'custom_type');

em vez de::

    $builder->add('name', new CustomType());

Validação de Metadados
----------------------

Em andamento..

Traduções
---------

Em andamento...
