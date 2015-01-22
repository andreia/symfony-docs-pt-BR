.. index::
   single: Formulário; Form::submit()

Como usar a função submit() para lidar com Submissões de Formulário
===================================================================

.. versionadded:: 2.3
    O método :method:`Symfony\\Component\\Form\\FormInterface::handleRequest`
    foi introduzido no Symfony 2.3.

Com o método ``handleRequest()``, é realmente fácil lidar com submissões de
formulário::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function newAction(Request $request)
    {
        $form = $this->createFormBuilder()
            // ...
            ->getForm();

        $form->handleRequest($request);

        if ($form->isValid()) {
            // perform some action...

            return $this->redirect($this->generateUrl('task_success'));
        }

        return $this->render('AcmeTaskBundle:Default:new.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. tip::

    Para ver mais sobre esse método, leia :ref:`book-form-handling-form-submissions`.

.. _cookbook-form-call-submit-directly:

Chamando o Form::submit() manualmente
-------------------------------------

.. versionadded:: 2.3
    Antes do Symfony 2.3, o método ``submit()`` era conhecido como ``bind()``.

Em alguns casos, você quer um controle melhor sobre quando exatamente o formulário é enviado
e quais dados são passados ​​para ele. Em vez de usar o
método :method:`Symfony\\Component\\Form\\FormInterface::handleRequest`
, passe os dados submetidos diretamente ao
:method:`Symfony\\Component\\Form\\FormInterface::submit`::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function newAction(Request $request)
    {
        $form = $this->createFormBuilder()
            // ...
            ->getForm();

        if ($request->isMethod('POST')) {
            $form->submit($request->request->get($form->getName()));

            if ($form->isValid()) {
                // perform some action...

                return $this->redirect($this->generateUrl('task_success'));
            }
        }

        return $this->render('AcmeTaskBundle:Default:new.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. tip::

    Formulários que consistem de campos aninhados esperam um array no
    :method:`Symfony\\Component\\Form\\FormInterface::submit`. Você também pode enviar
    campos individuais chamando :method:`Symfony\\Component\\Form\\FormInterface::submit`
    diretamente no campo::

        $form->get('firstName')->submit('Fabien');

.. _cookbook-form-submit-request:

Passando uma Requisição para Form::submit() (Obsoleto)
------------------------------------------------------

.. versionadded:: 2.3
    Antes do Symfony 2.3, o método ``submit`` era conhecido como ``bind``.

Antes do Symfony 2.3, o método :method:`Symfony\\Component\\Form\\FormInterface::submit`
aceitava um objeto :class:`Symfony\\Component\\HttpFoundation\\Request` como
um atalho conveniente para o exemplo anterior::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function newAction(Request $request)
    {
        $form = $this->createFormBuilder()
            // ...
            ->getForm();

        if ($request->isMethod('POST')) {
            $form->submit($request);

            if ($form->isValid()) {
                // perform some action...

                return $this->redirect($this->generateUrl('task_success'));
            }
        }

        return $this->render('AcmeTaskBundle:Default:new.html.twig', array(
            'form' => $form->createView(),
        ));
    }

Passar o :class:`Symfony\\Component\\HttpFoundation\\Request` diretamente para
:method:`Symfony\\Component\\Form\\FormInterface::submit` ainda funciona, mas está
obsoleto e será removido no Symfony 3.0. Ao invés, você deve usar o método
:method:`Symfony\\Component\\Form\\FormInterface::handleRequest`.
