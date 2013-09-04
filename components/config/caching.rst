.. index::
   single: Config; Caching baseado em recursos

Caching baseado em recursos
==========================

Quando todos os recursos de configuração são carregados, você poderá processar a configuração de valores e combiná-los em um único arquivo. Esse arquivo funciona 
como um cache. Seu conteúdo não precisa ser recriados cada vez que o aplicativo é 
executado – apenas quando os meios de configuração são modificadas.

Por exemplo, o componente de roteamento de Symfony permite que você carregue todas as 
rotas, e, em seguida, despeje um combinador de URL ou um gerador de URL com base nessas 
rotas. Neste caso, quando um dos recursos é modificado (e você estiver trabalhando em um 
ambiente de desenvolvimento), o arquivo gerado deve ser invalidado e recuperado.
This can be accomplished by making use of the :class:`Symfony\\Component\\Config\\ConfigCache`
class.

O exemplo abaixo mostra como coletar recursos, em seguida, gerar um código com base nos 
recursos que foram carregados, e escrever este código para o cache. O cache também
recebe o conjunto de recursos que foram utilizados para gerar o código. Olhando para a
data e hora da "última modificação" desses recursos, o cache pode informar se o conteúdo
ainda está válido ou se deve ser atualizado::

    use Symfony\Component\Config\ConfigCache;
    use Symfony\Component\Config\Resource\FileResource;

    $cachePath = __DIR__.'/cache/appUserMatcher.php';

    // o segundo argumento indica se você deseja ou não usar o modo de depuração
    $userMatcherCache = new ConfigCache($cachePath, true);

    if (!$userMatcherCache->isFresh()) {
        // preencha essa variável com um array de path de arquivos 'users.yml'
        $yamlUserFiles = ...;

        $resources = array();

        foreach ($yamlUserFiles as $yamlUserFile) {
            // Veja o artigo anterior "Loading resources" para
            // ver de onde $delegatingLoader vem
            $delegatingLoader->load($yamlUserFile);
            $resources[] = new FileResource($yamlUserFile);
        }

        // o código para o UserMatcher é gerado em outro em outro lugar
        $code = ...;

        $userMatcherCache->write($code, $resources);
    }

    // Você pode incluir o código de cache:
    require $cachePath;

No modo de depuração, um ``.meta`` arquivo será criado no mesmo diretório que o arquivo 
de cache. Esse ``.meta`` arquivo contém recursos serializado, 
que são utilizados para determinar se o cache ainda está recente. Quando não estiver em
modo de depuração, o cache é considerado como "recente" conforme exista,
e portanto, não será gerado o ``.meta`` arquivo.
