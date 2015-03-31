.. index::
   single: Sessões, Proxy de Sessão, Proxy

Exemplos de Proxy de Sessão
===========================

O mecanismo de proxy de sessão possui uma variedade de usos e este exemplo demonstra
dois usos comuns. Ao invés de injetar o manipulador de sessão como habitualmente, um manipulador
é injetado no proxy e registrado com o driver de armazenamento de sessão::

    use Symfony\Component\HttpFoundation\Session\Session;
    use Symfony\Component\HttpFoundation\Session\Storage\NativeSessionStorage;
    use Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler;

    $proxy = new YourProxy(new PdoSessionHandler());
    $session = new Session(new NativeSessionStorage(array(), $proxy));

Abaixo, você vai aprender dois exemplos reais que podem ser utilizados para ``YourProxy``:
criptografia de dados de sessão e sessões somente leitura para visitantes.

Criptografia de Dados da Sessão
-------------------------------

Se quiser criptografar os dados da sessão, você pode usar o proxy para criptografar
e descriptografar a sessão conforme necessário::

    use Symfony\Component\HttpFoundation\Session\Storage\Proxy\SessionHandlerProxy;

    class EncryptedSessionProxy extends SessionHandlerProxy
    {
        private $key;

        public function __construct(\SessionHandlerInterface $handler, $key)
        {
            $this->key = $key;

            parent::__construct($handler);
        }

        public function read($id)
        {
            $data = parent::read($id);

            return mcrypt_decrypt(\MCRYPT_3DES, $this->key, $data);
        }

        public function write($id, $data)
        {
            $data = mcrypt_encrypt(\MCRYPT_3DES, $this->key, $data);

            return parent::write($id, $data);
        }
    }

Sessões Somente Leitura para Visitantes
---------------------------------------

Existem algumas aplicações onde uma sessão é necessária para usuários visitantes, mas
não existe necessidade particular para persistir a sessão. Nesse caso você
pode interceptar a sessão antes dela ser gravada::

    use Foo\User;
    use Symfony\Component\HttpFoundation\Session\Storage\Proxy\SessionHandlerProxy;

    class ReadOnlyGuestSessionProxy extends SessionHandlerProxy
    {
        private $user;

        public function __construct(\SessionHandlerInterface $handler, User $user)
        {
            $this->user = $user;

            parent::__construct($handler);
        }

        public function write($id, $data)
        {
            if ($this->user->isGuest()) {
                return;
            }

            return parent::write($id, $data);
        }
    }
