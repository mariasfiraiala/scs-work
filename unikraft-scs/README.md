# Issues found while working with clang's scs support on Unikraft

## Additions
1. to unikraft/unikraft : https://github.com/mariasfiraiala/unikraft/pull/1
2. to unikraft/app-helloworld: https://github.com/mariasfiraiala/app-helloworld/pull/1

## Setup

Use [this gist](https://gist.github.com/mariasfiraiala/6e5d5ad67952c46b79cb12b9875a7241).

## `traps_arm64.c` documentation

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

* more about EL0, EL1, EL2, EL3 [here](https://developer.arm.com/documentation/102412/0100/Privilege-and-Exception-levels)
