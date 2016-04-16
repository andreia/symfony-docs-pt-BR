.. index::
    single: Como o Front Controller, o ``AppKernel`` e os Ambientes
    Funcionam Juntos

Compreendendo como o Front Controller, o Kernel e os Ambientes Funcionam Juntos
===============================================================================

A seção :doc:`/cookbook/configuration/environments` explicou o básico
sobre como o Symfony utiliza ambientes para executar a sua aplicação com diferentes definições de
configuração. Esta seção irá explicar um pouco mais em profundidade o que acontece quando
sua aplicação é inicializada. Para entender esse processo, você precisa entender
três partes que funcionam em conjunto:

* `O Front Controller`_
* `A Classe Kernel`_
* `Os Ambientes`_

.. note::

    Normalmente, você não precisará definir seu próprio front controller ou
    classe ``AppKernel`` pois a `Edição Standard do Symfony`_ fornece
    implementações padrão coerentes.

    Esta seção de documentação é fornecida para explicar o que está acontecendo nos
    bastidores.

O Front Controller
------------------

O `front controller`_ é um padrão de projeto bem conhecido; é uma parte de
código por onde *todas* as requisições servidas por uma aplicação passam.

Na `Edição Standard do Symfony`_, esse papel é feito pelos arquivos `app.php`_
e `app_dev.php`_ que estão no diretório ``web/``. Esses são os primeiros
scripts PHP executados quando uma requisição é processada.

O principal objetivo do front controller é criar uma instância do
``AppKernel`` (mais sobre isso em um segundo), fazê-lo lidar com a requisição
e retornar a resposta resultante para o navegador.

Uma vez que cada requisição é encaminhada através dele, o front controller pode ser
usado para executar a inicialização global, antes de configurar o kernel ou
para `decorar`_ o kernel com recursos adicionais. Os exemplos incluem:

* Configurar o autoloader ou a adição de mecanismos de autoloading adicionais;
* Adicionar cache nível HTTP envolvendo o kernel com uma instância de
  :ref:`AppCache <symfony-gateway-cache>`;
* Ativar (ou pular) a :doc:`ClassCache </cookbook/debugging>`;
* Ativar o :doc:`Componente Debug </components/debug/introduction>`.

O front controller pode ser escolhido ao requisitar as URLs da seguinte forma:

.. code-block:: text

    http://localhost/app_dev.php/some/path/...

Como você pode ver, essa URL contém o script PHP para ser usado como front
controller. Você pode usar isso para mudar facilmente o front controller ou usar
um personalizado colocando-o no diretório ``web/`` (ex. ``app_cache.php``).

Ao usar o Apache e o `RewriteRule que vem com a Edição Standard do Symfony`_,
você pode omitir o nome do arquivo na URL e o RewriteRule irá usar ``app.php``
como padrão.

.. note::

    Praticamente todos os outros servidores web devem ser capazes de obter um
    comportamento semelhante ao do RewriteRule descrito acima.
    Verifique a documentação do seu servidor para obter mais detalhes ou veja
    :doc:`/cookbook/configuration/web_server_configuration`.

.. note::

    Certifique-se de que você protegeu adequadamente seus front controllers contra acesso
    não autorizado. Por exemplo, você não quer fazer ter um ambiente de depuração
    disponível para usuários arbitrários em seu ambiente de produção.

Tecnicamente, o script `bin/console`_ utilizado ao executar o Symfony na linha
de comando também é um front controller, mas apenas não é usado para web, mas sim
para requisições de linha de comando.

A classe Kernel
---------------

A :class:`Symfony\\Component\\HttpKernel\\Kernel` é o núcleo do
Symfony. Ela é responsável por configurar todos os bundles que compõem
sua aplicação e fornecer a eles a configuração da aplicação.
Em seguida, ela cria o container de serviço antes de servir as requisições em seu
método :method:`Symfony\\Component\\HttpKernel\\HttpKernelInterface::handle`
.

Existem dois métodos declarados na
:class:`Symfony\\Component\\HttpKernel\\KernelInterface` que
não foram implementados em :class:`Symfony\\Component\\HttpKernel\\Kernel`
e, assim, servem como `template methods`_:

