#ifndef DEFN_IMPL_H
#define	DEFN_IMPL_H

#ifdef	__cplusplus
extern "C" {
#endif
#define MAX(x,y) ((x>=y)?(x):(y))
#define MIN(x,y) ((x<=y)?(x):(y))
#include <string.h>
#include <stdlib.h>
#include "defn.h"
#include <stdio.h>
#include "list.h"
#define WORD_LENGTH MAX_WORD_LENGTH+1

    struct Word {
        unsigned int length;
        char *word;
        //char word[WORD_LENGTH];
    };

    struct Query {
        QueryID id;
        unsigned int num_of_words;
        List words;
        unsigned int match_dist;
        enum MatchType matchtype;
    };

    struct Document {
        DocID id;
        List words;
        unsigned int num_of_words;
    };


#ifdef	__cplusplus
}
#endif

#endif	/* DEFN_IMPL_H */