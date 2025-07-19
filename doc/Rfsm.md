The `Grasp` application is actually a graphical front-end to the
`rfsmc` compiler provided by the [RFSM](https://github.com/jserot/rfsm) package. 

It allows the definition, compilation and simulation of FSM-based models without knowing the
syntax of the RFSM language. 

There are some limitations, however, compared to what is possible when using the RFSM textual
language:

- with `Grasp`, there's no distinction between an FSM model and a corresponding instance
- there's no `type`, `constant`, `function` and `global` (`shared`) declarations
- no _priority_ can be attached to transitions
- the types are limited to `int`, `bool` and `event`; `int`s have no associated size; `char`,
  `float`, `arrays` and `records` are not supported

Some of these limitations may be removed in future versions.
