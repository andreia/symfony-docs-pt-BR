Introdução
==========

O `Symfony`_ é um conjunto reutilizável de componentes PHP coesivos, independentes e desacoplados 
que solucionam os problemas comuns do desenvolvimento web.

Em vez de usar esses componentes de baixo nível, você pode utilizar o framework web full-stack Symfony2, 
pronto para uso, que utiliza esses componentes como base... ou
você pode criar o seu próprio framework. Esta série é sobre esse último assunto.

Por que você desejaria criar o seu próprio framework?
-----------------------------------------------------

Em primeiro lugar, porque você gostaria de criar o seu próprio framework? Se você
olhar em volta, todos vão dizer que é algo ruim reinventar a
roda, e, que é melhor você escolher um framework existente e esquecer completamente 
de criar o seu próprio. Na maioria das vezes, eles estão certos, mas existem
algumas boas razões para você começar a criar o seu próprio framework:

* Para aprender mais sobre a arquitetura de baixo nível dos frameworks web modernos
  em geral e sobre o funcionamento interno do framework full-stack Symfony2, em particular;

* Para criar um framework adequado às suas necessidades muito específicas (não se esqueça
  de, primeiro, certificar-se que as suas necessidades são realmente muito específicas);

* Para experimentar criar um framework para se divertir (em uma abordagem "aprender e 
  descartar");

* Para refatorar uma aplicação antiga/existente que precisa de uma boa dose das
  melhores práticas de desenvolvimento web recentes;

* Para provar ao mundo que você realmente pode criar um framework próprio (...
  mas com pouco esforço).

Esse tutorial irá guiá-lo através da criação de um framework web, um passo de cada
vez. Em cada etapa, você terá um framework totalmente funcional que já poderá utilizá-lo,
ou, então, usar como ponto de partida para o seu próprio. Começaremos com um framework simples
e, com o tempo, mais recursos serão adicionados. Eventualmente, você terá um
framework web full-stack com recursos completos.

E, claro, cada passo será a ocasião para aprender mais sobre alguns dos
Componentes do Symfony2.

.. tip::

    Se você não tem tempo para ler todo o livro, ou se você deseja obter
    um início rápido, também pode olhar o `Silex`_, um micro-framework que possui
    como base os Componentes do Symfony2. O código é bastante enxuto e aproveita
    muitos aspectos dos Componentes do Symfony2.

Muitos frameworks web modernos anunciam-se como sendo frameworks MVC. Esse tutorial não irá falar
sobre o padrão MVC, pois os Componentes do Symfony2 são capazes de criar qualquer tipo de frameworks,
não apenas os que seguem a arquitetura MVC. Enfim, se você olhar através da semântica MVC, 
esta série é sobre como criar a parte do Controlador de um framework.
Para o Modelo e a Visão, realmente vai depender da sua preferência pessoal e você pode
utilizar qualquer uma das bibliotecas existentes de terceiros (Doctrine,
Propel, ou apenas o PDO para o Modelo; e PHP ou Twig para a Visão).

Ao criar um framework, seguir o padrão MVC não é a forma correta. O objetivo principal
deve ser a **Separação de Responsabilidades** (Separation of Concerns, em inglês); esse é
provavelmente é o único padrão de projeto que você deve realmente se preocupar. Os princípios
fundamentais dos Componentes do Symfony2 estão centrados na especificação
HTTP. Como tal, o framework que vamos criar deve ser mais precisamente
rotulados como um framework HTTP ou framework Request/Response.

Antes de Começarmos
-------------------

Ler sobre como criar um framework não é suficiente. Você terá que acompanhar
juntamente, digitando todos os exemplos incluídos nesse tutorial. Para isso, você precisa
de uma versão recente do PHP (5.3.9 ou posterior é suficiente), um servidor web (como o
Apache, Nginx ou o servidor web embutido do PHP), um bom conhecimento de PHP e uma compreensão
de programação Orientada a Objeto.

Pronto? Vamos começar!

Bootstrapping
-------------

Antes de sequer pensar em criar o primeiro framework, você precisa pensar
sobre algumas convenções: onde irá armazenar o código, como irá nomear as classes,
como irá referenciar as dependências externas, etc.

Para armazenar seu novo framework, crie um diretório em algum lugar na sua máquina:

.. code-block:: bash

    $ mkdir framework
    $ cd framework

Gerenciamento de Dependências
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Para instalar os Componentes do Symfony2 que você precisa para seu framework, será
utilizado o `Composer`_, um gerenciador de dependências de projeto para PHP. Se você
ainda não tem ele, faça o :doc:`download e instalação do Composer </cookbook/composer>` agora.

Nosso Projeto
-------------

Em vez de criar nosso framework a partir do zero, vamos escrever a mesma
"aplicação" repetidamente, adicionando uma abstração de cada vez. Vamos 
iniciar com a aplicação web mais simples que podemos pensar em PHP::

    // framework/index.php

    $input = $_GET['name'];

    printf('Hello %s', $input);

Se você tem o PHP 5.4, pode usar o servidor embutido do PHP para testar essa
aplicação no navegador (``http://localhost:4321/index.php?name=Fabien``).
Caso contrário, utilize o seu próprio servidor (Apache, Nginx, etc.):

.. code-block:: bash

    $ php -S 127.0.0.1:4321

No próximo capítulo, vamos apresentar o Componente HttpFoundation
e ver o que ele nos traz.

.. _`Symfony`: http://symfony.com/
.. _`documentation`: http://symfony.com/doc
.. _`Silex`: http://silex.sensiolabs.org/
.. _`Composer`: http://packagist.org/about-composer
