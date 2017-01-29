.. index::
    single: Servidor Web

Configurando um Servidor Web
============================

A forma preferida para desenvolver sua aplicação Symfony é usar
o :doc:`servidor web interno do PHP </cookbook/web_server/built_in>`. Contudo,
ao utilizar uma versão do PHP mais antiga ou ao executar a aplicação no ambiente
de produção, você vai precisar usar um servidor web completo. Este artigo
descreve várias maneiras de usar o Symfony com o Apache ou Nginx.

Ao usar o Apache, você pode configurar o PHP como um
:ref:`módulo do Apache <web-server-apache-mod-php>` ou com FastCGI usando
:ref:`PHP FPM <web-server-apache-fpm>`. FastCGI também é a forma preferida
para usar o PHP :ref:`com o Nginx <web-server-nginx>`.

.. sidebar:: O Diretório Web

    O diretório web é o home de todos os arquivos públicos e estáticos
    da sua aplicação, incluindo os arquivos de imagens, folhas de estilo e JavaScript.
    É também onde os front controllers (``app.php`` e ``app_dev.php``) residem.

    O diretório web serve como raiz dos documentos ao configurar o
    servidor web. Nos exemplos abaixo, o diretório ``web/`` será o
    raiz dos documentos. Esse diretório é ``/var/www/project/web/``.

    Se o seu provedor de hospedagem exige que você mude o diretório ``web/`` para
    outro local (por exemplo, ``public_html/``), certifique-se de
    :ref:`sobrescrever a localização do diretório web/  <override-web-dir>`.

.. _web-server-apache-mod-php:

Apache com mod_php/PHP-CGI
--------------------------

A **configuração mínima** para fazer sua aplicação executar no Apache é:

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        DocumentRoot /var/www/project/web
        <Directory /var/www/project/web>
            AllowOverride All
            Order Allow,Deny
            Allow from All
        </Directory>

        # uncomment the following lines if you install assets as symlinks
        # or run into problems when compiling LESS/Sass/CoffeScript assets
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

.. tip::

    Se o seu sistema suporta a variável ``APACHE_LOG_DIR``, você pode querer
    usar ``${APACHE_LOG_DIR}/`` em vez de deixar fixo ``/var/log/apache2/``.

Use a seguinte **configuração otimizada** para desativar o suporte ao ``.htaccess``
e aumentar o desempenho do servidor web:

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        DocumentRoot /var/www/project/web
        <Directory /var/www/project/web>
            AllowOverride None
            Order Allow,Deny
            Allow from All

            <IfModule mod_rewrite.c>
                Options -MultiViews
                RewriteEngine On
                RewriteCond %{REQUEST_FILENAME} !-f
                RewriteRule ^(.*)$ app.php [QSA,L]
            </IfModule>
        </Directory>

        # uncomment the following lines if you install assets as symlinks
        # or run into problems when compiling LESS/Sass/CoffeScript assets
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        # optionally disable the RewriteEngine for the asset directories
        # which will allow apache to simply reply with a 404 when files are
        # not found instead of passing the request into the full symfony stack
        <Directory /var/www/project/web/bundles>
            <IfModule mod_rewrite.c>
                RewriteEngine Off
            </IfModule>
        </Directory>
        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

.. tip::

    Se você estiver usando **php-cgi**, o Apache, por padrão, não passa nome de usuário e
    senha HTTP básico para o PHP. Para contornar essa limitação, você deve usar
    o seguinte trecho de configuração:

    .. code-block:: apache

        RewriteRule .* - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization}]

Usando mod_php/PHP-CGI com o Apache 2.4
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No Apache 2.4, ``Order Allow,Deny`` foi substituído por ``Require all granted``.
Por isso, é necessário modificar suas configurações de permissão ``Directory`` da seguinte forma:

.. code-block:: apache

    <Directory /var/www/project/web>
        Require all granted
        # ...
    </Directory>

