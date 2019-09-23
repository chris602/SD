#include "core.h"
#include "index.h"
#include "defn_impl.h"
#include "index_impl.h"

#define SQUARE(x) ((x)*(x))

#define NUMBER_OF_NODES 3
#define NUMBER_OF_TEST_NODES 5

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void sort(int *a, int size) {
    int i, j;
    for (i = 0; size > i; i++) {
        for (j = 1; size > j; j++) {
            if (a[j-1] > a[j])
                swap(&(a[j-1]), &(a[j]));
        }
    }
}

double deviation(List *list, Entry *entry, enum MatchType type) {
    int distance;
    double sumXi = 0.0, sumXi2 = 0.0;
    ListIterator i;

    for (i = list->first; NULL != i; i = i->next) {
        if (MT_HAMMING_DIST) {
            distance = HammingDistance(entry->word, ((Entry *) i->data)->word);
        } else {
        distance = EditDistance(entry->word, ((Entry *) i->data)->word);
        }
        sumXi += ((double) distance);
        sumXi2 += ((double) SQUARE(distance));
    }

    return (sumXi2 / list->size) -SQUARE(sumXi / list->size);
}

int MedianOfMedians(EntryList *entryList, Entry *entry, enum MatchType type) {
    int i, x; // indicator variables
    int *matrix, *distance, result, lala;
    unsigned int numb;
    ListIterator it;
    if (NULL == entryList || NULL == entry) {
        return 0;
    }
    if (10 > entryList->list.size) {
        matrix = malloc(sizeof(int)*entryList->list.size);
        for (x = 0, it = entryList->list.first; entryList->list.size > x; x++) {
            if (type == MT_HAMMING_DIST) {
                matrix[x] = HammingDistance(((Entry*) it->data)->word,
                        entry->word);
            } else {
                matrix[x] = EditDistance(((Entry*) it->data)->word,
                        entry->word);
            }
        }
        sort(matrix, entryList->list.size);
        result = matrix[entryList->list.size>>1];
        free(matrix);
        return result;
    } else {
        lala = entryList->list.size/10;
        it = entryList->list.first;
        numb = GetNumberOfEntries(entryList);
        numb = (numb + lala - 1) / lala;
        
        if (NULL == (matrix = malloc(numb*sizeof (int)))) {
            printf("Couldn't allocate matrix for middle values, median -1\n");
            return -1;
        }
        if (NULL == (distance = malloc(lala*sizeof(int))))
            exit(EXIT_FAILURE);
        
        for (x = 0, it = entryList->list.first; numb > x; x++) {
            for (i = 0; lala > i; i++) {
                if (NULL == it) {
                    break;
                }
                if (type == MT_HAMMING_DIST) {
                distance[i] = HammingDistance(((Entry*)it->data)->word,
                        entry->word);
                } else {
                distance[i] = EditDistance(((Entry*)it->data)->word,
                        entry->word);
                }
                it = it->next;
            }
            sort(distance, i+1);
            matrix[x] = distance[i>>1];
        }
        
        
        result = matrix[(numb - 1) / 2 ];
        free(matrix);
        free(distance);
        return result;
    }
}

enum ErrorCode ChooseVantagePoint(EntryList *entryList,
        enum MatchType type,
        Entry **result) {
    List VPcanditates, RandomSubset;
    double max_deviation = -1.0, cur_deviation;
    ListIterator it, final;

    if (NULL == entryList)
        return EC_FAIL;
    if (0 == entryList->list.size)
        return EC_FAIL;

    if (2 >= entryList->list.size) {
        final = entryList->list.first;
    } else if (8 >= entryList->list.size) {
        ListIterator it2;
        for (it = entryList->list.first; NULL != it; ) {
            it2 = it->next;
            if (EC_FAIL == ListRemoveNode(&(entryList->list), it))
                return EC_FAIL;
            cur_deviation = deviation(&(entryList->list), (Entry *) it->data, type);
            if (cur_deviation > max_deviation) {
                cur_deviation = max_deviation;
                final = it;
            }
            if (EC_FAIL == ListAddNode(&(entryList->list), it))
                return EC_FAIL;
            it = it2;
        }
    } else {
        if (EC_FAIL == ListCreate(&VPcanditates, ENTRY))
            return EC_FAIL;
        if (EC_FAIL == ListCreate(&RandomSubset, ENTRY))
            return EC_FAIL;
        if (EC_FAIL == ListSubset(&(entryList->list), NUMBER_OF_NODES, &VPcanditates)) {
            return EC_FAIL;
        }
        if (EC_FAIL == ListSubset(&(entryList->list), entryList->list.size / 2, &RandomSubset)) {
            return EC_FAIL;
        }

        for (it = VPcanditates.first; NULL != it; it = it->next) {
            cur_deviation = deviation(&RandomSubset, (Entry *) it->data, type);
            if (max_deviation < cur_deviation) {
                max_deviation = cur_deviation;
                final = it;
            }
        }

        for (it = VPcanditates.first; NULL != it; it = VPcanditates.first) {
            if (EC_FAIL == ListRemoveNode(&VPcanditates, it))
                return EC_FAIL;
            if (EC_FAIL == ListAddNode(&(entryList->list), it))
                return EC_FAIL;
        }
        for (it = RandomSubset.first; NULL != it; it = RandomSubset.first) {
            if (EC_FAIL == ListRemoveNode(&RandomSubset, it))
                return EC_FAIL;
            if (EC_FAIL == ListAddNode(&(entryList->list), it))
                return EC_FAIL;
        }
        
        //printf("finalizing\n");
    }
    if (EC_FAIL == ListRemoveNode(&(entryList->list), final))
        return EC_FAIL;
    *result = final->data;
    free(final);
    return EC_SUCCESS;
}

