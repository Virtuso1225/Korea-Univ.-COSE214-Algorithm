//
//  main.c
//  2-gram
//
//  Created by 차승민 on 2020/12/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct index{
    char *data;
    struct index *next;
}INDEX_NODE;

typedef struct{
    INDEX_NODE *head;
    INDEX_NODE *tail;
}INDEX;

typedef struct{
    INDEX *digit[26][26];
    INDEX *one_digit[26];
}INDEX_DATA;

INDEX_NODE *createNode(char *data, unsigned long size){
    INDEX_NODE *newNode = (INDEX_NODE *)malloc(sizeof(INDEX_NODE));
    newNode->data = (char *)malloc(sizeof(char *) * size);
    strcpy(newNode->data, data);
    newNode->next = NULL;
    return newNode;
}

INDEX_DATA *createINDEX(){
    INDEX_DATA *index = (INDEX_DATA *)malloc(sizeof(INDEX_DATA));
    for(int i = 0; i < 26; ++i){
        index->one_digit[i] = (INDEX *)malloc(sizeof(INDEX));
        index->one_digit[i]->head = NULL;
        index->one_digit[i]->tail = NULL;
        for(int j = 0; j < 26; ++j){
            index->digit[i][j] = (INDEX *)malloc(sizeof(INDEX));
            index->digit[i][j]->head = NULL;
            index->digit[i][j]->tail = NULL;
        }
    }
    return index;
}

void INDEXING(INDEX_DATA *index, FILE *infp){
    
    char buffer[50];
    int digit_1, digit_2;
    int digit_1_check = 0;
    int digit_2_check = 0;
    int digit = 0;
    int digit_check = 0;
    
    while(fscanf(infp, "%s", buffer) != EOF){
        
        unsigned long len = strlen(buffer);
        
        if(len < 6){
            for(int i = 0; i < len; ++i){
                digit = buffer[i] - 97;
                if(digit_check & (1 << digit)) continue;
                INDEX_NODE *node = createNode(buffer, len);
                if(index->one_digit[digit]->tail != NULL){
                    index->one_digit[digit]->tail->next = node;
                    index->one_digit[digit]->tail = node;
                }
                else{
                    index->one_digit[digit]->head = index->one_digit[digit]->tail = node;
                }
                digit_check |= (1 << digit);
            }
            digit_check = 0;
        }
        
        for(int i = 0; buffer[i + 1] != '\0'; ++i){
            digit_1 = buffer[i] - 97;
            digit_2 = buffer[i + 1] - 97;
            if((digit_1_check & (1 << digit_1)) && (digit_2_check & (1 << digit_2))) continue;
            INDEX_NODE *newNode = createNode(buffer, strlen(buffer));
            if(index->digit[digit_1][digit_2]->tail != NULL){
                index->digit[digit_1][digit_2]->tail->next = newNode;
                index->digit[digit_1][digit_2]->tail = newNode;
            }
            else{
                index->digit[digit_1][digit_2]->head = index->digit[digit_1][digit_2]->tail = newNode;
            }
            digit_1_check |= (1 << digit_1);
            digit_2_check |= (1 << digit_2);
        }
        digit_1_check = 0;
        digit_2_check = 0;
        if(feof(infp)) break;
    }
}

void printINDEX(INDEX_DATA *index){
    for(int i = 0; i < 26; ++i){
        for(int j = 0; j < 26; ++j){
            char name[10];
            sprintf(name, "%c%c.txt", i + 97, j + 97);
            FILE *fp = fopen(name, "wt");
            INDEX_NODE *temp = index->digit[i][j]->head;
            while(temp){
                fprintf(fp, "%s\n", temp->data);
                temp = temp->next;
            }
            fclose(fp);
        }
    }
    for(int i = 0; i < 26; ++i){
        char name[10];
        sprintf(name, "%c.txt", i + 97);
        FILE *fp = fopen(name, "wt");
        INDEX_NODE *temp = index->one_digit[i]->head;
        while(temp){
            fprintf(fp, "%s\n", temp->data);
            temp = temp->next;
        }
        fclose(fp);
    }
}

void destroy(INDEX_DATA *index){
    for(int i = 0; i < 26; ++i){
        for(int j = 0; j < 26; ++j){
            INDEX_NODE *temp = index->digit[i][j]->head;
            while(temp){
                INDEX_NODE *del = temp;
                temp = temp->next;
                free(del->data);
                free(del);
            }
            free(index->digit[i][j]);
        }
        INDEX_NODE *temp = index->one_digit[i]->head;
        while(temp){
            INDEX_NODE *del = temp;
            temp = temp->next;
            free(del->data);
            free(del);
        }
        free(index->one_digit[i]);
    }
    free(index);
}

int main(int argc, char **argv){
    
    INDEX_DATA *index = createINDEX();
    
    FILE *infp = fopen(argv[1], "rt");
    
    INDEXING(index, infp);
    
    printINDEX(index);
    
    destroy(index);
    
    fclose(infp);
    
    return 0;
}
