The `Rfsm-light` application is actually a graphical front-end to the
`rfsmc` compiler provided by the [RFSM](https://github.com/jserot/rfsm) package. 

It allows the definition, compilation and simulation of FSM-based models without knowing the
syntax of the RFSM language. 

There are some limitations, however, compared to what is possible when using the RFSM textual
language:

- with `Rfsm-light`, the model consists of a single FSM; there's no distinction between an FSM model
  and a corresponding instance
- there's no _type_, _constant_, _function_ and _global_ (_shared_) declarations
- the models cannot be _parameterized_
- no _priority_ can be attached to transitions
- the types are limited to `int`, `bool` and `event`; `int`s have no associated size; `char`,
  `float`, `arrays` and `records` are not supported

Some of these limitations may be removed in future versions.
