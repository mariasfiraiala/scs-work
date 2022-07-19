# Shadow Stack for Unikraft complex applications

I am testing 3 complex applications `SQLite`, `redis` and `nginx` in order to bring Shadow Stack support.

| App\Compiler | gcc - x86 | gcc - aarch64 | clang - x86 | clang - aarch64 | clang with scs | gcc-12 with scs |
|--------------|-----------|---------------|-------------|-----------------|----------------|-----------------|
| SQLite | :heavy_check_mark: | :soon: | :soon: | :soon: | :soon: | :soon: |
| redis | :heavy_check_mark: | :soon: | :soon: | :soon: | :soon: | :soon: |
| nginx | :soon: | :soon: | :soon: | :soon: | :soon: | :soon: |

`SQLite` with
1. `gcc` on `x86`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [SQLite](https://github.com/unikraft/app-sqlite)
        1. [lib-sqlite](https://github.com/unikraft/lib-sqlite), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded)

    * the file hierarchy should look somethig like this:
    ```
    workdir
    |---apps/
    |      |---app-sqlite/
    |---libs/
    |      |---lib-newlib/
    |      |---lib-pthread-embedded/
    |      |---lib-sqlite/
    |---unikraft/
    ```

    * create a `Makefile` in the `app-sqlite` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-sqlite

    all:
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-sqlite` directory:
    ```Makefile
    $(eval $(call addlib,appsqlite))
    ```

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. choose all `Virtio` options from `Platform configuration`->`KVM guest`->`Virtio`
        1. choose the `SQLite`, `vfscore` libraries
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`

    * build your app: `make`

    * create a directory named `fs0` in your `app-sqlite` directory

    * inside it create this `scrip.sql` script:
    ```sql
    CREATE TABLE tab (d1 int, d2 text);
    INSERT INTO tab VALUES (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text)),
    (random(), cast(random() as text));    
    ```

    * run your app:
    ```bash
    sudo qemu-system-x86_64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                        -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                        -kernel "build/app-sqlite_kvm-x86_64" \
                        -enable-kvm \
                        -nographic
    ```
    * test your app:
    ```   
    Booting from ROM..Powered by
    o.   .o       _ _               __ _
    Oo   Oo  ___ (_) | __ __  __ _ ' _) :_
    oO   oO ' _ `| | |/ /  _)' _` | |_|  _)
    oOo oOO| | | | |   (| | | (_) |  _) :_
    OoOoO ._, ._:_:_,\_._,  .__,_:_, \___)
                    Hyperion 0.9.0~114d68f
    SQLite version 3.30.1 2019-10-10 20:19:45
    Enter ".help" for usage hints.
    Connected to a transient in-memory database.
    Use ".open FILENAME" to reopen on a persistent database.
    sqlite> .read script.sql
    sqlite> select * from tab;
    -2598232588344095435|1923627277247678914
    7790299239285968477|4973033440056760928
    3020038246602366850|8695866250257312490
    6217426662191579420|-1592250482760658193
    608890250097999548|5290270790231607559
    -7147332586259853486|-2310422059032105026
    -7017396189164542140|572686747851281464
    2391719607325216351|7670576770537357123
    -8700988160769355826|8437480409959948743
    4777281798949903565|5443697212645358365
    sqlite> .exit
    ```