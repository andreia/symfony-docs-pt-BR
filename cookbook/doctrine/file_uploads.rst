.. index::
   single: Doctrine; Upload de Arquivos


Como Manipular o Upload de Arquivos com o Doctrine
==================================================

Gerenciar o upload de arquivos utilizando entidades do Doctrine não é diferente de manusear
qualquer outro upload de arquivo. Em outras palavras, você é livre para mover o arquivo em seu
controlador após a manipulação do envio de um formulário. Para exemplos de como fazer isso,
veja a página de :doc:`referência do tipo arquivo</reference/forms/types/file>`.

Se você quiser, também pode integrar o upload de arquivo no ciclo de vida de sua entidade
(ou seja, criação, atualização e remoção). Neste caso, como a sua entidade é criada,
atualizada e removida pelo Doctrine, o tratamento do upload e da remoção de arquivos 
será realizado automaticamente (sem precisar fazer nada em
seu controlador);

Para fazer este trabalho, você precisa cuidar de uma série de detalhes, que
serão abordados neste artigo do cookbook.

Configuração Básica
-------------------

Primeiro, crie uma classe Entity simples do Doctrine para você trabalhar::

    // src/Acme/DemoBundle/Entity/Document.php
    namespace Acme\DemoBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;

    /**
     * @ORM\Entity
     */
    class Document
    {
        /**
         * @ORM\Id
         * @ORM\Column(type="integer")
         * @ORM\GeneratedValue(strategy="AUTO")
         */
        public $id;

        /**
         * @ORM\Column(type="string", length=255)
         * @Assert\NotBlank
         */
        public $name;

        /**
         * @ORM\Column(type="string", length=255, nullable=true)
         */
        public $path;

        public function getAbsolutePath()
        {
            return null === $this->path
                ? null
                : $this->getUploadRootDir().'/'.$this->path;
        }

        public function getWebPath()
        {
            return null === $this->path
                ? null
                : $this->getUploadDir().'/'.$this->path;
        }

        protected function getUploadRootDir()
        {
            // the absolute directory path where uploaded
            // documents should be saved
            return __DIR__.'/../../../../web/'.$this->getUploadDir();
        }

        protected function getUploadDir()
        {
            // get rid of the __DIR__ so it doesn't screw up
            // when displaying uploaded doc/image in the view.
            return 'uploads/documents';
        }
    }

A entidade ``Document`` tem um nome e ele é associado a um arquivo. A propriedade ``path``
armazena o caminho relativo para o arquivo e é persistida no banco de dados.
O ``getAbsolutePath()`` é um método de conveniência que retorna o caminho absoluto
para o arquivo enquanto o ``getWebPath()`` é um método de conveniência que retorna
o caminho web, que podem ser utilizados em um template para obter o link do arquivo 
que foi feito o upload.

.. tip::

    Se não tiver feito isso, você deve ler primeiro a
    documentação sobre o tipo arquivo :doc:`file</reference/forms/types/file>` para
    entender como funciona o processo básico de upload.

.. note::

    Se você estiver usando anotações para especificar as suas regras de validação (como mostrado
    neste exemplo), certifique-se de que tenha ativado a validação por anotação
    (veja :ref:`configuração de validação<book-validation-configuration>`).

Para lidar com o upload do arquivo no formulário, use um campo "virtual" ``file``.
Por exemplo, se você está construindo o seu formulário diretamente em um controlador,
ele poderia parecer com o seguinte::

    public function uploadAction()
    {
        // ...

        $form = $this->createFormBuilder($document)
            ->add('name')
            ->add('file')
            ->getForm();

        // ...
    }

Em seguida, crie essa propriedade em sua classe ``Document`` e adicione algumas regras de
validação:: 

    // src/Acme/DemoBundle/Entity/Document.php

    // ...
    class Document
    {
        /**
         * @Assert\File(maxSize="6000000")
         */
        public $file;

        // ...
    }

