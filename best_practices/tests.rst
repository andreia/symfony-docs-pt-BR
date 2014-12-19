Testes
======

Em termos gerais, existem dois tipos de teste. O teste de unidade permite
testar a entrada e saída de funções específicas. O teste funcional permite comandar
um "browser", onde é possível navegar pelas páginas do seu site, clicar em
links, preencher formulários e afirmar que existem certas coisas na página.

Testes de Unidade (ou Testes Unitários)
---------------------------------------

Os testes de unidade são usados ​​para testar a sua "lógica de negócios", que deve residir em
classes que são independentes do Symfony. Por essa razão, o Symfony realmente não
tem uma opinião sobre as ferramentas que você usa para testes de unidade. No entanto, as
ferramentas mais populares são `PhpUnit`_ e `PhpSpec`_.

Testes Funcionais
-----------------

Criar bons testes funcionais realmente pode ser difícil, então, alguns desenvolvedores ignoram
estes completamente. Não ignore os testes funcionais! Ao definir alguns testes funcionais
*simples*, você pode detectar rapidamente quaisquer grandes erros antes de implantá-los:

.. best-practice::

    Defina um teste funcional, que, pelo menos, verifique se as páginas da sua aplicação
    foram carregadas com sucesso.

Um teste funcional pode ser simples assim:

.. code-block:: php

    /** @dataProvider provideUrls */
    public function testPageIsSuccessful($url)
    {
        $client = self::createClient();
        $client->request('GET', $url);

        $this->assertTrue($client->getResponse()->isSuccessful());
    }

    public function provideUrls()
    {
        return array(
            array('/'),
            array('/posts'),
            array('/post/fixture-post-1'),
            array('/blog/category/fixture-category'),
            array('/archives'),
            // ...
        );
    }

Esse código verifica se todas as URLs informadas foram carregadas com sucesso, significando que
seu código de status de resposta HTTP está entre ``200`` e ``299``. Isso pode
não parecer muito útil, mas levando em consideração o pouco esforço necessário, vale a pena
ter em sua aplicação.

Em software de computador, esse tipo de teste é chamado `teste de fumaça`_ e consiste
de *"testes preliminares para revelar falhas simples graves o suficiente para rejeitar um
potencial lançamento de software"*.

URLs no próprio código em um Teste Funcional
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alguns de vocês podem estar se perguntando por que o teste funcional anterior não usa um
serviço gerador de URL:

.. best-practice::

    Utilizar, nos testes funcionais, as URLs no próprio código em vez de usar um gerador
    de URL.

Considere o seguinte teste funcional que usa o serviço ``router`` para
gerar a URL da página testada:

.. code-block:: php

    public function testBlogArchives()
    {
        $client = self::createClient();
        $url = $client->getContainer()->get('router')->generate('blog_archives');
        $client->request('GET', $url);

        // ...
    }

Isso irá funcionar, mas tem uma *enorme* desvantagem. Se um desenvolvedor altera por engano
o caminho da rota ``blog_archives``, o teste ainda vai passar,
mas a URL original (antiga) não vai funcionar! Isso significa que quaisquer bookmarks para
aquela URL serão quebrados e você vai perder qualquer ranking da página nas engines de busca.

Testando Funcionalidade JavaScript
----------------------------------

O cliente de teste funcional integrado é ótimo, mas não pode ser usado para
testar qualquer comportamento JavaScript em suas páginas. Se você precisa testar isso, considere
usar a biblioteca `Mink`_ do PHPUnit.

É claro que, se você tem um frontend JavaScript pesado, deve considerar o uso
ferramentas de teste em JavaScript puro.

Saiba mais sobre Testes Funcionais
----------------------------------

Considere o uso das bibliotecas `Faker`_ e `Alice`_ para gerar dados parecidos com os reais 
para os seus dados de ensaio de teste.

.. _`Faker`: https://github.com/fzaninotto/Faker
.. _`Alice`: https://github.com/nelmio/alice
.. _`PhpUnit`: https://phpunit.de/
.. _`PhpSpec`: http://www.phpspec.net/
.. _`Mink`: http://mink.behat.org
.. _`teste de fumaça`: http://en.wikipedia.org/wiki/Smoke_testing_(software)
