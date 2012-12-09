.. index::
   single: Console; Criar comandos

Como criar um Comando de Console
================================

A página Console na seção Componentes (:doc:`/components/console/introduction`) aborda
como criar um comando de Console. Este artigo do cookbook abrange as diferenças
ao criar comandos do Console com o framework Symfony2.

Registrando os Comandos Automaticamente
---------------------------------------

Para disponibilizar os comandos de console automaticamente com o Symfony2, adicione um
diretório ``Command`` dentro de seu bundle e crie um arquivo php com o sufixo
``Command.php`` para cada comando que você deseja fornecer. Por exemplo, se você
deseja estender o ``AcmeDemoBundle`` (disponível na Edição Standard do Symfony)
para cumprimentar-nos a partir da linha de comando, crie o ``GreetCommand.php``
e adicione o seguinte código::

    // src/Acme/DemoBundle/Command/GreetCommand.php
    namespace Acme\DemoBundle\Command;

    use Symfony\Bundle\FrameworkBundle\Command\ContainerAwareCommand;
    use Symfony\Component\Console\Input\InputArgument;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Input\InputOption;
    use Symfony\Component\Console\Output\OutputInterface;

    class GreetCommand extends ContainerAwareCommand
    {
        protected function configure()
        {
            $this
                ->setName('demo:greet')
                ->setDescription('Greet someone')
                ->addArgument('name', InputArgument::OPTIONAL, 'Who do you want to greet?')
                ->addOption('yell', null, InputOption::VALUE_NONE, 'If set, the task will yell in uppercase letters')
            ;
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $name = $input->getArgument('name');
            if ($name) {
                $text = 'Hello '.$name;
            } else {
                $text = 'Hello';
            }

            if ($input->getOption('yell')) {
                $text = strtoupper($text);
            }

            $output->writeln($text);
        }
    }

Este comando estará disponível automaticamente para executar:

.. code-block:: bash

    $ app/console demo:greet Fabien

Testando Comandos
-----------------

Ao testar os comandos utilizados como parte do framework completo :class:`Symfony\\Bundle\\FrameworkBundle\\Console\\Application`
deve ser usado em vez de :class:`Symfony\\Component\\Console\\Application`::

    use Symfony\Component\Console\Tester\CommandTester;
    use Symfony\Bundle\FrameworkBundle\Console\Application;
    use Acme\DemoBundle\Command\GreetCommand;

    class ListCommandTest extends \PHPUnit_Framework_TestCase
    {
        public function testExecute()
        {
            // mock the Kernel or create one depending on your needs
            $application = new Application($kernel);
            $application->add(new GreetCommand());

            $command = $application->find('demo:greet');
            $commandTester = new CommandTester($command);
            $commandTester->execute(array('command' => $command->getName()));

            $this->assertRegExp('/.../', $commandTester->getDisplay());

            // ...
        }
    }

Obtendo Serviços do Container de Serviços
-----------------------------------------

Usando :class:`Symfony\\Bundle\\FrameworkBundle\\Command\\ContainerAwareCommand` 
como classe base para o comando (em vez do mais básico 
:class:`Symfony\\Component\\Console\\Command\\Command`), você tem acesso ao 
container de serviço. Em outras palavras, você tem acesso a qualquer serviço configurado.
Por exemplo, você pode facilmente estender a tarefa para a mesma ser traduzível::

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        $name = $input->getArgument('name');
        $translator = $this->getContainer()->get('translator');
        if ($name) {
            $output->writeln($translator->trans('Hello %name%!', array('%name%' => $name)));
        } else {
            $output->writeln($translator->trans('Hello!'));
        }
    }
