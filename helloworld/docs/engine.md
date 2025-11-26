/*!
\page engine Engine (Example Runner)

This project contains a small "engine" that discovers and runs examples.

Key pieces:
- A global registry of examples (see examples/registry.h)
- A macro to register each example function at static initialization time
- A bootstrap main (main.cpp) that provides CLI controls and runs selected examples

Below is a simplified PlantUML diagram of the relationships.

\startuml{engine_arch.png}
skinparam monochrome true
hide methods

class "examples::Item" as Item {
  +name: const char*
  +fn: int(int,char**)
  +help: const char*
}
class "examples::Registrar" as Registrar
class "examples::registry()" as Reg
class "main()" as Main

Registrar --> Reg : register_example(Item)
Main --> Reg : all(), find(name)
Reg --> Item : stores
Main ..> Item : calls fn(argc,argv)
\enduml

*/