Para as opções de configurações avançadas do Apache, leia a `documentação do Apache`_ oficial.

.. _web-server-apache-fpm:

Apache com PHP-FPM
-------------------

Para usar o PHP5-FPM com o Apache, você primeiro tem que garantir que tem
o gerenciador de processo FastCGI ``php-fpm`` binário e o módulo FastCGI do Apache
instalado (por exemplo, em um sistema baseado em Debian você tem que instalar o
os pacotes ``libapache2-mod-fastcgi`` e ``php5-fpm``).

O PHP-FPM usa os chamados *pools* para lidar com as requisições FastCGI recebidas. Você pode
configurar um número arbitrário de pools na configuração FPM. Em um pool
você configura um socket TCP (IP e porta) ou um socket de domínio Unix para
escutar. Cada pool também pode ser executado sob um UID e GID diferente:

.. code-block:: ini

    ; a pool called www
    [www]
    user = www-data
    group = www-data

    ; use a unix domain socket
    listen = /var/run/php5-fpm.sock

    ; or listen on a TCP socket
    listen = 127.0.0.1:9000

Usando mod_proxy_fcgi com Apache 2.4
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Se você estiver executando o Apache 2.4, pode facilmente usar ``mod_proxy_fcgi`` para passar
as requisições de entrada para o PHP-FPM. Configure o PHP-FPM para escutar em um socket TCP
(``mod_proxy`` atualmente `não suporta sockets Unix`_), ative ``mod_proxy``
e ``mod_proxy_fcgi`` na sua configuração do Apache e use a diretiva ``SetHandler``
para passar as requisições aos arquivos PHP para o PHP FPM:

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        # Uncomment the following line to force Apache to pass the Authorization
        # header to PHP: required for "basic_auth" under PHP-FPM and FastCGI
        #
        # SetEnvIfNoCase ^Authorization$ "(.+)" HTTP_AUTHORIZATION=$1

        # For Apache 2.4.9 or higher
        # Using SetHandler avoids issues with using ProxyPassMatch in combination
        # with mod_rewrite or mod_autoindex
        <FilesMatch \.php$>
            SetHandler proxy:fcgi://127.0.0.1:9000
        </FilesMatch>

        # If you use Apache version below 2.4.9 you must consider update or use this instead
        # ProxyPassMatch ^/(.*\.php(/.*)?)$ fcgi://127.0.0.1:9000/var/www/project/web/$1

        # If you run your Symfony application on a subpath of your document root, the
        # regular expression must be changed accordingly:
        # ProxyPassMatch ^/path-to-app/(.*\.php(/.*)?)$ fcgi://127.0.0.1:9000/var/www/project/web/$1

        DocumentRoot /var/www/project/web
        <Directory /var/www/project/web>
            # enable the .htaccess rewrites
            AllowOverride All
            Require all granted
        </Directory>

        # uncomment the following lines if you install assets as symlinks
        # or run into problems when compiling LESS/Sass/CoffeScript assets
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

PHP-FPM com Apache 2.2
~~~~~~~~~~~~~~~~~~~~~~

No Apache 2.2 ou inferior, você não pode usar ``mod_proxy_fcgi``. Ao invés, você tem
que usar a diretiva `FastCgiExternalServer`_. Portanto, a configuração do Apache
deve ser semelhante a seguinte:

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        AddHandler php5-fcgi .php
        Action php5-fcgi /php5-fcgi
        Alias /php5-fcgi /usr/lib/cgi-bin/php5-fcgi
        FastCgiExternalServer /usr/lib/cgi-bin/php5-fcgi -host 127.0.0.1:9000 -pass-header Authorization

        DocumentRoot /var/www/project/web
        <Directory /var/www/project/web>
            # enable the .htaccess rewrites
            AllowOverride All
            Order Allow,Deny
            Allow from all
        </Directory>

        # uncomment the following lines if you install assets as symlinks
        # or run into problems when compiling LESS/Sass/CoffeScript assets
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

