.. index::
   single: Console; Criar comandos

Comandos de Console
===================

O framework Symfony disponibiliza muitos comandos através do script ``bin/console``
(por exemplo, o comando bem conhecido ``bin/console cache:clear``). Esses comandos são
criados com o :doc:`componente Console </components/console>`. Você também pode
usá-lo para criar os seus próprios comandos.

Criar o comando
---------------

Os comandos são definidos nas classes que devem ser criadas no namespace ``Command``
de seu bundle (por exemplo, ``AppBundle\Command``) e os seus nomes devem terminar com
sufixo ``Command``.

Por exemplo, um comando chamado ``CreateUser`` deve seguir essa estrutura::

    // src/AppBundle/Command/CreateUserCommand.php
    namespace AppBundle\Command;

    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class CreateUserCommand extends Command
    {
        protected function configure()
        {
            // ...
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            // ...
        }
    }

Configurar o comando
--------------------

Antes de tudo, você deve configurar o nome do comando no método
``configure()``. Então você pode, opcionalmente, definir uma mensagem de ajuda
e as :doc:`opções e argumentos de entrada </console/input>`::

    // ...
    protected function configure()
    {
        $this
            // the name of the command (the part after "bin/console")
            ->setName('app:create-users')

            // the short description shown while running "php bin/console list"
            ->setDescription('Creates new users.')

            // the full command description shown when running the command with
            // the "--help" option
            ->setHelp("This command allows you to create users...")
        ;
    }

Executar o comando
------------------

Após configurar o comando, você pode executá-lo no terminal:

.. code-block:: terminal

    $ php bin/console app:create-users

Como esperado, esse comando não fará nada pois você não escreveu nenhuma lógica
ainda. Adicione a sua própria lógica dentro do método ``execute()``, que tem acesso ao
fluxo de entrada (por exemplo, opções e argumentos) e ao fluxo de saída (para escrever
mensagens no console)::

    // ...
    protected function execute(InputInterface $input, OutputInterface $output)
    {
        // outputs multiple lines to the console (adding "\n" at the end of each line)
        $output->writeln([
            'User Creator',
            '============',
            '',
        ]);

        // outputs a message followed by a "\n"
        $output->writeln('Whoa!');

        // outputs a message without adding a "\n" at the end of the line
        $output->write('You are about to ');
        $output->write('create a user.');
    }

Agora, tente executar o comando:

.. code-block:: terminal

    $ php bin/console app:create-user
    User Creator
    ============

    Whoa!
    You are about to create a user.

Entrada de Console
------------------

Use opções de entrada ou argumentos para passar informações ao comando::

    use Symfony \ Component \ Console \ Input \ InputArgument;

    use Symfony\Component\Console\Input\InputArgument;

    // ...
    protected function configure()
    {
        $this
            // configure an argument
            ->addArgument('username', InputArgument::REQUIRED, 'The username of the user.')
            // ...
        ;
    }

    // ...
    public function execute(InputInterface $input, OutputInterface $output)
    {
        $output->writeln([
            'User Creator',
            '============',
            '',
        ]);

        // retrieve the argument value using getArgument()
        $output->writeln('Username: '.$input->getArgument('username'));
    }

Agora, você pode passar o nome de usuário para o comando:

.. code-block:: terminal

    $ php bin/console app:create-user Wouter
    User Creator
    ============

    Username: Wouter

.. seealso::

    Leia :doc:`/console/input` para obter mais informações sobre as opções e argumentos
    de console.

Obter Serviços do Container de Serviços
---------------------------------------

Para realmente criar um novo usuário, o comando tem que acessar alguns
:doc:`serviços </service_container>`. Isso pode ser realizado fazendo o comando
estender a :class:`Symfony\\Bundle\\FrameworkBundle\\Command\\ContainerAwareCommand`
::

    // ...
    use Symfony\Bundle\FrameworkBundle\Command\ContainerAwareCommand;

    class CreateUserCommand extends ContainerAwareCommand
    {
        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            // ...

            // access the container using getContainer()
            $userManager = $this->getContainer()->get('app.user_manager');
            $userManager->create($input->getArgument('username'));

            $output->writeln('User successfully generated!');
        }
    }

Agora, depois de ter criado os serviços e a lógica necessária, o comando irá executar
o método ``create()`` do serviço ``app.user_manager`` e o usuário
será criado.

Ciclo de Vida do Comando
------------------------

