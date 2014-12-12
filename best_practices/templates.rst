Templates
=========

Quando o PHP foi criado, há 20 anos atrás, os desenvolvedores amavam sua simplicidade e como
ele misturava bem código HTML e dinâmico. Mas, à medida que o tempo passava, outras linguagens
de template - como o `Twig`_ - foram criadas para tornar os templates ainda melhores.

.. best-practice::

    Use o formato Twig para seus templates.

De um modo geral, os templates PHP são muito mais verbosos do que no Twig porque
eles não têm suporte nativo para muitas características modernas necessárias nos templates,
como herança, escaping automático e argumentos nomeados para filtros e
funções.

Twig é o formato de template padrão no Symfony e, de todas as engines de template não-PHP,
tem o maior apoio da comunidade (ele é usado em projetos de grande destaque
como o Drupal 8).

Além disso, Twig é o único formato de template com suporte garantido no Symfony
3.0. Na verdade, o PHP pode ser removido do suporte oficial
das engines de template.

Localizações de Template
------------------------

.. best-practice::

    Armazene todos os templates da sua aplicação no diretório ``app/Resources/views/``.

Tradicionalmente, os desenvolvedores Symfony armazenam os templates da aplicação no
diretório ``Resources/views/`` de cada bundle. Então, usam o nome lógico
para referenciá-los (por exemplo, ``AcmeDemoBundle:Default:index.html.twig``).

Mas, para os templates usados ​​na sua aplicação, é muito mais conveniente
armazená-los no diretório ``app/Resources/views/``. Para quem está iniciando, isso
simplifica drasticamente os nomes lógicos:

=================================================  ==================================
Templates Armazenados dentro de Bundles            Templates Armazenado em ``app/``
=================================================  ==================================
``AcmeDemoBundle:Default:index.html.twig``         ``default/index.html.twig``
``::layout.html.twig``                             ``layout.html.twig``
``AcmeDemoBundle::index.html.twig``                ``index.html.twig``
``AcmeDemoBundle:Default:subdir/index.html.twig``  ``default/subdir/index.html.twig``
``AcmeDemoBundle:Default/subdir:index.html.twig``  ``default/subdir/index.html.twig``
=================================================  ==================================

Outra vantagem é que, centralizar os seus templates simplifica o trabalho
dos seus designers. Eles não precisam procurar por templates em vários diretórios
dispersos através de vários bundles.

Extensões Twig
--------------

.. best-practice::

    Defina suas extensões Twig no diretório ``AppBundle/Twig/`` e
    configure-as usando o arquivo ``app/config/services.yml``.

Nossa aplicação precisa de um filtro personalizado ``md2html`` do Twig para que possamos transformar
conteúdos Markdown de cada post em HTML.

Para fazer isso, primeiro instale o excelente parser Markdown `Parsedown`_ como
uma nova dependência do projeto:

.. code-block:: bash

    $ composer require erusev/parsedown

Em seguida, crie um novo serviço ``Markdown`` que será usado mais tarde pela extensão do
Twig. A definição de serviço requer apenas o caminho para a classe:

.. code-block:: yaml

    # app/config/services.yml
    services:
        # ...
        markdown:
            class: AppBundle\Utils\Markdown

E a classe ``Markdown`` só precisa definir um único método para transformar
o conteúdo Markdown em HTML::

    namespace AppBundle\Utils;

    class Markdown
    {
        private $parser;

        public function __construct()
        {
            $this->parser = new \Parsedown();
        }

        public function toHtml($text)
        {
            $html = $this->parser->text($text);

            return $html;
        }
    }

Em seguida, crie uma nova extensão Twig e defina um novo filtro chamado ``md2html``
usando a classe ``Twig_SimpleFilter``. Injete o serviço ``markdown`` recém-definido
no construtor da extensão Twig:

.. code-block:: php

    namespace AppBundle\Twig;

    use AppBundle\Utils\Markdown;

    class AppExtension extends \Twig_Extension
    {
        private $parser;

        public function __construct(Markdown $parser)
        {
            $this->parser = $parser;
        }

        public function getFilters()
        {
            return array(
                new \Twig_SimpleFilter(
                    'md2html',
                    array($this, 'markdownToHtml'),
                    array('is_safe' => array('html'))
                ),
            );
        }

        public function markdownToHtml($content)
        {
            return $this->parser->toHtml($content);
        }

        public function getName()
        {
            return 'app_extension';
        }
    }

Por fim, defina um novo serviço para habilitar essa extensão Twig na aplicação (o
nome do serviço é irrelevante, porque você nunca irá usá-lo em seu próprio código):

.. code-block:: yaml

    # app/config/services.yml
    services:
        app.twig.app_extension:
            class:     AppBundle\Twig\AppExtension
            arguments: ["@markdown"]
            tags:
                - { name: twig.extension }

.. _`Twig`: http://twig.sensiolabs.org/
.. _`Parsedown`: http://parsedown.org/
.. _`variáveis globais do Twig`: http://symfony.com/doc/master/cookbook/templating/global_variables.html
.. _`sobrescrever páginas de erro`: http://symfony.com/doc/current/cookbook/controller/error_pages.html
.. _`renderizar um template sem usar um controlador`: http://symfony.com/doc/current/cookbook/templating/render_without_controller.html
