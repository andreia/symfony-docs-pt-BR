.. index::
    single: Sessões, cookies

Evite Iniciar Sessões para Usuários Anônimos
============================================

As sessões são iniciadas automaticamente quando você lê, escreve ou até mesmo verifica a
existência de dados na sessão. Isso significa que se você precisa evitar a criação de
um cookie de sessão para alguns usuários, pode ser difícil: você deve evitar *completamente*
acessar a sessão.

Por exemplo, um problema comum nessa situação envolve a verificação de
mensagens flash, que são armazenadas na sessão. O código a seguir garantirá
que uma sessão seja *sempre* iniciada:

.. code-block:: html+jinja

    {% for flashMessage in app.session.flashbag.get('notice') %}
        <div class="flash-notice">
            {{ flashMessage }}
        </div>
    {% endfor %}

Mesmo que o usuário não esteja logado e que você não tenha criado nenhuma mensagem flash,
apenas chamar o método ``get()`` (ou mesmo ``has()``) do `` flashbag`` irá
iniciar uma sessão. Isso pode prejudicar o desempenho do aplicação, porque todos os usuários
irão receber um cookie de sessão. Para evitar esse comportamento, adicione uma verificação antes de tentar
acessar as mensagens flash:

.. code-block:: html+jinja

    {% if app.request.hasPreviousSession %}
        {% for flashMessage in app.session.flashbag.get('notice') %}
            <div class="flash-notice">
                {{ flashMessage }}
            </div>
        {% endfor %}
    {% endif %}
