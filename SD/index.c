#include <stdlib.h>

#include "core.h"
#include "index.h"
#include "defn_impl.h"
#include "index_impl.h"

enum ErrorCode CreateEntry(const Word* word, Entry* entry) {
    if (NULL == word || NULL == entry) {
        return EC_FAIL; ///
    }
    /*
    if (NULL == (entry->word = malloc(sizeof (Word)))) {
        printf("In function CreateEntry(index.c): malloc failed.\n");
        exit(EXIT_FAILURE);
    }
    */
    entry->payload = NULL; /////////
    //CreateWord(word->word, entry->word);
    entry->word = word;
    return EC_SUCCESS;
}

enum ErrorCode AddEntry(EntryList* entryList, const Entry* entry) {
    Entry *newData;
    if (NULL == entryList || NULL == entry)
        return EC_FAIL;
    if (NULL == (newData = malloc(sizeof(Entry))))
        return EC_FAIL;
    if (EC_FAIL == CreateEntry(entry->word, newData))
        return EC_FAIL;
    return ListInsert(&(entryList->list), newData);
}

enum ErrorCode DestroyEntry(Entry *entry) {
    if (NULL == entry) {
        return EC_FAIL;
    }
    //free(entry->word->word);
    //free(entry->word);
    free(entry->payload);
    free(entry);
    return EC_SUCCESS;
}

enum ErrorCode CreateEntryList(EntryList* entryList) {
    if (NULL == entryList)
        return EC_FAIL;
    entryList->curr = NULL;
    return ListCreate(&(entryList->list), ENTRY);
}

Entry* GetFirst(EntryList* entryList) {
    if (NULL == entryList) {
        return NULL;
    }
    if (0 == entryList->list.size) {
        return NULL;
    }
    entryList->curr = entryList->list.first;
    return entryList->curr->data;
}

Entry* GetNext(EntryList* entryList) {
    if (NULL == entryList) {
        return NULL;
    }
    if (NULL == entryList->curr)
        return NULL;
    entryList->curr = entryList->curr->next;
    if (NULL == entryList->curr)
        return NULL;
    return entryList->curr->data;
}

enum ErrorCode DestroyEntryList(EntryList* entryList) {
    ListIterator it1, it2;
    
    for (it1 = entryList->list.first; NULL != it1; ) {
        it2 = it1->next;
        free(((Entry *)it1->data)->payload);
        free(it1->data);
        free(it1);
        it1 = it2;
    }
    entryList->curr = NULL;
    return ListCreate(&(entryList->list), ENTRY);
}

unsigned int GetNumberOfEntries(const EntryList* entryList) {
    if (NULL == entryList) {
        return -1;
    }
    return entryList->list.size;
}

enum ErrorCode BuildEntryIndex(EntryList* entry_list,
                               enum MatchType type,
                               Index* index) {
    
    EntryList copy;
    ListIterator it;
    if (MT_HAMMING_DIST != type && MT_EDIT_DIST != type)
        return EC_FAIL;
    CreateEntryList(&copy);
    index->type = type;
    index->root = NULL;
    if  (MT_HAMMING_DIST == type) {
        index->length = ((Entry *)entry_list->list.first->data)->word->length;
    }
    for (it = entry_list->list.first; NULL != it; it = it->next) {
        if (MT_HAMMING_DIST == index->type && 
                index->length != ((Entry *)it->data)->word->length) {
            DestroyEntryList(&copy);
            return EC_FAIL;
        }
        AddEntry(&copy, it->data);
    }
    index->type = type;
    index->root = NULL;
    return CreateVPTree(&copy, type, &(index->root));
}

enum ErrorCode DestroyEntryIndex(Index* index) {
    return DestroyVPTree(index->root);
}

/* 
1) if d(Q,vp) <= threshold, then vp is part of the solution

2) if d(Q,vp) <= threshold + median, then check the left subtree

3) if Median <= threshold + d(Q,vp), then check the right subtree
 */

enum ErrorCode LookupEntryIndex(const Word* word,
                                int threshold,
                                const Index* index,
                                EntryList* result) {

    return SearchVPTree(word, threshold, index->root, result);
}


