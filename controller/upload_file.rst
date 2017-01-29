.. index::
   single: Controlador; Upload; Arquivo

Como Fazer Upload de Arquivos
=============================

.. note::

    Em vez de você mesmo lidar com o upload de arquivos, considere o uso do
    bundle `VichUploaderBundle`_ criado pela comunidade. Esse bundle fornece todas as
    operações comuns (como renomear, salvar e excluir arquivos) e é bem
    integrado com o ORM Doctrine, ODM MongoDB, ODM PHPCR e Propel.

Imagine que você tem uma entidade ``Product`` em sua aplicação e você quer
adicionar um catálogo PDF para cada produto. Para fazer isso, adicione uma nova propriedade
chamada `` brochure`` na entidade ``Product``::

    // src/AppBundle/Entity/Product.php
    namespace AppBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;

    class Product
    {
        // ...

        /**
         * @ORM\Column(type="string")
         *
         * @Assert\NotBlank(message="Please, upload the product brochure as a PDF file.")
         * @Assert\File(mimeTypes={ "application/pdf" })
         */
        private $brochure;

        public function getBrochure()
        {
            return $this->brochure;
        }

        public function setBrochure($brochure)
        {
            $this->brochure = $brochure;

            return $this;
        }
    }

Note que o tipo da coluna ``brochure`` é ``string`` em vez de ``binary``
ou ``blob`` porque ela só armazena o nome do arquivo PDF em vez do conteúdo do arquivo.

Em seguida, adicione um novo campo ``brochure`` ao formulário que gerencia a entidade ``Product``::

    // src/AppBundle/Form/ProductType.php
    namespace AppBundle\Form;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\Extension\Core\Type\FileType;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('brochure', FileType::class, array('label' => 'Brochure (PDF file)'))
                // ...
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'AppBundle\Entity\Product',
            ));
        }

        public function getName()
        {
            return 'product';
        }
    }

Agora, atualize o template que processa o formulário para exibir o novo campo ``brochure``
(o código exato do template para adicionar depende do método usado pela sua aplicação
para :doc:`personalizar a renderização do formulário </cookbook/form/form_customization>`):

.. code-block:: html+twig

    {# app/Resources/views/product/new.html.twig #}
    <h1>Adding a new product</h1>

    {{ form_start() }}
        {# ... #}

        {{ form_row(form.brochure) }}
    {{ form_end() }}

Finalmente, você precisa atualizar o código do controlador que manipula o formulário::

    // src/AppBundle/Controller/ProductController.php
    namespace AppBundle\ProductController;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Request;
    use AppBundle\Entity\Product;
    use AppBundle\Form\ProductType;

    class ProductController extends Controller
    {
        /**
         * @Route("/product/new", name="app_product_new")
         */
        public function newAction(Request $request)
        {
            $product = new Product();
            $form = $this->createForm(ProductType::class, $product);
            $form->handleRequest($request);

            if ($form->isValid()) {
                // $file stores the uploaded PDF file
                /** @var Symfony\Component\HttpFoundation\File\UploadedFile $file */
                $file = $product->getBrochure();

                // Generate a unique name for the file before saving it
                $fileName = md5(uniqid()).'.'.$file->guessExtension();

                // Move the file to the directory where brochures are stored
                $brochuresDir = $this->container->getParameter('kernel.root_dir').'/../web/uploads/brochures';
                $file->move($brochuresDir, $fileName);

                // Update the 'brochure' property to store the PDF file name
                // instead of its contents
                $product->setBrochure($fileName);

                // ... persist the $product variable or any other work

                return $this->redirect($this->generateUrl('app_product_list'));
            }

            return $this->render('product/new.html.twig', array(
                'form' => $form->createView(),
            ));
        }
    }

Existem algumas coisas importantes a considerar no código do controlador acima:

#. Quando o formulário é enviado, a propriedade ``brochure`` contém todo o conteúdo
   do arquivo PDF. Uma vez que esta propriedade armazena apenas o nome do arquivo, você deve definir
   seu novo valor antes persistir as alterações da entidade;
#. Em aplicações Symfony, os arquivos enviados são objetos da
   classe :class:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile`, que
   fornece métodos para as operações mais comuns ao lidar com o upload de arquivos;
#. Uma melhor prática de segurança bem conhecida é nunca confiar nos dados fornecidos por
   usuários. Isso também se aplica para os arquivos enviados por seus visitantes. A classe
   ``Uploaded`` fornece métodos para obter a extensão de arquivo original
   (:method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getExtension`),
   o tamanho do arquivo original (:method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientSize`)
   e o nome do arquivo original (:method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientOriginalName`).
   No entanto, eles são considerados *não seguros* porque um usuário mal-intencionado poderia adulterar
   essas informações. Por isso é sempre melhor gerar um nome único e
   usar o método :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::guessExtension`
   para deixar o Symfony escolher a extensão correta de acordo com o tipo MIME do arquivo;
#. A classe ``UploadedFile`` também fornece um método :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::move`
   para armazenar o arquivo no diretório pretendido. Definir o caminho deste diretório
   como uma opção de configuração da aplicação é considerado uma boa prática que
   simplifica o código: ``$this->container->getParameter('brochures_dir')``.

Agora você pode usar o código a seguir para conectar-se a brochura PDF de um produto:

.. code-block:: html+twig

    <a href="{{ asset('uploads/brochures/' ~ product.brochure) }}">View brochure (PDF)</a>

.. _`VichUploaderBundle`: https://github.com/dustin10/VichUploaderBundle
