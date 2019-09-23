#include "list.h"
#include "defn_impl.h"

enum ErrorCode ListCreate(List *list, DataType type) {
    if (NULL == list) {
        return EC_FAIL;
    }
    list->first = list->last = NULL;
    list->size = 0;
    list->type = type;
    return EC_SUCCESS;
}

enum ErrorCode ListDestroy(List *list) {
    ListIterator del;
    if (NULL == list) {
        return EC_FAIL;
    }

    del = list->first;
    while (NULL != del)
        ListDelete(list, &del);

    return EC_SUCCESS;
}

enum ErrorCode ListDestroyNodes(List *list) {
    ListIterator del;
    if (NULL == list) {
        return EC_FAIL;
    }

    if (0 < list->size) {
        while (1) {
            if (NULL == (del = list->first))
                break;
            list->first = list->first->next;
            free(del);
        }
    }
    list->first = list->last = NULL;
    list->size = 0;

    return EC_SUCCESS;
}

enum ErrorCode ListInsert(List *list, void *newData) {
    if (NULL == list || NULL == newData) {
        return EC_FAIL;
    }
    if (0 == list->size) {
        if (NULL == (list->first = list->last = malloc(sizeof (ListNode)))) {
            return EC_FAIL;
        }
        list->first->next = list->first->previous = NULL;
        list->first->data = newData;
    } else {
        if (NULL == (list->last->next = malloc(sizeof (ListNode)))) {
            return EC_FAIL;
        }
        list->last->next->previous = list->last;
        list->last->next->next = NULL;
        list->last = list->last->next;
        list->last->data = newData;
    }
    (list->size)++;
    return EC_SUCCESS;
}

enum ErrorCode ListDelete(List *list, ListIterator *node) {
    ListIterator del = *node;
    if (NULL == list || NULL == node) {
        return EC_FAIL;
    }
    if (NULL == *node) {
        return EC_FAIL;
    }
    if (1 == list->size) {
        list->first = list->last = NULL;
    } else if (list->first == *node) {
        list->first = list->first->next;
        list->first->previous = NULL;
    } else if (list->last == *node) {
        list->last = list->last->previous;
        list->last->next = NULL;
    } else {
        (*node)->next->previous = (*node)->previous;
        (*node)->previous->next = (*node)->next;
    }
    *node = (*node)->next;
    if (WORD == list->type) {
        free(((Word *) del->data)->word);
    } else if (ENTRY == list->type) {
        free(((Entry *) del->data)->word->word);
        free(((Entry *) del->data)->payload);
    } else if (QUERY == list->type) {
        ListDestroyNodes(&(((Query *) del->data)->words));
    } else if (DOCUMENT == list->type) {
        ListDestroyNodes(&(((Document *) del->data)->words));
    }
    free(del->data);
    free(del);
    (list->size)--;

    return EC_SUCCESS;
}

enum ErrorCode ListAddNode(List *list, ListIterator node) {
    if (NULL == list || NULL == node) {
        return EC_FAIL;
    }
    if (0 == list->size) {
        list->first = list->last = node;
        node->next = node->previous = NULL;
    } else {
        node->next = list->first;
        node->previous = NULL;
        list->first->previous = node;
        list->first = node;
    }
    (list->size)++;
    return EC_SUCCESS;
}

enum ErrorCode ListRemoveNode(List *list, ListIterator node) {
    if (NULL == list || NULL == node) {
        return EC_FAIL;
    }
    if (1 == list->size) {
        list->first = list->last = NULL;
    } else if (list->first == node) {
        list->first = list->first->next;
        list->first->previous = NULL;
    } else if (list->last == node) {
        list->last = list->last->previous;
        list->last->next = NULL;
    } else {
        node->next->previous = node->previous;
        node->previous->next = node->next;
    }
    node->next = node->previous = NULL;
    (list->size)--;
    return EC_SUCCESS;
}

enum ErrorCode ListGetNthNode(List *list, unsigned int position, void **result) {
    ListIterator i;
    if (NULL == list || NULL == result)
        return EC_FAIL;
    for (i = list->first; NULL != i && 0 < position; position--, i = i->next);
    if (NULL == i) {
        *result = NULL;
    } else {
        *result = i->data;
    }
    return EC_SUCCESS;
}

enum ErrorCode ListEqual(void *data1, void *data2, DataType type, int *equal) {
    if (NULL == data1 || NULL == data2) {
        return EC_FAIL;
    }
    if (WORD == type) {
        *equal = !Equal(data1, data2);
    } else if (ENTRY == type) {
        *equal = !Equal(((Entry*) data1)->word, ((Entry*) data2)->word);
    } else if (INTEGER == type) {
        *equal = ((*((int*) data1) == *((int*) data2)) ? 1 : 0);
    } else if (QUERY == type) {
        *equal = ((Query *) data1)->id == ((Query *) data2)->id ? 1 : 0;
    } else if (DOCUMENT == type) {
        *equal = ((Document *) data1)->id == ((Document *) data2)->id ? 1 : 0;
    }
    return EC_SUCCESS;
}

