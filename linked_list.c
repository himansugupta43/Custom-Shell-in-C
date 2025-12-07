#include "headers.h"

LL_Head create_LL() {
    LL_Head LL = (LL_Head) malloc(sizeof(LL_Head_struct));
    LL->no_of_nodes = 0;
    LL->first = NULL;
    LL->last = NULL;
    return LL;
}

LL_Node create_node(int pid, int flag, char** argument_tokens) {
    LL_Node N = (LL_Node) malloc(sizeof(LL_Node_struct));

    char* command = (char*) calloc(MAX_LEN, sizeof(char));
    int idx = 0;
    while (argument_tokens[idx] != NULL) {
        strcat(command, " ");
        strcat(command, argument_tokens[idx]);
        idx++;
    }
    remove_leading_and_trailing_spaces(command);
    N->cmd = command;
    N->pid = pid;
    N->flag = flag;
    N->next = NULL;
    return N;
}

void insert_in_LL(int pid, int flag, char** argument_tokens) {
    LL_Node N = create_node(pid, flag, argument_tokens);
    if (LL->no_of_nodes == 0) {
        LL->first = N;
        LL->last = N;
        LL->no_of_nodes++;
    } else if (LL->no_of_nodes == 1) {
        LL->last = N;
        LL->first->next = N;
        LL->no_of_nodes++;
    } else {
        LL->last->next = N;
        LL->last = N;
        LL->no_of_nodes++;
    }
}

void free_node(LL_Node node) {
    int pid_to_free = node->pid;
    LL_Node trav = LL->first;
    LL_Node prev = NULL;
    while (trav != NULL) {
        if (trav->pid == pid_to_free) {
            if (prev != NULL) {
                prev->next = trav->next;
                free(trav->cmd);
                free(trav);
                LL->no_of_nodes--;
                if (LL->no_of_nodes == 0) {
                    LL->first = NULL;
                    LL->last = NULL;
                }
                break;
            } else {
                LL->first = trav->next;
                free(trav->cmd);
                free(trav);
                LL->no_of_nodes--;
                if (LL->no_of_nodes == 0) {
                    LL->first = NULL;
                    LL->last = NULL;
                }
                break;
            }
        }
        prev = trav;
        trav = trav->next;
    }
}

void check_and_print() {
    LL_Node trav = LL->first;
    while (trav != NULL) {
        int cstatus;
        if (waitpid(trav->pid, &cstatus, WNOHANG) == trav->pid) {
            if (WIFEXITED(cstatus)) {
                printf("Process exited normally (%d)\n", trav->pid);
            } else {
                printf("Process exited abnormally (%d)\n", trav->pid);
            }
            LL_Node temp = trav;
            trav = trav->next;
            free_node(temp);
        } else {
            trav = trav->next;
        }
    }
}

void insert_node_in_LL(int pid, int flag, char* command) {
    LL_Node N = (LL_Node) malloc(sizeof(LL_Node_struct));
    N->cmd = command;
    N->flag = flag;
    N->pid = pid;
    N->next = NULL;

    if (LL->first == NULL) {
        LL->first = N;
        LL->last = N;
        LL->no_of_nodes++;
        return;
    } else {
        LL_Node trav = LL->first;
        LL_Node prev = NULL;
        while (trav != NULL) {
            prev = trav;
            trav = trav->next;
        }
        prev->next = N;
        LL->no_of_nodes++;
        LL->last = N;
        return;
    }
}

