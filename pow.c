#include <stdio.h>
#include <asm/prctl.h>
#include <sys/prctl.h>
#include <stdlib.h>

// Compile with: clang-8 -fsanitize=shadow-call-stack pow.c -o pow

int arch_prctl(int code, unsigned long *addr);

void __attribute__ ((constructor)) __attribute__((no_sanitize("shadow-call-stack"))) setupgs()
{
    void *shadow = malloc(16384);
    arch_prctl(ARCH_SET_GS, shadow);
}

int foo() {
    return 12 * 12;
}


int main(int argc, char **argv) {
    printf("%d\n", foo());
    return 0;
}