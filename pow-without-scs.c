#include <stdio.h>
#include <stdlib.h>

int foo() {
    return 12 * 12;
}

int main(int argc, char **argv) {
    printf("%d\n", foo());
    return 0;
}