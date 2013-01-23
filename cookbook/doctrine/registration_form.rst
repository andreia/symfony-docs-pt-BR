.. index::
   single: Doctrine; Formulário de Registro Simples
   single: Formulário; Formulário de Registro Simples

Como implementar um Formulário de Registro Simples
==================================================

Alguns formulários possuem campos extras cujos valores não precisam ser armazenados no
banco de dados. Por exemplo, você pode criar um formulário de registo com alguns
campos extras (como um campo checkbox "termos de aceite") e incorporar o formulário
que realmente armazena as informações da conta.

O modelo simples ``User``
-------------------------

Você tem uma entidade simples ``User`` mapeada para o banco de dados::

    // src/Acme/AccountBundle/Entity/User.php
    namespace Acme\AccountBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;
    use Symfony\Bridge\Doctrine\Validator\Constraints\UniqueEntity;

    /**
     * @ORM\Entity
     * @UniqueEntity(fields="email", message="Email already taken")
     */
    class User
    {
        /**
         * @ORM\Id
         * @ORM\Column(type="integer")
         * @ORM\GeneratedValue(strategy="AUTO")
         */
        protected $id;

        /**
         * @ORM\Column(type="string", length=255)
         * @Assert\NotBlank()
         * @Assert\Email()
         */
        protected $email;

        /**
         * @ORM\Column(type="string", length=255)
         * @Assert\NotBlank()
         */
        protected $plainPassword;

        public function getId()
        {
            return $this->id;
        }

        public function getEmail()
        {
            return $this->email;
        }

        public function setEmail($email)
        {
            $this->email = $email;
        }

        public function getPlainPassword()
        {
            return $this->plainPassword;
        }

        public function setPlainPassword($password)
        {
            $this->plainPassword = $password;
        }
    }

Esta entidade ``User`` contém três campos, e dois deles (``email`` e
``plainPassword``) devem ser exibos no formulário. A propriedade e-mail deve ser única
no banco de dados, isto é aplicado através da adição da validação no topo
da classe.

.. note::

    Se você quiser integrar este ``User`` com o sistema de segurança, você precisa
    implementar a :ref:`UserInterface<book-security-user-entity>` do
    componente de segurança.

Criar um Formulário para o Modelo
---------------------------------

Em seguida, crie o formulário para modelo ``User``::

    // src/Acme/AccountBundle/Form/Type/UserType.php
    namespace Acme\AccountBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilder;

    class UserType extends AbstractType
    {
        public function buildForm(FormBuilder $builder, array $options)
        {
            $builder->add('email', 'email');
            $builder->add('plainPassword', 'repeated', array(
               'first_name' => 'password',
               'second_name' => 'confirm',
               'type' => 'password',
            ));
        }

        public function getDefaultOptions(array $options)
        {
            return array('data_class' => 'Acme\AccountBundle\Entity\User');
        }

        public function getName()
        {
            return 'user';
        }
    }

Há apenas dois campos:``email`` e ``plainPassword`` (repetido para confirmar
a senha digitada). A opção ``data_class`` diz ao formulário o nome da
classe de dados (ou seja, a sua entidade ``User``).

.. tip::

    Para explorar mais sobre o componente de formulário, leia :doc:`/book/forms`.

Incorporando o Formulário do User no Formulário de Registro
-----------------------------------------------------------

O formulário que você vai usar para a página de registo não é o mesmo que o formulário
usado apenas para modificar o ``User`` (ou seja, ``UserType``). O formulário de registro
conterá novos campos como o "aceitar os termos", cujo valor não
será armazenado no banco de dados.

Comece criando uma classe simples que representa o "registro"::

    // src/Acme/AccountBundle/Form/Model/Registration.php
    namespace Acme\AccountBundle\Form\Model;

    use Symfony\Component\Validator\Constraints as Assert;

    use Acme\AccountBundle\Entity\User;

    class Registration
    {
        /**
         * @Assert\Type(type="Acme\AccountBundle\Entity\User")
         */
        protected $user;

        /**
         * @Assert\NotBlank()
         * @Assert\True()
         */
        protected $termsAccepted;

        public function setUser(User $user)
        {
            $this->user = $user;
        }

        public function getUser()
        {
            return $this->user;
        }

        public function getTermsAccepted()
        {
            return $this->termsAccepted;
        }

        public function setTermsAccepted($termsAccepted)
        {
            $this->termsAccepted = (Boolean) $termsAccepted;
        }
    }

Em seguida, crie o formulário para este modelo ``Registration``::

    // src/Acme/AccountBundle/Form/Type/RegistrationType.php
    namespace Acme\AccountBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilder;

    class RegistrationType extends AbstractType
    {
        public function buildForm(FormBuilder $builder, array $options)
        {
            $builder->add('user', new UserType());
            $builder->add(
                'terms',
                'checkbox',
                array('property_path' => 'termsAccepted')
            );
        }

        public function getName()
        {
            return 'registration';
        }
    }

Você não precisa usar um método especial para incorporar o formulário ``UserType``.
Um formulário também é um campo - logo, você pode adicionar ele como qualquer
outro campo, com a certeza de que a propriedade ``Registration.user`` irá manter uma
instância da classe ``User``.

Manuseando a Submissão do Formulário
------------------------------------

Em seguida, você precisa de um controlador para lidar com o formulário. Comece criando um
controlador simples para exibir o formulário de registro::

    // src/Acme/AccountBundle/Controller/AccountController.php
    namespace Acme\AccountBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Response;

    use Acme\AccountBundle\Form\Type\RegistrationType;
    use Acme\AccountBundle\Form\Model\Registration;

    class AccountController extends Controller
    {
        public function registerAction()
        {
            $form = $this->createForm(
                new RegistrationType(),
                new Registration()
            );

            return $this->render(
                'AcmeAccountBundle:Account:register.html.twig',
                array('form' => $form->createView())
            );
        }
    }

e o seu template:

.. code-block:: html+jinja

    {# src/Acme/AccountBundle/Resources/views/Account/register.html.twig #}
    <form action="{{ path('create')}}" method="post" {{ form_enctype(form) }}>
        {{ form_widget(form) }}

        <input type="submit" />
    </form>

Por fim, adicione o controlador que lida com a submissão do formulário. Ele realiza
a validação e salva os dados no banco de dados::

    public function createAction()
    {
        $em = $this->getDoctrine()->getEntityManager();

        $form = $this->createForm(new RegistrationType(), new Registration());

        $form->bindRequest($this->getRequest());

        if ($form->isValid()) {
            $registration = $form->getData();

            $em->persist($registration->getUser());
            $em->flush();

            return $this->redirect(...);
        }

        return $this->render(
            'AcmeAccountBundle:Account:register.html.twig',
            array('form' => $form->createView())
        );
    }

Pronto! O seu formulário agora valida e permite que você salve o objeto
``User`` no banco de dados. O checkbox extra ``terms`` na classe de modelo
``Registration`` é utilizado durante a validação, mas não é utilizado posteriormente
quando salvamos o usuário no banco de dados.
