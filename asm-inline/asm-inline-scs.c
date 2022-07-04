#include <stdio.h>
#include <stdlib.h>

// Compile with: clang-8 -fsanitize=shadow-call-stack asm-inline-scs.c -o asm-inline-scs

void __attribute__ ((constructor)) __attribute__((no_sanitize("shadow-call-stack"))) setupgs()
{
    void *shadow = malloc(16384);
    
    asm volatile("mov %0, %%gs;" :: "r" (shadow));
}

int bar() {
    return 42;
}

int foo() {
    return bar() + 1;
}


int main(void)
{
    printf("Hello, world %d!\n", foo());
    return 0;
}