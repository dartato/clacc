#include <string.h>
#include <stdio.h>
int main() {
    int i = 0x00001234;
    printf("%02x %02x %02x\n", *(&i), *((char*)&i+1), *((char*)&i+0));
    return 0;
}

