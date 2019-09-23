#include "defn_impl.h"
#include "list.h"

void CreateWord(const char* word_str, Word* word) {
    if (NULL == word || NULL == word_str) {
        printf("In function CreateWord (defn_impl.c): NULL argument(s)");
        exit(EXIT_FAILURE);
    }
    word->length = strlen(word_str);
    word->word = malloc(sizeof (char)*(word->length + 1));
    strcpy(word->word, word_str);
}

void CreateQuery(QueryID query_id, const char* query_str,
        enum MatchType match_type, unsigned int match_dist, Query* query) {
    char *temp_str, *text;
    Word *word;

    if (NULL == query || NULL == query_str) {
        printf("In function CreateQuery (defn_impl.c): NULL argument(s)");
        exit(EXIT_FAILURE);
    }

    query->id = query_id;
    query->match_dist = match_dist;
    query->matchtype = match_type;

    if (NULL == (text = malloc(sizeof (char)*(strlen(query_str) + 1)))) {
        printf("In function CreateQuery (defn_impl.c): malloc failed");
        exit(EXIT_FAILURE);
    }
    strcpy(text, query_str);

    ListCreate(&(query->words), WORD);

    if (NULL != (temp_str = strtok(text, " "))) {
        if (NULL == (word = malloc(sizeof (Word)))) {
            exit(1);
        }
        CreateWord(temp_str, word);
        ListInsert(&(query->words), word);
        while (NULL != (temp_str = strtok(NULL, " "))) {
            if (NULL == (word = malloc(sizeof (Word)))) {
                exit(1);
            }
            CreateWord(temp_str, word);
            ListInsert(&(query->words), word);
        }
    }
    query->num_of_words = query->words.size;
    free(text);
}

int GetNumQueryWords(struct Query* query) {

    if (NULL == query) {
        printf("In function GetNumberQueryWords (defn_impl.c): NULL argument");
        exit(EXIT_FAILURE);
    }

    return query->num_of_words;
}

const struct Word* getQueryWord(unsigned int word, Query* query) {
    Word *result;
    if (NULL == query) {
        printf("In function getQueryWord (defn_impl.c): NULL argument");
        exit(EXIT_FAILURE);
    }

    ListGetNthNode(&(query->words), word, ((void**) &result));
    return result;
}

void CreateDocument(DocID doc_id, const char* doc_str, Document* document) {
    document->id = doc_id;
    char *temp_str, *text;
    Word *word;

    if (NULL == document || NULL == doc_str) {
        printf("In function CreateDocument (defn_impl.c): NULL argument(s)");
        exit(EXIT_FAILURE);
    }

    if (NULL == (text = malloc(sizeof (char)*(strlen(doc_str) + 1)))) {
        printf("In function CreateDocument (defn_impl.c): malloc failed");
        exit(EXIT_FAILURE);
    }
    strcpy(text, doc_str);
    ListCreate(&(document->words), WORD);

    if (NULL != (temp_str = strtok(text, " "))) {
        if (NULL == (word = malloc(sizeof (Word)))) {
            exit(1);
        }
        CreateWord(temp_str, word);
        ListInsert(&(document->words), word);
        while (NULL != (temp_str = strtok(NULL, " "))) {
            if (NULL == (word = malloc(sizeof (Word)))) {
                exit(1);
            }
            CreateWord(temp_str, word);
            ListInsert(&(document->words), word);
        }
    }
    document->num_of_words = document->words.size;
    free(text);
}

int GetNumDocumentWords(struct Document* doc) {
    if (NULL == doc) {
        printf("In function GetNumDocumentWords (defn_impl.c): NULL argument");
        exit(EXIT_FAILURE);
    }

    return doc->num_of_words;
}

const struct Word* getDocumentWord(unsigned int word, Document* doc) {
    Word *result;
    if (NULL == doc) {
        printf("In function getDocumentWord (defn_impl.c): NULL argument");
        exit(EXIT_FAILURE);
    }
    ListGetNthNode(&(doc->words), word, ((void**) &result));
    return result;
}

int Equal(const Word* w1, const Word* w2) {
    if (NULL == w1 || NULL == w2) {
        printf("In function Equal (defn_impl.c): NULL argument(s)");
        exit(EXIT_FAILURE);
    }
    //printf("(%s)(%s)\n", w1->word, w2->word);
    if (w1->length == w2->length) {
        if (!strcmp(w1->word, w2->word))
            return 0;
    }
    return 1;
}

int HammingDistance(const Word* w1, const Word* w2) {
    int i, j, result = 0;

    if (NULL == w1 || NULL == w2) {
        printf("In function HammingDistance (defn_impl.c): NULL argument(s)");
        exit(EXIT_FAILURE);
    }

    if (w1->length == w2->length) {
        for (i = 0, j = w1->length - 1; j >= i; i++, j--) {
            if (i == j) {
                if (w1->word[j] != w2->word[j]) {
                    result++;
                }
                return result;
            }
            if (w1->word[i] != w2->word[i]) {
                result++;
            }
            if (w1->word[j] != w2->word[j]) {
                result++;
            }
        }
        return result;
    }
    return -1;
}

int EditDistance(const Word* w1, const Word* w2) {
    int **matrix;
    int i, j; //indicators
    int l1, l2; //lenghts
    int dest = 0, src = 1; //matrix indicators
    int max, flag = 0, result;

    if (NULL == w1 || NULL == w2) {
        printf("In function EditDistance (defn_impl.c): NULL argument(s)");
        exit(EXIT_FAILURE);
    }

    l1 = w1->length;
    l2 = w2->length;
    matrix = malloc(2 * sizeof (int*));

    if (l1 > l2) {
        max = l1;
    } else {
        max = l2;
        flag = 1;
    }

    for (i = 0; i < 2; i++) {
        matrix[i] = malloc((max + 1) * sizeof (int));
    }
    
    for (i = 0; max >= i; i++)
        matrix[0][i] = matrix[1][i] = 0;
    for (i = 0; i <= l1 * flag + (1 - flag) * l2; i++) {
        for (j = 0; j <= l2 * flag + (1 - flag) * l1; j++) {
            if (0 == j) {
                matrix[dest][i] = i;
            } else if (0 == i) {
                matrix[dest][j] = j;
            } else {
                int check;
                matrix[dest][j] = MIN(matrix[src][j] + 1,
                        matrix[dest][j - 1] + 1);
                check =
                        (w1->word[i * flag + j * (1 - flag) - 1] ==
                        w2->word[j * flag + i * (1 - flag) - 1]) ? 0 : 1;

                matrix[dest][j] = MIN(matrix[dest][j],
                        matrix[src][j - 1] + check);
            }
        }
        dest = 1 - dest;
        src = 1 - src;
    }

    result = matrix[src][max];
    if(result < 0) {
        printf("(%s,%s,%d|%d|%d[%d,%d])\n", w1->word, w2->word, result, src, max,w1->length,w2->length);
        for (i = 0; i <= max; i++)
            printf("[%d][%d]\n", matrix[0][i], matrix[1][i]);
        getchar();
    }
    free(matrix[0]);
    free(matrix[1]);
    free(matrix);
    //printf("[%d]\n", result);
    return result;
}

void RemoveDuplicates(Document* doc) {
    if (NULL == doc) {
        printf("In function RemoveDublicates (defn_impl.c): NULL argument");
        exit(EXIT_FAILURE);
    }
    ListRemoveDublicates(&(doc->words));
    doc->num_of_words = doc->words.size;
}
