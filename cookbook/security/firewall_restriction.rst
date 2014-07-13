.. index::
   single: Segurança; Restringir Firewalls de Segurança a uma Requisição

Como Restringir Firewalls a uma Requisição Específica
=====================================================

Ao usar o componente de Segurança, você pode criar firewalls que correspondam a determinadas opções da solicitação.
Na maioria dos casos, fazer a comparação com a URL é suficiente, mas em casos especiais é possível ainda
restringir a inicialização de um firewall de acordo com outras opções da requisição.

.. note::

    Você pode usar qualquer uma dessas restrições individualmente ou misturá-las em conjunto para obter
    a configuração de firewall desejada.

Restringir por Padrão
---------------------

Esta é a restrição padrão e restringe um firewall para apenas ser inicializado se a URL da solicitação
corresponde ao ``pattern``configurado.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml

        # ...
        security:
            firewalls:
                secured_area:
                    pattern: ^/admin
                    # ...

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" pattern="^/admin">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ...
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern' => '^/admin',
                    // ...
                ),
            ),
        ));

O ``pattern`` é uma expressão regular. Nesse exemplo, o firewall só será
ativado se a URL começa (devido ao caracter regex ``^``) com ``/admin``. Se
a URL não corresponde a esse padrão, o firewall não será ativado e firewalls
posteriores terão a oportunidade de corresponder a esta requisição.

Restringindo pelo Host
----------------------

.. versionadded:: 2.4
    O suporte para restringir firewalls de segurança a um host específico foi introduzido no
    Symfony 2.4.

Se corresponder somente com ``pattern`` não é suficiente, a requisição também pode ser comparada com
``host``. Quando a opção de configuração ``host`` é definida, o firewall será restrito a
só inicializar se o host da requisição corresponde com a configuração.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml

        # ...
        security:
            firewalls:
                secured_area:
                    host: ^admin\.example\.com$
                    # ...

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" host="^admin\.example\.com$">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php

        // ...
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'host' => '^admin\.example\.com$',
                    // ...
                ),
            ),
        ));

O ``host`` (como o ``pattern``) é uma expressão regular. Nesse exemplo,
o firewall só será ativado se o host é exatamente igual (devido aos
caracteres regex ``^`` e ``$``) ao hostname ``admin.example.com``.
Se o hostname não corresponder a esse padrão, o firewall não será ativado
e os firewalls subseqüentes terão a oportunidade de corresponder a essa
requisição.
