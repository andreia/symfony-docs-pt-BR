.. index::
   single: Controlador; Como Serviços

Como definir Controladores como Serviços
========================================

No livro, você aprendeu como um controlador pode ser facilmente usado quando ele
estende a classe base
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`. Mesmo
isso funcionando bem, os controladores também podem ser especificados como serviços.

Para referir-se a um controlador que é definido como um serviço, utilize a notação de um único dois pontos (:)
. Por exemplo, supondo que você definiu um serviço chamado
``my_controller`` e que deseja encaminhar para um método chamado ``indexAction()``
dentro do serviço::

    $this->forward('my_controller:indexAction', array('foo' => $bar));

Você precisa usar a mesma notação ao definir o valor da rota 
``_controller``:

.. code-block:: yaml

    my_controller:
        pattern:   /
        defaults:  { _controller: my_controller:indexAction }

Para utilizar um controlador desta forma, ele deve estar definido na configuração do
container de serviço. Para mais informações, consulte o capítulo :doc:`Service Container
</book/service_container>`

Ao usar um controlador definido como um serviço, ele provavelmente não estenderá
a classe base ``Controller``. Em vez de contar com seus métodos de atalho,
você vai interagir diretamente com os serviços que você precisa. Felizmente, isto é
normalmente muito fácil e a classe base ``Controller`` em si é uma grande fonte
sobre a forma de realizar muitas das tarefas comuns.

.. note::

    Especificar um controlador como um serviço leva um pouco mais de trabalho. A
    principal vantagem é que todo o controlador ou quaisquer serviços passados ​​para
    o controlador podem ser modificados através da configuração do container de serviço.
    Isto é especialmente útil no desenvolvimento de um bundle open-source ou qualquer
    bundle que será utilizado em vários projetos diferentes. Assim, mesmo se você não
    especificar os seus controladores como serviços, provavelmente verá isto feito em alguns
    bundles open-source do Symfony2.

Usando Anotação no Roteamento
-----------------------------

Ao usar anotações para configurar as rotas em um controlador definido como um
serviço, é necessário especificar o serviço da seguinte forma::

    /**
     * @Route("/blog", service="my_bundle.annot_controller")
     * @Cache(expires="tomorrow")
     */
    class AnnotController extends Controller
    {
    }

Neste exemplo, ``my_bundle.annot_controller`` deve ser o id da
instância ``AnnotController`` definida no container de serviço. Isto está
documentado no capítulo :doc:`/bundles/SensioFrameworkExtraBundle/annotations/routing`
.
