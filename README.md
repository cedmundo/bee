# Bee

Bee (or BeeLang) it's a general propose programming language, the objective is to create a base to support experimental features such:

* Effects and effects handlers
 * Automatic memory management without GC
 * Seamless C interaction
 * Dynamic multi-pass compilation (language extension)
* ADTs
  * Layout optimisation for vectorization
  * Dependant types

Current milestone version is the v0.1.0, which will add support for:

* Interpreted execution (through libjit)
* Small runtime
* Static keywords and operators support
* All basic mathematical and logical operations
* Simple type support (no duck-typing or inferred type support)
* Effects and effect handlers
* Lazy map, filter, reduce
