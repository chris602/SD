#ifndef INDEX_IMPL_H
#define	INDEX_IMPL_H

#include "index.h"
#include "list.h"
#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
    
typedef struct _VPNode_ VPNode;

struct _VPNode_ {
    int median;
    Entry *data;
    VPNode *left;
    VPNode *right;
};
    
struct Index {
    VPNode *root;
    enum MatchType type;
    unsigned int length;//for hamming distance
};


struct EntryList {
    List list;
    ListIterator curr;
};

enum ErrorCode ChooseVantagePoint(EntryList *entryList, enum MatchType type, Entry **result);
int MedianOfMedians(EntryList *entryList, Entry *entry, enum MatchType type);
enum ErrorCode DestroyVPTree(VPNode *parent);
enum ErrorCode CreateVPTree(EntryList *list,
                            enum MatchType type,
                            VPNode **parent);
enum ErrorCode SearchVPTree(const Word* word,
                            int threshold,
                            const VPNode *parent,                            
                            EntryList* result);


#ifdef	__cplusplus
}
#endif

#endif	/* INDEX_IMPL_H */
