Controladores
=============

O Symfony segue a filosofia de *"controladores magros e modelos gordos"*. Isso
significa que os controladores devem realizar apenas a fina camada de *juntar o código*
necessário para coordenar as diferentes partes da aplicação.

Como regra geral, você deve seguir a regra 5-10-20, onde os controladores devem
apenas definir cinco variáveis ​​ou menos, conter 10 ações ou menos e incluir 20 linhas
de código ou menos em cada ação. Essa não é uma ciência exata, mas pode
ajudar a perceber quando o código deve ser refatorado para fora do controlador e
dentro de um serviço.

.. best-practice::

    Faça o seu controlador estender o controlador base ``FrameworkBundle`` e
    use anotações para configurar o roteamento, cache e segurança sempre que possível.

O acoplamento dos controladores para a estrutura subjacente do framework permite aproveitar
todos os seus recursos e aumenta a sua produtividade.

E, uma vez que os controladores devem ser magros e conter nada mais do que
algumas linhas de *junção de código*, gastar horas tentando desacoplá-los de seu
framework não beneficiará a longo prazo. A quantidade de tempo *desperdiçado*
não vale o benefício.

Além disso, usar anotações para roteamento, cache e segurança simplifica
a configuração. Você não precisa procurar em dezenas de arquivos criados com diferentes
formatos (YAML, XML, PHP): toda a configuração está apenas onde você precisa dela
e usa apenas um formato.

Em geral, isso significa que você deve desacoplar agressivamente sua lógica de negócio
do framework, enquanto, ao mesmo tempo, acoplar de forma agressiva os seus controladores
e roteamento *ao* framework, a fim de tirar o máximo proveito dele.

Configuração de Roteamento
--------------------------

Para carregar rotas definidas com anotações em seus controladores, adicione a seguinte
configuração no arquivo de configuração de roteamento principal:

.. code-block:: yaml

    # app/config/routing.yml
    app:
        resource: "@AppBundle/Controller/"
        type:     annotation

Essa configuração irá carregar as anotações de qualquer controlador armazenado dentro do
diretório ``src/AppBundle/Controller/`` e até mesmo de seus subdiretórios.
Portanto, se a sua aplicação define muitos controladores, não há nenhum problema em
reorganizá-los dentro de subdiretórios:

.. code-block:: text

    <your-project>/
    ├─ ...
    └─ src/
       └─ AppBundle/
          ├─ ...
          └─ Controller/
             ├─ DefaultController.php
             ├─ ...
             ├─ Api/
             │  ├─ ...
             │  └─ ...
             └─ Backend/
                ├─ ...
                └─ ...

Configuração do Template
------------------------

.. best-practice::

    Não use a anotação ``@Template()`` para configurar o template usado pelo
    controlador.

A anotação ``@Template`` é útil, mas envolve um pouco de magia. Por essa
razão, não recomendamos o seu uso.

Na maioria das vezes, ``@Template`` é usado sem quaisquer parâmetros, o que torna
mais difícil saber qual template está sendo processado. Também torna
menos óbvio para iniciantes que um controlador deve sempre retornar um objeto
Response (a não ser que você está usando uma camada de visão).

Por último, a anotação ``@Template`` usa uma classe ``TemplateListener`` que conecta
ao evento ``kernel.view`` despachado pelo framework. Esse ouvinte adiciona
um impacto mensurável no desempenho. Na aplicação blog de exemplo, renderizar a
homepage levou 5 milissegundos usando o método ``$this->render()`` e 26 milissegundos
usando a anotação ``@Template``.

Como é a Aparência do Controlador
---------------------------------

Considerando tudo isso, aqui está um exemplo de como deve ser a aparência do controlador
para a página inicial da nossa app:

.. code-block:: php

    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            $em = $this->getDoctrine()->getManager();
            $posts = $em->getRepository('App:Post')->findLatest();

            return $this->render('default/index.html.twig', array(
                'posts' => $posts
            ));
        }
    }

.. _best-practices-paramconverter:

Usando o ParamConverter
-----------------------

Se você estiver usando o Doctrine, então, *opcionalmente* pode usar o `ParamConverter`_
para consultar automaticamente por uma entidade e passar ela como um argumento para o seu controlador.

.. best-practice::

    Use o truque ParamConverter para consultar automaticamente por entidades do Doctrine
    quando for simples e conveniente.

Por exemplo:

.. code-block:: php

    /**
     * @Route("/{id}", name="admin_post_show")
     */
    public function showAction(Post $post)
    {
        $deleteForm = $this->createDeleteForm($post);

        return $this->render('admin/post/show.html.twig', array(
            'post'      => $post,
            'delete_form' => $deleteForm->createView(),
        ));
    }

Normalmente, você esperaria um argumento ``$id`` para ``showAction``. Em vez disso, ao
criar um novo argumento (``$post``) com o tipo classe
``Post`` (que é uma entidade do Doctrine), o ParamConverter consulta automaticamente
por um objeto em que a propriedade ``$id`` coincide com o valor ``{id}``. Irá
também exibir uma página 404 se nenhum ``Post`` for encontrado.

Quando as coisas ficam mais avançadas
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Isso funciona sem qualquer configuração, pois o nome curinga ``{id}`` coincide
com o nome da propriedade na entidade. Se isso não for verdade, ou se ainda você tiver
uma lógica mais complexa, a melhor coisa a fazer é apenas consultar pela entidade
manualmente. Em nossa aplicação, temos essa situação no ``CommentController``:

.. code-block:: php

    /**
     * @Route("/comment/{postSlug}/new", name = "comment_new")
     */
    public function newAction(Request $request, $postSlug)
    {
        $post = $this->getDoctrine()
            ->getRepository('AppBundle:Post')
            ->findOneBy(array('slug' => $postSlug));

        if (!$post) {
            throw $this->createNotFoundException();
        }

        // ...
    }

Você também pode usar a configuração ``@ParamConverter``, que é infinitamente
flexível:

.. code-block:: php

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\ParamConverter;

    /**
     * @Route("/comment/{postSlug}/new", name = "comment_new")
     * @ParamConverter("post", options={"mapping": {"postSlug": "slug"}})
     */
    public function newAction(Request $request, Post $post)
    {
        // ...
    }

O ponto é este: o atalho ParamConverter é ótimo para situações simples.
Mas você não deve esquecer que a consulta diretamente às entidades ainda é muito
fácil.

Pré e Pós Hooks
---------------

Se você precisa executar algum código antes ou depois da execução de seus controladores,
você pode usar o componente EventDispatcher para `configurar filtros antes/depois`_.

.. _`ParamConverter`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`configurar filtros antes/depois`: http://symfony.com/doc/current/cookbook/event_dispatcher/before_after_filters.html
