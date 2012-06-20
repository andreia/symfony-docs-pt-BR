Padrões de Codificação
======================

Para contribuir com código para o Symfony2, você deve seguir seus padrões de
codificação. Para encurtar a história, esta é a regra de ouro: **Imite o código
existente no Symfony2**. Muitos Bundles e bilbiotecas usados pelo Symfony2
também seguem as mesmas regras, e você também deveria.

Lembre-se que a principal vantagem de padrões é que cada pedaço de código parece
familiar, não é sobre isso ou aquilo ser mais legível.

Como uma imagem — ou código — diz mais que mil palavras, aqui está um exemplo
curto, contendo a maior parte dos aspectos descrito abaixo.

.. code-block:: php

    <?php

    /*
     * This file is part of the Symfony package.
     *
     * (c) Fabien Potencier <fabien@symfony.com>
     *
     * For the full copyright and license information, please view the LICENSE
     * file that was distributed with this source code.
     */

    namespace Acme;

    class Foo
    {
        const SOME_CONST = 42;

        private $foo;

        /**
         * @param string $dummy Some argument description
         */
        public function __construct($dummy)
        {
            $this->foo = $this->transform($dummy);
        }

        /**
         * @param string $dummy Some argument description
         * @return string|null Transformed input
         */
        private function transform($dummy)
        {
            if (true === $dummy) {
                return;
            }
            if ('string' === $dummy) {
                $dummy = substr($dummy, 0, 5);
            }

            return $dummy;
        }
    }


Estrutura
---------

* Nunca use *short tags* (`<?`);

* Não termine arquivos de classe com o a tag `?>` de costume;

* Indentação é feita em passos de 4 espaços (tabulações nunca são permitidas);

* Use o caractere de nova linha (LF ou `0x0A`) para encerrar linhas;

* Coloque um único espaço depois de cada vírgula;

* Não coloque espaços depois de abrir parênteses ou antes de fechá-los;

* Coloque um único espaço em volta de operadores (`==`, `&&`, …);

* Coloque um único espaço antes de abrir parênteses de uma palavra de controle
(`if`, `else`, `for`, `while`, …);

* Coloque uma linha em braco antes de uma declaração `return`, a não ser que a
declaração esteja sozinha dentro de um bloco (como dentro de um `if`);

* Não acrescente espaços no final das linhas;

* Use chaves para indicar o corpo de estruturas de controle, não importando o
númedo de declarações que ele contém;

* Coloque chaves nas suas prórpias linhas na declaração de classes, métodos e funções;

* Separe as declarações de estruturas de controle (`if`, `else`, …) e suas
chaves de abertura com um único espaço e nenhuma linha em branco;

* Declare explicitamente a visibilidade de classes, métodos e propriedades. O
uso de `var` é proibido;

* Use as constantes nativas do PHP em caixa baixa: `false`, `true` e `null`. O
mesmo vale para `array()`;

* Use caixa alta para constantes, com as palavras separadas por `_`;

* Defina uma classe por arquivo;

* Declare as propriedades da classe antes dos métodos;

* Declare métodos públicos primeiro, depois os protegidos e, finalmente, os
privados.


Padrões de nomeação
-------------------

* Use camelCase, não underscores (`_`), para variáveis, funções e métodos;

* Use underscores para nomes de opções, argumentos e parâmetros;

* Use namespaces em todas as classes;

* Sufixe nomes de interface com `Interface`;

* Use caracteres alfanuméricos e underscores para nomes de arquivos;

* Não se esqueça de ver no documento mais explicativo :doc:`conventions` para
considerações de nomeação mais subjetivas.


Documentação
------------

* Insira blocks PHPDoc para todas as classes, métodos e funções;

* Omita a tag `@return` se o método não  retorna nada;

* As anotações `@package` e `@subpackage` não são usadas.


Licença
-------

* Symfony é distribuido sob a licença MIT, e o bloco de licença deve estar
presente no topo de todo arquivo PHP, antes do namespace.
