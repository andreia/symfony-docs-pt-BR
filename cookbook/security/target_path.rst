.. index::
   single: Segurança; Target Path de redirecionamento

Como alterar o comportamento padrão do Target Path
==================================================

Por padrão, o componente de Segurança mantém as informações da última URI do pedido
em uma variável de sessão chamada ``_security.main.target_path`` (onde ``main`` é o
nome do firewall, definido em ``security.yml``). Após o login bem-sucedido,
o usuário é redirecionado para esse caminho, para ajudá-lo a continuar a partir da
última página conhecida que ele visitou.

Em algumas situações, isso não é ideal. Por exemplo, quando a última URI do pedido
for um XMLHttpRequest que retornou uma resposta não-HTML ou uma resposta HTML parcial,
o usuário é redirecionado de volta a uma página que o navegador não pode renderizar.

Para contornar esse comportamento, você precisa simplesmente estender a classe
``ExceptionListener`` e sobrescrever o método padrão chamado ``setTargetPath()``.

Primeiro, sobrescreva o parâmetro ``security.exception_listener.class`` no seu
arquivo de configuração. Isso pode ser feito a partir do seu arquivo de configuração principal (em
``app/config``) ou a partir de um arquivo de configuração a ser importado de um bundle:

.. configuration-block::

        .. code-block:: yaml

            # src/Acme/HelloBundle/Resources/config/services.yml
            parameters:
                # ...
                security.exception_listener.class: Acme\HelloBundle\Security\Firewall\ExceptionListener

        .. code-block:: xml

            <!-- src/Acme/HelloBundle/Resources/config/services.xml -->
            <parameters>
                <!-- ... -->
                <parameter key="security.exception_listener.class">Acme\HelloBundle\Security\Firewall\ExceptionListener</parameter>
            </parameters>

        .. code-block:: php

            // src/Acme/HelloBundle/Resources/config/services.php
            // ...
            $container->setParameter('security.exception_listener.class', 'Acme\HelloBundle\Security\Firewall\ExceptionListener');

Em seguida, crie o seu próprio ``ExceptionListener``::

    // src/Acme/HelloBundle/Security/Firewall/ExceptionListener.php
    namespace Acme\HelloBundle\Security\Firewall;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Http\Firewall\ExceptionListener as BaseExceptionListener;

    class ExceptionListener extends BaseExceptionListener
    {
        protected function setTargetPath(Request $request)
        {
            // Do not save target path for XHR requests
            // You can add any more logic here you want
            // Note that non-GET requests are already ignored
            if ($request->isXmlHttpRequest()) {
                return;
            }

            parent::setTargetPath($request);
        }
    }

Adicione aqui a lógica necessária para o seu cenário!
