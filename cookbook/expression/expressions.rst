.. index::
   single: Expressões no Framework

Como usar Expressões em Segurança, Roteamento, Serviços e Validação
===================================================================

No Symfony 2.4, um poderoso componente :doc:`ExpressionLanguage </components/expression_language/introduction>`
foi adicionado ao Symfony. Isso nos permite adicionar lógica altamente personalizada
dentro da configuração.

O Framework Symfony utiliza expressões prontas para uso nas seguintes
formas:

* :ref:`Configuração de serviços <book-services-expressions>`;
* :ref:`Condições de correspondência na rota <book-routing-conditions>`;
* :ref:`Verificação de segurança <book-security-expressions>` (explicado abaixo) e
  :ref:`controles de acesso com allow_if <book-security-allow-if>`;
* :doc:`Validação </reference/constraints/Expression>`.

Para mais informações sobre como criar e trabalhar com expressões, consulte
:doc:`/components/expression_language/syntax`.

.. _book-security-expressions:

Segurança: Controles de Acesso Complexos com Expressões
-------------------------------------------------------

Adicionalmente a um papel como ``ROLE_ADMIN``, o método ``isGranted`` também
aceita um objeto :class:`Symfony\\Component\\ExpressionLanguage\\Expression`::

    use Symfony\Component\ExpressionLanguage\Expression;
    // ...

    public function indexAction()
    {
        if (!$this->get('security.authorization_checker')->isGranted(new Expression(
            '"ROLE_ADMIN" in roles or (user and user.isSuperAdmin())'
        ))) {
            throw $this->createAccessDeniedException();
        }

        // ...
    }

Nesse exemplo, se o usuário atual possui ``ROLE_ADMIN`` ou se o método
``isSuperAdmin()`` do objeto do usuário atual retornar ``True``, então o acesso
será concedido (nota: o seu objeto User pode não ter um método ``isSuperAdmin``,
esse método foi inventado para esse exemplo).

Isso usa uma expressão e para aprender mais sobre a sintaxe da linguagem de expressão
, consulte :doc:`/components/expression_language/syntax`.

.. _book-security-expression-variables:

Dentro da expressão, você tem acesso a uma série de variáveis:

``user``
    O objeto usuário (ou a string ``anon`` caso não estiver autenticado).
``roles``
    O array de papéis que o usuário possui, inclusive da
    :ref:`hierarquia de papel <security-role-hierarchy>` mas não incluindo os
    atributos ``IS_AUTHENTICATED_*`` (veja as funções abaixo).
``User``.
     O objeto (se houver) que é passado como segundo argumento para ``isGranted``.
``Token``
    O objeto token.
``trust_resolver``
    O objeto :class:`Symfony\\Component\\Security\\Core\\Authentication\\AuthenticationTrustResolverInterface`,
    você provavelmente usará as funções ``is_*`` abaixo em vez disso.

Além disso, você tem acesso a uma série de funções dentro da expressão:

``is_authenticated``
    Retorna `` True`` se o usuário é autenticado via "remember-me" ou "Totalmente"
    autenticado - ou seja, retorna true se o usuário estiver "logado".
``is_anonymous``
    É o mesmo que usar ``IS_AUTHENTICATED_ANONYMOUSLY`` com a função ``isGranted``.
``is_remember_me``
    Semelhante, mas não igual a ``IS_AUTHENTICATED_REMEMBERED``, veja abaixo.
``is_fully_authenticated``
    Semelhante, mas não igual a ``IS_AUTHENTICATED_FULLY``, veja abaixo.
``has_role``
    Verifica se o usuário tem o papel atribuído - o equivalente a uma expressão como
    ``'ROLE_ADMIN' in roles``.

.. sidebar:: ``is_remember_me`` is different than checking ``IS_AUTHENTICATED_REMEMBERED``

    As funções ``is_remember_me`` e ``is_authenticated_fully`` são *semelhantes*
    ao uso de ``IS_AUTHENTICATED_REMEMBERED`` e ``IS_AUTHENTICATED_FULLY``
    com a função ``isGranted`` - mas elas **não** são o mesmo. Veja
    a seguir a diferença::

        use Symfony\Component\ExpressionLanguage\Expression;
        // ...

        $ac = $this->get('security.authorization_checker');
        $access1 = $ac->isGranted('IS_AUTHENTICATED_REMEMBERED');

        $access2 = $ac->isGranted(new Expression(
            'is_remember_me() or is_fully_authenticated()'
        ));

    Aqui, ``$access1`` e ``$access2`` terão o mesmo valor. Ao contrário do
    comportamento de ``IS_AUTHENTICATED_REMEMBERED`` e ``IS_AUTHENTICATED_FULLY``,
    a função ``is_remember_me`` *apenas* retorna true se o usuário é autenticado
    através de um cookie remember-me e ``is_fully_authenticated`` *apenas* retorna
    true se o usuário realmente fez o login durante esta sessão (ou seja, é
    full-fledged).