:method:`Symfony\\Component\\HttpKernel\\KernelInterface::registerBundles`
    Ele deve retornar um array de todos os bundles necessários para executar a aplicação.
:method:`Symfony\\Component\\HttpKernel\\KernelInterface::registerContainerConfiguration`
    Ele carrega a configuração da aplicação.

Para preencher esses (pequenos) espaços em branco, sua aplicação precisa criar uma subclasse do
Kernel e implementar esses métodos. A classe resultante é convencionalmente
chamada de ``AppKernel``.

Mais uma vez, a Edição Standard do Symfony fornece um `AppKernel`_ ​​no diretório
``app/``. Essa classe usa o nome do ambiente - que é passado para
o método :method:`construtor <Symfony\\Component\\HttpKernel\\Kernel::__construct>`
do Kernel e está disponível via :method:`Symfony\\Component\\HttpKernel\\Kernel::getEnvironment` -
para decidir quais bundles criar. A lógica para isso está em ``registerBundles()``,
um método destinado a ser estendido por você quando você começar a adicionar bundles em sua
aplicação.

Você está, é claro, livre para criar suas próprias variantes do ``AppKernel``, alternativas
ou adicionais. Tudo o que você precisa é adaptar seu (ou adicionar um novo) front
controller para usar o novo kernel.

.. note::

    O nome e a localização do ``AppKernel`` não são fixos. Ao
    colocar vários kernels em uma única aplicação,
    pode fazer sentido adicionar sub-diretórios adicionais,
    por exemplo ``app/admin/AdminKernel.php`` e
    ``app/api/ApiKernel.php``. Tudo o que importa é que seu front
    controller seja capaz de criar uma instância do kernel apropriada.

Ter ``AppKernels`` diferentes pode ser útil para habilitar diferentes front
controllers (em potencialmente diferentes servidores) para executar partes da sua aplicação
independentemente (por exemplo, UI administrador, UI front-end e migrações de banco de dados).

.. note::

    Há muito mais no que o ``AppKernel`` pode ser usado, por exemplo
    :doc:`sobrescrever a estrutura de diretório padrão </cookbook/configuration/override_dir_structure>`.
    Mas as probabilidades são altas de que você não necessite mudar coisas como essa em
    tempo real ao ter várias implementações ``AppKernel``.

Os Ambientes
------------

Como já mencionado, o ``AppKernel`` tem que implementar outro método -
:method:`Symfony\\Component\\HttpKernel\\KernelInterface::registerContainerConfiguration`.
Esse método é responsável por carregar a configuração da aplicação
a partir do *ambiente* correto.

Ambientes foram cobertos extensivamente
:doc:`no capítulo anterior </cookbook/configuration/environments>`,
e você provavelmente lembra que a Edição Standard Symfony vem com três
deles - ``dev``, ``prod`` e ``test``.

Mais tecnicamente, esses nomes são nada mais do que strings passadas do
front controller para o construtor do ``AppKernel``. Esse nome pode então ser
utilizado no método :method:`Symfony\\Component\\HttpKernel\\KernelInterface::registerContainerConfiguration`
para decidir quais arquivos de configuração carregar.

A classe `AppKernel`_ ​​da Edição Standard do Symfony implementa esse método ao
simplesmente carregar o arquivo ``app/config/config_*environment*.yml``. Você está, evidentemente,
livre para implementar esse método de forma diferente caso precisar de uma forma mais sofisticada
para carregar a sua configuração.

.. _front controller: https://en.wikipedia.org/wiki/Front_Controller_pattern
.. _Edição Standard do Symfony: https://github.com/symfony/symfony-standard
.. _app.php: https://github.com/symfony/symfony-standard/blob/master/web/app.php
.. _app_dev.php: https://github.com/symfony/symfony-standard/blob/master/web/app_dev.php
.. _bin/console: https://github.com/symfony/symfony-standard/blob/master/bin/console
.. _AppKernel: https://github.com/symfony/symfony-standard/blob/master/app/AppKernel.php
.. _decorate: https://en.wikipedia.org/wiki/Decorator_pattern
.. _RewriteRule que vem com a Edição Standard do Symfony: https://github.com/symfony/symfony-standard/blob/master/web/.htaccess
.. _template methods: https://en.wikipedia.org/wiki/Template_method_pattern
