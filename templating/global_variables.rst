.. index::
   single: Templating; Global variables

Utilizando variáveis em todas templates (Variáveis globais)
===========================================================

Algumas vezes você quer que uma variável esteja disponível em todas as templates que utiliza.
Isto é possível configurando o twig dentro do arquivo ``app/config/config.yml`` :

.. code-block:: yaml

    # app/config/config.yml
    twig:
        # ...
        globals:
            ga_tracking: UA-xxxxx-x

Agora a variável ``ga_tracking`` está disponível em todas templates Twig e pode ser acessada
da seguinte forma.

.. code-block:: html+jinja

    <p>Our google tracking code is: {{ ga_tracking }} </p>

É fácil! Você também pode utilizar do sistema de parâmetros (:ref:`book-service-container-parameters`),
que permite você isolar e reutilizar o valor como a seguir.

.. code-block:: ini

    ; app/config/parameters.yml
    [parameters]
        ga_tracking: UA-xxxxx-x

.. code-block:: yaml

    # app/config/config.yml
    twig:
        globals:
            ga_tracking: %ga_tracking%

A mesma variável está disponível exatamente como antes.

Variáveis globais mais complexas
--------------------------------

Se a variável global que deseja definir é mais complexa, como um objeto por exemplo,
então você não poderá utilizar o método acima. Ao invés disso, precisa criar uma
extensão Twig (:ref:`Twig Extension<reference-dic-tags-twig-extension>`) e retornar
a variável global como uma das entradas no método ``getGlobals``.
