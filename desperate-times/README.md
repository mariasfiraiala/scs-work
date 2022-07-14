# Desperate times require desperate measures

To check whether the `clang's` scs really does something to the Unikraft `helloworld` app, I compared 2 executables (one with scs support, one without) using `arch64-linux-gnu-objdump`. The one with scs support uses `x18` 6 times more than the one without scs support.

Attached are the 2 asm codes. Have fun reading 23264 lines of nonsense.
