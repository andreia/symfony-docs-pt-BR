.. index::
   single: Segurança; Forçar HTTPS

Como forçar HTTPS ou HTTP para URLs Diferentes
==============================================

Através da configuração de segurança, você pode forçar que áreas do seu site usem o
protocolo ``HTTPS``. Isso é feito através das regras ``access_control`` usando a opção
``requires_channel``. Por exemplo, se você quiser forçar que todas as URLs que começam
com ``/secure`` utilizem ``HTTPS`` então você poderia usar a seguinte configuração:

.. configuration-block::

        .. code-block:: yaml

            access_control:
                - path: ^/secure
                  roles: ROLE_ADMIN
                  requires_channel: https

        .. code-block:: xml

            <access-control>
                <rule path="^/secure" role="ROLE_ADMIN" requires_channel="https" />
            </access-control>

        .. code-block:: php

            'access_control' => array(
                array(
                    'path'             => '^/secure',
                    'role'             => 'ROLE_ADMIN',
                    'requires_channel' => 'https',
                ),
            ),

O próprio formulário de login precisa permitir acesso anônimo, caso contrário, os usuários
não seriam capazes de se autenticar. Para forçá-lo a usar ``HTTPS`` você pode ainda usar
regras ``access_control`` com o papel 
``IS_AUTHENTICATED_ANONYMOUSLY``:

.. configuration-block::

        .. code-block:: yaml

            access_control:
                - path: ^/login
                  roles: IS_AUTHENTICATED_ANONYMOUSLY
                  requires_channel: https

        .. code-block:: xml

            <access-control>
                <rule path="^/login" 
                      role="IS_AUTHENTICATED_ANONYMOUSLY" 
                      requires_channel="https" />
            </access-control>

        .. code-block:: php

            'access_control' => array(
                array(
                    'path'             => '^/login',
                    'role'             => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'requires_channel' => 'https',
                ),
            ),

Também é possível especificar o uso de ``HTTPS`` na configuração de roteamento.
Veja :doc:`/cookbook/routing/scheme` para mais detalhes.
