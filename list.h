/*
 * API:
 *
 * By design, API-user has no idea what's going on with the List itself.
 * Accessing Nodes directly is impossible.
 * User only owns the handle to the list and can perform few operations based on it.
 * Item stored inside of Node is generic and its value can be accessed by casting and dereferencing it.
 *
 * All functions that push/get/pop items on front/back are FAST
 *      because pointers to front/back are stored inside of ListHandle.
 * All functions that push/get/pop items at index are SLOW as
 *      they must traverse through the list each time when used
 *      (using them inside of index-based loop is not too smart)
 *
 * You're expected to initialize the item's memory on your own after pushing it!
 *      Node will be allocated automatically but item must be initiallized.
 *      That's why every "push" function returns Item*
 * If an item contains pointer which owns any memory,
 *      you have to free said memory on your own before popping it.
 *      (It might be nice to introduce "destructor" in the future)
 *
 * There is no good way to reference the Node and to quickly perform anything
 *      based on said reference, like pushing/poping before/after the referenced Node.
 *      You can only reference the item itself by storing its adress
 *      (This is possible to fix by introducing NodeHandle/NodeView but
 *          would require a bit of refactoring and rethinking about whole API)
 */


/* Creating/Destroying List
 * LIST_NEW creates empty List and returns handle that we can use to manipulate said List.
 * list_destroy empties the list dealocating all its Items and invalidates the handle.
 * Do not use _list_new directly! Use LIST_NEW by providing struct or prymitive type. */
typedef void Item;
typedef void* ListHandle;
ListHandle _list_new(int item_size);
void list_destroy(ListHandle* target_ref); // Notice that this function takes handle pointer!
#define LIST_NEW(ITEM_TYPE) _list_new(sizeof(ITEM_TYPE));

/* Adding Items to the List
 * Notice that these functions return pointer to the Item
 *  and user is expected to initialize the Item on its own
 *  by casting and dereferencing it. */
Item* list_push_front(ListHandle target);
Item* list_push_back(ListHandle target);
Item* list_push_at(ListHandle target, int index);

/* Accessing Items from the List
 * Notice that these functions return Item*
 *      and user is expected to cast and dereference said pointer on its own. */
Item* list_get_front(ListHandle target);
Item* list_get_back(ListHandle target);
Item* list_get_at(ListHandle target, int index);

/* Removing Items from the List */
void list_pop_front(ListHandle target);
void list_pop_back(ListHandle target);
void list_pop_at(ListHandle target, int index);
void list_pop_all(ListHandle target);

/* Iterating over the List
 * list_foreach begins at the "front" and ends at the "back".
 * You can break from the loop by returning any value different than 0
 *      said value will be also returned by loop, if it breaks.
 * Pushing and popping while traversing through list is dangerous! */
int list_foreach(ListHandle target, int(*function)(Item* item, void* data), void* inout_data);
int list_foreach_reversed(ListHandle target, int(*function)(Item* item, void* data), void* inout_data);

/* Obtaining information about the List
 * All these operations are almost instant. */
int list_is_valid(ListHandle target);
int list_is_empty(ListHandle target);
int list_get_num(ListHandle target);
int list_get_item_size(ListHandle target);
int list_get_node_size(ListHandle target);
int list_get_current_size(ListHandle target);


