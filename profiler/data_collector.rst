.. index::
   single: Profiling; Coletor de Dados

Como criar um Coletor de Dados personalizado
============================================

O :ref:`Profiler <internals-profiler>` do Symfony2 delega a coleta de dados para os
coletores de dados. O Symfony2 vem com alguns deles, mas você pode criar o seu próprio
facilmente.

Criando um Coletor de Dados Personalizado
-----------------------------------------

Criar um coletor de dados personalizado é tão simples quanto implementar o
:class:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollectorInterface`::

    interface DataCollectorInterface
    {
        /**
         * Collects data for the given Request and Response.
         *
         * @param Request    $request   A Request instance
         * @param Response   $response  A Response instance
         * @param \Exception $exception An Exception instance
         */
        function collect(Request $request, Response $response, \Exception $exception = null);

        /**
         * Returns the name of the collector.
         *
         * @return string The collector name
         */
        function getName();
    }

O método ``getName()`` deve retornar um nome único. Ele é usado para acessar a
informação mais tarde (veja o :doc:`/cookbook/testing/profiling` por
exemplo).

O método ``collect()`` é responsável por armazenar os dados que ele quer fornecer
acesso nas propriedades locais.

.. caution::

    Como o profiler serializa instâncias do coletor de dados, você não deve
    armazenar objetos que não podem ser serializados (como objetos PDO), ou você precisa
    fornecer o seu próprio método ``serialize()``.

Na maioria das vezes, é conveniente estender o
:class:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollector` e
popular a propriedade ``$this->data`` (que cuida da serialização da
propriedade ``$this->data``)::

    class MemoryDataCollector extends DataCollector
    {
        public function collect(Request $request, Response $response, \Exception $exception = null)
        {
            $this->data = array(
                'memory' => memory_get_peak_usage(true),
            );
        }

        public function getMemory()
        {
            return $this->data['memory'];
        }

        public function getName()
        {
            return 'memory';
        }
    }

.. _data_collector_tag:

Ativando Coletores de Dados Personalizados
------------------------------------------

Para ativar um coletor de dados, adicione-o como um serviço regular, em uma de suas
configurações, e adicione a tag ``data_collector`` a ele:

.. configuration-block::

    .. code-block:: yaml

        services:
            data_collector.your_collector_name:
                class: Fully\Qualified\Collector\Class\Name
                tags:
                    - { name: data_collector }

    .. code-block:: xml

        <service id="data_collector.your_collector_name" class="Fully\Qualified\Collector\Class\Name">
            <tag name="data_collector" />
        </service>

    .. code-block:: php

        $container
            ->register('data_collector.your_collector_name', 'Fully\Qualified\Collector\Class\Name')
            ->addTag('data_collector')
        ;

Adição de Templates no Profiler Web
-----------------------------------

Quando você quiser exibir os dados coletados pelo seu Coletor de Dados na barra de ferramentas
para debug web ou no profiler web, adicione um template Twig seguindo este
esqueleto:

.. code-block:: jinja

    {% extends 'WebProfilerBundle:Profiler:layout.html.twig' %}

    {% block toolbar %}
        {# the web debug toolbar content #}
    {% endblock %}

    {% block head %}
        {# if the web profiler panel needs some specific JS or CSS files #}
    {% endblock %}

    {% block menu %}
        {# the menu content #}
    {% endblock %}

    {% block panel %}
        {# the panel content #}
    {% endblock %}

Cada bloco é opcional. O bloco ``toolbar`` é usado para a barra de ferramentas para debug web
e o ``menu`` e ``panel`` são usados ​​para adicionar um painel no
profiler web.

Todos os blocos têm acesso ao objeto ``collector``.

.. tip::

    Templates integrados usam uma imagem codificada em base64 para a barra de ferramentas (``<img
    src="data:image/png;base64,..."``). Você pode facilmente calcular o
    valor base64 para uma imagem com este pequeno script: ``echo
    base64_encode(file_get_contents($_SERVER['argv'][1]));``.

Para ativar o template, adicione um atributo ``template`` para a tag ``data_collector``
em sua configuração. Por exemplo, assumindo que o seu template está em algum
``AcmeDebugBundle``:

.. configuration-block::

    .. code-block:: yaml

        services:
            data_collector.your_collector_name:
                class: Acme\DebugBundle\Collector\Class\Name
                tags:
                    - { name: data_collector, template: "AcmeDebugBundle:Collector:templatename", id: "your_collector_name" }

    .. code-block:: xml

        <service id="data_collector.your_collector_name" class="Acme\DebugBundle\Collector\Class\Name">
            <tag name="data_collector" template="AcmeDebugBundle:Collector:templatename" id="your_collector_name" />
        </service>

    .. code-block:: php

        $container
            ->register('data_collector.your_collector_name', 'Acme\DebugBundle\Collector\Class\Name')
            ->addTag('data_collector', array(
                'template' => 'AcmeDebugBundle:Collector:templatename',
                'id'       => 'your_collector_name',
            ))
        ;
