# Issues found while working with clang's scs support on Unikraft

## Additions
1. to unikraft/unikraft : https://github.com/mariasfiraiala/unikraft/pull/1
2. to unikraft/app-helloworld: https://github.com/mariasfiraiala/app-helloworld/pull/1

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
