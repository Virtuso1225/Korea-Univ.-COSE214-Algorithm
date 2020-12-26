//
//  search.c
//
//
//  Created by 차승민 on 2020/12/20.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define capacity 3000000

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10

#define INSERT_COST    1
#define DELETE_COST    1
#define SUBSTITUTE_COST    1
#define TRANSPOSE_COST    1

#define swap(x, y, z){z = x; x = y; y = z;}

typedef struct node{
    struct node* child[26];
    bool finish;
    int count;
    char data[50];
}TRIE_NODE;

typedef struct {
    TRIE_NODE* root[26];
}TRIE;

typedef struct HeapNode{
    int freq;
    char data[50];
}HEAP_NODE;

typedef struct{
    HEAP_NODE **heap;
    int size;
    int count;
}HEAP;

typedef struct CANDIDATE{
    int dist;
    char data[50];
}CANDIDATE;

TRIE *createTrie(){
    TRIE *trie = (TRIE *)malloc(sizeof(TRIE));
    for(int i = 0; i < 26; ++i) trie->root[i] = NULL;
    return trie;
}

TRIE_NODE *newNode(){
    TRIE_NODE *node = (TRIE_NODE *)malloc(sizeof(TRIE_NODE));
    for(int i = 0; i < 26; ++i) node->child[i] = NULL;
    return node;
}

void destroyNode(TRIE_NODE *node){
    for(int i = 0; i < 26; ++i){
        if(node->child[i]){
            destroyNode(node->child[i]);
        }
    }
    free(node);
}

void destroyTrie(TRIE *Trie){
    for(int i = 0; i < 26; ++i){
        if(Trie->root[i])
            destroyNode(Trie->root[i]);
    }
    free(Trie);
}

HEAP *createHeap(){
    HEAP *Heap = (HEAP *)malloc(sizeof(HEAP));
    Heap->heap = (HEAP_NODE **)malloc(sizeof(HEAP_NODE *) *capacity);
    Heap->size = 0;
    Heap->count = 0;
    return Heap;
}

void destroyHeap(HEAP *Heap){
    if(Heap){
        for(int i = 0; i < Heap->size; ++i){
            if(Heap->heap[i]){
                free(Heap->heap[i]);
            }
        }
        free(Heap->heap);
        free(Heap);
    }
}

int compare(const void *a, const void *b){

    struct CANDIDATE *ptrA = (struct CANDIDATE *)a;
    struct CANDIDATE *ptrB = (struct CANDIDATE *)b;

    if(ptrA->dist < ptrB->dist) return -1;
    else if(ptrA->dist == ptrB->dist) return 0;
    else return 1;
}