Os comandos possuem três métodos de ciclo de vida que são invocados durante a execução do
do mesmo:

:method:`Symfony\\Component\\Console\\Command\\Command::initialize` *(opcional)*
    Esse método é executado antes dos métodos ``interact()`` e ``execute()``
    . Sua principal finalidade é inicializar variáveis ​​usadas pelos outros
    métodos do comando.

:method:`Symfony\\Component\\Console\\Command\\Command::interact` *(opcional)*
    Esse método é executado após o ``initialize()`` e antes do ``execute()``.
    Sua finalidade é verificar se estão faltando algumas das opções/argumentos 
    e interativamente pedir ao usuário esses valores. Esse é o último lugar
    onde você pode pedir pelas opções/argumentos que estiverem faltando. Após esse
    comando, as opções/argumentos que estiverem faltando irão resultar em um erro.

:method:`Symfony\\Component\\Console\\Command\\Command::execute` *(obrigatório)*
    Esse método é executado após o ``interact()`` e ``initialize()``.
    Ele contém a lógica que você deseja que o comando execute.

.. _console-testing-commands:

Testando os Comandos
--------------------

O Symfony oferece várias ferramentas para ajudar você a testar seus comandos. A mais
útil é :class:`Symfony\\Component\\Console\\Tester\\CommandTester`
. Ela usa as classes de entrada e saída especiais para facilitar o teste sem um
console real::

    // tests/AppBundle/Command/CreateUserCommandTest.php
    namespace Tests\AppBundle\Command;

    use AppBundle\Command\CreateUserCommand;
    use Symfony\Bundle\FrameworkBundle\Console\Application;
    use Symfony\Bundle\FrameworkBundle\Test\KernelTestCase;
    use Symfony\Component\Console\Tester\CommandTester;

    class CreateUserCommandTest extends KernelTestCase
    {
        public function testExecute()
        {
            self::bootKernel();
            $application = new Application(self::$kernel);

            $application->add(new CreateUserCommand());

            $command = $application->find('app:create-user');
            $commandTester = new CommandTester($command);
            $commandTester->execute(array(
                'command'  => $command->getName(),

                // pass arguments to the helper
                'username' => 'Wouter',

                // prefix the key with a double slash when passing options,
                // e.g: '--some-option' => 'option_value',
            ));

            // the output of the command in the console
            $output = $commandTester->getDisplay();
            $this->assertContains('Username: Wouter', $output);

            // ...
        }
    }

.. tip::

    Você também pode testar uma aplicação inteira de console usando
    :class:`Symfony\\Component\\Console\\Tester\\ApplicationTester`.

.. note::

    Ao usar o componente Console em um projeto independente, use
    :class:`Symfony\\Component\\Console\\Application <Symfony\\Component\\Console\\Application>`
    e estenda o normal ``\PHPUnit_Framework_TestCase``.

Para poder utilizar o contêiner de serviço totalmente configurado para seus testes de console
você pode estender o seu teste de
:class:`Symfony\\Bundle\\FrameworkBundle\\Test\\KernelTestCase`::

    // ...
    use Symfony\Component\Console\Tester\CommandTester;
    use Symfony\Bundle\FrameworkBundle\Console\Application;
    use Symfony\Bundle\FrameworkBundle\Test\KernelTestCase;

    class CreateUserCommandTest extends KernelTestCase
    {
        public function testExecute()
        {
            $kernel = $this->createKernel();
            $kernel->boot();

            $application = new Application($kernel);
            $application->add(new CreateUserCommand());

            $command = $application->find('app:create-user');
            $commandTester = new CommandTester($command);
            $commandTester->execute(array(
                'command'  => $command->getName(),
                'username' => 'Wouter',
            ));

            $output = $commandTester->getDisplay();
            $this->assertContains('Username: Wouter', $output);

            // ...
        }
    }

Saiba mais
----------

.. toctree::
    :maxdepth: 1
    :glob:

    console/*

O componente Console também contém um conjunto de "helpers" - pequenas ferramentas
capazes de ajudá-lo com tarefas diferentes:

* :doc:`/components/console/helpers/questionhelper`:  interativamente pedir informações ao usuário
* :doc:`/components/console/helpers/formatterhelper`: personalizar a colorização de saída
* :doc:`/components/console/helpers/progressbar`: mostrar uma barra de progresso
* :doc:`/components/console/helpers/table`: exibir dados tabulares como uma tabela
