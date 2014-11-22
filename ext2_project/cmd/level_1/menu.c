#include <cmd.h>

int menu(int argc, char* argv[])
{
    print_title("Menu", '=');
    printf("ls  cd  pwd quit\n");
    print_divider('-');

    return 0;
}
