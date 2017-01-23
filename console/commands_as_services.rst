.. index::
    single: Console; Comandos como Serviços

Como Definir Comandos como Serviços
===================================

.. versionadded:: 2.4
   O suporte para registrar comandos no contêiner de serviço foi introduzido no
   Symfony 2.4.

Por padrão, o Symfony vai procurar no diretório ``Command`` de cada
bundle e registar automaticamente os seus comandos. Se um comando estende a
:class:`Symfony\\Bundle\\FrameworkBundle\\Command\\ContainerAwareCommand`,
o Symfony ainda vai injetar o contêiner.
Enquanto torna a vida mais fácil, isso tem algumas limitações:

* O seu comando deve residir no diretório ``Command``;
* Não há nenhuma forma de registrar condicionalmente seu serviço com base no ambiente
  ou na disponibilidade de algumas dependências;
* Você não pode acessar o contêiner no método ``configure()`` (porque
  ``setContainer`` não foi chamado ainda);
* Você não pode usar a mesma classe para criar muitos comandos (ou seja, cada um com
  configuração diferente).

Para resolver esses problemas, você pode registrar seu comando como um serviço e adicionar a tag
``console.command``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            acme_hello.command.my_command:
                class: Acme\HelloBundle\Command\MyCommand
                tags:
                    -  { name: console.command }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="acme_hello.command.my_command"
                    class="Acme\HelloBundle\Command\MyCommand">
                    <tag name="console.command" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container
            ->register('acme_hello.command.my_command', 'Acme\HelloBundle\Command\MyCommand')
            ->addTag('console.command')
        ;

Usando Dependências e Parâmetros para Definir Valores Padrão para as Opções
---------------------------------------------------------------------------

Imagine que você deseja fornecer um valor padrão para a opção ``name``. Você poderia
passar uma das seguintes opções como o quinto argumento de ``addOption()``:

* Uma string no próprio código;
* Um parâmetro de contêiner (ex., algo do parameters.yml);
* Um valor calculado por um serviço (ex., um repositório).

Ao estender ``ContainerAwareCommand``, apenas a primeira opção é possível, porque você
não pode acessar o contêiner dentro do método ``configure()``. Em vez disso, injete
qualquer parâmetro ou serviço que você precisa no construtor. Por exemplo, suponha que você
tem algum serviço ``NameRepository`` que vai usar para obter seu valor padrão::

    // src/Acme/DemoBundle/Command/GreetCommand.php
    namespace Acme\DemoBundle\Command;

    use Acme\DemoBundle\Entity\NameRepository;
    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Input\InputOption;
    use Symfony\Component\Console\Output\OutputInterface;

    class GreetCommand extends Command
    {
        protected $nameRepository;

        public function __construct(NameRepository $nameRepository)
        {
            $this->nameRepository = $nameRepository;
            
            parent::__construct();
        }

        protected function configure()
        {
            $defaultName = $this->nameRepository->findLastOne();

            $this
                ->setName('demo:greet')
                ->setDescription('Greet someone')
                ->addOption('name', '-n', InputOption::VALUE_REQUIRED, 'Who do you want to greet?', $defaultName)
            ;
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $name = $input->getOption('name');

            $output->writeln($name);
        }
    }

Agora, basta atualizar os argumentos da sua configuração do serviço como normal para
injetar o ``NameRepository``. Ótimo, agora você tem um valor padrão dinâmico!

.. caution::

    Tenha cuidado para não fazer qualquer trabalho em ``configure`` (ex., fazer consultas de banco de dados
    ), pois seu código será executado, mesmo se você estiver usando o console para
    executar um comando diferente.
