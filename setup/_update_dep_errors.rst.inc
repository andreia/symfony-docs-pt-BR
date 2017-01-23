Erros de Dependência
~~~~~~~~~~~~~~~~~~~~

Se você receber um erro de dependência, pode significar somente que você precisa atualizar
também outras dependências Symfony. Nesse caso, tente o seguinte comando:

.. code-block:: bash

    $ composer update symfony/symfony --with-dependencies

Isso atualiza ``symfony/symfony`` e *todos* os pacotes dos quais ele depende, o que irá
incluir vários outros pacotes. Usando restrições de versão limitadas no
``composer.json``, você pode controlar para quais versões cada biblioteca atualiza.

Se isso ainda não funcionar, o seu arquivo ``composer.json`` pode especificar uma versão
para uma biblioteca que não é compatível com a versão mais recente Symfony. Nesse
caso, a atualização da biblioteca para uma versão mais recente no ``composer.json`` pode resolver
o problema.

Ou, você pode ter problemas mais profundos, onde diferentes bibliotecas dependem de versões conflitantes
de outras bibliotecas. Verifique a sua mensagem de erro para depuração.
