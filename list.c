
#include "list.h"
#include <stdlib.h> /* malloc free */
#include <string.h> /* memcpy */

#define LIST_DEBUG

#ifdef LIST_DEBUG
#include <assert.h>
#define _ASSERT(COND) assert(COND)
#else
#define _ASSERT(COND)
#endif

/*
 * Because Item is generic, Node's memory size can vary
 *      and we cannot just simply declare it as struct.
 *
 * We store a size_t variable called "both" at the begining of Node's memory block,
 *      it is a XOR'ed value of the adresses to the next and previous Nodes.
 * After that, there is generic "item" variable, a data ment to be maniluplated by API-user.
 *
 * We can access "both" by casting Node to size_t and dereferencing it.
 * We can access "item" by offseting the Node's adress with sizeof(size_t)
 * There are multiple functions staring with _node... that abstract this!
 *
 * both     item
 * bbbbbbbb|iiiiiiiiiiiiiii.....
 * ^size_t  ^void
 * ^_Node   ^_Node+sizeof(size_t)
 */
typedef void _Node;

/* Same as ListHandle
 * We hide it from API-user! */
typedef struct {
    _Node* front;
    _Node* back;
    int ITEM_SIZE;
    int num;
} _ListData;

/* Helpers for manipulating Node's memory */
_Node* _node_malloc(_ListData* template);
_Node* _node_get_both(_Node* target);
_Node* _node_get_next(_Node* target, _Node* prev);
Item* _node_get_item(_Node* target);
void _node_set_both(_Node* target, _Node* value);

/* Helpers for manipulating the List */
_ListData* _list_get_data(ListHandle target);
ListHandle _list_get_handle(_ListData* target);
void _list_get_node_at(_ListData* target, int index_natural, _Node** out_prev_ref, _Node** out_it_ref);
Item* _list_push_edge(_ListData* target, _Node** front_or_back_ref);
void _list_pop_edge(_ListData* target, _Node** front_or_back_ref);
int _list_foreach_internal(_ListData* target, int(*function)(Item* item, void* inout_data), void* data, _Node* start);

void* _xor_pointers(void* a, void* b);


ListHandle _list_new(int item_size)
{
    _ASSERT(item_size > 0);
    _ListData* list = (_ListData*)malloc(sizeof(_ListData));
    _ASSERT(list);
    list->ITEM_SIZE = item_size;
    list->num = 0;
    list->front = NULL;
    list->back = NULL;
    return (ListHandle)list;
}


void list_destroy(ListHandle* target_ref)
{
    _ASSERT(target_ref && *target_ref);
    list_pop_all(*target_ref);
    free(*target_ref);
    *target_ref = NULL;
}


Item* list_push_front(ListHandle target)
{
    _ListData* target_data = _list_get_data(target);
    _Node** front_ref = &target_data->front;
    return _list_push_edge(target_data, front_ref);
}


Item* list_push_back(ListHandle target)
{
    _ListData* target_data = _list_get_data(target);
    _Node** back_ref = &target_data->back;
    return _list_push_edge(target_data, back_ref);
}


Item* list_push_at(ListHandle target, int index)
{
    _ASSERT(list_is_valid(target));
    _ASSERT(index <= list_get_num(target));
    _ASSERT(index >= -list_get_num(target)-1);
    const int num = list_get_num(target);
    const int index_natural = (index >= 0) ? (index) : (index + num);

    if(index_natural == 0)
        return list_push_front(target);
    if(index_natural == num)
        return list_push_back(target);

    _ListData* target_data = _list_get_data(target);
    _Node* it;
    _Node* prev;
    _list_get_node_at(target_data, index_natural, &prev, &it);
    _Node* new_node = _node_malloc(target_data);
    _node_set_both(prev, _xor_pointers(new_node, _node_get_next(prev, it)));
    _node_set_both(it, _xor_pointers(new_node, _node_get_next(it, prev)));
    _node_set_both(new_node, _xor_pointers(prev, it));
    ++(target_data->num);
    return _node_get_item(new_node);
}


Item* list_get_front(ListHandle target)
{
    _ASSERT(!list_is_empty(target));
    _ListData* data = _list_get_data(target);
    return _node_get_item(data->front);
}


