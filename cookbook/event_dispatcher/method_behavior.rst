.. index::
   single: Dispatcher de Evento

Como personalizar o Comportamento do Método sem o uso de Herança
================================================================

Realizar algo antes ou após a Chamada de um Método
--------------------------------------------------

Se você deseja realizar algo logo antes ou após um método ser chamado, você
pode despachar um evento, respectivamente, no início ou no fim do
método::

    class Foo
    {
        // ...

        public function send($foo, $bar)
        {
            // do something before the method
            $event = new FilterBeforeSendEvent($foo, $bar);
            $this->dispatcher->dispatch('foo.pre_send', $event);

            // get $foo and $bar from the event, they may have been modified
            $foo = $event->getFoo();
            $bar = $event->getBar();

            // the real method implementation is here
            $ret = ...;

            // do something after the method
            $event = new FilterSendReturnValue($ret);
            $this->dispatcher->dispatch('foo.post_send', $event);

            return $event->getReturnValue();
        }
    }

Neste exemplo, dois eventos são lançados: ``foo.pre_send``, antes do método ser
executado, e ``foo.post_send`` após o método ser executado. Cada um usa uma
classe ``Event`` personalizada para comunicar informações para os ouvintes dos dois
eventos. Essas classes de evento teriam que ser criadas por você e, devem permitir, 
neste exemplo, que as variáveis ``$foo``, ``$bar`` e ``$ret`` sejam recuperadas
e definidas pelos ouvintes.

Por exemplo, supondo que o ``FilterSendReturnValue`` tem um método ``setReturnValue``,
um ouvinte pode ter o seguinte aspecto:

.. code-block:: php

    public function onFooPostSend(FilterSendReturnValue $event)
    {
        $ret = $event->getReturnValue();
        // modify the original ``$ret`` value

        $event->setReturnValue($ret);
    }
