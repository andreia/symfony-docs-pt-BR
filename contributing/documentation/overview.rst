Contribuinto para a Documentação
================================

A documentação é tão importante quanto o código. Segue os mesmos princípios:
DRY, testes, fácil manutenção, extensibilidade, otimização, e refatoração
só para citar alguns. E claro, documentação tem erros, erros de digitação,
difícil de ler tutoriais e mais.

Contribuindo
------------

Antes de contribuir, você precisa se familiarizar com :doc:`markup
  language <format>` usado para a documentação.

  A documentação Symfony2 é hospedada no GitHub:

.. code-block:: text

    https://github.com/symfony/symfony-docs

Se você quiser enviar uma patch, `fork`_ o repositório oficial no GitHub e
clone seu fork:

.. code-block:: bash

    $ git clone git://github.com/YOURUSERNAME/symfony-docs.git

De acordo com o código fonte do Symfony, o repositório de documentação é divida
em multiplos branches, correspondentes às diferentes versões do Symfony em si.
O branch ``master`` detém a documentação para o branch de desenvolvimento do
código.

A menos que você esteja documentando um feature que foi introduzida *após*
Symfony 2.3 (ex. no Symfony 2.4), as alterações devem ser sempre baseadas no 
branch 2.3. Para fazer isso, faça o checkout do branch 2.3 antes do próximo
passo:

.. code-block:: bash

    $ git checkout 2.3

.. tip::

    Seu branch base (ex. 2.3) se tornará "Aplica-se a" no :ref:`doc-
    contributing-pr-format` que você usará mais tarde.

Próximo, crie um branch dedicado para suas alterações (para organizar):

.. code-block:: bash

    $ git checkout -b improving_foo_and_bar

Você pode fazer suas alterações diretamente neste branch e faça o commit. Quando
terminar, envie este branch para *seu* fork do GitHub e inicie um pull request.

Criando um Pull Request
~~~~~~~~~~~~~~~~~~~~~~~

Seguinte o exemplo, o pull request será o padrão entre seu branch
``melhoria_foo_e_bar`` e o branch ``master`` ``symfony-docs``.

Se você tiver feito suas alterações baseadas no branch 2.3 então você precisa
mudar o branch base para ser 2.3 na página de visualização clicando no botão
``editar`` no canto superior esquerdo:

.. image:: /images/docs-pull-request-change-base.png
:align: center

.. note::

  Todas as alterações feitas para um branch (ex. 2.3) será incorporada a cada 
  "novo" branch (ex. 2.4, master, etc) para o próximo release semanal.

GitHub abrange o tópico do `pull requests`_ em detalhes.

.. note::

    A documentação do Symfony2 é licensiado sob uma :doc:`Licença <license>`
    Creative Commons Attribution-Share Alike 3.0 Unported.

Você também pode prefixar o título do seu pull request em alguns casos:

* ``[WIP]`` (Work in Progress) é usado quando seu pull request ainda não está
  acabado, mas você gostaria que fosse revisado. O pull request não será 
  incorporado até você dizer que está pronto.

* ``[WCM]`` (Waiting Code Merge) é usado quando você está documentando um novo
  feature ou mudança que ainda não foi aceita no código do core. O pull request
  não será incorporado até ser feito o merge no código do core (ou fechado se a 
  mudança for rejeitada).

.. _doc-contributing-pr-format:

Formato do Pull Request
~~~~~~~~~~~~~~~~~~~~~~~

A menos que esteja corrigindo erros menores, a descrição do pull request **deve**
incluir o seguinte lista para garantir que as contribuições podem ser revisadas
sem precisar de feedback e que suas contribuições podem ser incluídas na
documentação o mais rápido possível:

