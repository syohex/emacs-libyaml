# libyaml.el

[libyaml](https://github.com/yaml/libyaml) binding for Emacs Lisp.

## Requirements

- Emacs 25 or higher(configured with `--with-modules`)
- libyaml

## Interfaces

#### `(yaml-read-from-string string)`

Parse `string` and return Emacs Lisp objects.

#### `(yaml-read-file file)`

Read YAML file and return Emacs Lisp objects.

#### `(yaml-dump obj)`

Dump Emacs Lisp object to YAML string.

## Conversion Rule

| YAML                     | Emacs Lisp   |
|:-------------------------|:-------------|
| 123                      | integer      |
| 123.4                    | float        |
| "123"                    | string       |
| t, On, Yes, TRUE         | t : symbol   |
| n, No, Off, False        | nil : symbol |
| - aaa<br /> - bbb        | vector       |
| name: Foo<br /> year: 19 | hash-table   |
