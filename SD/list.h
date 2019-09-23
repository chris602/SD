/* 
 * File:   list.h
 * Author: peter
 *
 * Created on February 6, 2014, 4:48 PM
 */

#ifndef LIST_H
#define	LIST_H

#include <stdlib.h>
#include "index.h"

typedef enum _DataType_ {
    WORD, ENTRY, INTEGER, QUERY, DOCUMENT
} DataType;

typedef struct _Hash_ Hash;
typedef struct _List_ List;
typedef struct _ListNode_ ListNode;
typedef ListNode * ListIterator;

struct _ListNode_ {
    void *data;
    ListIterator next;
    ListIterator previous;
};

struct _List_ {
    ListIterator first;
    ListIterator last;
    unsigned int size;
    DataType type;
};

struct _Hash_ {
    void *data;
    List OverflowList;
    DataType type;
};


enum ErrorCode ListCreate(List *, DataType);
enum ErrorCode ListDestroy(List *);
enum ErrorCode ListDestroyNodes(List *);
enum ErrorCode ListInsert(List *, void *);
enum ErrorCode ListDelete(List *, ListIterator *);
enum ErrorCode ListAddNode(List *, ListIterator);
enum ErrorCode ListRemoveNode(List *, ListIterator);
enum ErrorCode ListGetNthNode(List *, unsigned int, void **);
enum ErrorCode ListEqual(void *, void *, DataType, int *);
enum ErrorCode ListMember(List *, void *, int *);
enum ErrorCode ListSubset(List *, unsigned int, List *);
enum ErrorCode ListRemoveDublicates(List *);


unsigned int HashFunction(void *, DataType, unsigned int);
enum ErrorCode HashTableCreate(unsigned int, DataType, Hash **);
enum ErrorCode HashTableDestroy(Hash *, unsigned int);


void ListPrint(List *);

#endif	/* LIST_H */

