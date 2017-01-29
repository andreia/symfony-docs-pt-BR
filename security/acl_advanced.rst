.. index::
   single: Segurança; Conceitos Avançados de ACL

Como usar Conceitos Avançados de ACL
====================================

O objetivo deste capítulo é fornecer uma visão mais aprofundada do sistema de ACL, e
também explicar algumas das decisões de projeto por trás dele.

Conceitos de Projeto
--------------------

Os recursos de uma instância do objeto de segurança do Symfony2 tem base no conceito de uma
Lista de Controle de Acesso (ACL). Cada **instância** do objeto de domínio tem a sua própria ACL. A
instância ACL contém uma lista detalhada de Entradas de Controle de Acesso (ACEs), que são
usadas para tomar decisões de acesso. O sistema ACL do Symfony2 concentra-se em dois objetivos
principais:

- fornecer uma maneira de recuperar uma grande quantidade de ACLs/ACEs de forma eficiente
  para os seus objetos de domínio e para modificá-los;
- fornecer uma maneira de tomar decisões facilmente para saber se uma pessoa está autorizada a
  executar uma ação em um objeto de domínio ou não.

Conforme indicado no primeiro ponto, uma das principais capacidades do sistema ACL do
Symfony2 é uma forma de recuperar ACLs/ACEs com alto desempenho. Isto é
extremamente importante, pois cada ACL pode ter várias ACEs, e herdam
de outra ACL em forma de árvore. Portanto, nenhum ORM é utilizado, em vez disso,
a implementação padrão interage com a sua conexão diretamente usando o DBAL do
Doctrine.

Identidades de Objeto
~~~~~~~~~~~~~~~~~~~~~

O sistema de ACL é completamente desacoplado de seus objetos de domínio. Eles não
tem que ser armazenados na mesma base de dados, ou no mesmo servidor. De forma
a atingir esse desacoplamento, os seus objetos, no sistema ACL, são representados
através de objetos de identidade de objeto. Toda vez que você deseja recuperar a ACL para um
objeto de domínio, o sistema de ACL vai primeiro criar uma identidade de objeto do seu
objeto de domínio e, em seguida, passar essa identidade de objeto ao provedor ACL para
processamento adicional.


Identidades de Segurança
~~~~~~~~~~~~~~~~~~~~~~~~

Isto é análogo à identidade de objeto, mas representa um usuário ou um papel em
sua aplicação. Cada papel ou usuário tem a sua própria identidade de segurança.


Estrutura da Tabela de Banco de Dados
-------------------------------------

A implementação padrão usa cinco tabelas de banco de dados, conforme listado abaixo. Em uma aplicação típica,
as tabelas são ordenadas da com menos linhas para a com mais linhas:

- *acl_security_identities*: Esta tabela registra todas as identidades de segurança (SID)
  que detêm ACEs. A implementação padrão vem com duas identidades de
  segurança: ``RoleSecurityIdentity`` e ``UserSecurityIdentity``
- *acl_classes*: Esta classe mapeia nomes de classes para um ID único, que pode ser
  referenciado a partir de outras tabelas.
- *acl_object_identities*: Cada linha nessa tabela representa um único domínio
  de instância de objeto.
- *acl_object_identity_ancestors*: Esta tabela permite que todos os ancestrais de
  uma ACL, possam ser determinados de uma maneira muito eficiente.
- *acl_entries*: Esta tabela contém todas as ACEs. Essa é tipicamente a tabela
  com o maior número de linhas. Ela pode conter dezenas de milhões sem afetar
  significativamente o desempenho.


Escopo das Entradas de Controle de Acesso
-----------------------------------------

Entradas de controle de acesso podem ter escopos diferentes nos quais se aplicam. No
Symfony2, existem basicamente dois escopos diferentes:

- Class-Scope: Essas entradas se aplicam a todos os objetos com a mesma classe.
- Object-Scope: Este foi o escopo utilizado unicamente no capítulo anterior, e
  ele só se aplica a um objeto específico.

Às vezes, você vai encontrar a necessidade de aplicar uma ACE apenas a um campo específico do
objeto. Vamos dizer que você quer que o ID somente seja visualizado por um administrador,
mas não por seu serviço do cliente. Para resolver este problema comum, mais dois sub-escopos
foram adicionados:

- Classe-Field-Scope: Essas entradas se aplicam a todos os objetos com a mesma classe,
  mas apenas a um campo específico dos objetos.
- Object-Field-Scope: Essas entradas se aplicam a um objeto específico, e apenas a um
  campo específico do objeto.

Decisões de Pré-Autorização
---------------------------

Para as decisões de pré-autorização, que são as decisões tomadas antes de qualquer método seguro (ou
ação segura) ser invocado, o serviço AccessDecisionManager é usado.
O AccessDecisionManager também é usado para tomar decisões de autorização com base
em papéis. Assim como os papéis, o sistema de ACL adiciona vários atributos novos que podem ser
utilizados para verificar se há permissões diferentes.