enum ErrorCode DestroyVPTree(VPNode *parent) {
    if (NULL != parent) {
        if (EC_FAIL == DestroyEntry(parent->data)) {
            return EC_FAIL;
        }
        if (NULL != parent->left)
            if (EC_FAIL == DestroyVPTree(parent->left)) {
                return EC_FAIL;
            }
        if (NULL != parent->right)
            if (EC_FAIL == DestroyVPTree(parent->right)) {
                return EC_FAIL;
            }
        free(parent);
    }

    return EC_SUCCESS;
}

enum ErrorCode CreateVPTree(EntryList *entryList,
        enum MatchType type,
        VPNode **parent) {

    EntryList RightEntries, *LeftEntries = entryList;
    Entry *VantagePoint;
    ListIterator it, it2;
    int distance;
    
    if (NULL == (*parent = malloc(sizeof(VPNode))))
        return EC_FAIL;
    
    (*parent)->data = NULL;
    (*parent)->left = (*parent)->right = NULL;
    (*parent)->median -1;

    //printf("(%d)\n", entryList->list.size);
    if (1 == entryList->list.size) {
        (*parent)->left = (*parent)->right = NULL;
        (*parent)->median = -1;
        it = entryList->list.first;
        if (EC_FAIL == ListRemoveNode(&(entryList->list), it))
            return EC_FAIL;
        (*parent)->data = it->data;
        free(it);
    } else {
        //printf("choosing vantage point\n");
        if (EC_FAIL == ChooseVantagePoint(entryList, type, &VantagePoint))
            return EC_FAIL;
        //printf("found vp = %s\n", VantagePoint->word->word);
        
        //printf("median\n");
        distance = MedianOfMedians(entryList, VantagePoint, type);
        (*parent)->median = distance;
        //printf("yes!\n");
        (*parent)->data = VantagePoint;

        CreateEntryList(&RightEntries);

        //printf("separating the list\n");
        for (it = entryList->list.first; NULL != it;) {
            if (MT_HAMMING_DIST == type) {
                distance = HammingDistance(((Entry *) it->data)->word,
                        ((Entry*) (*parent)->data)->word);
            } else {//MT_EDIT_DIST
                distance = EditDistance(((Entry *) it->data)->word,
                        ((Entry*) (*parent)->data)->word);
            }
            if ((*parent)->median < distance) {
                it2 = it->next;
                if (EC_FAIL == ListRemoveNode(&(entryList->list), it))
                    return EC_FAIL;
                if (EC_FAIL == ListAddNode(&(RightEntries.list), it))
                    return EC_FAIL;
                it = it2;
            } else {
                it = it->next;
            }
        }
        //printf("%d ->(%d , %d)\n", lala, LeftEntries->list.size, RightEntries.list.size);
        if (LeftEntries->list.size) {
            if (EC_FAIL == CreateVPTree(LeftEntries, type, &((*parent)->left))) {
                return EC_FAIL;
            }
        }
        if (RightEntries.list.size) {
            if (EC_FAIL == CreateVPTree(&RightEntries, type, &((*parent)->right))) {
                return EC_FAIL;
            }
        }
    }
    return EC_SUCCESS;
}

enum ErrorCode SearchVPTree(const Word* word,
        int threshold,
        const VPNode *parent,
        EntryList* result) {
    int DistanceWordVP = EditDistance(word, parent->data->word); //(d(Q, vp)

    if (DistanceWordVP <= threshold) {
        AddEntry(result, parent->data);
    }

    if (DistanceWordVP <= threshold + parent->median && NULL != parent->left) {
        if (EC_FAIL == SearchVPTree(word, threshold, parent->left, result)) {
            return EC_FAIL;
        }
    }

    if (DistanceWordVP + threshold >= parent->median && NULL != parent->right) {
        if (EC_FAIL == SearchVPTree(word, threshold, parent->right, result)) {
            return EC_FAIL;
        }
    }

    return EC_SUCCESS;
}