.. note::

    Como você está usando a constraint ``File``, o Symfony2 irá "adivinhar" automaticamente
    que o campo do formulário é do tipo para upload de arquivos. É por isso que você não tem
    que defini-lo explicitamente ao criar o formulário acima (``->add('file')``).

O controlador a seguir mostra como lidar com todo o processo::

    use Acme\DemoBundle\Entity\Document;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;
    // ...

    /**
     * @Template()
     */
    public function uploadAction()
    {
        $document = new Document();
        $form = $this->createFormBuilder($document)
            ->add('name')
            ->add('file')
            ->getForm()
        ;

        if ($this->getRequest()->isMethod('POST')) {
            $form->bind($this->getRequest());
            if ($form->isValid()) {
                $em = $this->getDoctrine()->getManager();

                $em->persist($document);
                $em->flush();

                $this->redirect($this->generateUrl(...));
            }
        }

        return array('form' => $form->createView());
    }

.. note::

    Ao escrever o template, não esqueça de definir o atributo ``enctype``:

    .. code-block:: html+jinja

        <h1>Upload File</h1>

        <form action="#" method="post" {{ form_enctype(form) }}>
            {{ form_widget(form) }}

            <input type="submit" value="Upload Document" />
        </form>

O controlador anterior irá persistir automaticamente a entidade ``Document``
com o nome submetido, mas ele não fará nada a respeito do arquivo e a propriedade ``path``
ficará em branco.

Uma maneira fácil de lidar com o upload do arquivo é movê-lo pouco antes da entidade ser
persistida e, em seguida, definir a propriedade ``path`` de acordo. Comece chamando
o novo método ``upload()`` na classe ``Document``, que você vai criar
no momento para lidar com o upload do arquivo::

    if ($form->isValid()) {
        $em = $this->getDoctrine()->getManager();

        $document->upload();

        $em->persist($document);
        $em->flush();

        $this->redirect(...);
    }

