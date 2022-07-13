# Issues found while working with clang's scs support on Unikraft

## Additions
1. to unikraft/unikraft : https://github.com/mariasfiraiala/unikraft
2. to unikraft/app-helloworld: https://github.com/mariasfiraiala/app-helloworld (needed for scs flags and additional constructor in `main.c`)

What's very interesting is the fact that even though the `main()` is somehow succesfully called and executed (the `HelloWorld` message is printed), I still get a trap.

This is how it looks like:
```
Powered by
o.   .o       _ _               __ _
Oo   Oo  ___ (_) | __ __  __ _ ' _) :_
oO   oO ' _ `| | |/ /  _)' _` | |_|  _)
oOo oOO| | | | |   (| | | (_) |  _) :_
 OoOoO ._, ._:_:_,\_._,  .__,_:_, \___)
          Hyperion 0.9.0~7f62c36-custom
Hello
Hello world!
Arguments:  "app-helloworld"
[    0.012419] CRIT: [libkvmplat] <traps_arm64.c @  193> EL1 sync trap caught
[    0.012943] CRIT: [libkvmplat] <traps_arm64.c @  159> 	 SP       : 0x0000000047fffe20
[    0.013078] CRIT: [libkvmplat] <traps_arm64.c @  160> 	 ESR_EL1  : 0x0000000096000004
[    0.013209] CRIT: [libkvmplat] <traps_arm64.c @  161> 	 ELR_EL1  : 0x0000000040108674
[    0.013340] CRIT: [libkvmplat] <traps_arm64.c @  162> 	 LR (x30) : 0x0000000040113cb8
[    0.013475] CRIT: [libkvmplat] <traps_arm64.c @  163> 	 PSTATE   : 0x0000000060000345
[    0.013605] CRIT: [libkvmplat] <traps_arm64.c @  164> 	 FAR_EL1  : 0xfffffffffffffff8
[    0.013794] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x00 ~ x03: 0x0000000000000000 0x0000000000000001 0x0000000040118e4f 0x0000000047fffac0
[    0.014019] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x04 ~ x07: 0x0000000000000073 0x0000000000000050 0x000000004010db84 0x0000000040119943
[    0.014225] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x08 ~ x11: 0x0000000000000001 0x0000000000000001 0x000000004011b000 0x000000000000000d
[    0.014423] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x12 ~ x15: 0x000000000000000a 0x0000000009000000 0x0000000000000000 0x0000000000000000
[    0.014630] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x16 ~ x19: 0x0000000000000000 0x0000000000000000 0x0000000000000000 0x0000000047ffff90
[    0.014829] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x20 ~ x23: 0x000000004011ac08 0x000000004011ac08 0x0000000040143000 0x0000000000000009
[    0.015030] CRIT: [libkvmplat] <traps_arm64.c @  169> 	 x24 ~ x27: 0x0000000000000031 0x0000000040000000 0x000000004011b018 0x000000004013d000
[    0.015243] CRIT: [libkvmplat] <traps_arm64.c @  172> 	 x28 ~ x29: 0x0000000000000000 0x0000000047ffff40

```

After debugging using `gdb` (see how [here](https://gist.github.com/mariasfiraiala/34a7b5b41c4e5515c7f0ad8a2c220ef9)), I came to the conclusion that the problem arises from `unikraft/plat/kvm/arm/exceptions.S`.

Additional investigation using `gdb` revealed the fact that right before the last `printf("\n")` call from `main()` something (TODO: find out what) modifies the `x18` value, making it 0, so the trap that I get is due to the fact that in the function epilogue, `clang's` scs tries to dereference a NULL pointer.

You'll find a snippet of the `gdb` investigation here:

```
main (argc=1, argv=0x4013c3d0 <ukplat_entry_argp.argv>)
    at /home/maria/demo/02-hello-world-with-shadow-stack/apps/app-helloworld/main.c:35
35      {
(gdb) n
40              printf("Hello world!\n");
(gdb) p $x18
$3 = 1207730200
(gdb) n
43              printf("Arguments: ");
(gdb) p $x18
$4 = 1207730200
(gdb) n
44              for (i=0; i<argc; ++i)
(gdb) p $x18
$5 = 1207730200
(gdb) n
45                      printf(" \"%s\"", argv[i]);
(gdb) n
44              for (i=0; i<argc; ++i)
(gdb) p $x18
$6 = 0
(gdb) n
46              printf("\n");
(gdb) n
61      }
(gdb) n
el1_sync ()
    at /home/maria/demo/02-hello-world-with-shadow-stack/unikraft/plat/kvm/arm/exceptions.S:163
163             ENTER_TRAP 1
(gdb) n
164             mov x0, sp
(gdb) n
165             mrs x1, far_el1
(gdb) n
166             bl trap_el1_sync
(gdb) n
[Inferior 1 (process 1) exited normally]

```


(**Not so**) Fun fact: When called without the additions to ukboot, the constructor placed in `main.c` is completely ignored, however, when present in `ukboot` the constructor is called twice! Once for the bootstrapping process and once for the `main.c` instance.

## Setup

Use [this gist](https://gist.github.com/mariasfiraiala/6e5d5ad67952c46b79cb12b9875a7241).

## Critical documentation

### `traps_arm64.c` documentation

* signals different issues:

    1. invalid operations
    1. page faults
    1. bus errors
    1. floating point
    1. security

* handles them through `invalid_trap_handler` or `trap_el1_sync`

* before (most likely) terminating or halting the program through `ukplat_crash` (macro in `bootstrap.h`) -> `ukplat_terminate`(function called by `ukplat_crash`), print error message and dump used registers (`dump_registers`)

* interesting function `trap_el1_sync`:

    1. constructs a struct which will contain all the info needed for finding and signaling an exception (`ctx` -> struct defined in `unikraft/arch/arm/arm64/include/uk/asm/traps.h`)
    1. constructs a trap, based on the exception status register (more about it here: `unikraft/arch/arm/arm64/include/uk/asm/lcpu.h`)
    1. handles the event (`uk_raise_event_ptr`-> defined in `unikraft/include/uk/event.h`)
    1. dumps registers and terminates execution

* more about EL0, EL1, EL2, EL3 [here](https://developer.arm.com/documentation/102412/0100/Privilege-and-Exception-levels).

### `ukboot` documentation

Motivaton:

1. I'll have to move [my constructor](https://gist.github.com/mariasfiraiala/60389dd16fef0fdc11d7f7972e320a9a) in `ukboot`, either as a macro or as a function.
1. If placed in `main.c`, the initialization of `x18` is completely ignored. 

Implementation:

`ukboot` provides an interface which resolves all the desired initializations before actually calling the `main` function.

* `main_thread_func` (`__no_return` -> doesn't return to the caller; has exceptions, exits or loops forever)
    1. calls `init` function, if it fails, terminates the program 
    1. calls pre-init `constructors` (TODO: how are they different from `init`)
    1. finally, calls `main`
    1. halts or terminates program depending on `ret` value

* `ukplat_entry_argp`
    1. sets `argc` and `argv` before calling `ukplat_entry`

* `ukplat_entry` (of real interest to me)
    1. configures a multitude of functionalities, based on macros
    
        -> allocator
        
        -> scheduler
        
        -> param (TODO: ask about this one)
        
    1. if the program requires multiple threads, enable the the main thread and its subsequent threads (TODO: ask about this)
    1. otherwise, just enables interrupts and calls the `main_thread_func`