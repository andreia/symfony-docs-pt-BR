.. index::
   single: Tests; Database

Como testar código que interage com Banco de dados
==================================================

Se seu código interage com banco de dados, ex. lê dados dele ou armazena dados
nele, você precisa ajustar seus testes para levar isso em conta. Existem várias
formas de lidar com isso. Em um teste de unidade, você pode criar um mock para
um ``Repository`` e usá-lo para retornar objetos esperados. Em um teste funcional,
você deve precisar preparar um banco de dados de teste com valores predefinidos a 
fim de garantir que seu teste sempre trabalhe com os mesmos dados.

.. note::

    Se você quiser testar suas consultas diretamente, veja :doc:`/cookbook/testing/doctrine`.

Mockando o ``Repository`` em um teste de unidade
------------------------------------------------

Se você quer testar código que depende de um repósitorio Doctrine isoladamente,
você precisa mockar o ``Repository``. Normalmente você injeta o ``EntityManager``
nas suas classes e o usa para pegar o repositório. Isso torna as coisas um pouco
mais difíceis, pois você precisa mockar o ``EntityManager`` e sua classe de 
repositório.

.. tip::

    É possível (e uma boa ideia) injetar seu repositório diretamente registrando-o
    como um :doc:`factory service </components/dependency_injection/factories>`.
    É mais trabalhoso para configurar, mas torna os testes mais fáceis, pois você
    precisar mockar apenas o repositório.
    
Suponha que a classe que você quer testar se pareça com a seguinte::

    namespace AppBundle\Salary;

    use Doctrine\Common\Persistence\ObjectManager;

    class SalaryCalculator
    {
        private $entityManager;

        public function __construct(ObjectManager $entityManager)
        {
            $this->entityManager = $entityManager;
        }

        public function calculateTotalSalary($id)
        {
            $employeeRepository = $this->entityManager
                ->getRepository('AppBundle:Employee');
            $employee = $employeeRepository->find($id);

            return $employee->getSalary() + $employee->getBonus();
        }
    }

Uma vez que o ``ObjectManager`` é injetado na classe através do construtor,
é fácil passar um objeto mock dentro de um teste::

    use AppBundle\Salary\SalaryCalculator;

    class SalaryCalculatorTest extends \PHPUnit_Framework_TestCase
    {
        public function testCalculateTotalSalary()
        {
            // First, mock the object to be used in the test
            $employee = $this->getMock('\AppBundle\Entity\Employee');
            $employee->expects($this->once())
                ->method('getSalary')
                ->will($this->returnValue(1000));
            $employee->expects($this->once())
                ->method('getBonus')
                ->will($this->returnValue(1100));

            // Now, mock the repository so it returns the mock of the employee
            $employeeRepository = $this
                ->getMockBuilder('\Doctrine\ORM\EntityRepository')
                ->disableOriginalConstructor()
                ->getMock();
            $employeeRepository->expects($this->once())
                ->method('find')
                ->will($this->returnValue($employee));

            // Last, mock the EntityManager to return the mock of the repository
            $entityManager = $this
                ->getMockBuilder('\Doctrine\Common\Persistence\ObjectManager')
                ->disableOriginalConstructor()
                ->getMock();
            $entityManager->expects($this->once())
                ->method('getRepository')
                ->will($this->returnValue($employeeRepository));

            $salaryCalculator = new SalaryCalculator($entityManager);
            $this->assertEquals(2100, $salaryCalculator->calculateTotalSalary(1));
        }
    }

Neste exemplo, você está construindo os mocks de dentro pra fora, primeiro criando
o employee que é retornado pelo ``Repository``, que por sua vez é retornado pelo
``EntityManager``. Desta forma, nenhuma classe real é envolvida nos testes.

Alterando configurações do banco de dados para testes funcionais
----------------------------------------------------------------
Se vocẽ tem testes funcionais, você vai querer interagir com um banco de dados 
real. Na maior parte do tempo você vai querer usar uma conexão de banco de dados 
dedicada para ter certeza de não sobrescrever dados inseridos durante o desenvolvimento
da aplicação e também para ser capaz de limpar o banco de dados após cada teste.

Para fazer isso, você pode especificar uma configuração de banco de dados que sobrescreve
a configuração padrão.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_test.yml
        doctrine:
            # ...
            dbal:
                host:     localhost
                dbname:   testdb
                user:     testdb
                password: testdb

    .. code-block:: xml

        <!-- app/config/config_test.xml -->
        <doctrine:config>
            <doctrine:dbal
                host="localhost"
                dbname="testdb"
                user="testdb"
                password="testdb"
            />
        </doctrine:config>

    .. code-block:: php

        // app/config/config_test.php
        $configuration->loadFromExtension('doctrine', array(
            'dbal' => array(
                'host'     => 'localhost',
                'dbname'   => 'testdb',
                'user'     => 'testdb',
                'password' => 'testdb',
            ),
        ));

Certifique-se que seu banco de dados roda no localhost e tem o banco de dados
definido e as credenciais de usuários configuradas.
