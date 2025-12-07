#ifndef __LL2_H
#define __LL2_H

typedef struct linked_list_head_struct* linked_list_head;
typedef struct linked_list_node_struct* linked_list_node;

typedef struct linked_list_head_struct {
    int number_of_nodes;
    struct linked_list_node_struct* first;
    struct linked_list_node_struct* last;
} linked_list_head_struct;

typedef struct linked_list_node_struct {
    char* path;
    struct linked_list_node_struct* next;
} linked_list_node_struct;

linked_list_head create_linked_list_head();
linked_list_node create_linked_list_node(char* path);
void insert_in_linked_list(linked_list_head linked_list, char* path);
void free_linked_list(linked_list_head linked_list);
void traverse_and_print(linked_list_head linked_list, int file, int dir, char* path_to_base_dir);

#endif