
+++
title = "Shadow Stack: Roadmap for the last 6 weeks of GSoC'22"
date = "2022-08-02T16:00:00+01:00"
author = "Maria Sfiraiala"
tags = ["GSoC'22"]
+++

<img width="100px" src="https://summerofcode.withgoogle.com/assets/media/gsoc-2022-badge.svg" align="right" />

# Shadow Stack: Roadmap for the last 6 weeks of GSoC'22

This blog provides an aproximative plan for the final 6 weeks of GSoC, together with a series of milestones.

## Week 7: 1 - 7 Aug

* Test `SQLite`, `redis`, `nginx` on `AArch64` using `gcc`.

* Test `SQLite`, `redis`, `nginx` on `AArch64` using `clang`.

* Create issues and documentation for each step.

## Week 8: 8 - 14 Aug

* Use Shadow Stack for `SQLite`, `redis`, `nginx` with both `gcc-12` and `clang`.

* Optimize allocation for Shadow Stack, insert the constructor for the Shadow Stack into the Unikraft constructor table.

* Take feedback from Marc, Michalis and Simon regarding how to better allocate the Shadow Stack.

* Provide a clear proposal regarding other security mechanisms (such as `CET` or `Safe Stack`) and their impact on Shadow Stack.

## Week 9: 15 - 21 Aug

* Prepare an internal library (`ukshadow`) that would incorporate all code related to Shadow Stack mechanisms.

* Prepare configuration parameters for Shadow Stack which will be used for the `make build system`.

* Start optimizing, take as reference previous work done for [Android](https://android-review.googlesource.com/c/kernel/common/+/694163).

## Week 10: 22 - 28 Aug

* Test overhead: use `uktest`(optional).

* Finalize optimization work.

* Take advice from Vlad.

* Prepare upstreaming the product.

## Week 11: 29 Aug - 4 Sept

* Test overhead and behaviour on complex apps: `SQLite`, `redis`, `nginx`.

* Provide the final PR.

## Week 12: 5 - 11 Sept

* Feedback week: make everythig pretty.

* Integrate (if needed) all feedback on the provided PR into the final version of the project.

## Extra work
(must be done in order to make sure Shadow Stack support is widely accessible on Unikraft)

* Create issues and PRs for every encountered error:

1. `newlib` compilation issue for `AArch64`: modify `newlib/include/limits.h`
1. `newlib` issue for accepting requests on `redis`: modify `newlib/include/fcntl.h`
1. `virtio` issue: modify `unikraft/plat/drivers/virtio/virtio_pci.c`
1. `TLS` issue: modify `unikraft/lib/posix-user/user.c`

* Test other complex apps on `AArch64`: `python3`, `lua`.