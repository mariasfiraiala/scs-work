#include <stdio.h>
#include <asm/prctl.h>
#include <sys/prctl.h>
#include <stdlib.h>

// Compile with: clang-8 -fsanitize=shadow-call-stack test.c -o test

int arch_prctl(int code, unsigned long *addr);

void __attribute__ ((constructor)) __attribute__((no_sanitize("shadow-call-stack"))) setupgs()
{
    void *shadow = malloc(16384);
    arch_prctl(ARCH_SET_GS, shadow);
}

int bar() {
    return 42;
}

int foo() {
    return bar() + 1;
}


int main(int argc, char **argv) {
    printf("Hello, world %d!\n", foo());
    return 0;
}