Item* list_get_back(ListHandle target)
{
    _ASSERT(!list_is_empty(target));
    _ListData* data = _list_get_data(target);
    return _node_get_item(data->back);
}


Item* list_get_at(ListHandle target, int index)
{
    _ASSERT(list_is_valid(target));
    _ASSERT(index < list_get_num(target));
    _ASSERT(index >= -list_get_num(target));
    const int num = list_get_num(target);
    const int index_natural = (index >= 0) ? (index) : (index + num);

    if(index_natural == 0)
        return list_get_front(target);
    if(index_natural == num-1)
        return list_get_back(target);

    _ListData* target_data = _list_get_data(target);
    _Node* prev;
    _Node* it;
    _list_get_node_at(target_data, index_natural, &prev, &it);
    return _node_get_item(it);
}


void list_pop_front(ListHandle target)
{
    _ListData* target_data = _list_get_data(target);
    _Node** front_ref = &target_data->front;
    _list_pop_edge(target_data, front_ref);
}


void list_pop_back(ListHandle target)
{
    _ListData* target_data = _list_get_data(target);
    _Node** back_ref = &target_data->back;
    _list_pop_edge(target_data, back_ref);
}


void list_pop_at(ListHandle target, int index)
{
    _ASSERT(list_is_valid(target));
    _ASSERT(index < list_get_num(target));
    _ASSERT(index >= -list_get_num(target));
    const int num = list_get_num(target);
    const int index_natural = (index >= 0) ? (index) : (index + num);

    if(index_natural == 0)
        return list_pop_front(target);
    if(index_natural == num-1)
        return list_pop_back(target);

    _ListData* target_data = _list_get_data(target);
    _Node* it;
    _Node* prev;
    _list_get_node_at(target_data, index_natural, &prev, &it);
    _Node* next = _node_get_next(it, prev);
    _node_set_both(prev, _xor_pointers(next, _node_get_next(prev, it)));
    _node_set_both(next, _xor_pointers(prev, _node_get_next(next, it)));
    free(it);
    --(target_data->num);
}


void list_pop_all(ListHandle target)
{
    _ASSERT(list_is_valid(target));
    _ListData* target_data = _list_get_data(target);
    _Node* it = target_data->front;
    _Node* prev = NULL;
    while(it != NULL)
    {
        _Node* next = _node_get_next(it, prev);
        prev = it;
        it = next;
        free(prev);
    }
    target_data->num = 0;
}


int list_foreach(ListHandle target, int(*function)(Item* item, void* data), void* inout_data)
{
    _ListData* list_data = _list_get_data(target);
    return _list_foreach_internal(list_data, function, inout_data, list_data->front);
}


int list_foreach_reversed(ListHandle target, int(*function)(Item* item, void* data), void* inout_data)
{
    _ListData* list_data = _list_get_data(target);
    return _list_foreach_internal(list_data, function, inout_data, list_data->back);
}


int list_is_valid(ListHandle target)
{
    return target != NULL;
}


int list_is_empty(ListHandle target)
{
    _ASSERT(list_is_valid(target));
    return list_get_num(target) == 0;
}


int list_get_num(ListHandle target)
{
    _ASSERT(list_is_valid(target));
    const int num = _list_get_data(target)->num;
    _ASSERT(num >= 0);
    return num;
}


int list_get_item_size(ListHandle target)
{
    _ASSERT(list_is_valid(target));
    return _list_get_data(target)->ITEM_SIZE;
}


int list_get_node_size(ListHandle target)
{
    _ASSERT(list_is_valid(target));
    return sizeof(size_t) + list_get_item_size(target);
}


int list_get_current_size(ListHandle target)
{
    _ASSERT(list_is_valid(target));
    const _ListData* data = _list_get_data(target);
    return sizeof(_ListData) + (data->num * list_get_node_size(target));
}


_Node* _node_malloc(_ListData* template)
{
    _ASSERT(template);
    const size_t node_size = list_get_node_size(template);
    return (_Node*)malloc(node_size);
}


_Node* _node_get_both(_Node* target)
{
    size_t both = *(size_t*)target;
    return (_Node*)both;
}


_Node* _node_get_next(_Node* target, _Node* prev)
{
    _ASSERT(target);
    _Node* both = _node_get_both(target) ;
    _Node* next = _xor_pointers(prev, both);
    return next;
}


