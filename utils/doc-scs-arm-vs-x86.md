# What makes `AArch64` so great and `x86` so bad for scs?

Put on your seatbelts, ladies and gents, because we're going for a ride.

On today's list of what other documentation Maria has to do we have the eternal battle between `ARM` and `x86` architectural decisions.

## Initial information

Now, let me gather my technical knowledge and resources:

> Like the previous implementation of ShadowCallStack on x86_64, it is inherently racy due to the architecture’s use of the stack for calls and returns.[1]

And that's how I've started my research.

Looks like nobody quite knows why `clang` decided to remove[2] the ShadowCallStack for `x86`.

Official sources[1] talk about some Chromium tests performed using the ShadowCallStack which were so disastrous they remained hiding in the *shadows* (I wish this was just a pun); all in all, they demonstrated that a `x86` implementation for the Shadow Stack wasn't worth it[3].

What's more, some critics pointed out the abnormality of the syscalls used to provide a runtime for Shadow Stacks:
> It is always dangerous to ship code using kernel features that have not yet been accepted and merged; shadow stacks turn out to be an example of why. According to the cover letter on the current series, the arch_prctl() API was ""abandoned for being strange"". But those shadow-stack-ready binaries deployed on systems worldwide were built expecting that API, strange or not, to be present; if the kernel respects the markings in the ELF file and enables shadow stacks for those programs, some of them will break. That would cause system administrators worldwide to disable shadow stacks until at least 2040, rather defeating the purpose of the whole exercise.[4]

Other articles didn't hesitate to bash both Shadow Stacks and CET, but didn't quite provide a well documented reason; one must take them with a grain of salt as they are direct competitors[5] to technology provided by `intel`:

> One can't help but notice a striking similarity with Intel's MPX, another software-dependent technology announced with great fanfare a few years ago that failed to live up to its many promises and never reached its intended adoption as the solution to end buffer overflow attacks and exists only as yet another bounds-checking based debugging technology.[6]

## In depth analysis

The really nice details are discussed in this paper[7]. It is evident that the major problem present in the `x86` ShadowCallStack approach was due to internal designs, to be more precise, the TOCTOU (time-of-check-to-time-of-use) vulnerability. 

This isn't the case for the `AArch64` implementation: the address of the called function is stored in a register, as opposed to `x86`, in which case it is placed on the stack.

>On `x86`, instrumenting the function prologues results in a one-instruction wide Time Of Check To Time Of Use (TOCTTOU) opportunity due to architectural limitations. The `call` instruction pushes the return address to the stack where it may be modified by an attacker before it is picked up by the prologue in the called function. Architectures, such as ARM, where the address of the called function is stored in a register, do not have this limitation.[7]

The paper goes on, bragging about their proposed implementation, however what's of interest to us are the findings of the Microsoft team:

> While the TOCTTOU window exists, given the extremely precise timing required, we do not believe this potential weakness to be readily exploitable. Any such attack would rely on accurately timing the victim process, and manipulating the OS scheduler to pause the victim’s execution precisely between the call and mov instruction. After the `call` instruction pushes the return address onto the stack, it remains in the cache and the `mov` instruction can immediately use the value, resulting in a minimal window of only a few cycles. **Microsoft researchers proposed and redacted Return Flow Guard (RFG) as it was vulnerable to TOCTTOU windows in the prologue and epilogue. The Microsoft red team discovered a viable attack against their proposed epilogue, targeting epilogues of leaf functions.**[7]

In the end, one has to question oneself: isn't it better for the mistery of `x86` *Shadow* Stacks to remain in the realm of *shadows*?

[1]: https://clang.llvm.org/docs/ShadowCallStack.html
[2]: https://github.com/llvm-mirror/llvm/commit/863ea8c618b1f88ba8c9ec355a07cb3783481642#diff-03af556abbf2bdd9222fceba351d42fc01255178192b3f1780a6952525c2df83
[3]: https://www.phoronix.com/scan.php?page=news_item&px=LLVM-Drops-ShadowCallStack-x64
[4]: https://lwn.net/Articles/885220/
[5]: https://stackoverflow.com/questions/49420204/shadow-stack-implementation-on-x86-64
[6]: https://grsecurity.net/effectiveness_of_intel_cet_against_code_reuse_attacks
[7]: https://hexhive.epfl.ch/publications/files/19Oakland.pdf