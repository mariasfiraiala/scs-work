# Shadow Stack for Unikraft complex applications

I am testing 3 complex applications `SQLite`, `redis` and `nginx` in order to bring Shadow Stack support.

| App\Compiler | gcc - x86 | gcc - aarch64 | clang - x86 | clang - aarch64 | clang with scs | gcc-12 with scs |
|--------------|-----------|---------------|-------------|-----------------|----------------|-----------------|
| SQLite | :heavy_check_mark: | :heavy_check_mark: | :soon: | :heavy_check_mark: | :soon: | :soon: |
| redis | :heavy_check_mark: | :heavy_check_mark: | :soon: | :heavy_check_mark: | :soon: | :soon: |
| nginx | :heavy_check_mark: | :heavy_check_mark: | :soon: | :heavy_check_mark: | :soon: | :soon: |

`SQLite` with
1. `gcc` on `x86`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [SQLite](https://github.com/unikraft/app-sqlite)
        1. [lib-sqlite](https://github.com/unikraft/lib-sqlite), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded)

    * the file hierarchy should look something like this:
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

    * inside it create this `script.sql` script:
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

2. `gcc` on `AArch64`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [SQLite](https://github.com/unikraft/app-sqlite)
        1. [lib-sqlite](https://github.com/unikraft/lib-sqlite), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded)

    * the file hierarchy should look something like this:
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

    * modify `libs/lib-newlib/include/limits.h` by adding `defined(__ARM_64__)` like so
    ```C
    #if defined(__x86_64__) || defined(__ARM_64__)
    # define LONG_MAX       0x7fffffffffffffffL
    # define ULONG_MAX      0xffffffffffffffffUL
    #else
    # define LONG_MAX       0x7fffffffL
    # define ULONG_MAX      0xffffffffUL
    #endif
    #define LONG_MIN        (-LONG_MAX-1L)
    #define LLONG_MAX       0x7fffffffffffffffLL
    #define LLONG_MIN       (-LLONG_MAX-1LL)
    #define ULLONG_MAX      0xffffffffffffffffULL
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

    * in `unikraft/lib/posix-user/user.c` remove the `__uk_tls` from these 2 functions:
    ```C
    static __uk_tls struct passwd_entry
    [...]
    static __uk_tls struct group_entry
    ```

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. choose all `Virtio` options from `Platform configuration`->`KVM guest`->`Virtio`
        1. choose the `SQLite`, `vfscore` libraries
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`

    * build your app: `make`

    * create a directory named `fs0` in your `app-sqlite` directory

    * inside it create this `script.sql` script:
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
    sudo qemu-system-aarch64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                             -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                             -kernel "build/app-sqlite_kvm-arm64" \
                             -machine virt \
                             -cpu cortex-a57 \
                             -nographic
    ```

    * test your app:
    ```
    Powered by
    o.   .o       _ _               __ _
    Oo   Oo  ___ (_) | __ __  __ _ ' _) :_
    oO   oO ' _ `| | |/ /  _)' _` | |_|  _)
    oOo oOO| | | | |   (| | | (_) |  _) :_
    OoOoO ._, ._:_:_,\_._,  .__,_:_, \___)
            Hyperion 0.9.0~a10a19b-custom
    SQLite version 3.30.1 2019-10-10 20:19:45
    Enter ".help" for usage hints.
    Connected to a transient in-memory database.
    Use ".open FILENAME" to reopen on a persistent database.
    sqlite> .read script.sql
    sqlite> select * from tab;
    1110980029405251417|4790364924879920278
    1139113440994134737|1770109440632367248
    -431795700753794536|2260207729281847092
    1203125865995321955|-6480314585029203527
    602727467839614662|7985152841375895325
    -4375719181948920944|-8601960380790611479
    5769441887793396766|6690185065200255103
    3505855085187169777|-3449770537330452658
    7295426976800648951|-7727476964388433668
    2408680503256646790|-4737393212733069271
    sqlite> .exit
    ```

3. `clang` on `AArch64`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [SQLite](https://github.com/unikraft/app-sqlite)
        1. [lib-sqlite](https://github.com/unikraft/lib-sqlite), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded)

    * the file hierarchy should look something like this:
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

    * modify `libs/lib-newlib/include/limits.h` by adding `defined(__ARM_64__)` like so
    ```C
    #if defined(__x86_64__) || defined(__ARM_64__)
    # define LONG_MAX       0x7fffffffffffffffL
    # define ULONG_MAX      0xffffffffffffffffUL
    #else
    # define LONG_MAX       0x7fffffffL
    # define ULONG_MAX      0xffffffffUL
    #endif
    #define LONG_MIN        (-LONG_MAX-1L)
    #define LLONG_MAX       0x7fffffffffffffffLL
    #define LLONG_MIN       (-LLONG_MAX-1LL)
    #define ULLONG_MAX      0xffffffffffffffffULL
    ```

    * in `unikraft/lib/posix-user/user.c` remove the `__uk_tls` from these 2 functions:
    ```C
    static __uk_tls struct passwd_entry
    [...]
    static __uk_tls struct group_entry
    ```

    * modify these files as it follows:
    ```patch
    diff --git a/Makefile b/Makefile
    index e9f4044..b00796e 100644
    --- a/Makefile
    +++ b/Makefile
    @@ -616,6 +616,10 @@ CC_VERSION	:= $(shell $(CC) --version | \
    CC_VER_MAJOR   := $(word 1,$(subst ., ,$(CC_VERSION)))
    CC_VER_MINOR   := $(word 2,$(subst ., ,$(CC_VERSION)))
    
    +CFLAGS          += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +ASFLAGS         += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +CXXFLAGS        += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +
    ASFLAGS		+= -DCC_VERSION=$(CC_VERSION)
    CFLAGS		+= -DCC_VERSION=$(CC_VERSION)
    CXXFLAGS	+= -DCC_VERSION=$(CC_VERSION)
    diff --git a/plat/common/pci_ecam.c b/plat/common/pci_ecam.c
    index c884f11..7c069db 100644
    --- a/plat/common/pci_ecam.c
    +++ b/plat/common/pci_ecam.c
    @@ -226,7 +226,7 @@ int gen_pci_irq_parse(const fdt32_t *addr, struct fdt_phandle_args *out_irq)
        fdt32_t initial_match_array[16];
        const fdt32_t *match_array = initial_match_array;
        const fdt32_t *tmp, *imap, *imask;
    -	const fdt32_t dummy_imask[] = { [0 ... 16] = cpu_to_fdt32(~0) };
    +	const fdt32_t dummy_imask[] = { 0 };
        int intsize, newintsize;
        int addrsize, newaddrsize = 0;
        int imaplen, match, i, rc = -EINVAL;
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
        1. disable all erratum options from `Architecture Selection` -> `Arm8 Compatible` -> `Workaround for [...] erratum`
        1. choose all `Virtio` options from `Platform configuration`->`KVM guest`->`Virtio`
        1. choose the `SQLite`, `vfscore` libraries
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`
        1. configure the `Custom cross-compiler LLVM target` from `Build Options` -> `Custom cross-compiler LLVM target`; write `aarch64-none-elf`

    * get your cross-compiling toolchain from [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads)

    * build your app: 
    ```
    make CC=clang LD=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-gcc OBJCOPY=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-objcopy STRIP=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-strip
    ```

    * fix `gcc`-isms errors using [this link](https://sourceware.org/pipermail/glibc-cvs/2021q3/074785.html)

    * create a directory named `fs0` in your `app-sqlite` directory

    * inside it create this `script.sql` script:
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
    sudo qemu-system-aarch64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                             -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                             -kernel "build/app-sqlite_kvm-arm64" \
                             -machine virt \
                             -cpu cortex-a57 \
                             -nographic
    ```

    * test your app:
    ```
    Powered by
    o.   .o       _ _               __ _
    Oo   Oo  ___ (_) | __ __  __ _ ' _) :_
    oO   oO ' _ `| | |/ /  _)' _` | |_|  _)
    oOo oOO| | | | |   (| | | (_) |  _) :_
    OoOoO ._, ._:_:_,\_._,  .__,_:_, \___)
            Hyperion 0.9.0~a10a19b-custom
    SQLite version 3.30.1 2019-10-10 20:19:45
    Enter ".help" for usage hints.
    Connected to a transient in-memory database.
    Use ".open FILENAME" to reopen on a persistent database.
    sqlite> .read script.sql
    sqlite> select * from tab;
    1110980029405251417|4790364924879920278
    1139113440994134737|1770109440632367248
    -431795700753794536|2260207729281847092
    1203125865995321955|-6480314585029203527
    602727467839614662|7985152841375895325
    -4375719181948920944|-8601960380790611479
    5769441887793396766|6690185065200255103
    3505855085187169777|-3449770537330452658
    7295426976800648951|-7727476964388433668
    2408680503256646790|-4737393212733069271
    sqlite> .exit
    ```

-------------------------------------------------------------------------------

`redis` with
1. `gcc` on `x86`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [redis](https://github.com/unikraft/app-redis)
        1. [lib-redis](https://github.com/unikraft/lib-redis), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look something like this:
    ```
    workdir
    |---apps/
    |      |---app-redis/
    |---libs/
    |      |---lib-lwip/
    |      |---lib-newlib/
    |      |---lib-pthread-embedded/
    |      |---lib-redis/
    |---unikraft/
    ```

    * create a `Makefile` in the `app-redis` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-lwip:$(UK_LIBS)/lib-redis

    all:
	    @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
	    @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-redis` directory:
    ```Makefile
    $(eval $(call addlib,appredis))
    ```

    * create a directory named `fs0` in the `app-redis` directory, download [this file](https://github.com/unikraft/summer-of-code-2021/blob/main/content/en/docs/sessions/04-complex-applications/sol/03-set-up-and-run-redis/redis.conf) and move it there

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. choose your usuals from `Platform Configuration`
        1. choose the `libredis`, `lwip`, `libnewlib`, `libpthread-embedded` libraries from `Library Configuration`
        1. in the `libredis` library choose the `Provide main function` option
        1. in the `lwip` library make sure to have `IPv4`, `UDP support`, `TCP support`, `ICMP support`, `DHCP support`, `Socket API` enabled
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`

    * build your app: `make`

    * use these commands to prepare running your app:
    ```bash
    $ sudo brctl addbr kraft0
    $ sudo ip a a  172.44.0.1/24 dev kraft0
    $ sudo ip l set dev kraft0 up
    ```

    * check your setup:
    ```
    $ ip a s kraft0
    6: kraft0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UNKNOWN group default qlen 1000
        link/ether 8a:08:a1:69:85:31 brd ff:ff:ff:ff:ff:ff
        inet 172.44.0.1/24 scope global kraft0
        valid_lft forever preferred_lft forever
        inet6 fe80::8808:a1ff:fe69:8531/64 scope link 
        valid_lft forever preferred_lft forever
    ```

    * run your app:
    ```
    sudo qemu-system-x86_64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                            -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                            -netdev bridge,id=en0,br=kraft0 \
                            -device virtio-net-pci,netdev=en0 \
                            -kernel "build/app-redis_kvm-x86_64" \
                            -append "netdev.ipv4_addr=172.44.0.2 netdev.ipv4_gw_addr=172.44.0.1 netdev.ipv4_subnet_mask=255.255.255.0 -- /redis.conf" \
                            -cpu host \
                            -enable-kvm \
                            -nographic
    ```

    * clean up your work:
    ```
    $ sudo ip l set dev kraft0 down
    $ sudo brctl delbr kraft0
    ```

2. `gcc` on `AArch64`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [redis](https://github.com/unikraft/app-redis)
        1. [lib-redis](https://github.com/unikraft/lib-redis), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look something like this:
    ```
    workdir
    |---apps/
    |      |---app-redis/
    |---libs/
    |      |---lib-lwip/
    |      |---lib-newlib/
    |      |---lib-pthread-embedded/
    |      |---lib-redis/
    |---unikraft/
    ```

    * modify `libs/lib-newlib/include/limits.h` by adding `defined(__ARM_64__)` like so
    ```C
    #if defined(__x86_64__) || defined(__ARM_64__)
    # define LONG_MAX       0x7fffffffffffffffL
    # define ULONG_MAX      0xffffffffffffffffUL
    #else
    # define LONG_MAX       0x7fffffffL
    # define ULONG_MAX      0xffffffffUL
    #endif
    #define LONG_MIN        (-LONG_MAX-1L)
    #define LLONG_MAX       0x7fffffffffffffffLL
    #define LLONG_MIN       (-LLONG_MAX-1LL)
    #define ULLONG_MAX      0xffffffffffffffffULL
    ```

    * create a `Makefile` in the `app-redis` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-lwip:$(UK_LIBS)/lib-redis

    all:
	    @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
	    @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-redis` directory:
    ```Makefile
    $(eval $(call addlib,appredis))
    ```

    * create a directory named `fs0` in the `app-redis` directory, download [this file](https://github.com/unikraft/summer-of-code-2021/blob/main/content/en/docs/sessions/04-complex-applications/sol/03-set-up-and-run-redis/redis.conf) and move it there

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. choose your usuals from `Platform Configuration`
        1. choose the `libredis`, `lwip`, `libnewlib`, `libpthread-embedded` libraries from `Library Configuration`
        1. in the `libredis` library choose the `Provide main function` option
        1. in the `lwip` library make sure to have `IPv4`, `UDP support`, `TCP support`, `ICMP support`, `DHCP support`, `Socket API` enabled
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`

    * build your app: `make`

    * use these commands to prepare running your app:
    ```bash
    $ sudo brctl addbr kraft0
    $ sudo ip a a  172.44.0.1/24 dev kraft0
    $ sudo ip l set dev kraft0 up
    ```

    * check your setup:
    ```
    $ ip a s kraft0
    6: kraft0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UNKNOWN group default qlen 1000
        link/ether 8a:08:a1:69:85:31 brd ff:ff:ff:ff:ff:ff
        inet 172.44.0.1/24 scope global kraft0
        valid_lft forever preferred_lft forever
        inet6 fe80::8808:a1ff:fe69:8531/64 scope link 
        valid_lft forever preferred_lft forever
    ```

    * run your app:
    ```
    sudo qemu-system-aarch64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                             -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                             -netdev bridge,id=en0,br=kraft0 \
                             -device virtio-net-pci,netdev=en0 \
                             -kernel "build/app-redis_kvm-arm64" \
                             -append "netdev.ipv4_addr=172.44.0.2 netdev.ipv4_gw_addr=172.44.0.1 netdev.ipv4_subnet_mask=255.255.255.0 -- /redis.conf" \
                             -machine virt \
                             -cpu cortex-a57 \
                             -nographic
    ```

    * in order to be able to make `redis` accept requests you'll have to modify `lib-newlib/include/fcntl.h` as it follows:
    ```C
    [...]
    #elif ((defined CONFIG_ARCH_ARM_64) || (defined CONFIG_ARCH_ARM_32))
    // redefine FNONBLOCK as well
    #undef FNONBLOCK
    #define FNONBLOCK 04000

    // redefine O_NONBLOCK
    #undef O_NONBLOCK
    #define O_NONBLOCK  FNONBLOCK
    [...]
    ```

    * test your app: download [this executable](https://github.com/unikraft/summer-of-code-2021/blob/main/content/en/docs/sessions/04-complex-applications/sol/03-set-up-and-run-redis/redis-cli) and run it using this command:
    ```
    $./redis-cli -h 172.44.0.2 -p 6379
    172.44.0.2:6379> PING
    PONG
    172.44.0.2:6379> exit
    ```

    * clean up your work:
    ```
    $ sudo ip l set dev kraft0 down
    $ sudo brctl delbr kraft0
    ```

3. `clang` on `AArch64`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [redis](https://github.com/unikraft/app-redis)
        1. [lib-redis](https://github.com/unikraft/lib-redis), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look something like this:
    ```
    workdir
    |---apps/
    |      |---app-redis/
    |---libs/
    |      |---lib-lwip/
    |      |---lib-newlib/
    |      |---lib-pthread-embedded/
    |      |---lib-redis/
    |---unikraft/
    ```

    * modify `libs/lib-newlib/include/limits.h` by adding `defined(__ARM_64__)` like so
    ```C
    #if defined(__x86_64__) || defined(__ARM_64__)
    # define LONG_MAX       0x7fffffffffffffffL
    # define ULONG_MAX      0xffffffffffffffffUL
    #else
    # define LONG_MAX       0x7fffffffL
    # define ULONG_MAX      0xffffffffUL
    #endif
    #define LONG_MIN        (-LONG_MAX-1L)
    #define LLONG_MAX       0x7fffffffffffffffLL
    #define LLONG_MIN       (-LLONG_MAX-1LL)
    #define ULLONG_MAX      0xffffffffffffffffULL
    ```

    * modify these files as it follows:
    ```patch
    diff --git a/Makefile b/Makefile
    index e9f4044..b00796e 100644
    --- a/Makefile
    +++ b/Makefile
    @@ -616,6 +616,10 @@ CC_VERSION	:= $(shell $(CC) --version | \
    CC_VER_MAJOR   := $(word 1,$(subst ., ,$(CC_VERSION)))
    CC_VER_MINOR   := $(word 2,$(subst ., ,$(CC_VERSION)))
    
    +CFLAGS          += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +ASFLAGS         += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +CXXFLAGS        += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +
    ASFLAGS		+= -DCC_VERSION=$(CC_VERSION)
    CFLAGS		+= -DCC_VERSION=$(CC_VERSION)
    CXXFLAGS	+= -DCC_VERSION=$(CC_VERSION)
    diff --git a/plat/common/pci_ecam.c b/plat/common/pci_ecam.c
    index c884f11..7c069db 100644
    --- a/plat/common/pci_ecam.c
    +++ b/plat/common/pci_ecam.c
    @@ -226,7 +226,7 @@ int gen_pci_irq_parse(const fdt32_t *addr, struct fdt_phandle_args *out_irq)
        fdt32_t initial_match_array[16];
        const fdt32_t *match_array = initial_match_array;
        const fdt32_t *tmp, *imap, *imask;
    -	const fdt32_t dummy_imask[] = { [0 ... 16] = cpu_to_fdt32(~0) };
    +	const fdt32_t dummy_imask[] = {cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0),
    +				                   cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0),
    +				                   cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0),
    +				                   cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0)};
        int intsize, newintsize;
        int addrsize, newaddrsize = 0;
        int imaplen, match, i, rc = -EINVAL;
    ```

    * to be able to run with `clang`, modify `unikraft/plat/drivers/virtio/virtio_pci.c`
    ```patch
    @@ -273,10 +273,12 @@ static int vpci_legacy_pci_config_get(struct virtio_dev *vdev, __u16 offset,
                    VIRTIO_PCI_CONFIG_OFF + offset, buf, len,
                    type_len);
        } else {
    +		__u32 buf_len_bytes = len * type_len;
    +
            rc = virtio_cread_bytes_many(
                    (void *) (unsigned long)vpdev->pci_base_addr,
    -				VIRTIO_PCI_CONFIG_OFF + offset,	buf, len);
    -		if (rc != (int)len)
    +				VIRTIO_PCI_CONFIG_OFF + offset,	buf, buf_len_bytes);
    +		if (rc != (int)(buf_len_bytes))
                return -EFAULT;
        }
    ```

    * create a `Makefile` in the `app-redis` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-lwip:$(UK_LIBS)/lib-redis

    all:
	    @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
	    @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-redis` directory:
    ```Makefile
    $(eval $(call addlib,appredis))
    ```

    * create a directory named `fs0` in the `app-redis` directory, download [this file](https://github.com/unikraft/summer-of-code-2021/blob/main/content/en/docs/sessions/04-complex-applications/sol/03-set-up-and-run-redis/redis.conf) and move it there
    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. disable all erratum options from `Architecture Selection` -> `Arm8 Compatible` -> `Workaround for [...] erratum`
        1. choose all `Virtio` options from `Platform configuration`->`KVM guest`->`Virtio`
        1. choose the `libredis`, `lwip`, `libnewlib`, `libpthread-embedded` libraries from `Library Configuration`
        1. in the `libredis` library choose the `Provide main function` option
        1. in the `lwip` library make sure to have `IPv4`, `UDP support`, `TCP support`, `ICMP support`, `DHCP support`, `Socket API` enabled
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`
        1. configure the `Custom cross-compiler LLVM target` from `Build Options` -> `Custom cross-compiler LLVM target`; write `aarch64-none-elf`

    * get your cross-compiling toolchain from [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads)

    * build your app: 
    ```
    make CC=clang LD=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-gcc OBJCOPY=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-objcopy STRIP=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-strip
    ```

    * fix `gcc`-isms errors using [this link](https://sourceware.org/pipermail/glibc-cvs/2021q3/074785.html)

    * use these commands to prepare running your app:
    ```bash
    $ sudo brctl addbr kraft0
    $ sudo ip a a  172.44.0.1/24 dev kraft0
    $ sudo ip l set dev kraft0 up
    ```

    * check your setup:
    ```
    $ ip a s kraft0
    6: kraft0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UNKNOWN group default qlen 1000
        link/ether 8a:08:a1:69:85:31 brd ff:ff:ff:ff:ff:ff
        inet 172.44.0.1/24 scope global kraft0
        valid_lft forever preferred_lft forever
        inet6 fe80::8808:a1ff:fe69:8531/64 scope link 
        valid_lft forever preferred_lft forever
    ```

    * run your app:
    ```
    sudo qemu-system-aarch64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                             -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                             -netdev bridge,id=en0,br=kraft0 \
                             -device virtio-net-pci,netdev=en0 \
                             -kernel "build/app-redis_kvm-arm64" \
                             -append "netdev.ipv4_addr=172.44.0.2 netdev.ipv4_gw_addr=172.44.0.1 netdev.ipv4_subnet_mask=255.255.255.0 -- /redis.conf" \
                             -machine virt \
                             -cpu cortex-a57 \
                             -nographic
    ```

    * in order to be able to make `redis` accept requests you'll have to modify `lib-newlib/include/fcntl.h` as it follows:
    ```C
    [...]
    #elif ((defined CONFIG_ARCH_ARM_64) || (defined CONFIG_ARCH_ARM_32))
    // redefine FNONBLOCK as well
    #undef FNONBLOCK
    #define FNONBLOCK 04000

    // redefine O_NONBLOCK
    #undef O_NONBLOCK
    #define O_NONBLOCK  FNONBLOCK
    [...]
    ```

    * test your app: download [this executable](https://github.com/unikraft/summer-of-code-2021/blob/main/content/en/docs/sessions/04-complex-applications/sol/03-set-up-and-run-redis/redis-cli) and run it using this command:
    ```
    $./redis-cli -h 172.44.0.2 -p 6379
    172.44.0.2:6379> PING
    PONG
    172.44.0.2:6379> exit
    ```

    * clean up your work:
    ```
    $ sudo ip l set dev kraft0 down
    $ sudo brctl delbr kraft0
    ```

-------------------------------------------------------------------------------

`nginx` with 
1. `gcc` on `x86`
    
    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [nginx](https://github.com/unikraft/app-nginx)
        1. [lib-nginx](https://github.com/unikraft/lib-nginx), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look something like this:
    ```
    workdir
    |---apps/
    |      |---app-nginx/
    |---libs/
    |      |---lib-lwip/
    |      |---lib-newlib/
    |      |---lib-nginx/
    |      |---lib-pthread-embedded/
    |---unikraft/
    ```

    * create a `Makefile` in the `app-nginx` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-lwip:$(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-nginx

    all:
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-nginx` directory:
    ```Makefile
    $(eval $(call addlib,appnginx))
    ```

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. choose your usuals from `Platform Configuration`
        1. choose the `libnginx`, `lwip`, `libnewlib`, `libpthread-embedded` libraries from `Library Configuration`
        1. in the `libnginx` library choose the `Provide main function` option
        1. in the `lwip` library make sure to have `IPv4`, `UDP support`, `TCP support`, `ICMP support`, `DHCP support`, `Socket API` enabled
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`

    * build your app: `make`

    * use these commands to prepare running your app:
    ```bash
    $ sudo brctl addbr kraft0
    $ sudo ip a a  172.44.0.1/24 dev kraft0
    $ sudo ip l set dev kraft0 up
    ```

    * check your setup:
    ```
    $ ip a s kraft0
    6: kraft0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UNKNOWN group default qlen 1000
        link/ether 8a:08:a1:69:85:31 brd ff:ff:ff:ff:ff:ff
        inet 172.44.0.1/24 scope global kraft0
        valid_lft forever preferred_lft forever
        inet6 fe80::8808:a1ff:fe69:8531/64 scope link 
        valid_lft forever preferred_lft forever
    ```

    * run your app:
    ```
    sudo qemu-system-x86_64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                            -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                            -netdev bridge,id=en0,br=kraft0 \
                            -device virtio-net-pci,netdev=en0 \
                            -kernel "build/app-nginx_kvm-x86_64" \
                            -append "netdev.ipv4_addr=172.44.0.2 netdev.ipv4_gw_addr=172.44.0.1 netdev.ipv4_subnet_mask=255.255.255.0 --" \
                            -cpu host \
                            -enable-kvm \
                            -nographic
    ```

    * if you're met with this error
    ```
    failed to parse default acl file `/etc/qemu/bridge.conf'
    qemu-system-x86_64: bridge helper failed
    ```
    make sure to add this to your `/etc/qemu/bridge.conf` file; if it doesn't exist, create it
    ```
    allow br0
    allow uk0
    allow all
    ```

    * test your app: open a new terminal, or use `tmux` and give this command to retrive data from the server
    ```bash
    $ wget 172.44.0.2
    --2021-08-18 16:47:38--  http://172.44.0.2/
    --2022-07-19 21:17:17--  http://172.44.0.2/
    Connecting to 172.44.0.2:80... connected.
    HTTP request sent, awaiting response... 200 OK
    Length: 180 [text/html]
    Saving to: ‘index.html’

    index.html                                         100%[===============================================================================================================>]     180  --.-KB/s    in 0s      

    2022-07-19 21:17:17 (14,7 MB/s) - ‘index.html’ saved [180/180]
    ```

    ```bash
    $ cat index.html 
    <!DOCTYPE html>
    <html>
    <head>
    <title>Hello, world!</title>
    </head>
    <body>
    <h1>Hello, world!</h1>
    <p>Powered by <a href="http://unikraft.org">Unikraft</a>.</p>
    </body>
    </html>
    ```

    * clean up your work:
    ```
    $ sudo ip l set dev kraft0 down
    $ sudo brctl delbr kraft0
    ```

2. `gcc` on `AArch64`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [nginx](https://github.com/unikraft/app-nginx)
        1. [lib-nginx](https://github.com/unikraft/lib-nginx), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look something like this:
    ```
    workdir
    |---apps/
    |      |---app-nginx/
    |---libs/
    |      |---lib-lwip/
    |      |---lib-newlib/
    |      |---lib-nginx/
    |      |---lib-pthread-embedded/
    |---unikraft/
    ```

    * modify `libs/lib-newlib/include/limits.h` by adding `defined(__ARM_64__)` like so
    ```C
    #if defined(__x86_64__) || defined(__ARM_64__)
    # define LONG_MAX       0x7fffffffffffffffL
    # define ULONG_MAX      0xffffffffffffffffUL
    #else
    # define LONG_MAX       0x7fffffffL
    # define ULONG_MAX      0xffffffffUL
    #endif
    #define LONG_MIN        (-LONG_MAX-1L)
    #define LLONG_MAX       0x7fffffffffffffffLL
    #define LLONG_MIN       (-LLONG_MAX-1LL)
    #define ULLONG_MAX      0xffffffffffffffffULL
    ```

    * in `unikraft/lib/posix-user/user.c` remove the `__uk_tls` from these 2 functions:
    ```C
    static __uk_tls struct passwd_entry
    [...]
    static __uk_tls struct group_entry
    ```

    * create a `Makefile` in the `app-nginx` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-lwip:$(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-nginx

    all:
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-nginx` directory:
    ```Makefile
    $(eval $(call addlib,appnginx))
    ```

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. choose your usuals from `Platform Configuration`
        1. choose the `libnginx`, `lwip`, `libnewlib`, `libpthread-embedded` libraries from `Library Configuration`
        1. in the `libnginx` library choose the `Provide main function` option
        1. in the `lwip` library make sure to have `IPv4`, `UDP support`, `TCP support`, `ICMP support`, `DHCP support`, `Socket API` enabled
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`

    * build your app: `make`

    * use these commands to prepare running your app:
    ```bash
    $ sudo brctl addbr kraft0
    $ sudo ip a a  172.44.0.1/24 dev kraft0
    $ sudo ip l set dev kraft0 up
    ```

    * check your setup:
    ```
    $ ip a s kraft0
    6: kraft0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UNKNOWN group default qlen 1000
        link/ether 8a:08:a1:69:85:31 brd ff:ff:ff:ff:ff:ff
        inet 172.44.0.1/24 scope global kraft0
        valid_lft forever preferred_lft forever
        inet6 fe80::8808:a1ff:fe69:8531/64 scope link 
        valid_lft forever preferred_lft forever
    ```

    * run your app:
    ```
    sudo qemu-system-aarch64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                             -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                             -netdev bridge,id=en0,br=kraft0 \
                             -device virtio-net-pci,netdev=en0 \
                             -kernel "build/app-nginx_kvm-arm64" \
                             -append "netdev.ipv4_addr=172.44.0.2 netdev.ipv4_gw_addr=172.44.0.1 netdev.ipv4_subnet_mask=255.255.255.0 --" \
                             -machine virt \
                             -cpu cortex-a57 \
                             -nographic
    ```

    * test your app: open a new terminal, or use `tmux` and give this command to retrive data from the server
    ```bash
    $ wget 172.44.0.2
    --2021-08-18 16:47:38--  http://172.44.0.2/
    --2022-07-19 21:17:17--  http://172.44.0.2/
    Connecting to 172.44.0.2:80... connected.
    HTTP request sent, awaiting response... 200 OK
    Length: 180 [text/html]
    Saving to: ‘index.html’

    index.html                                         100%[===============================================================================================================>]     180  --.-KB/s    in 0s      

    2022-07-19 21:17:17 (14,7 MB/s) - ‘index.html’ saved [180/180]
    ```

    ```bash
    $ cat index.html 
    <!DOCTYPE html>
    <html>
    <head>
    <title>Hello, world!</title>
    </head>
    <body>
    <h1>Hello, world!</h1>
    <p>Powered by <a href="http://unikraft.org">Unikraft</a>.</p>
    </body>
    </html>
    ```

    * clean up your work:
    ```
    $ sudo ip l set dev kraft0 down
    $ sudo brctl delbr kraft0
    ```

3. `clang` on `AArch64`

    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [nginx](https://github.com/unikraft/app-nginx)
        1. [lib-nginx](https://github.com/unikraft/lib-nginx), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look something like this:
    ```
    workdir
    |---apps/
    |      |---app-nginx/
    |---libs/
    |      |---lib-lwip/
    |      |---lib-newlib/
    |      |---lib-nginx/
    |      |---lib-pthread-embedded/
    |---unikraft/
    ```

    * modify `libs/lib-newlib/include/limits.h` by adding `defined(__ARM_64__)` like so
    ```C
    #if defined(__x86_64__) || defined(__ARM_64__)
    # define LONG_MAX       0x7fffffffffffffffL
    # define ULONG_MAX      0xffffffffffffffffUL
    #else
    # define LONG_MAX       0x7fffffffL
    # define ULONG_MAX      0xffffffffUL
    #endif
    #define LONG_MIN        (-LONG_MAX-1L)
    #define LLONG_MAX       0x7fffffffffffffffLL
    #define LLONG_MIN       (-LLONG_MAX-1LL)
    #define ULLONG_MAX      0xffffffffffffffffULL
    ```

    * in `unikraft/lib/posix-user/user.c` remove the `__uk_tls` from these 2 functions:
    ```C
    static __uk_tls struct passwd_entry
    [...]
    static __uk_tls struct group_entry
    ```

    * modify these files as it follows:
    ```patch
    diff --git a/Makefile b/Makefile
    index e9f4044..b00796e 100644
    --- a/Makefile
    +++ b/Makefile
    @@ -616,6 +616,10 @@ CC_VERSION	:= $(shell $(CC) --version | \
    CC_VER_MAJOR   := $(word 1,$(subst ., ,$(CC_VERSION)))
    CC_VER_MINOR   := $(word 2,$(subst ., ,$(CC_VERSION)))
    
    +CFLAGS          += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +ASFLAGS         += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +CXXFLAGS        += --target=$(CONFIG_LLVM_TARGET_ARCH)
    +
    ASFLAGS		+= -DCC_VERSION=$(CC_VERSION)
    CFLAGS		+= -DCC_VERSION=$(CC_VERSION)
    CXXFLAGS	+= -DCC_VERSION=$(CC_VERSION)
    diff --git a/plat/common/pci_ecam.c b/plat/common/pci_ecam.c
    index c884f11..7c069db 100644
    --- a/plat/common/pci_ecam.c
    +++ b/plat/common/pci_ecam.c
    @@ -226,7 +226,7 @@ int gen_pci_irq_parse(const fdt32_t *addr, struct fdt_phandle_args *out_irq)
        fdt32_t initial_match_array[16];
        const fdt32_t *match_array = initial_match_array;
        const fdt32_t *tmp, *imap, *imask;
    -	const fdt32_t dummy_imask[] = { [0 ... 16] = cpu_to_fdt32(~0) };
    +	const fdt32_t dummy_imask[] = {cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0),
    +				                   cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0),
    +				                   cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0),
    +				                   cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0), cpu_to_fdt32(~0)};
        int intsize, newintsize;
        int addrsize, newaddrsize = 0;
        int imaplen, match, i, rc = -EINVAL;
    ```

    * to be able to run with `clang`, modify `unikraft/plat/drivers/virtio/virtio_pci.c`
    ```patch
    @@ -273,10 +273,12 @@ static int vpci_legacy_pci_config_get(struct virtio_dev *vdev, __u16 offset,
                    VIRTIO_PCI_CONFIG_OFF + offset, buf, len,
                    type_len);
        } else {
    +		__u32 buf_len_bytes = len * type_len;
    +
            rc = virtio_cread_bytes_many(
                    (void *) (unsigned long)vpdev->pci_base_addr,
    -				VIRTIO_PCI_CONFIG_OFF + offset,	buf, len);
    -		if (rc != (int)len)
    +				VIRTIO_PCI_CONFIG_OFF + offset,	buf, buf_len_bytes);
    +		if (rc != (int)(buf_len_bytes))
                return -EFAULT;
        }
    ```

    * create a `Makefile` in the `app-nginx` directory:
    ```Makefile
    UK_ROOT ?= $(PWD)/../../unikraft
    UK_LIBS ?= $(PWD)/../../libs
    LIBS := $(UK_LIBS)/lib-lwip:$(UK_LIBS)/lib-pthread-embedded:$(UK_LIBS)/lib-newlib:$(UK_LIBS)/lib-nginx

    all:
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

    $(MAKECMDGOALS):
        @$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)
    ```

    * create a `Makefile.uk` in the `app-nginx` directory:
    ```Makefile
    $(eval $(call addlib,appnginx))
    ```

    * configure your app: `make menuconfig`
        1. choose your usuals from `Architecture Selection`
        1. disable all erratum options from `Architecture Selection` -> `Arm8 Compatible` -> `Workaround for [...] erratum`
        1. choose all `Virtio` options from `Platform configuration`->`KVM guest`->`Virtio`
        1. choose the `libnginx`, `lwip`, `libnewlib`, `libpthread-embedded` libraries from `Library Configuration`
        1. in the `libnginx` library choose the `Provide main function` option
        1. in the `lwip` library make sure to have `IPv4`, `UDP support`, `TCP support`, `ICMP support`, `DHCP support`, `Socket API` enabled
        1. from the `vfscore` library,choose the `Default root filesystem` to be `9pfs`
        1. configure the `Custom cross-compiler LLVM target` from `Build Options` -> `Custom cross-compiler LLVM target`; write `aarch64-none-elf`

    * get your cross-compiling toolchain from [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads)

    * build your app: 
    ```
    make CC=clang LD=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-gcc OBJCOPY=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-objcopy STRIP=~/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-elf/bin/aarch64-none-elf-strip
    ```

    * fix `gcc`-isms errors using [this link](https://sourceware.org/pipermail/glibc-cvs/2021q3/074785.html)

    * use these commands to prepare running your app:
    ```bash
    $ sudo brctl addbr kraft0
    $ sudo ip a a  172.44.0.1/24 dev kraft0
    $ sudo ip l set dev kraft0 up
    ```

    * check your setup:
    ```
    $ ip a s kraft0
    6: kraft0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UNKNOWN group default qlen 1000
        link/ether 8a:08:a1:69:85:31 brd ff:ff:ff:ff:ff:ff
        inet 172.44.0.1/24 scope global kraft0
        valid_lft forever preferred_lft forever
        inet6 fe80::8808:a1ff:fe69:8531/64 scope link 
        valid_lft forever preferred_lft forever
    ```

    * run your app:
    ```
    sudo qemu-system-aarch64 -fsdev local,id=myid,path=$(pwd)/fs0,security_model=none \
                             -device virtio-9p-pci,fsdev=myid,mount_tag=rootfs,disable-modern=on,disable-legacy=off \
                             -netdev bridge,id=en0,br=kraft0 \
                             -device virtio-net-pci,netdev=en0 \
                             -kernel "build/app-nginx_kvm-arm64" \
                             -append "netdev.ipv4_addr=172.44.0.2 netdev.ipv4_gw_addr=172.44.0.1 netdev.ipv4_subnet_mask=255.255.255.0 --" \
                             -machine virt \
                             -cpu cortex-a57 \
                             -nographic
    ```

    * test your app: open a new terminal, or use `tmux` and give this command to retrive data from the server
    ```bash
    $ wget 172.44.0.2
    --2021-08-18 16:47:38--  http://172.44.0.2/
    --2022-07-19 21:17:17--  http://172.44.0.2/
    Connecting to 172.44.0.2:80... connected.
    HTTP request sent, awaiting response... 200 OK
    Length: 180 [text/html]
    Saving to: ‘index.html’

    index.html                                         100%[===============================================================================================================>]     180  --.-KB/s    in 0s      

    2022-07-19 21:17:17 (14,7 MB/s) - ‘index.html’ saved [180/180]
    ```

    ```bash
    $ cat index.html 
    <!DOCTYPE html>
    <html>
    <head>
    <title>Hello, world!</title>
    </head>
    <body>
    <h1>Hello, world!</h1>
    <p>Powered by <a href="http://unikraft.org">Unikraft</a>.</p>
    </body>
    </html>
    ```

    * clean up your work:
    ```
    $ sudo ip l set dev kraft0 down
    $ sudo brctl delbr kraft0
    ```

-------------------------------------------------------------------------------

## Compile with `gcc-12` on `AArch64`

(this isn't completely correct, as we are using the linux toolchain, not the baremetal one, however it works)

* Add this to `unikraft/arch/arm/arm64/Makefile.uk`:

```
ARCHFLAGS-$(call gcc_version_ge, 10, 0)     += -mno-outline-atomics
```

* you might need to also modify `unikraft/plat/drivers/virtio/virtio_pci.c`
```patch
@@ -273,10 +273,12 @@ static int vpci_legacy_pci_config_get(struct virtio_dev *vdev, __u16 offset,
                VIRTIO_PCI_CONFIG_OFF + offset, buf, len,
                type_len);
    } else {
+		__u32 buf_len_bytes = len * type_len;
+
        rc = virtio_cread_bytes_many(
                (void *) (unsigned long)vpdev->pci_base_addr,
-				VIRTIO_PCI_CONFIG_OFF + offset,	buf, len);
-		if (rc != (int)len)
+				VIRTIO_PCI_CONFIG_OFF + offset,	buf, buf_len_bytes);
+		if (rc != (int)(buf_len_bytes))
            return -EFAULT;
    }
```