Item* _node_get_item(_Node* target)
{
    _ASSERT(target);
    const size_t item_adress = (size_t)target + sizeof(size_t);
    return (void*)item_adress;
}


void _node_set_both(_Node* target, _Node* adress)
{
    _ASSERT(target);
    // HACK: "both" is located at the begining of Node's memory block, so the adress is the same
    _Node* both = target;
    size_t adress_value = (size_t)adress;
    memcpy(both, &adress_value, sizeof(size_t));
}


_ListData* _list_get_data(ListHandle target)
{
    return (_ListData*)target;
}


ListHandle _list_get_handle(_ListData* target)
{
    return (_ListData*)target;
}


void _list_get_node_at(_ListData* target, int index_natural, _Node** out_prev_ref, _Node** out_it_ref)
{
    _ASSERT(target);
    _ASSERT(out_prev_ref);
    _ASSERT(out_it_ref);
    const int num = target->num;
    const int b_start_at_front = 0 > (index_natural - num/2);
    _Node* it = b_start_at_front ? target->front : target->back;
    _Node* prev = NULL;
    const int end = (b_start_at_front) ? (index_natural) : (num - index_natural - 1);
    for(int i = 0; i<end ;i++)
    {
        _Node* next = _node_get_next(it, prev);
        prev = it;
        it = next;
    }
    *out_prev_ref = prev;
    *out_it_ref = it;
}


Item* _list_push_edge(_ListData* target, _Node** front_or_back_ref)
{
    _ASSERT(target);
    _ASSERT(front_or_back_ref == &target->front || front_or_back_ref == &target->back);
    Item* out_item = NULL;
    if(list_is_empty(target))
    {
        _Node* first = _node_malloc(target);
        _ASSERT(first);
        _node_set_both(first, NULL);
        target->front = first;
        target->back = first;
        out_item = _node_get_item(first);
    }
    else
    {
        _Node* new_edge = _node_malloc(target);
        _ASSERT(new_edge);
        _Node* old_edge = *front_or_back_ref;
        _Node* old_edge_next = _node_get_next(old_edge, NULL);
        _node_set_both(new_edge, old_edge);
        _Node* old_edge_new_both = _xor_pointers(new_edge, old_edge_next);
        _node_set_both(old_edge, old_edge_new_both);
        (front_or_back_ref == &target->front) ? (target->front = new_edge) : (target->back = new_edge);
        out_item = _node_get_item(new_edge);
    }
    ++(target->num);
    _ASSERT(out_item);
    return out_item;
}


void _list_pop_edge(_ListData* target, _Node** front_or_back_ref)
{
    _ASSERT(list_is_valid(target));
    _ASSERT(!list_is_empty(target));
    _ASSERT(front_or_back_ref);
    _ASSERT(&target->front == front_or_back_ref || &target->back == front_or_back_ref);
    const int target_num = list_get_num(target);
    if(target_num == 1)
    {
        free(target->front);
        target->back = NULL;
        target->front = NULL;
    }
    else
    {
        _Node* old_edge = *front_or_back_ref;
        _Node* new_edge = _node_get_next(old_edge, NULL);
        _Node* new_edge_next = _node_get_next(new_edge, old_edge);
        _Node* new_edge_both = _xor_pointers(NULL, new_edge_next);
        _node_set_both(new_edge, new_edge_both);
        (front_or_back_ref == &target->front) ? (target->front = new_edge) : (target->back = new_edge);
        free(old_edge);
    }
    --(target->num);
}


int _list_foreach_internal(_ListData* target, int(*function)(Item* item, void* data), void* inout_data, _Node* start)
{
    _ASSERT(list_is_valid(target));
    _ASSERT(function);
    _ASSERT(target->front == start || target->back == start);
    _Node* it = start;
    _Node* prev = NULL;
    while(it != NULL)
    {
        Item* item = _node_get_item(it);
        _Node* next = _node_get_next(it, prev);
        prev = it;
        it = next;
        const int break_result = function(item, inout_data);
        if(break_result)
            return break_result;
    }
    return 0;
}


void* _xor_pointers(void* a, void* b)
{
    const size_t xor_adress_value = (size_t)a ^ (size_t)b;
    return (void*)xor_adress_value;
}