.. code-block:: text

    | Q             | A
    | ------------- | ---
    | Doc fix?      | [sim|não]
    | New docs?     | [sim|não] (PR # se aplicável ao symfony/symfony)
    | Applies to    | [Número das versões do Symfony onde se aplica]
    | Fixed tickets | [lista de tickets corrigidos separados por vírgula no PR]

Um exemplo de envio pode ser visto como seguinte:

.. code-block:: text

    | Q             | A
    | ------------- | ---
    | Doc fix?      | yes
    | New docs?     | yes (symfony/symfony#2500)
    | Applies to    | all (or 2.3+)
    | Fixed tickets | #1075

.. tip::

    Por favor seja paciente. Isso pode levar a partir de 15 minutos até vários
    dias para suas mudanças aparecerem no site do symfony.com depois que o time
    da documentação fazer o merge do seu pull request. Você pode verificar se
    suas mudanças introduziram alguns markup issues indo para 
    `Erros no Build da Documentação`_ page (ele é atualizado a cada madrugada 
    francesa às 3H, quando o servidor reconstrói a documentação).

Documentando novos Features ou mudanças de comportamento
--------------------------------------------------------

Se você está documentando um novo feature ou uma mudança que foi feita no
Symfony2, você deve preceder sua descrição da mudança com uma tag
``.. versionadded:: 2.X`` e uma descrição breve:

.. code-block:: text

    .. versionadded:: 2.3
O método ``askHiddenResponse`` foi introduzido no Symfony 2.3.

    Você pode também fazer uma pergunta e ocultar a resposto. Isto é particularmente...

Se você está documentando uma mudança de comportamento, pode ser útil descrever
*brevemente* como o comportamento mudou.

.. code-block:: text

    .. versionadded:: 2.3
A função ``include()`` é um novo feature do Twig que está disponível no Symfony
2.3. Antes, a tag ``{% include %}`` era usada.

Sempre que uma versão menor do Symfony2 é liberada (ex. 2.4, 2.5, etc), um novo
branch da documentação é criado a partir do branch ``master``.

Neste ponto, toda tag ``versionadded`` para as versões do Symfony2 que atigiram
o fim da vida serão removidas. Por exemplo, se o Symfony 2.5 hoi liberado
hoje, e 2.2 atingiu seu fim da vida, a tag 2.2 ``versionadded`` seria removida
a partir do novo branch 2.5.

Padrões
-------

Toda a documentação do Symfony deve seguir
:doc:`os padrões de documentação <standards>`.

Reportando um problema
----------------------

A contribuição mais simples que pode fazer é relatar problemas: erro de
digitação, erro de gramática, um bug no código de exemplo, uma explicação
esquecida, e assim por diante.

Passos:

* Enviar um bug no bug tracker;

* *(opcional)* Enviar uma correçãop.

Traduzindo
----------

Leia o :doc:`documento <translations>` dedicado.

Gerenciando Releases
--------------------

Symfony tem um processo de release muito padronizado, que você pode ler mais
sobre na seção :doc:`/contributing/community/releases`.

Para acompanhar o processo de release, o time de documentação fez várias
alterações para a documentação em várias partes do ciclo de vida.

Quando um Release atingge o "fim da manutenção"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cada release eventualmente chegará ao "fim da manutenção". Para mais detalhes,
veja :ref:`contributing-release-maintenance`.

Quando um release atinge o fim da manutenção, os seguintes itens são feitos.
Para este exemplo, supondo que a versão 2.1 atingiu o fim da manutenção:

* Mundanças e pull requests não serão incorporados ao branch (2.1), exceto para
  atualizações de segurança, que serão incorporadas até o release atinja o 
  "fim da vida".

* Todos os branches ainda em manutenção (ex. 2.2 e superior) devem receber as
  atualizações do pull request, este deve iniciar a partir da mais antiga versão
  de manutenção (ex. 2.2) - incluindo os detalhes do arquivo README.

* Remover todas diretivas ``versionadded`` - e quaisquer outras notas
  relacionadas à alterações do feature ou sendo usado - para a versão (ex. 2.1)
  a partir do branch master.
  O resultado é que o próximo release (que é o primeiro que vem inteiramente
  *após* o fim da manutenção deste branch), não mencionará a última versão (ex.
  2.1).

Quando um novo Branch é criado para um Release
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Durante a :ref:`fase de estabilização <contributing-release-development>`, um
novo branch na documentação é criado. Por exemplo, se a versão 2.3 estava sendo
estabilizada, então um novo branch 2.3 seria criado para ele. Quando isto
acontece, o seguinte item é feito:

* Altere todas as versões e referências do master para a versão correta (ex. 
  2.3). Por exemplo, nos capítulos de instalação, que faz referência a versão
  que você deve usar para instalação. Como exemplo, veja as alterações feitas no
  `PR #2688`_.

.. _`fork`:                           https://help.github.com/articles/fork-a-repo
.. _`pull requests`:                  https://help.github.com/articles/using-pull-requests
.. _`Erros no Build da Documentação`: http://symfony.com/doc/build_errors
.. _`PR #2688`:                       https://github.com/symfony/symfony-docs/pull/2688
