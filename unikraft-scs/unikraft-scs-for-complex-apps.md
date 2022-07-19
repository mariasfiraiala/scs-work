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
    SeaBIOS (version 1.10.2-1ubuntu1)
    Booting from ROM...
    Powered by
    o.   .o       _ _               __ _
    Oo   Oo  ___ (_) | __ __  __ _ ' _) :_
    oO   oO ' _ `| | |/ /  _)' _` | |_|  _)
    oOo oOO| | | | |   (| | | (_) |  _) :_
    OoOoO ._, ._:_:_,\_._,  .__,_:_, \___)
                    Tethys 0.5.0~825b115
    SQLite version 3.30.1 2019-10-10 20:19:45
    Enter ".help" for usage hints.
    sqlite> .read script.sql
    sqlite> select * from tab;
    -4482895989777805454|-110319092326802521
    1731384004930241734|4521105937488475129
    394829130239418471|-5931220326625632549
    4715172377251814631|3421393665393635031
    2633802986882468389|174376437407985264
    -1691186051150364618|3056262814461654943
    -4054754806183404125|-2391909815601847844
    -4437812378917371546|-6267837926735068846
    8830824471222267926|7672933566995619644
    4185269687730257244|-3477150175417807640
    sqlite> .exit
    ```