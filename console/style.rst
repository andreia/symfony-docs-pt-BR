.. index::
   single: Console; Estilo dos Comandos

Como Adicionar Estilos a um Comando de Console
==============================================

Uma das tarefas mais tediosas ao criar comandos de console é lidar com os
estilos de entrada e saída do comando. Exibir títulos e tabelas ou fazer
perguntas ao usuário envolve uma grande quantidade de código repetitivo.

Considere, por exemplo, o código utilizado para exibir o título do seguinte comando::

    // src/AppBundle/Command/GreetCommand.php
    namespace AppBundle\Command;

    use Symfony\Bundle\FrameworkBundle\Command\ContainerAwareCommand;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class GreetCommand extends ContainerAwareCommand
    {
        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $output->writeln(array(
                '<info>Lorem Ipsum Dolor Sit Amet</>',
                '<info>==========================</>',
                '',
            ));

            // ...
        }
    }

Exibir um título simples requer três linhas de código, para alterar a cor da fonte,
sublinhar o conteúdo e deixar uma linha em branco adicional após o título. Lidar
com estilos é necessário para obter comandos bem projetados, mas isso complica seu código
desnecessariamente.

A fim de reduzir o código repetitivo, os comandos Symfony podem, opcionalmente, utilizar o
**Guia de Estilo do Symfony**. Esses estilos são implementados como um conjunto de métodos auxiliares
(helpers) que permitem criar comandos *semânticos* e não se preocupar com os estilos.

Uso Básico
----------

Em seu comando, instancie a classe :class:`Symfony\\Component\\Console\\Style\\SymfonyStyle`
e passe as variáveis ``$input`` e ``$output`` ​​como seus argumentos. Então,
você pode começar a usar qualquer um dos seus helpers, como ``title()``, que exibe o
título do comando::

    // src/AppBundle/Command/GreetCommand.php
    namespace AppBundle\Command;

    use Symfony\Bundle\FrameworkBundle\Command\ContainerAwareCommand;
    use Symfony\Component\Console\Style\SymfonyStyle;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class GreetCommand extends ContainerAwareCommand
    {
        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $io = new SymfonyStyle($input, $output);
            $io->title('Lorem Ipsum Dolor Sit Amet');

            // ...
        }
    }

Métodos Auxiliares (Helpers)
----------------------------

A classe :class:`Symfony\\Component\\Console\\Style\\SymfonyStyle` define alguns
métodos helper que cobrem as interações mais comumente executadas por comandos de console.

Métodos para Títulos
~~~~~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::title`
    Exibe a string informada como título do comando. Esse método destina-se a
    ser usado apenas uma vez em um determinado comando, mas, nada impede que você use
    ele repetidamente::

        $io->title('Lorem ipsum dolor sit amet');

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::section`
    Exibe a string informada como o título de alguma seção do comando. É necessário
    somente em comandos complexos que desejam separar melhor seus conteúdos::

        $io->section('Adding a User');

        // ...

        $io->section('Generating the Password');

        // ...

Métodos para Conteúdo
~~~~~~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::text`
    Exibe a string ou array de strings informado como texto normal. É útil
    para renderizar mensagens de ajuda e instruções para o usuário executando o
    comando::

        // use simple strings for short messages
        $io->text('Lorem ipsum dolor sit amet');

        // ...

        // consider using arrays when displaying long messages
        $io->text(array(
            'Lorem ipsum dolor sit amet',
            'Consectetur adipiscing elit',
            'Aenean sit amet arcu vitae sem faucibus porta',
        ));

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::listing`
    Exibe uma lista não ordenada de elementos passados ​​como um array::

        $io->listing(array(
            'Element #1 Lorem ipsum dolor sit amet',
            'Element #2 Lorem ipsum dolor sit amet',
            'Element #3 Lorem ipsum dolor sit amet',
        ));

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::table`
    Exibe o array de cabeçalhos e linhas informado como uma tabela compacta::

        $io->table(
            array('Header 1', 'Header 2'),
            array(
                array('Cell 1-1', 'Cell 1-2'),
                array('Cell 2-1', 'Cell 2-2'),
                array('Cell 3-1', 'Cell 3-2'),
            )
        );

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::newLine`
    Exibe uma linha em branco na saída do comando. Embora possa parecer útil,
    na maioria das vezes você não vai precisar dele. A razão é que cada helper
    já adiciona suas próprias linhas em branco, assim você não precisa se preocupar com o
    espaçamento vertical::

        // outputs a single blank line
        $io->newLine();

        // outputs three consecutive blank lines
        $io->newLine(3);

Métodos para Advertência
~~~~~~~~~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::note`
    Exibe a string ou array de strings informado como uma advertência destacada.
    Utilize esse helper com moderação para evitar confusão na saída do comando::

        // use simple strings for short notes
        $io->note('Lorem ipsum dolor sit amet');

        // ...

        // consider using arrays when displaying long notes
        $io->note(array(
            'Lorem ipsum dolor sit amet',
            'Consectetur adipiscing elit',
            'Aenean sit amet arcu vitae sem faucibus porta',
        ));

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::caution`
    Similar ao helper ``note()``, mas o conteúdo é mais proeminente
    . Os conteúdos resultantes assemelham-se a uma mensagem de erro, então você deve
    evitar o uso desse helper, a menos que estritamente necessário::

        // use simple strings for short caution message
        $io->caution('Lorem ipsum dolor sit amet');

        // ...

        // consider using arrays when displaying long caution messages
        $io->caution(array(
            'Lorem ipsum dolor sit amet',
            'Consectetur adipiscing elit',
            'Aenean sit amet arcu vitae sem faucibus porta',
        ));

