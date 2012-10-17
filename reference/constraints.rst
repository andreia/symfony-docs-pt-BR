Referência das Constraints de Validação 
=======================================

.. toctree::
   :maxdepth: 1
   :hidden:

   constraints/NotBlank
   constraints/Blank
   constraints/NotNull
   constraints/Null
   constraints/True
   constraints/False
   constraints/Type

   constraints/Email
   constraints/MinLength
   constraints/MaxLength
   constraints/Length
   constraints/Url
   constraints/Regex
   constraints/Ip

   constraints/Max
   constraints/Min
   constraints/Range

   constraints/Date
   constraints/DateTime
   constraints/Time

   constraints/Choice
   constraints/Collection
   constraints/Count
   constraints/UniqueEntity
   constraints/Language
   constraints/Locale
   constraints/Country

   constraints/File
   constraints/Image

   constraints/Callback
   constraints/All
   constraints/UserPassword
   constraints/Valid

O Validator é projetado para validar objetos comparando com *constraints*.
Na vida real, uma constraint poderia ser: "O bolo não deve ser queimado". No
Symfony2, as constraints são semelhantes: Elas são afirmações de que uma condição 
é verdadeira.

Constraints Suportadas
----------------------

As seguintes constraints estão disponíveis nativamente no Symfony2:

.. include:: /reference/constraints/map.rst.inc