void HeapUp(HEAP *Heap, int freq, char *data){
    int i = ++(Heap->size) - 1;
    Heap->heap[i] = (HEAP_NODE *)malloc(sizeof(HEAP_NODE));
    strcpy(Heap->heap[i]->data, data);
    Heap->heap[i]->freq = freq;
    HEAP_NODE *temp;
    temp = Heap->heap[i];
    while(i > 0 && Heap->heap[(i - 1) / 2]->freq < freq){
        Heap->heap[i] = Heap->heap[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    Heap->heap[i] = temp;
    Heap->count++;
}

void delete(HEAP *Heap, char *string){
    if(Heap->size <= 0) return;
    strcpy(string, Heap->heap[0]->data);
    HEAP_NODE *del = Heap->heap[0];
    free(del);
    Heap->heap[0] = Heap->heap[--(Heap->size)];
    int here = 0;
    while(here < Heap->size){
        HEAP_NODE *temp;
        int left = here * 2 + 1, right = here * 2 + 2;
        if(left >= Heap->size) break;
        int next = here;
        if(Heap->heap[next]->freq < Heap->heap[left]->freq) next = left;
        if(right < Heap->size && Heap->heap[next]->freq < Heap->heap[right]->freq) next = right;
        if(next == here) break;
        swap(Heap->heap[next], Heap->heap[here], temp);
        here = next;
    }
}

void find_Trie(HEAP *Heap, TRIE_NODE *node){
    if(node->finish == true){
        node->finish = false;
        if(Heap->size < capacity) HeapUp(Heap, node->count, node->data);
    }
    for(int i = 0; i < 26; ++i){
        if(node->child[i]) find_Trie(Heap, node->child[i]);
    }
    return;
}

static int __GetMin3( int a, int b, int c)
{
    int min = a;
    if (b < min)
        min = b;
    if(c < min)
        min = c;
    return min;
}

static int __GetMin4( int a, int b, int c, int d)
{
    int min = __GetMin3( a, b, c);
    return (min > d) ? d : min;
}

int min_editdistance(char *str1, char *str2){
    int dp[50][50];
    unsigned long n = strlen(str1);
    unsigned long m = strlen(str2);
    
    int op_matrix[(n + 1) * (m + 1)];
    
    for(int i = 0; i < (n + 1) * (m + 1); ++i) op_matrix[i] = 0;
    
    for(int i = 0; i < m + 1; ++i){
        dp[0][i] = i;
    }
    for(int j = 0; j < n + 1; ++j){
        dp[j][0] = j;
    }

    for(int i = 1; i < n + 1; ++i){
        for(int j = 1; j < m + 1; ++j){
            if(str1[i - 1] == str2[j - 1]){
                if(str1[i - 2] == str2[j - 1] && str1[i - 1] == str2[j - 2]){
                    dp[i][j] = __GetMin4(dp[i][j - 1] + INSERT_COST, dp[i - 1][j] + DELETE_COST, dp[i - 1][j - 1], dp[i - 2][j - 2] + TRANSPOSE_COST);
                    if(dp[i][j] == dp[i][j - 1] + INSERT_COST){
                        op_matrix[i * m + j] |= INSERT_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j] + DELETE_COST){
                        op_matrix[i * m + j] |= DELETE_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j - 1]){
                        op_matrix[i * m + j] |= MATCH_OP;
                    }
                    if(dp[i][j] == dp[i - 2][j - 2] + TRANSPOSE_COST){
                        op_matrix[i * m + j] |= TRANSPOSE_OP;
                    }
                }
                else{
                    dp[i][j] = __GetMin3(dp[i][j - 1] + INSERT_COST, dp[i - 1][j] + DELETE_COST, dp[i - 1][j - 1]);
                    if(dp[i][j] == dp[i][j - 1] + INSERT_COST){
                            op_matrix[i * m + j] |= INSERT_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j] + DELETE_COST){
                        op_matrix[i * m + j] |= DELETE_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j - 1]){
                        op_matrix[i * m + j] |= MATCH_OP;
                    }
                }
            }
            else{
                if(str1[i - 2] == str2[j - 1] && str1[i - 1] == str2[j - 2]){
                    dp[i][j] = __GetMin4(dp[i][j - 1] + INSERT_COST, dp[i - 1][j] + DELETE_COST, dp[i - 1][j - 1] + SUBSTITUTE_COST, dp[i - 2][j - 2] + TRANSPOSE_COST);
                    if(dp[i][j] == dp[i][j - 1] + INSERT_COST){
                        op_matrix[i * m + j] |= INSERT_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j] + DELETE_COST){
                        op_matrix[i * m + j] |= DELETE_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j - 1] + SUBSTITUTE_COST){
                        op_matrix[i * m + j] |= SUBSTITUTE_OP;
                    }
                    if(dp[i][j] == dp[i - 2][j - 2] + TRANSPOSE_COST){
                        op_matrix[i * m + j] |= TRANSPOSE_OP;
                    }
                }
                else{
                    dp[i][j] = __GetMin3(dp[i][j - 1] + INSERT_COST, dp[i - 1][j] + DELETE_COST, dp[i - 1][j - 1] + SUBSTITUTE_COST);
                    if(dp[i][j] == dp[i][j - 1] + INSERT_COST){
                            op_matrix[i * m + j] |= INSERT_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j] + DELETE_COST){
                        op_matrix[i * m + j] |= DELETE_OP;
                    }
                    if(dp[i][j] == dp[i - 1][j - 1] + SUBSTITUTE_COST){
                        op_matrix[i * m + j] |= SUBSTITUTE_OP;
                    }
                }
            }
        }
    }
    return dp[strlen(str1)][strlen(str2)];
}

void print_candidate(CANDIDATE *candidate){
    for(int i = 0; i < 10; ++i){
        printf("%d. %s - distance : %d\n", i + 1, candidate[i].data, candidate[i].dist);
    }
}

