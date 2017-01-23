.. index::
   single: Extensões Twig

Como Escrever uma Extensão Twig Personalizada
=============================================

A principal motivação para escrever uma extensão é mover código usado frequentemente
em uma classe reutilizável como adicionar suporte para internacionalização.
Uma extensão pode definir tags, filtros, testes, operadores, variáveis ​​globais,
funções e node visitors.

Criar uma extensão também contribui para uma melhor separação do código que é
executado em tempo de compilação e código necessário em tempo de execução. Assim, ela torna
o seu código mais rápido.

.. tip::

    Antes de escrever suas próprias extensões, verifique o
    `repositório de extensão oficial do Twig`_.

Criar a Classe de Extensão
--------------------------

.. note::

    Este cookbook descreve como escrever uma extensão Twig personalizada a partir do
    Twig 1.12. Se você estiver usando uma versão mais antiga, por favor leia a
    `documentação das extensões Twig legada`_.

Para obter a sua funcionalidade personalizada, primeiro você deve criar uma classe de extensão Twig.
Como exemplo, você vai criar um filtro de preço para formatar um determinado número em preço::

    // src/AppBundle/Twig/AppExtension.php
    namespace AppBundle\Twig;

    class AppExtension extends \Twig_Extension
    {
        public function getFilters()
        {
            return array(
                new \Twig_SimpleFilter('price', array($this, 'priceFilter')),
            );
        }

        public function priceFilter($number, $decimals = 0, $decPoint = '.', $thousandsSep = ',')
        {
            $price = number_format($number, $decimals, $decPoint, $thousandsSep);
            $price = '$'.$price;

            return $price;
        }

        public function getName()
        {
            return 'app_extension';
        }
    }

.. tip::

    Junto com os filtros personalizados, você também pode adicionar `funções` personalizadas e registrar
    `variáveis globais`.

Registrar uma Extensão como um Serviço
--------------------------------------

Agora você deve deixar o Container de Serviço saber sobre a sua extensão Twig recém-criada:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.twig_extension:
                class: AppBundle\Twig\AppExtension
                public: false
                tags:
                    - { name: twig.extension }

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <services>
            <service id="app.twig_extension"
                class="AppBundle\Twig\AppExtension"
                public="false">
                <tag name="twig.extension" />
            </service>
        </services>

    .. code-block:: php

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container
            ->register('app.twig_extension', '\AppBundle\Twig\AppExtension')
            ->setPublic(false)
            ->addTag('twig.extension');

Usar a Extensão Personalizada
-----------------------------

Usar sua extensão Twig recém-criada não é diferente de qualquer outra:

.. code-block:: twig

    {# outputs $5,500.00 #}
    {{ '5500'|price }}

Passando outros argumentos para o seu filtro:

.. code-block:: twig

    {# outputs $5500,2516 #}
    {{ '5500.25155'|price(4, ',', '') }}

Aprender mais
-------------

Para aprofundar mais sobre Extensões Twig, por favor verifique a
`documentação de extensões Twig`_.

.. _`repositório de extensão oficial do Twig`: https://github.com/twigphp/Twig-extensions
.. _`documentação de extensões Twig`: http://twig.sensiolabs.org/doc/advanced.html#creating-an-extension
.. _`variáveis globais`: http://twig.sensiolabs.org/doc/advanced.html#id1
.. _`funções`: http://twig.sensiolabs.org/doc/advanced.html#id2
.. _`documentação das extensões Twig legada`: http://twig.sensiolabs.org/doc/advanced_legacy.html#creating-an-extension