enum ErrorCode ListMember(List *list, void *data, int *result) {
    ListIterator it;
    int equal = 0;
    if (NULL == list || NULL == data) {
        return EC_FAIL;
    }
    *result = 0;

    for (it = list->first; NULL != it && 1 != equal; it = it->next) {
        if (EC_FAIL == ListEqual(it->data, data, list->type, &equal))
            return EC_FAIL;
    }
    *result = equal;
    return EC_SUCCESS;
}

enum ErrorCode ListSubset(List *list, unsigned int num_of_nodes, List *subset) {
    int i, random;
    ListIterator node;

    if (NULL == list || NULL == subset) {
        return EC_FAIL;
    }
    if (num_of_nodes > list->size || subset->size) {
        return EC_FAIL;
    }

    for (i = 0; num_of_nodes > i; ++i) {
        random = rand() % list->size;

        for (node = list->first; 0 < random; node = node->next, random--);

        if (EC_FAIL == ListRemoveNode(list, node)) {
            return EC_FAIL;
        }
        if (EC_FAIL == ListAddNode(subset, node)) {
            return EC_FAIL;
        }
    }

    return EC_SUCCESS;
}

enum ErrorCode ListRemoveDublicates(List *list) {
    Hash *HashTable;
    ListIterator i;
    unsigned int h, mod;
    int exists1, exists2;

    if (NULL == list)
        return EC_FAIL;
    if (WORD != list->type && ENTRY != list->type)
        return EC_FAIL;
    if (0 == (mod = list->size))
        return EC_FAIL;

    HashTableCreate(mod, list->type, &HashTable);

    for (i = list->first; NULL != i;) {
        h = HashFunction(i->data, list->type, mod);
        if (NULL == HashTable[h].data) {
            HashTable[h].data = i->data;
            i = i->next;
        } else {
            if (WORD == HashTable[h].type) {
                exists1 = Equal(HashTable[h].data, i->data);
                ListMember(&(HashTable[h].OverflowList), (Word *) i->data, &exists2);
            } else {
                exists1 = Equal(((Entry *) HashTable[h].data)->word,
                        ((Entry *) i->data)->word);
                ListMember(&(HashTable[h].OverflowList), ((Entry *) i->data)->word, &exists2);
            }

            if (1 == exists1 && 0 == exists2) {//doesn't match
                ListInsert(&(HashTable[h].OverflowList), i->data);
                i = i->next;
            } else {
                ListDelete(list, &i);
            }
        }
    }
    HashTableDestroy(HashTable, mod);
    return EC_SUCCESS;
}

enum ErrorCode HashTableCreate(unsigned int size, DataType type, struct _Hash_ **HashTable) {
    int i;

    if (0 == size) {
        return EC_FAIL;
    }

    if (NULL == (*HashTable = malloc(sizeof (Hash) * size))) {
        return EC_FAIL;
    }

    for (i = 0; size > i; ++i) {
        (*HashTable)[i].data = NULL;
        ListCreate(&((*HashTable)[i].OverflowList), type);
        (*HashTable)[i].type = type;
    }

    return EC_SUCCESS;
}

enum ErrorCode HashTableDestroy(Hash *HashTable, unsigned int size) {
    int i;
    if (NULL == HashTable || 0 == size) {
        return EC_FAIL;
    }

    for (i = size - 1; 0 <= i; --i) {
        ListDestroyNodes(&(HashTable[i].OverflowList));
        //free(HashTable[i].data);
    }
    free(HashTable);

    return EC_SUCCESS;
}

unsigned int HashFunction(void *data, DataType type, unsigned int mod) {
    //a simple hash function. Will be updated
    unsigned int hash = 5381;
    int i;
    Word *word;

    if (NULL == data || 0 == mod) {
        printf("In function GetHash (defn_impl.c): NULL argument");
        exit(EXIT_FAILURE);
    }

    if (WORD == type) {
        word = data;
    } else if (ENTRY == type) {
        word = ((Entry *) data)->word;
    } else if (INTEGER == type) {
        return *((int *) data) % mod;
    }
    for (i = 0; i < word->length; ++i)
        hash = ((hash << 5) + hash) + word->word[i]; /* hash * 33 + c */

    return hash % mod;
}

void ListPrint(List *list) {
    ListIterator i;
    printf("{\n");
    for (i = list->first; NULL != i; i = i->next) {
        //printf("%s\n", ((Word *)i->data)->word);
        printf("%s\n", ((Entry *) i->data)->word->word);
    }
    printf("}\n");
}
