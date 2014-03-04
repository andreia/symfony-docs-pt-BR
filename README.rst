Contribuindo com a tradução da documentação oficial do Symfony2:
================================================================

Informando os documentos que deseja traduzir
--------------------------------------------

Informe o documento que irá traduzir abrindo um **issue** no repositório ( https://github.com/andreia/symfony-docs-pt-BR/issues/new ), e seguindo o padrão:

No título: 
**[Traduzir | Revisar][Livro][Capítulo] nome-do-documento**

- Se o documento ainda não foi traduzido, usar [Traduzir]. Se for um documento já traduzido que está sendo revisado ou está sendo adicionada uma tradução faltante usar [Revisar].

Na descrição/comentário: 
Usar a tabela abaixo no topo do comentário e logo em seguida outros comentários, se necessário.

    | P                          | R
    | -------------------------- | ---
    | Nova tradução?             | [sim|não]
    | Revisão?                   | [sim|não]
    | Revisão com nova tradução? | [sim|não]
    | Aplica para as versões     | [Número das versões do Symfony onde se aplica, ex. todas, 2.2+, 2.0]

Informar **sim** em **Revisão com nova tradução** para os documentos que já haviam sido traduzidos mas que foram acrescentados trechos com novas traduções (e precisariam de uma posterior revisão).

Utilizando o Git e o Github
---------------------------

1. Para começar, faça o fork do repositório master da tradução para o português no github:

   1.1 Acessar o repositório master no github: https://github.com/andreia/symfony-docs-pt-BR
   1.2 Clicar no botão fork

2. Agora, você irá "clonar" o projeto para a sua máquina local:
   
   2.1 Copiar a URL do repositório do seu fork, para utilizá-la com o comando git clone (por exemplo)::

    $ git clone git@github.com:seuusername/symfony-docs.git

   2.2 Após completar o clone do repositório, ele terá o nome remoto "origin". Não confundir, apesar do nome ser origin ele não está apontando para o repo master, mas sim para o seu fork no github.

3. Selecione a branch que você irá trabalhar, por exemplo, 2.4, 2.3, ... ::

    $ git checkout 2.4

4. Se for uma nova tradução, copie o(s) documento(s) que você irá traduzir (verifique se os mesmos já não foram traduzidos em https://github.com/andreia/symfony-docs-pt-BR) do repositório oficial em inglês ( https://github.com/symfony/symfony-docs ).
    Pronto, agora é só trabalhar na tradução do(s) documento(s).

5. Copie o(s) documento(s) que você traduziu para os respectivos diretórios em seu repositório local (o que você criou no passo 2).

6. Finalizadas as traduções e/ou revisões, adicione os novos documentos (se for uma nova tradução) e faça o commit das alterações no seu repositório local::

    $ git add <nome_do_arquivo>
    $ git commit –a –m "pt_BR translation"

7. Atualize o seu repositório no servidor github com as alterações realizadas localmente::

    $ git push origin <branch>

    onde <branch> é a branch em que você está trabalhando, por exemplo, 2.4, 2.3, ...

8. O último passo é informar sobre as suas alterações ao responsável pelo repositório de origem, para realizar um pull das alterações no repositório. 

    Para isso, basta converter o seu **Issue** criado anteriormente em um **Pull Request**:

    Para facilitar o processo, primeiro instale esta excelente ferramenta (caso ainda não tenha instalado):

    https://github.com/github/hub

    E execute o comando abaixo, que irá associar o PR na branch atual ao issue, por exemplo, número 42 já existente - substituindo 42 pelo número do seu issue:

        $ hub pull-request -i 42

    Segue uma outra referência sobre esse assunto: http://www.topbug.net/blog/2012/03/25/attach-a-pull-request-to-an-existing-github-issue/


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

Glossário
~~~~~~~~~

Para mantermos consistente a tradução dos documentos, verifique no glossário os termos que não devem ser traduzidos e aqueles que devem seguir a mesma tradução:
http://andreia.github.com/symfony-docs-pt-BR/

Visualização dos documentos traduzidos
--------------------------------------

Para facilitar a revisão dos documentos, sempre que uma nova tradução é adicionada/modificada aqui no repositório ela é renderizada em:

- Versão 2.4 - http://andreiabohner.org/symfony2docs/2.4/index.html
- Versão 2.3 - http://andreiabohner.org/symfony2docs/2.3/index.html
- Versão 2.2 - http://andreiabohner.org/symfony2docs/2.2/index.html
- Versão 2.1 - http://andreiabohner.org/symfony2docs/2.1/index.html
- Versão 2.0 - http://andreiabohner.org/symfony2docs/2.0/index.html

Referências
-----------

- SSH issues: Guia contendo as soluções para os problemas mais comuns referentes a conexão SSH no GitHub (chave pública, ...): http://help.github.com/ssh-issues/
- Mencionar alguém em um ``pull request`` ou ``issue``: https://github.com/blog/1004-mention-autocompletion
