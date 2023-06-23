# schemesh
## Fusion between a Unix shell and a Lisp REPL

### Current status: ALPHA.
### Some features work, many others are incomplete or missing.
### Use at your own risk!

Schemesh is a fusion between a Unix shell and a Lisp REPL - Chez Scheme REPL, to be exact.

It supports familiar POSIX shell syntax for starting commands, including redirections, pipelines,
job concatenation with && ||, groups surrounded by { }, and managing foreground/background jobs.

For scripting and serious programming, it completely replaces the clumsy scripting language
of a traditional shell (yes, the author has opinions) with a full-featured Lisp REPL.

This means you can use Lisp control structures, loops and functions such as
```lisp
(if (some_expression arg1 (sub_expression2))
  (then_run_here)
  (otherwise_run_here))
```
instead of typical shell syntax, which is error prone as it's based on string expansion and splitting,
and geared toward command execution, as for example:
```shell
if some_command "$arg1" "$(sub_command)"
then
  then_run_this_command
else
  else_run_this_command
fi
```

Switching between shell syntax and Lisp syntax is extremely simple, and can be done basically everywhere:
* ( i.e. open parenthesis switches to Lisp syntax until the corresponding closed parenthesis i.e. )
* { i.e. open brace switches to shell syntax until the corresponding closed brace i.e. }
* directives #!scheme #!chezscheme #!r6rs switch to Lisp syntax (with the appropriate flavor)
  until the end of current list inside { } or ( ).
  if entered at top level, it is effective until another directive is entered at top level.
* directive #!shell switches to shell syntax until the end of current list inside { } or ( ).
  if entered at top level, it is effective until another directive is entered at top level.

Examples:

```shell
find (lisp-function-returning-some-path) -type f | grep ^lib | wc -l
```

```lisp
(define job {ls -l | grep some-file-name})
(sh-start job)
(sh-fg job)
```

Schemesh can be used as:
* a replacement for traditional interactive Unix shell, as for example bash/ksh/zsh etc.

* a Unix shell scriptable in Lisp

* a Lisp REPL with additional syntax and functions to start, redirect and manage Unix processes

* a Lisp library for starting, redirecting and managing Unix processes
