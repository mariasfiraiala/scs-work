# Shadow Stack for Unikraft complex applications

I am testing 3 complex applications `SQLite`, `redis` and `nginx` in order to bring Shadow Stack support.

| App\Compiler | gcc - x86 | gcc - aarch64 | clang - x86 | clang - aarch64 | clang with scs | gcc-12 with scs |
|--------------|-----------|---------------|-------------|-----------------|----------------|-----------------|
| SQLite | :heavy_check_mark: | :soon: | :soon: | :soon: | :soon: | :soon: |
| redis | :heavy_check_mark: | :soon: | :soon: | :soon: | :soon: | :soon: |
| nginx | :heavy_check_mark: | :soon: | :soon: | :soon: | :soon: | :soon: |

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

`nginx` with 
1. `gcc` on `x86`
    
    * clone your dependencies:

        1. [unikraft](https://github.com/unikraft/unikraft)
        1. [nginx](https://github.com/unikraft/app-nginx)
        1. [lib-nginx](https://github.com/unikraft/lib-nginx), [lib-pthread-embedded](https://github.com/unikraft/lib-pthread-embedded), [lib-newlib](https://github.com/unikraft/lib-newlib), [lib-lwip](https://github.com/unikraft/lib-lwip)

    * the file hierarchy should look somethig like this:
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

    * create a `Makefile.uk` in the `app-sqlite` directory:
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
    Saving to: ???index.html???

    index.html                                         100%[===============================================================================================================>]     180  --.-KB/s    in 0s      

    2022-07-19 21:17:17 (14,7 MB/s) - ???index.html??? saved [180/180]
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