Se você preferir usar um socket Unix, você tem que usar a opção ``-socket``
ao invés:

.. code-block:: apache

    FastCgiExternalServer /usr/lib/cgi-bin/php5-fcgi -socket /var/run/php5-fpm.sock -pass-header Authorization

.. _web-server-nginx:

Nginx
-----

A **configuração mínima** para fazer sua aplicação executar no Nginx é:

.. code-block:: nginx

    server {
        server_name domain.tld www.domain.tld;
        root /var/www/project/web;

        location / {
            # try to serve file directly, fallback to app.php
            try_files $uri /app.php$is_args$args;
        }
        # DEV
        # This rule should only be placed on your development environment
        # In production, don't include this and don't deploy app_dev.php or config.php
        location ~ ^/(app_dev|config)\.php(/|$) {
            fastcgi_pass unix:/var/run/php5-fpm.sock;
            fastcgi_split_path_info ^(.+\.php)(/.*)$;
            include fastcgi_params;
            # When you are using symlinks to link the document root to the
            # current version of your application, you should pass the real
            # application path instead of the path to the symlink to PHP
            # FPM.
            # Otherwise, PHP's OPcache may not properly detect changes to
            # your PHP files (see https://github.com/zendtech/ZendOptimizerPlus/issues/126
            # for more information).
            fastcgi_param  SCRIPT_FILENAME  $realpath_root$fastcgi_script_name;
            fastcgi_param DOCUMENT_ROOT $realpath_root;
        }
        # PROD
        location ~ ^/app\.php(/|$) {
            fastcgi_pass unix:/var/run/php5-fpm.sock;
            fastcgi_split_path_info ^(.+\.php)(/.*)$;
            include fastcgi_params;
            # When you are using symlinks to link the document root to the
            # current version of your application, you should pass the real
            # application path instead of the path to the symlink to PHP
            # FPM.
            # Otherwise, PHP's OPcache may not properly detect changes to
            # your PHP files (see https://github.com/zendtech/ZendOptimizerPlus/issues/126
            # for more information).
            fastcgi_param  SCRIPT_FILENAME  $realpath_root$fastcgi_script_name;
            fastcgi_param DOCUMENT_ROOT $realpath_root;
            # Prevents URIs that include the front controller. This will 404:
            # http://domain.tld/app.php/some-path
            # Remove the internal directive to allow URIs like this
            internal;
        }

        error_log /var/log/nginx/project_error.log;
        access_log /var/log/nginx/project_access.log;
    }

.. note::

    Dependendo de sua configuração PHP-FPM, o ``fastcgi_pass`` também pode ser
    ``fastcgi_pass 127.0.0.1:9000``.

.. tip::

    Isso executa **apenas** ``app.php``, ``app_dev.php`` e ``config.php`` no
    diretório web. Todos os outros arquivos serão servidos como texto. Você **deve**
    também certificar-se de que se você *fez* a implantação de ``app_dev.php`` ou ``config.php``,
    que esses arquivos estão protegidos e não disponíveis para qualquer usuário externo (o
    código de verificação de endereços IP na parte superior de cada arquivo faz isso por padrão).

    Se você tiver outros arquivos PHP em seu diretório web que precisam ser executados,
    certifique-se de incluí-los no bloco ``location`` acima.

Para opções de configuração Nginx avançadas, leia a `documentação do Nginx`_ oficial.

.. _`documentação do Apache`: http://httpd.apache.org/docs/
.. _`não suporta sockets Unix`: https://bz.apache.org/bugzilla/show_bug.cgi?id=54101
.. _`FastCgiExternalServer`: http://www.fastcgi.com/mod_fastcgi/docs/mod_fastcgi.html#FastCgiExternalServer
.. _`documentação do Nginx`: http://wiki.nginx.org/Symfony
