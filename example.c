
#include "list.h"
#include <stdio.h> /* NULL prinf */


int print_my_item(Item* my_item, void* inout_data)
{
	(void)inout_data;
    printf("%c", *(char*)my_item);
    return 0;
}


int main(void)
{
    ListHandle my_list = LIST_NEW(char);

    printf("\nFilling list with characters from 'a' to 'z'\n");
    for(char i = 'a'; i <= 'z'; ++i)
        *(char*)list_push_back(my_list) = i;
    list_foreach(my_list, print_my_item, NULL);

    printf("\n\nis valid: %d\n", list_is_valid(my_list));
    printf("is empty: %d\n", list_is_empty(my_list));
    printf("num: %d\n", list_get_num(my_list));
    printf("item size: %d byte(s)\n", list_get_item_size(my_list));
    printf("node size: %d bytes\n", list_get_node_size(my_list));
    printf("current size: %d bytes\n", list_get_current_size(my_list));
    list_foreach_reversed(my_list, print_my_item, NULL);

    printf("\n\npopping 3 items from front and 3 from back\n");
    for(int i = 0; i < 3; ++i)
    {
        list_pop_front(my_list);
        list_pop_back(my_list);
    }
    list_foreach(my_list, print_my_item, NULL);

    printf("\n\nPushing '0' into indexes 2 and -3\n");
    *(char*)list_push_at(my_list, 2) = '0';
    *(char*)list_push_at(my_list, -3) = '0';
    list_foreach(my_list, print_my_item, NULL);

    printf("\n\nPrinting values from indexes 1 and -2\n");
    printf("%c", *(char*)list_get_at(my_list, 1));
    printf("%c", *(char*)list_get_at(my_list, -2));

    printf("\n\nPopping from indexes 3 and -4\n");
    list_pop_at(my_list, 3);
    list_pop_at(my_list, -4);
    list_foreach(my_list, print_my_item, NULL);

    printf("\n\ndestroying list...\n");
    list_destroy(&my_list);
    // from now on my_list is hollow and passing it to anything else than list_is_valid will cause a crash
    printf("is valid: %d\n\n", list_is_valid(my_list));

    return 0;
}