O método ``upload()`` irá aproveitar o objeto :class:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile`
, que é o retornado após um campo ``file`` ser submetido::

    public function upload()
    {
        // the file property can be empty if the field is not required
        if (null === $this->file) {
            return;
        }

        // use the original file name here but you should
        // sanitize it at least to avoid any security issues

        // move takes the target directory and then the
        // target filename to move to
        $this->file->move(
            $this->getUploadRootDir(),
            $this->file->getClientOriginalName()
        );

        // set the path property to the filename where you've saved the file
        $this->path = $this->file->getClientOriginalName();

        // clean up the file property as you won't need it anymore
        $this->file = null;
    }

Utilizando Lifecycle Callbacks
------------------------------

Mesmo esta aplicação funcionando, ela sofre de uma grande falha: E se houver
um problema quando a entidade for persistida? O arquivo já teria sido movido
para seu local definitivo, apesar da propriedade ``path`` da entidade não
ter sido persistida corretamente.

Para evitar esses problemas, você deve alterar a implementação de forma que as operações
do banco de dados e a cópia do arquivo tornem-se atômicas: se há um problema
persistindo a entidade ou se o arquivo não pode ser movido, então *nada* deve
ser feito.

Para fazer isso, você precisa mover o arquivo no mesmo momento em que o Doctrine
persistir a entidade no banco de dados. Isto pode ser feito lifecycle da
entidade::

    /**
     * @ORM\Entity
     * @ORM\HasLifecycleCallbacks
     */
    class Document
    {
    }

Em seguida, refatore a classe ``Document`` para aproveitar esses callbacks::

    use Symfony\Component\HttpFoundation\File\UploadedFile;

    /**
     * @ORM\Entity
     * @ORM\HasLifecycleCallbacks
     */
    class Document
    {
        /**
         * @ORM\PrePersist()
         * @ORM\PreUpdate()
         */
        public function preUpload()
        {
            if (null !== $this->file) {
                // do whatever you want to generate a unique name
                $filename = sha1(uniqid(mt_rand(), true));
                $this->path = $filename.'.'.$this->file->guessExtension();
            }
        }

        /**
         * @ORM\PostPersist()
         * @ORM\PostUpdate()
         */
        public function upload()
        {
            if (null === $this->file) {
                return;
            }

            // if there is an error when moving the file, an exception will
            // be automatically thrown by move(). This will properly prevent
            // the entity from being persisted to the database on error
            $this->file->move($this->getUploadRootDir(), $this->path);

            unset($this->file);
        }

        /**
         * @ORM\PostRemove()
         */
        public function removeUpload()
        {
            if ($file = $this->getAbsolutePath()) {
                unlink($file);
            }
        }
    }

A classe agora faz tudo o que você precisa: ela gera um nome de arquivo único antes
de persistir, move o arquivo depois de persistir e remove o arquivo sempre que a
entidade for excluída.

Agora que a cópia do arquivo é tratada atomicamente pela entidade, a
chamada ``$document->upload()`` deve ser removida do controlador::

    if ($form->isValid()) {
        $em = $this->getDoctrine()->getManager();

        $em->persist($document);
        $em->flush();

        $this->redirect(...);
    }

.. note::

    Os callbacks dos eventos ``@ORM\PrePersist()`` e ``@ORM\PostPersist()`` são
    acionados antes e depois da entidade ser persistida no banco de dados. Por
    outro lado, a callback dos eventos ``@ORM\PreUpdate()`` e ``@ORM\PostUpdate()``
    são chamadas quando a entidade é atualizada.

.. caution::

    As callbacks ``PreUpdate`` e ``PostUpdate`` são acionadas somente se houver
    uma alteração em um dos campos de uma entidade que é persistida. Isto significa
    que, por padrão, se você modificar apenas a propriedade ``$file``, esses eventos
    não serão disparados, pois a propriedade não é diretamente persistida
    via Doctrine. Uma solução seria a utilização de um campo ``updated`` que é
    persistido pelo Doctrine e modificá-lo manualmente quando alterar o arquivo.

Usando o ``id`` como nome do arquivo
------------------------------------

Se você quiser usar o ``id`` como nome do arquivo, a implementação é
ligeiramente diferente pois você precisa salvar a extensão na propriedade
``path``, em vez do nome real::

    use Symfony\Component\HttpFoundation\File\UploadedFile;

    /**
     * @ORM\Entity
     * @ORM\HasLifecycleCallbacks
     */
    class Document
    {
        // a property used temporarily while deleting
        private $filenameForRemove;

        /**
         * @ORM\PrePersist()
         * @ORM\PreUpdate()
         */
        public function preUpload()
        {
            if (null !== $this->file) {
                $this->path = $this->file->guessExtension();
            }
        }

        /**
         * @ORM\PostPersist()
         * @ORM\PostUpdate()
         */
        public function upload()
        {
            if (null === $this->file) {
                return;
            }

            // you must throw an exception here if the file cannot be moved
            // so that the entity is not persisted to the database
            // which the UploadedFile move() method does
            $this->file->move(
                $this->getUploadRootDir(),
                $this->id.'.'.$this->file->guessExtension()
            );

            unset($this->file);
        }

        /**
         * @ORM\PreRemove()
         */
        public function storeFilenameForRemove()
        {
            $this->filenameForRemove = $this->getAbsolutePath();
        }

        /**
         * @ORM\PostRemove()
         */
        public function removeUpload()
        {
            if ($this->filenameForRemove) {
                unlink($this->filenameForRemove);
            }
        }

        public function getAbsolutePath()
        {
            return null === $this->path
                ? null
                : $this->getUploadRootDir().'/'.$this->id.'.'.$this->path;
        }
    }

Você vai notar que, neste caso, é necessário um pouco mais de trabalho 
a fim de remover o arquivo. Antes que seja removido, você deve armazenar o caminho do arquivo
(pois ele depende do id). Então, uma vez que o objeto foi totalmente removido
do banco de dados, você pode apagar o arquivo com segurança (em ``PostRemove``).
