.. index::
   single: Dispatcher de Eventos

Como estender uma Classe sem usar Herança
=========================================

Para permitir que várias classes adicionem métodos para uma outra, você pode definir o
método mágico ``__call()`` na classe que você deseja que seja estendida da seguinte forma:

.. code-block:: php

    class Foo
    {
        // ...

        public function __call($method, $arguments)
        {
            // cria um evento chamado 'foo.method_is_not_found'
            $event = new HandleUndefinedMethodEvent($this, $method, $arguments);
            $this->dispatcher->dispatch($this, 'foo.method_is_not_found', $event);

            // nenhum listener foi capaz de processar o evento? O método não existe
            if (!$event->isProcessed()) {
                throw new \Exception(sprintf('Call to undefined method %s::%s.', get_class($this), $method));
            }

            // retorna o valor retornado pelo listener 
            return $event->getReturnValue();
        }
    }

Ela utiliza um ``HandleUndefinedMethodEvent`` especial que também deve ser
criado. Esta é uma classe genérica que poderia ser reutilizada cada vez que você precisa
utilizar esse padrão de extensão de classe:

.. code-block:: php

    use Symfony\Component\EventDispatcher\Event;

    class HandleUndefinedMethodEvent extends Event
    {
        protected $subject;
        protected $method;
        protected $arguments;
        protected $returnValue;
        protected $isProcessed = false;

        public function __construct($subject, $method, $arguments)
        {
            $this->subject = $subject;
            $this->method = $method;
            $this->arguments = $arguments;
        }

        public function getSubject()
        {
            return $this->subject;
        }

        public function getMethod()
        {
            return $this->method;
        }

        public function getArguments()
        {
            return $this->arguments;
        }

        /**
         * Define o valor de retorno e pára a notificação para outros listeners
         */
        public function setReturnValue($val)
        {
            $this->returnValue = $val;
            $this->isProcessed = true;
            $this->stopPropagation();
        }

        public function getReturnValue($val)
        {
            return $this->returnValue;
        }

        public function isProcessed()
        {
            return $this->isProcessed;
        }
    }

Em seguida, crie uma classe que vai ouvir o evento ``foo.method_is_not_found``
e *adicionar* o método ``bar()``:

.. code-block:: php

    class Bar
    {
        public function onFooMethodIsNotFound(HandleUndefinedMethodEvent $event)
        {
            // queremos somente responder as chamadas do método 'bar'
            if ('bar' != $event->getMethod()) {
                // permite que outro listener cuide deste método desconhecido
                return;
            }

            // o objeto (a instância foo)
            $foo = $event->getSubject();

            // os argumentos do método bar
            $arguments = $event->getArguments();

            // faz algo
            // ...

            // define o valor de retorno
            $event->setReturnValue($someValue);
        }
    }

Por fim, adicione o novo método ``bar`` na classe ``Foo`` para registrar uma
instância de ``Bar`` com o evento ``foo.method_is_not_found``:

.. code-block:: php

    $bar = new Bar();
    $dispatcher->addListener('foo.method_is_not_found', $bar);
