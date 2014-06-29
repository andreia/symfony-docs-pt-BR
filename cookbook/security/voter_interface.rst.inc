.. code-block:: php

    interface VoterInterface
    {
        public function supportsAttribute($attribute);
        public function supportsClass($class);
        public function vote(TokenInterface $token, $object, array $attributes);
    }

O método :method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface::supportsAttribute`
é usado para verificar se o voter suporta o atributo de usuário fornecido (ou seja:
um papel - role - como ``ROLE_USER``, um ACL ``EDIT``, etc.).

O método :method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface::supportsClass`
é usado para verificar se o voter suporta a classe do objeto cujo
acesso está sendo verificado.

O método :method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface::vote`
deve implementar a lógica de negócio que verifica se o
usuário tem acesso. Esse método deve retornar um dos seguintes valores:

* ``VoterInterface::ACCESS_GRANTED``: A autorização será concedida por este voter;
* ``VoterInterface::ACCESS_ABSTAIN``: O voter não pode decidir se a autorização deve ser concedida;
* ``VoterInterface::ACCESS_DENIED``: A autorização será negada por este voter.
