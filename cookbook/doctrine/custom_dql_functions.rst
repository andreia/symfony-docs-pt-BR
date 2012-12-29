.. index::
   single: Doctrine; Funções DQL Personalizadas

Como Registrar Funções DQL Personalizadas
=========================================

O Doctrine permite especificar funções DQL personalizadas. Para mais informações
sobre este assunto, leia o artigo "`Funções DQL Definidas pelo Usuário`_" no cookbook do Doctrine.

No Symfony, você pode registrar suas funções DQL personalizadas da seguinte forma:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        doctrine:
            orm:
                # ...
                entity_managers:
                    default:
                        # ...
                        dql:
                            string_functions:
                                test_string: Acme\HelloBundle\DQL\StringFunction
                                second_string: Acme\HelloBundle\DQL\SecondStringFunction
                            numeric_functions:
                                test_numeric: Acme\HelloBundle\DQL\NumericFunction
                            datetime_functions:
                                test_datetime: Acme\HelloBundle\DQL\DatetimeFunction

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/doctrine http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:orm>
                    <!-- ... -->
                    <doctrine:entity-manager name="default">
                        <!-- ... -->
                        <doctrine:dql>
                            <doctrine:string-function name="test_string>Acme\HelloBundle\DQL\StringFunction</doctrine:string-function>
                            <doctrine:string-function name="second_string>Acme\HelloBundle\DQL\SecondStringFunction</doctrine:string-function>
                            <doctrine:numeric-function name="test_numeric>Acme\HelloBundle\DQL\NumericFunction</doctrine:numeric-function>
                            <doctrine:datetime-function name="test_datetime>Acme\HelloBundle\DQL\DatetimeFunction</doctrine:datetime-function>
                        </doctrine:dql>
                    </doctrine:entity-manager>
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('doctrine', array(
            'orm' => array(
                ...,
                'entity_managers' => array(
                    'default' => array(
                        ...,
                        'dql' => array(
                            'string_functions' => array(
                                'test_string'   => 'Acme\HelloBundle\DQL\StringFunction',
                                'second_string' => 'Acme\HelloBundle\DQL\SecondStringFunction',
                            ),
                            'numeric_functions' => array(
                                'test_numeric' => 'Acme\HelloBundle\DQL\NumericFunction',
                            ),
                            'datetime_functions' => array(
                                'test_datetime' => 'Acme\HelloBundle\DQL\DatetimeFunction',
                            ),
                        ),
                    ),
                ),
            ),
        ));

.. _`Funções DQL Definidas pelo Usuário`: http://docs.doctrine-project.org/projects/doctrine-orm/en/2.1/cookbook/dql-user-defined-functions.html
