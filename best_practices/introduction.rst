.. index::
   single: Melhores Práticas para o Framework Symfony

As Melhores Práticas para o Framework Symfony
=============================================

O framework Symfony é bem conhecido por ser *realmente* flexível e é usado
para construir micro-sites, aplicativos corporativos que lidam com bilhões de conexões
e até mesmo como base para *outros* frameworks. Desde o seu lançamento, em julho de 2011,
a comunidade tem aprendido muito sobre o que é possível e como fazer as coisas *melhor*.

Estes recursos da comunidade - como posts de blogs ou apresentações - criaram
um conjunto de recomendações não-oficiais para o desenvolvimento de aplicações Symfony.
Infelizmente, muitas dessas recomendações são desnecessárias para aplicações web.
Na maior parte do tempo, elas desnecessariamente complicam as coisas e não seguem a
filosofia pragmática original do Symfony.

Do que se trata este Guia?
--------------------------

Este guia destina-se a corrigir isso, descrevendo as **melhores práticas para o desenvolvimento de
aplicações web com o framework full-stack Symfony**. Essas são as melhores práticas que
se encaixam com a filosofia do framework como previsto pelo seu criador original
`Fabien Potencier`_.

.. note::

    **Melhor prática** é um substantivo que significa *"um procedimento bem definido que é
    conhecido por produzir resultados quase ótimos"*. E é exatamente isso o que esse
    guia visa proporcionar. Mesmo que você não concorde com todas as recomendações,
    acreditamos que elas irão ajudá-lo a construir ótimas aplicações com menos complexidade.

Esse guia é **especialmente adequado** para:

* Sites e aplicações web desenvolvidas com o framework full-stack Symfony.

Para outras situações, esse guia pode ser um bom **ponto de partida** que você pode
então **estender e encaixar às suas necessidades específicas**:

* Bundles compartilhados publicamente à comunidade Symfony;
* Desenvolvedores avançados ou equipes que criaram seus próprios padrões;
* Algumas aplicações complexas que têm requisitos altamente personalizados;
* Bundles que podem ser compartilhados internamente dentro de uma empresa.

Sabemos que velhos hábitos custam a ser eliminados e alguns de vocês ficarão chocados com algumas
dessas melhores práticas. Mas, seguindo elas, você será capaz de desenvolver
aplicações rapidamente, com menos complexidade e com a mesma ou até superior qualidade.
É também de caráter variável que continuará a melhorar.

Tenha em mente que estas são **recomendações opcionais** que você e sua
equipe podem ou não seguir para desenvolver aplicações Symfony. Se você quiser
continuar usando suas próprias melhores práticas e metodologias, pode, naturalmente,
fazê-lo. O Symfony é suficientemente flexível para se adaptar às suas necessidades. Isso nunca vai
mudar.

Para quem é este Livro (Dica: Não é um Tutorial)
------------------------------------------------

Qualquer desenvolvedor Symfony, se você é um especialista ou um recém-chegado, pode ler esse
guia. Mas, uma vez que este não é um tutorial, você vai precisar de alguns conhecimentos básicos de
Symfony para acompanhar tudo. Se você é totalmente novo no Symfony, bem-vindo!
Comece com :doc:`O Guia de Início Rápido </quick_tour/the_big_picture>` primeiro.

Mantemos deliberadamente esse guia curto. Não vamos repetir explicações que
você pode encontrar na vasta documentação do Symfony, como discussões sobre injeção de
dependência ou front controllers. Vamos apenas nos concentrar em explicar como fazer
o que você já sabe.

A Aplicação
-----------

Além desse guia, você encontrará um exemplo de aplicação desenvolvida com
todas essas melhores práticas em mente. **A aplicação é um mecanismo de blog simples**,
porque isso vai nos permitir concentrar nos conceitos e características do Symfony
sem ficar enterrado em detalhes difíceis.

Em vez de desenvolver a aplicação passo a passo nesse guia, você encontrará
snippets de código selecionados através dos capítulos. Por favor, consulte o último capítulo
desse guia para encontrar mais detalhes sobre essa aplicação e as instruções
para instalá-la.

Não Atualize suas Aplicações Existentes
---------------------------------------

Depois de ler esse manual, alguns podem estar pensando em refatorar suas
aplicações Symfony existentes. Nossa recomendação é sólida e clara: **você
não deve refatorar suas aplicações existentes para cumprir essas melhores
práticas**. As razões para não fazê-lo são várias:

* Suas aplicações existentes não estão erradas, elas apenas seguem um outro conjunto de
  orientações;
* A refatoração da base de código completa está propensa a apresentar erros em suas
  aplicações;
* A quantidade de trabalho gasto com isso poderia ser melhor dedicada a melhorar
  seus testes ou adicionar funcionalidades que agregam valor real para os usuários finais.

.. _`Fabien Potencier`: https://connect.sensiolabs.com/profile/fabpot
