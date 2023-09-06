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

## Examples

Example

```elisp
(let ((hash (make-hash-table)))
  (puthash 'a 12 hash)
  (puthash 'b 12 hash)
  (yaml-dump hash))
```

```
---
a: 12
b: 12
...
```

Example

```elisp
(let (hash
      (hash-1 (make-hash-table))
      (hash-2 (make-hash-table)))
  (puthash 'a 12 hash-1)
  (puthash 'b 12 hash-1)
  (puthash 'a 12 hash-2)
  (puthash 'b 12 hash-2)
  (setq hash `[,hash-1 ,hash-2])
  (yaml-dump hash))
```

```
---
- a: 12
  b: 12
- a: 12
  b: 12
...
```
