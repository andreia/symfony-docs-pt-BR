.. index::
   single: Segurança; Provedores Pré-Autenticados

Usando Firewalls de Segurança Pré-Autenticados
==============================================

Muitos módulos de autenticação já são fornecidos por alguns servidores web,
incluindo o Apache. Esses módulos geralmente definem algumas variáveis ​​de ambiente
que podem ser usadas para determinar qual usuário está acessando a sua aplicação. O Symfony
suporta a maioria dos mecanismos de autenticação, prontos para o uso.
Esses pedidos são chamados de pedidos *pré-autenticados* porque o usuário já está
autenticado quando chegar a sua aplicação.

Autenticação de Certificado de Cliente X.509
--------------------------------------------

Ao usar certificados de cliente, o seu servidor web estará fazendo todo o processo de
autenticação. Com o Apache, por exemplo, você usaria a
diretiva ``SSLVerifyClient Require``.

Ative a autenticação x509 para um firewall em particular na configuração de segurança:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    pattern: ^/
                    x509:
                        provider: your_user_provider

    .. code-block:: xml

        <?xml version="1.0" ?>
        <!-- app/config/security.xml -->
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services">

            <config>
                <firewall name="secured_area" pattern="^/">
                    <x509 provider="your_user_provider"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern' => '^/'
                    'x509'    => array(
                        'provider' => 'your_user_provider',
                    ),
                ),
            ),
        ));

Por padrão, o firewall fornece a variável ``SSL_CLIENT_S_DN_Email`` para
o provedor de usuário, e define a ``SSL_CLIENT_S_DN`` como credenciais na
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\PreAuthenticatedToken`.
Você pode sobrescrever elas definindo as chaves ``user`` e ``credentials``
respectivamente, na configuração do firewall x509.

.. note::

    Um provedor de autenticação só vai informar o provedor de usuário do username
    que fez o pedido. Você terá que criar (ou usar) um "provedor de usuário" que
    é referenciado pelo parâmetro de configuração ``provider`` (``your_user_provider``
    no exemplo de configuração). Esse provedor transformará o username para um objeto User
    de sua escolha. Para mais informações sobre como criar ou configurar um provedor de
    usuário, veja:

    * :doc:`/cookbook/security/custom_provider`
    * :doc:`/cookbook/security/entity_provider`
