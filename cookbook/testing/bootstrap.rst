Como customizar o processo de Bootstrap antes de rodar os testes
================================================================

Algumas vezes quando testes são rodados, você precisa fazer trabalho adicional
de inicialização antes de rodá-los. Por exemplo, se você esta rodando um teste funcional
e foi introduzido um novo recurso de tradução, então você vai precisar limpar seu cache
antes de rodar os testes. Este cookbook aborda como fazer isso.

Primeiramente, adicione o seguinte arquivo::

    // app/tests.bootstrap.php
    if (isset($_ENV['BOOTSTRAP_CLEAR_CACHE_ENV'])) {
        passthru(sprintf(
            'php "%s/console" cache:clear --env=%s --no-warmup',
            __DIR__,
            $_ENV['BOOTSTRAP_CLEAR_CACHE_ENV']
        ));
    }

    require __DIR__.'/bootstrap.php.cache';

Substitua o arquivo boostrap de teste ``bootstrap.php.cache`` em ``app/phpunit.xml.dist``
com ``tests.bootstrap.php``:

.. code-block:: xml

    <!-- app/phpunit.xml.dist -->

    <!-- ... -->
    <phpunit
        ...
        bootstrap = "tests.bootstrap.php"
    >

Agora, você pode definir em seu arquivo ``phpunit.xml.dist`` de qual ambiente você quer
que seja limpado o cache:

.. code-block:: xml

    <!-- app/phpunit.xml.dist -->
    <php>
        <env name="BOOTSTRAP_CLEAR_CACHE_ENV" value="test"/>
    </php>

Isto torna-se agora uma variável de ambiente (i.e. ``$_ENV``) que está disponpivel
no arquivo bootstrap personalizado (``tests.bootstrap.php``).