Métodos para Barra de Progresso
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::progressStart`
    Exibe uma barra de progresso com um número de passos igual ao argumento passado
    para o método (não passe nenhum valor se o tamanho da barra de progresso é
    desconhecido)::

        // displays a progress bar of unknown length
        $io->progressStart();

        // displays a 100-step length progress bar
        $io->progressStart(100);

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::progressAdvance`
    Faz a barra de progresso avançar o número de passos informado (ou ``1`` passo
    se nenhum argumento for passado)::

        // advances the progress bar 1 step
        $io->progressAdvance();

        // advances the progress bar 10 steps
        $io->progressAdvance(10);

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::progressFinish`
    Finaliza a barra de progresso (preenchendo todos os passos restantes quando seu
    tamanho é conhecido)::

        $io->progressFinish();

Métodos para Entrada do Usuário
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::ask`
    Pede ao usuário para fornecer algum valor::

        $io->ask('What is your name?');

    Você pode passar o valor padrão como segundo argumento, então, o usuário pode simplesmente
    pressionar a tecla <Enter> para selecionar esse valor::

        $io->ask('Where are you from?', 'United States');

    Caso for necessário validar o valor informado, passe um validador callback como
    terceiro argumento::

        $io->ask('Number of workers to start', 1, function ($number) {
            if (!is_integer($number)) {
                throw new \RuntimeException('You must type an integer.');
            }

            return $number;
        });

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::askHidden`
    É muito semelhante ao método ``ask()``, mas a entrada do usuário será ocultada
    e não é possível definir um valor padrão. Use-o quando for pedir informações confidenciais::

        $io->askHidden('What is your password?');

        // validates the given answer
        $io->askHidden('What is your password?', function ($password) {
            if (empty($password)) {
                throw new \RuntimeException('Password cannot be empty.');
            }

            return $password;
        });

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::confirm`
    Faz uma pergunta sim/não para o usuário e retorna somente ``true`` ou ``false``::

        $io->confirm('Restart the web server?');

    Você pode passar o valor padrão como segundo argumento para que o usuário simplesmente
    pressione a tecla <Enter> para selecionar esse valor::

        $io->confirm('Restart the web server?', true);

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::choice`
    Faz uma pergunta cuja resposta é limitada à lista informada de respostas
    válidas::

        $io->choice('Select the queue to analyze', array('queue1', 'queue2', 'queue3'));

    Você pode passar o valor padrão como terceiro argumento para que o usuário simplesmente
    pressione a tecla <Enter> para selecionar esse valor::

        $io->choice('Select the queue to analyze', array('queue1', 'queue2', 'queue3'), 'queue1');

Métodos para Resultado
~~~~~~~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::success`
    Exibe a string ou array de strings informada destacada como uma mensagem
    bem-sucedida (com um fundo verde e a label ``[OK]``). É destinado a ser usado
    uma única vez para exibir o resultado final da execução de determinado comando, mas você
    pode usá-lo várias vezes durante a execução do comando::

        // use simple strings for short success messages
        $io->success('Lorem ipsum dolor sit amet');

        // ...

        // consider using arrays when displaying long success messages
        $io->success(array(
            'Lorem ipsum dolor sit amet',
            'Consectetur adipiscing elit',
        ));

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::warning`
    Exibe a string ou array de strings informada destacada como uma mensagem
    de aviso (com um fundo vermelho e a label ``[WARNING]``). É destinado a ser usado
    uma única vez para exibir o resultado final da execução de determinado comando, mas você
    pode usá-lo várias vezes durante a execução do comando::

        // use simple strings for short warning messages
        $io->warning('Lorem ipsum dolor sit amet');

        // ...

        // consider using arrays when displaying long warning messages
        $io->warning(array(
            'Lorem ipsum dolor sit amet',
            'Consectetur adipiscing elit',
        ));

:method:`Symfony\\Component\\Console\\Style\\SymfonyStyle::error`
    Exibe a string ou array de strings informada destacada como uma mensagem
    de erro (com um fundo vermelho e a label ``[ERROR]``). É destinado a ser usado
    uma única vez para exibir o resultado final da execução de determinado comando, mas você
    pode usá-lo várias vezes durante a execução do comando::

        // use simple strings for short error messages
        $io->error('Lorem ipsum dolor sit amet');

        // ...

        // consider using arrays when displaying long error messages
        $io->error(array(
            'Lorem ipsum dolor sit amet',
            'Consectetur adipiscing elit',
        ));

Definindo os seus Próprios Estilos
----------------------------------

Se você não gosta do design dos comandos que usam o Estilo Symfony, você pode
definir o seu próprio conjunto de estilos para o console. Basta criar uma classe que implemente
a :class:`Symfony\\Component\\Console\\Style\\StyleInterface`::

    namespace AppBundle\Console;

    use Symfony\Component\Console\Style\StyleInterface;

    class CustomStyle implements StyleInterface
    {
        // ...implement the methods of the interface
    }

Em seguida, instancie essa classe personalizada em vez do padrão ``SymfonyStyle`` em
seus comandos. Graças a ``StyleInterface`` você não precisará alterar o código
de seus comandos para mudar sua aparência::

    namespace AppBundle\Console;

    use AppBundle\Console\CustomStyle;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;
    use Symfony\Component\Console\Style\SymfonyStyle;

    class GreetCommand extends ContainerAwareCommand
    {
        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            // Before
            // $io = new SymfonyStyle($input, $output);

            // After
            $io = new CustomStyle($input, $output);

            // ...
        }
    }
