.. index::
   single: Requisitos
   
Requisitos para o funcionamento Symfony2
=================================

Para funcionar o Symfony2, seu sistema precisa seguir uma lista de requisitos. Você pode
facilmente verificar se o sistema passa por todos os requisitos executando o ``web/config.php``
em sua distribuição Symfony. Desde a CLI, muitas vezes usa-se um arquivo de configuração 
do ``php.ini`` é diferente, também é interessante verificar as usuas necessidade para a linha de
comando via:

.. code-block:: bash

    php app/check.php

Abaixo está a lista de requisitos obrigatórios e opcionais.

Obrigatório
---------

* Você precisa ter pelo menos a versão 5.3.2 do PHP
* JSON precisa estar habilitado
* ctype precisa estar habilitado
* Seu PHP.ini precisa ter o date.timezone definido

Opcional
--------

* Você precisa ter o módulo PHP-XML instalado
* Você precisa ter pelo menos a versão 2.6.11 da libxml
* PHP tokenizer precisa estar habilitado
* As funções da mbstring precisam estar habilitadas
* iconv precisa estar habilitado
* POSIX precisa estar habilitado (apenas no \*nix)
* Intl precisa estar instalado com ICU 4+
* APC 3.0.17+ (ou outro apcode cache precisa estar instalado)
* Configurações recomendadas no PHP.ini

    * short_open_tags: off
    * magic_quotes_gpc: off
    * register_globals: off
    * session.autostart: off
    
Doctrine
--------

Se você quer usar o Doctrine, você precisará ter a PDO instalada. Além disso,
você precisa ter o driver PDO para o servidor de banco de dados que você deseja usar.