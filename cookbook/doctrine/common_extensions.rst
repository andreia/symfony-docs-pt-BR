.. index::
   single: Doctrine; Extensões comuns

Como usar as extensões do Doctrine: Timestampable, Sluggable, Translatable, etc.
================================================================================

O Doctrine2 é muito flexível e a comunidade já criou um série de extensões do 
Doctrine úteis para ajudar você com tarefas comuns relacionadas a entidades.

Uma biblioteca em particular - a biblioteca `DoctrineExtensions`_ - fornece
funcionalidade de integração para os comportamentos `Sluggable`_, `Translatable`_, 
`Timestampable`_, `Loggable`_, `Tree`_ e `Sortable`_.

O uso de cada uma destas extensões é explicado no repositório.

No entanto, para instalar/ativar cada extensão você deve se registrar e ativar um
:doc:`Listener de Evento</cookbook/doctrine/event_listeners_subscribers>`.
Para fazer isso, você tem duas opções:

#. Usar o `StofDoctrineExtensionsBundle`_, que integra a biblioteca acima.

#. Implementar este serviço diretamente seguindo a documentação para a integração
   com o Symfony2: `Instalando extensões Gedmo Doctrine2 no Symfony2`_

.. _`DoctrineExtensions`: https://github.com/l3pp4rd/DoctrineExtensions
.. _`StofDoctrineExtensionsBundle`: https://github.com/stof/StofDoctrineExtensionsBundle
.. _`Sluggable`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/sluggable.md
.. _`Translatable`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/translatable.md
.. _`Timestampable`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/timestampable.md
.. _`Loggable`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/loggable.md
.. _`Tree`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/tree.md
.. _`Sortable`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/sortable.md
.. _`Instalando extensões Gedmo Doctrine2 no Symfony2`: https://github.com/l3pp4rd/DoctrineExtensions/blob/master/doc/symfony2.md
