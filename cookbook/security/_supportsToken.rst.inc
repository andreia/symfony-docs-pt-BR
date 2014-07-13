Após o Symfony chamar ``createToken()``, ele irá então chamar ``supportsToken()``
em sua classe (e em quaisquer outros listeners de autenticação) para descobrir quem deve
lidar com o token. Esta é apenas uma maneira de permitir que vários mecanismos de autenticação
sejam utilizados para o mesmo firewall (dessa forma, você pode, por exemplo, primeiro tentar
autenticar o usuário via um certificado ou uma chave de API e fall back para
um formulário de login).

Na maioria das vezes, você só precisa ter certeza de que esse método retorna ``true`` para um
token que foi criado por ``createToken()``. Sua lógica provavelmente deve parecer
exatamente como neste exemplo.