void get_one_digit(TRIE *Trie, char *digit){
    char name[10];
    sprintf(name, "%c.txt", digit[0]);
    FILE *fp = fopen(name, "rt");
    char buffer[100];
    while(fscanf(fp, "%s", buffer)){
        if(feof(fp)) break;
        
        TRIE_NODE *node = NULL;
        if(Trie->root[buffer[0] - 97]){
            node = Trie->root[buffer[0] - 97];
        }
        else{
            Trie->root[buffer[0] - 97] = newNode();
            node = Trie->root[buffer[0] - 97];
        }
        if(buffer[1] == '\0'){
            node->finish = true;
            node->count++;
            strcpy(node->data, buffer);
        }
        for(int i = 1; buffer[i] != '\0'; ++i){
            if(node->child[buffer[i] - 97] == NULL) node->child[buffer[i] - 97] = newNode();
            node = node->child[buffer[i] - 97];
            if(buffer[i + 1] == '\0'){
                node->finish = true;
                node->count++;
                strcpy(node->data, buffer);
            }
        }
    }
    fclose(fp);
    HEAP *Heap = createHeap();
    for(int i = 0; i < 26; ++i){
        if(Trie->root[i]){
            find_Trie(Heap, Trie->root[i]);
        }
    }
    
    CANDIDATE *string_candidate = (CANDIDATE *)malloc(sizeof(CANDIDATE) * Heap->count);
    int count = 0;
    for(int i = 0; i < Heap->count; ++i){
        if(Heap->size <= 0) break;
        delete(Heap, string_candidate[i].data);
        count++;
    }
    
    for(int i = 0; i < count; ++i){
        string_candidate[i].dist = min_editdistance(digit, string_candidate[i].data);
    }
    
    qsort(string_candidate, count, sizeof(CANDIDATE), compare);
    print_candidate(string_candidate);
    free(string_candidate);
    destroyHeap(Heap);
}

void get_digit(TRIE *Trie, char *string){
    for(int i = 0; string[i + 1] != '\0'; ++i){
        int digit_1 = string[i];
        int digit_2 = string[i + 1];
        char name[10];
        sprintf(name, "%c%c.txt", digit_1, digit_2);
        FILE *fp = fopen(name, "rt");
        char buffer[100];
        while(fscanf(fp, "%s", buffer)){
            if(feof(fp)) break;
            
            TRIE_NODE *node = NULL;
            if(Trie->root[buffer[0] - 97]){
                node = Trie->root[buffer[0] - 97];
            }
            else{
                Trie->root[buffer[0] - 97] = newNode();
                node = Trie->root[buffer[0] - 97];
            }
            for(int i = 1; buffer[i] != '\0'; ++i){
                if(node->child[buffer[i] - 97] == NULL) node->child[buffer[i] - 97] = newNode();
                node = node->child[buffer[i] - 97];
                if(buffer[i + 1] == '\0'){
                    node->finish = true;
                    node->count++;
                    strcpy(node->data, buffer);
                }
            }
        }
        fclose(fp);
    }
    HEAP *Heap = createHeap();
    for(int i = 0; i <  26; ++i){
        if(Trie->root[i]){
            find_Trie(Heap, Trie->root[i]);
        }
    }
    CANDIDATE *string_candidate = (CANDIDATE *)malloc(sizeof(CANDIDATE) * Heap->count);
    int count = 0;
    for(int i = 0; i < Heap->count; ++i){
        if(Heap->size <= 0) break;
        delete(Heap, string_candidate[i].data);
        count++;
    }
    for(int i = 0; i < 50000; ++i){
        string_candidate[i].dist = min_editdistance(string, string_candidate[i].data);
    }
    
    qsort(string_candidate, 50000, sizeof(CANDIDATE), compare);
    print_candidate(string_candidate);
    free(string_candidate);
    destroyHeap(Heap);
}

int main(){
    char string[30];
    scanf("%s", string);
    unsigned long len = strlen(string);
    TRIE *Trie = createTrie();
    if(len == 1){
        get_one_digit(Trie, string);
    }
    else{
        get_digit(Trie, string);
    }
    destroyTrie(Trie);
    return 0;
}