Mapa de Permissão Integrado
~~~~~~~~~~~~~~~~~~~~~~~~~~~

+------------------+-------------------------------+-----------------------------+
| Atributo         | Significado Pretendido        | Bitmasks Inteiras           |
+==================+===============================+=============================+
| VIEW             | Se alguém tem permissão       | VIEW, EDIT, OPERATOR,       |
|                  | para ver o objeto de domínio. | MASTER ou OWNER             |
+------------------+-------------------------------+-----------------------------+
| EDIT             | Se alguém tem permissão       | EDIT, OPERATOR, MASTER      |
|                  | para fazer alterações no      | ou OWNER                    |
|                  | objeto de domínio.            |                             |
+------------------+-------------------------------+-----------------------------+
| CREATE           | Se alguém tem permissão       | CREATE, OPERATOR, MASTER    |
|                  | para criar o objeto de        | ou OWNER                    |
|                  | domínio.                      |                             |
+------------------+-------------------------------+-----------------------------+
| DELETE           | Se alguém tem permissão       | DELETE, OPERATOR, MASTER    |
|                  | excluir o objeto de           | ou OWNER                    |
|                  | domínio.                      |                             |
+------------------+-------------------------------+-----------------------------+
| UNDELETE         | Se alguém tem permissão para  | UNDELETE, OPERATOR, MASTER  |
|                  | restaurar o objeto de domínio | ou OWNER                    |
|                  | anteriormente excluído.       |                             |
+------------------+-------------------------------+-----------------------------+
| OPERATOR         | Se alguém tem permissão       | OPERATOR, MASTER ou OWNER   |
|                  | para executar todas as ações  |                             |
|                  | acima.                        |                             |
+------------------+-------------------------------+-----------------------------+
| MASTER           | Se alguém tem permissão       | MASTER ou OWNER             |
|                  | para executar todas as ações  |                             |
|                  | acima, e além disso é         |                             |
|                  | autorizada a conceder         |                             |
|                  | qualuqer uma das permissões   |                             |
|                  | acima para outros.            |                             |
+------------------+-------------------------------+-----------------------------+
| OWNER            | Se alguém é dono do objeto    | OWNER                       |
|                  | de domínio. Um dono pode      |                             |
|                  | executar qualquer uma das     |                             |
|                  | ações acima *e* conceder      |                             |
|                  | permissões master e owner     |                             |
+------------------+-------------------------------+-----------------------------+

Atributos de Permissão vs Bitmasks de Permissão
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Os atributos são usados ​​pelo AccessDecisionManager, assim como papéis. Muitas vezes, estes
atributos representam, de fato, um agregado de bitmasks inteiros. Bitmasks inteiros por outro
lado, são utilizados internamente pelo sistema de ACL para armazenar de forma eficiente suas
permissões de usuário no banco de dados e executar verificações de acesso usando
operações bitmask extremamente rápidas.

Extensibilidade
~~~~~~~~~~~~~~~

O mapa de permissão acima não é de forma alguma estático, e teoricamente poderia ser
completamente substituído. No entanto, ele deve cobrir a maioria dos problemas que
você encontra e para a interoperabilidade com outros bundles, você é incentivado a
manter o significado previsto por eles.

Decisões Pós-Autorização
------------------------

Decisões pós-autorização são feitas depois de um método seguro ter sido invocado,
e normalmente envolvem o objeto de domínio que é retornado por esse método.
Após a invocação de providers, também é permitido modificar ou filtrar o objeto de domínio
antes de ser devolvido.

Devido às limitações atuais da linguagem PHP, não existem
recursos de pós-autorização integrados no componente de segurança.
No entanto, existe um JMSSecurityExtraBundle_ experimental que adiciona
esses recursos. Consulte a documentação para obter mais informações sobre a
forma como isso é feito.

Processo para Tomar Decisões de Autorização
-------------------------------------------

A classe ACL fornece dois métodos para determinar se uma identidade de segurança
tem as bitmasks necessárias, ``isGranted`` e ``isFieldGranted``. Quando a ACL
recebe um pedido de autorização através de um desses métodos, ela delega
esse pedido para uma implementação de PermissionGrantingStrategy. Isso permite
a substituição da forma como as decisões de acesso são tomadas sem modificar
a própria classe ACL.

O PermissionGrantingStrategy primeiro verifica todos os seus ACEs object-scope, se nenhum
for aplicável, as ACEs class-scope serão verificadas, se nenhuma for aplicável,
em seguida, o processo vai ser repetido com as ACEs da ACL pai. Se não
existe nenhuma ACL pai, uma exceção será lançada.

.. _JMSSecurityExtraBundle: https://github.com/schmittjoh/JMSSecurityExtraBundle
