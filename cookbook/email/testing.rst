.. index::
   single: E-mails; Testando

Como testar se um email foi enviado em um teste funcional
=========================================================

O envio de e-mails com Symfony é bastante simples graças ao
SwiftmailerBundle, que aproveita o poder da `biblioteca `Swift Mailer`_.

Para testar funcionalmente se um e-mail foi enviado, e até mesmo o assert do assunto do email,
conteúdo ou quaisquer outros cabeçalhos, você pode usar o :ref:`o Profiler do Symfony <internals-profiler>`.

Comece com uma ação do controlador fácil que envia um e-mail::

    public function sendEmailAction($name)
    {
        $message = \Swift_Message::newInstance()
            ->setSubject('Hello Email')
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody('You should see me from the profiler!')
        ;

        $this->get('mailer')->send($message);

        return $this->render(...);
    }

.. note::

    Não se esqueça de habilitar o profiler como explicado em :doc:`/cookbook/testing/profiling`.

Em seu teste funcional, utilize o collector do ``swiftmailer`` no profiler
para obter informações sobre as mensagens enviadas no pedido anterior::

    // src/AppBundle/Tests/Controller/MailControllerTest.php
    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class MailControllerTest extends WebTestCase
    {
        public function testMailIsSentAndContentIsOk()
        {
            $client = static::createClient();

            // Enable the profiler for the next request (it does nothing if the profiler is not available)
            $client->enableProfiler();

            $crawler = $client->request('POST', '/path/to/above/action');

            $mailCollector = $client->getProfile()->getCollector('swiftmailer');

            // Check that an e-mail was sent
            $this->assertEquals(1, $mailCollector->getMessageCount());

            $collectedMessages = $mailCollector->getMessages();
            $message = $collectedMessages[0];

            // Asserting e-mail data
            $this->assertInstanceOf('Swift_Message', $message);
            $this->assertEquals('Hello Email', $message->getSubject());
            $this->assertEquals('send@example.com', key($message->getFrom()));
            $this->assertEquals('recipient@example.com', key($message->getTo()));
            $this->assertEquals(
                'You should see me from the profiler!',
                $message->getBody()
            );
        }
    }

.. _`Swift Mailer`: http://swiftmailer.org/
