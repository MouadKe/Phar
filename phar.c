#include <stdlib.h>
#include <stdio.h>

typedef struct date{
    int day;
    int month;
    int year;
}date;

typedef struct med {
    char brand[20];
    char name[20];
    int quantity;
    float price;
    date manf;
    date exp;
    
}med;

typedef struct node {
    med data;
    struct node *next;
}node;

node* init();


int main(){
    node* array= init();
    






    return 0;
}

node* init(){
    return (node*)malloc(sizeof(node)*26);
}

node* createNode(med data){
    node* temp = (node*)malloc(sizeof(node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}