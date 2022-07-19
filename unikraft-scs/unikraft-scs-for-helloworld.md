# Shadow Stack for Unikraft's `helloworld`

## Additions
1. to unikraft/unikraft : https://github.com/mariasfiraiala/unikraft/pull/3
2. to unikraft/app-helloworld: https://github.com/mariasfiraiala/app-helloworld/pull/2

## Proof of concept

### Assembly code

Assembly code for `helloworld` compiled **with** scs suppport:

```
Dump of assembler code for function main:
   0x0000000040108590 <+0>:     str     x30, [x18], #8
   0x0000000040108594 <+4>:     stp     x29, x30, [sp, #-48]!
   0x0000000040108598 <+8>:     str     x21, [sp, #16]
   0x000000004010859c <+12>:    stp     x20, x19, [sp, #32]
   0x00000000401085a0 <+16>:    mov     x29, sp
   0x00000000401085a4 <+20>:    mov     w20, w0
=> 0x00000000401085a8 <+24>:    adrp    x0, 0x40119000
   0x00000000401085ac <+28>:    add     x0, x0, #0x91f
   0x00000000401085b0 <+32>:    mov     x19, x1
   0x00000000401085b4 <+36>:    bl      0x4010e59c <printf>
   0x00000000401085b8 <+40>:    adrp    x0, 0x40119000
   0x00000000401085bc <+44>:    add     x0, x0, #0x92d
   0x00000000401085c0 <+48>:    bl      0x4010e59c <printf>
   0x00000000401085c4 <+52>:    cmp     w20, #0x1
   0x00000000401085c8 <+56>:    b.lt    0x401085ec <main+92>  // b.tstop
   0x00000000401085cc <+60>:    mov     w21, w20
   0x00000000401085d0 <+64>:    adrp    x20, 0x40119000
   0x00000000401085d4 <+68>:    add     x20, x20, #0x939
   0x00000000401085d8 <+72>:    ldr     x1, [x19], #8
   0x00000000401085dc <+76>:    mov     x0, x20
   0x00000000401085e0 <+80>:    bl      0x4010e59c <printf>
   0x00000000401085e4 <+84>:    subs    x21, x21, #0x1
   0x00000000401085e8 <+88>:    b.ne    0x401085d8 <main+72>  // b.any
   0x00000000401085ec <+92>:    adrp    x0, 0x40118000 <arch_timer_list>
   0x00000000401085f0 <+96>:    add     x0, x0, #0xe4f
   0x00000000401085f4 <+100>:   bl      0x4010e59c <printf>
   0x00000000401085f8 <+104>:   ldp     x20, x19, [sp, #32]
   0x00000000401085fc <+108>:   ldr     x21, [sp, #16]
   0x0000000040108600 <+112>:   mov     w0, wzr
   0x0000000040108604 <+116>:   ldp     x29, x30, [sp], #48
   0x0000000040108608 <+120>:   ldr     x30, [x18, #-8]!
   0x000000004010860c <+124>:   ret
```

Assembly code for `helloworld` compiled **without** scs support:
```
Dump of assembler code for function main:
   0x0000000040108590 <+0>:     stp     x29, x30, [sp, #-48]!
   0x0000000040108594 <+4>:     str     x21, [sp, #16]
   0x0000000040108598 <+8>:     stp     x20, x19, [sp, #32]
   0x000000004010859c <+12>:    mov     x29, sp
   0x00000000401085a0 <+16>:    mov     w20, w0
=> 0x00000000401085a4 <+20>:    adrp    x0, 0x40119000
   0x00000000401085a8 <+24>:    add     x0, x0, #0x91b
   0x00000000401085ac <+28>:    mov     x19, x1
   0x00000000401085b0 <+32>:    bl      0x4010e554 <printf>
   0x00000000401085b4 <+36>:    adrp    x0, 0x40119000
   0x00000000401085b8 <+40>:    add     x0, x0, #0x929
   0x00000000401085bc <+44>:    bl      0x4010e554 <printf>
   0x00000000401085c0 <+48>:    cmp     w20, #0x1
   0x00000000401085c4 <+52>:    b.lt    0x401085e8 <main+88>  // b.tstop
   0x00000000401085c8 <+56>:    mov     w21, w20
   0x00000000401085cc <+60>:    adrp    x20, 0x40119000
   0x00000000401085d0 <+64>:    add     x20, x20, #0x935
   0x00000000401085d4 <+68>:    ldr     x1, [x19], #8
   0x00000000401085d8 <+72>:    mov     x0, x20
   0x00000000401085dc <+76>:    bl      0x4010e554 <printf>
   0x00000000401085e0 <+80>:    subs    x21, x21, #0x1
   0x00000000401085e4 <+84>:    b.ne    0x401085d4 <main+68>  // b.any
   0x00000000401085e8 <+88>:    adrp    x0, 0x40118000 <arch_timer_list>
   0x00000000401085ec <+92>:    add     x0, x0, #0xe4b
   0x00000000401085f0 <+96>:    bl      0x4010e554 <printf>
   0x00000000401085f4 <+100>:   ldp     x20, x19, [sp, #32]
   0x00000000401085f8 <+104>:   ldr     x21, [sp, #16]
   0x00000000401085fc <+108>:   mov     w0, wzr
   0x0000000040108600 <+112>:   ldp     x29, x30, [sp], #48
   0x0000000040108604 <+116>:   ret
```

Notice how the prologue and epilogue have additional instructions for the code compiled with scs support.

### `x18` Shadow Stack pointer using `gdb`

```
main (argc=1, argv=0x4013c3d0 <ukplat_entry_argp.argv>)
    at /home/maria/demo/02-hello-world-with-shadow-stack/apps/app-helloworld/main.c:27
27      {
(gdb) x/i $pc
=> 0x40108590 <main>:   str     x30, [x18], #8
(gdb) si
0x0000000040108594      27      {
(gdb) x/i $pc
=> 0x40108594 <main+4>: stp     x29, x30, [sp, #-48]!
(gdb) x/x $x18
0x47fc0018:     0x00000000
(gdb) x/x $x30
0x40113c78 <main_thread_func+380>:      0x7100001f
(gdb) x/x 0x47fc0010
0x47fc0010:     0x40113c78
```

Notice how [`x18` - 8] stores pointer to `x30`, which at this point in time has the return address.

### Other observations

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
