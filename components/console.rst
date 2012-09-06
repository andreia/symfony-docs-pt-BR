.. index::
    single: Console; CLI
    single: Componentes; Console


O Componente Console
====================

    O componente Console facilita a criação de interfaces de linha de comando bonitas 
    e testáveis.

O componente Console permite a criação dos seus próprios comandos de linha de comando. 
Seus comandos do console podem ser usados ​​para qualquer tarefa recorrente, como cronjobs, 
importações ou outros trabalhos realizados em lote.

Instalação
----------

Você pode instalar o componente de várias formas diferentes:

* Usando o repositório Git oficial (https://github.com/symfony/Console);
* Instalando via PEAR (`pear.symfony.com/Console`);
* Instalando via Composer (`symfony/console` on Packagist).

Criando um comando básico
-------------------------

Para fazer um comando de console que nos cumprimenta na linha de comando, crie o ``GreetCommand.php``
e adicione o seguinte código nele::

    namespace Acme\DemoBundle\Command;

    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputArgument;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Input\InputOption;
    use Symfony\Component\Console\Output\OutputInterface;

    class GreetCommand extends Command
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

Você também precisa criar o arquivo para ser executado na linha de comando que cria
uma ``Application`` e adicionar comandos à ele::

    #!/usr/bin/env php
    # app/console
    <?php 

    use Acme\DemoBundle\Command\GreetCommand;
    use Symfony\Component\Console\Application;

    $application = new Application();
    $application->add(new GreetCommand);
    $application->run();

Teste o novo comando de console executando o seguinte

.. code-block:: bash

    $ app/console demo:greet Fabien

Isto irá imprimir o seguinte na linha de comando:

.. code-block:: text

    Hello Fabien

Você também pode usar a opção ``--yell`` para converter tudo em letras maiúsculas:

.. code-block:: bash

    $ app/console demo:greet Fabien --yell

Irá mostrar::

    HELLO FABIEN

Colorindo a saída
~~~~~~~~~~~~~~~~~

Sempre que a saída for um texto, você pode envolvê-lo com tags para colorir a sua
saída. Por exemplo::

    // green text
    $output->writeln('<info>foo</info>');

    // yellow text
    $output->writeln('<comment>foo</comment>');

    // black text on a cyan background
    $output->writeln('<question>foo</question>');

    // white text on a red background
    $output->writeln('<error>foo</error>');

É possível definir os seus próprios estilos usando a classe
:class:`Symfony\\Component\\Console\\Formatter\\OutputFormatterStyle`::

    $style = new OutputFormatterStyle('red', 'yellow', array('bold', 'blink'));
    $output->getFormatter()->setStyle('fire', $style);
    $output->writeln('<fire>foo</fire>');

As cores de primeiro plano e de fundo disponíveis são: ``black``, ``red``, ``green``,
``yellow``, ``blue``, ``magenta``, ``cyan`` e ``white``.

E as opções disponíveis são: ``bold``, ``underscore``, ``blink``, ``reverse`` e ``conceal``.

Usando argumentos de comando
----------------------------

A parte mais interessante dos comandos são os argumentos e opções que você pode 
disponibilizar. Argumentos são as strings - separados por espaços - que
vem após o nome do comando. Eles são ordenados, e podem ser opcionais ou 
obrigatórios. Por exemplo, adicione um argumento opcional ``last_name`` ao comando
e torne o argumento ``name`` obrigatório::

    $this
        // ...
        ->addArgument('name', InputArgument::REQUIRED, 'Who do you want to greet?')
        ->addArgument('last_name', InputArgument::OPTIONAL, 'Your last name?');

Agora, você tem acesso a um argumento ``last_name`` no seu comando::

    if ($lastName = $input->getArgument('last_name')) {
        $text .= ' '.$lastName;
    }

O comando pode agora ser usado em qualquer uma das seguintes formas:

.. code-block:: bash

    $ app/console demo:greet Fabien
    $ app/console demo:greet Fabien Potencier

Utilizando opções de comando
----------------------------

Ao contrário dos argumentos, as opções não são ordenadas (ou seja, você pode especificá-las 
em qualquer ordem) e são especificadas com dois traços (ex., ``--yell`` - você também pode
declarar um atalho de uma letra que pode chamar com um traço único, como
``-y``). As opções são *sempre* opcionais, e podem ser configuradas para aceitar um valor
(ex. ``dir=src``) ou simplesmente como um sinalizador booleano sem um valor (ex.
``yell``).

.. tip::

    Também é possível fazer uma opção *opcionalmente* aceitar um valor (de modo que
    ``--yell`` ou ``yell=loud`` funcionem). Opções também podem ser configuradas para
    aceitar um array de valores.

Por exemplo, adicione uma nova opção ao comando que pode ser usada para especificar
quantas vezes a mensagem deve ser impressa::

    $this
        // ...
        ->addOption('iterations', null, InputOption::VALUE_REQUIRED, 'How many times should the message be printed?', 1);

Em seguida, use o código abaixo no comando para imprimir a mensagem várias vezes:

.. code-block:: php

    for ($i = 0; $i < $input->getOption('iterations'); $i++) {
        $output->writeln($text);
    }

Agora, quando executar a tarefa, você pode, opcionalmente, especificar uma flag
``--iterations``:

.. code-block:: bash

    $ app/console demo:greet Fabien
    $ app/console demo:greet Fabien --iterations=5

O primeiro exemplo irá imprimir apenas uma vez, pois ``iterations`` está vazio e
o padrão é ``1`` (o último argumento de ``addOption``). O segundo exemplo
irá imprimir cinco vezes.

Lembre-se de que as opções não se importam com a ordem. Então, qualquer um dos seguintes 
comandos vai funcionar:

.. code-block:: bash

    $ app/console demo:greet Fabien --iterations=5 --yell
    $ app/console demo:greet Fabien --yell --iterations=5

Existem quatro variantes de opções que você pode usar:

===========================  =====================================================================================
Opção                        Valor
===========================  =====================================================================================
InputOption::VALUE_IS_ARRAY  Esta opção aceita vários valores (ex. ``--dir=/foo --dir=/bar``)
InputOption::VALUE_NONE      Não aceitar a entrada para esta opção (ex. ``--yell``)
InputOption::VALUE_REQUIRED  Este valor é obrigatório (ex. ``--iterations=5``), a opção em si ainda é opcional
InputOption::VALUE_OPTIONAL  Esta opção pode ou não ter um valor (ex. ``yell`` ou ``yell=loud``)
===========================  =====================================================================================

Você pode combinar VALUE_IS_ARRAY com VALUE_REQUIRED ou VALUE_OPTIONAL como abaixo:

.. code-block:: php

    $this
        // ...
        ->addOption('iterations', null, InputOption::VALUE_REQUIRED | InputOption::VALUE_IS_ARRAY, 'How many times should the message be printed?', 1);

Perguntando ao usuário informações
----------------------------------

Ao criar comandos, você tem a possibilidade de coletar mais informações
do usuário, fazendo-lhe perguntas. Por exemplo, suponha que você queira
confirmar uma ação antes de executá-la. Adicione o seguinte ao seu
comando::

    $dialog = $this->getHelperSet()->get('dialog');
    if (!$dialog->askConfirmation($output, '<question>Continue with this action?</question>', false)) {
        return;
    }

Neste caso, o usuário será perguntado "Continue with this action", e, a menos
que ele responda com ``y``, a tarefa não irá executar. O terceiro argumento para
``askConfirmation`` é o valor padrão para retornar se o usuário não informar
nenhuma entrada.

Você também pode fazer perguntas com mais do que uma simples resposta sim/não. Por exemplo, 
se você precisa saber o nome de alguma coisa, você pode fazer o seguinte::

    $dialog = $this->getHelperSet()->get('dialog');
    $name = $dialog->ask($output, 'Please enter the name of the widget', 'foo');

Testando Comandos 
-----------------

O Symfony2 fornece várias ferramentas para ajudar a testar os seus comandos. A mais
útil é a classe :class:`Symfony\\Component\\Console\\Tester\\CommandTester`
. Ela usa classes de entrada e saída especiais para facilitar o teste sem um console 
real::

    use Symfony\Component\Console\Application;
    use Symfony\Component\Console\Tester\CommandTester;
    use Acme\DemoBundle\Command\GreetCommand;

    class ListCommandTest extends \PHPUnit_Framework_TestCase
    {
        public function testExecute()
        {
            $application = new Application();
            $application->add(new GreetCommand());

            $command = $application->find('demo:greet');
            $commandTester = new CommandTester($command);
            $commandTester->execute(array('command' => $command->getName()));

            $this->assertRegExp('/.../', $commandTester->getDisplay());

            // ...
        }
    }

O método :method:`Symfony\\Component\\Console\\Tester\\CommandTester::getDisplay`
retorna o que teria sido exibido durante uma chamada normal ao
console.

Você pode testar o envio de argumentos e opções para o comando, passando-os
como um array para o método :method:`Symfony\\Component\\Console\\Tester\\CommandTester::getDisplay`
::

    use Symfony\Component\Console\Application;
    use Symfony\Component\Console\Tester\CommandTester;
    use Acme\DemoBundle\Command\GreetCommand;

    class ListCommandTest extends \PHPUnit_Framework_TestCase
    {
        // ...

        public function testNameIsOutput()
        {
            $application = new Application();
            $application->add(new GreetCommand());

            $command = $application->find('demo:greet');
            $commandTester = new CommandTester($command);
            $commandTester->execute(
                array('command' => $command->getName(), 'name' => 'Fabien')
            );

            $this->assertRegExp('/Fabien/', $commandTester->getDisplay());
        }
    }

.. tip::

    Você também pode testar uma aplicação de console inteira usando
    :class:`Symfony\\Component\\Console\\Tester\\ApplicationTester`.

Chamando um comando existente
-----------------------------

Se um comando depende que outro seja executado antes dele, em vez de pedir ao
usuário para lembrar a ordem de execução, você mesmo pode chamá-lo diretamente.
Isso também é útil se você quiser criar um comando "meta" que apenas executa 
vários outros comandos (por exemplo, todos os comandos que precisam ser executados 
quando código do projeto mudou nos servidores de produção: limpar o cache,
gerar proxies do Doctrine2, realizar o dump dos assets do Assetic, ...).

Chamar um comando a partir de outro é simples::

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        $command = $this->getApplication()->find('demo:greet');

        $arguments = array(
            'command' => 'demo:greet',
            'name'    => 'Fabien',
            '--yell'  => true,
        );

        $input = new ArrayInput($arguments);
        $returnCode = $command->run($input, $output);

        // ...
    }

Primeiro, você :method:`Symfony\\Component\\Console\\Application::find` o
comando que você deseja executar, passando o nome de comando.

Em seguida, você precisa cria um novo
:class:`Symfony\\Component\\Console\\Input\\ArrayInput` com os argumentos e
opções que você deseja passar para o comando.

Eventualmente, chamando o método ``run()`` efetivamente executa o comando e
retorna o código retornado pelo comando (valor de retorno do método
``execute()`` do comando).

.. note::

    Na maioria das vezes, chamando um comando a partir de código que não é executado 
    na linha de comando não é uma boa idéia por várias razões. Primeiro, a saída do 
    comando é otimizada para o console. Mas, mais importante, você pode pensar
    em um comando como sendo um controlador, que deve usar o modelo para fazer
    algo e exibir informações para o usuário. Assim, em vez de chamar um
    comando pela web, refatore o seu código e mova a lógica para um nova
    classe apropriada.
