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