#include <Windows.h>
#include <stdio.h>

#ifndef SPRING_CONSOLELESS
int main(int argc, char* argv[])
#else
int WINAPI WinMain(HWIND instance, HWIND previnstance, LPSTR cmdline, int cmdshow)
#endif
{
    printf("coucou\n%i\n", argc);
    for (short i = 0; i < argc; i++)
    {
        printf(argv[i]);
        printf("\n");
    }
    return true;
}