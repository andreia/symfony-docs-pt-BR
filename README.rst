Contribuindo com a tradução da documentação oficial do Symfony2:
================================================================

Informando os documentos que deseja traduzir
--------------------------------------------

Para melhor organização, primeiro informar o(s) documento(s) que pretende ajudar na tradução e/ou revisão na planilha de tradutores/revisores:
https://spreadsheets.google.com/ccc?key=0AtX-XMIXR2DAdFBMekh4UktObUNOMy1NX2RSMjJMUUE

Se ainda não possuir acesso de edição nesta planilha, solicitar o acesso para andreiabohner at gmail dot com.

Utilizando o Git e o Github
---------------------------

1. Para começar, fazer o fork do repositório master da tradução para o português no github:

   1.1 Acessar o repositório master no github: https://github.com/andreia/symfony-docs-pt-BR
   1.2 Clicar no botão fork

2. Agora, você irá "clonar" o projeto para a sua máquina local:
   
   2.1 Copiar a URL do repositório do seu fork, para utilizá-la com o comando git clone (por exemplo)::
    $ git clone git@github.com:seuusername/symfony-docs.git

   2.2 Após completar o clone do repositório, ele terá o nome remoto "origin". Não confundir, apesar do nome ser origin ele não está apontando para o repo master, mas sim para o seu fork no github.

3. Pronto, agora é só trabalhar na tradução do(s) documento(s).

4. Finalizadas as traduções e/ou revisões, faça o commit das alterações no seu repositório local::

    $ git commit –a –m "pt_BR translation"

5. Atualize o seu repositório no servidor github com as alterações realizadas localmente::

    $ git push origin master

6. O último passo é informar sobre as suas alterações ao responsável pelo repositório de origem para que ele faça um pull das alterações no repositório. Para isso, acesse a página do repositório original no github, em: https://github.com/andreia/symfony-docs-pt-BR e envie um pull request (clicando no botão "Pull Request"):
   Mais informações sobre o `Pull Request`_ 


Mantendo seu repositório local atualizado
-----------------------------------------

Sempre antes de fazer as suas alterações locais, lembrar de executar o comando ``pull`` para manter atualizado o seu repositório local trazendo as alterações do repositório de origem (o repositório que você fez o fork)::

    $ git remote add upstream git@github.com:andreia/symfony-docs-pt-BR.git
    $ git pull upstream master


Padrões a serem seguidos durante a tradução
-------------------------------------------

Mensagens dos Commits
~~~~~~~~~~~~~~~~~~~~~

As mensagens dos commits devem ser todas em INGLÊS e devem ter o seguinte prefixo:
[pt_BR][Livro][Capítulo] seguido da mensagem do commit.
Ex.: Se você estiver traduzindo o capítulo 'Validation' do livro, a mensagem do commit poderia ser semelhante a seguinte:
[pt_BR][Book][Validation] Fix typo.

Dica: Escrevendo boas mensagens de commit: https://github.com/erlang/otp/wiki/Writing-good-commit-messages

Código
~~~~~~

Os códigos serão mantidos todos em inglês.

Formato da documentação
~~~~~~~~~~~~~~~~~~~~~~~

A documentação do Symfony2 utiliza a linguagem markup reStructuredText juntamente com o Sphinx. Segue a referência: http://symfony.com/doc/2.0/contributing/documentation/format.html
Se preferir, existe um editor online, que pode auxiliar em: http://rst.ninjs.org/

.. _`Pull Request`: http://help.github.com/pull-requests/

Referências
-----------

- SSH issues: Guia contendo as soluções para os problemas mais comuns referentes a conexão SSH no GitHub (chave pública, ...): http://help.github.com/ssh-issues/
- Mencionar alguém em um ``pull request`` ou ``issue``: https://github.com/blog/1004-mention-autocompletion