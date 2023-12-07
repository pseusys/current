# Compiler design

## Typed and Untyped languages

**Typed** - a dedicated type is associated to each identifier (and expression).
**Untyped** - a single universal type is associated to each identifier.
**Explicitly typed** - types are part of syntax.

## Trapped and Untrapped errors

> Well-typed programs never go wrong.

**Trapped** - - stop computation immediately.
**Untrapped** - may be unnoticed (e.g. illegal access to a legal address).

> Well-typed program never executes out-of-semantics (undefined) behavios.
> Typing contributes to safety but isn't neither necessary nor sufficient.

## Static and Dynamic checking

**Static** - compile time.
**Dynamic** - run time (necessary to handle correctly!): polymorphism, arrays, subtyping, dynamic variables and functions.
