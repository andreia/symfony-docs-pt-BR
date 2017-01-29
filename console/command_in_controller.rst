.. index::
   single: Console; Como Chamar um Comando dentro de um Controlador

Como Chamar um Comando dentro de um Controlador
===============================================

A :doc:`documentação do componente Console </components/console/introduction>`
aborda como criar um comando de console. Este artigo aborda como
usar um comando de console diretamente em seu controlador.

Você pode ter a necessidade de executar alguma função que somente está disponível em um
comando de console. Normalmente, você deve refatorar o comando e mover alguma lógica
em um serviço que pode ser reutilizado no controlador. No entanto, quando o comando
é parte de uma biblioteca de terceiros, você não gostaria de modificar ou duplicar
o seu código. Em vez disso, você pode executar o comando diretamente.

.. caution::

    Em comparação com uma chamada direta do console, chamar um comando
    dentro um controlador tem um leve impacto no desempenho por causa da sobrecarga no stack da
    requisição.

Imagine que você deseja enviar as mensagens que estão no spool do Swift Mailer
:doc:`usando o comando swiftmailer:spool:send </cookbook/email/spool>`.
Execute esse comando dentro de seu controlador da seguinte forma::

    // src/AppBundle/Controller/SpoolController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Console\Application;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\Console\Input\ArrayInput;
    use Symfony\Component\Console\Output\BufferedOutput;
    use Symfony\Component\HttpFoundation\Response;

    class SpoolController extends Controller
    {
        public function sendSpoolAction($messages = 10)
        {
            $kernel = $this->get('kernel');
            $application = new Application($kernel);
            $application->setAutoExit(false);

            $input = new ArrayInput(array(
               'command' => 'swiftmailer:spool:send',
               '--message-limit' => $messages,
            ));
            // You can use NullOutput() if you don't need the output
            $output = new BufferedOutput();
            $application->run($input, $output);

            // return the output, don't use if you used NullOutput()
            $content = $output->fetch();
            
            // return new Response(""), if you used NullOutput()
            return new Response($content);
        }
    }

Mostrando a Saída do Comando Colorida
-------------------------------------

Ao dizer que ``BufferedOutput`` é decorado através do segundo parâmetro,
ele irá retornar o conteúdo codificado por cores Ansi. O `conversor AnsiToHtml da SensioLabs`_
pode ser usado para converter isso em HTML colorido.

Em primeiro lugar, instale o pacote:

.. code-block:: bash

    $ composer require sensiolabs/ansi-to-html

Agora, use-o em seu controlador::

    // src/AppBundle/Controller/SpoolController.php
    namespace AppBundle\Controller;

    use SensioLabs\AnsiConverter\AnsiToHtmlConverter;
    use Symfony\Component\Console\Output\BufferedOutput;
    use Symfony\Component\Console\Output\OutputInterface;
    use Symfony\Component\HttpFoundation\Response;
    // ...

    class SpoolController extends Controller
    {
        public function sendSpoolAction($messages = 10)
        {
            // ...
            $output = new BufferedOutput(
                OutputInterface::VERBOSITY_NORMAL,
                true // true for decorated
            );
            // ...

            // return the output
            $converter = new AnsiToHtmlConverter();
            $content = $output->fetch();

            return new Response($converter->convert($content));
        }
    }

O ``AnsiToHtmlConverter`` também pode ser registrado `como uma Extensão Twig`_,
e suporta temas opcionais.

.. _`conversor AnsiToHtml da SensioLabs`: https://github.com/sensiolabs/ansi-to-html
.. _`como uma Extensão Twig`: https://github.com/sensiolabs/ansi-to-html#twig-integration
