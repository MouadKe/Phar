#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

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

node** init();
node* createNode(med data);
void readmed(med* m);
bool expiring(med m);
void Textform(char* N);
void fill(node** array,med m);
void PrintStock(const node** array);


int main(){
    
    node** array= init();
    int N;
    printf("Enter the number of medicines: ");
    scanf("%d",&N);
    med m;
    for(int i = 0;i<N;i++){
        readmed(&m);
        fill(array,m);
    }
    PrintStock(array);





    return 0;
}

node** init(){
    return (node**)calloc(sizeof(node*),26);
}

node* createNode(med data){
    node* temp = (node*)malloc(sizeof(node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}

void readmed(med* m){
    printf("Enter brand: ");
    scanf("%s",m->brand);
    Textform(m->brand);

    printf("Enter name: ");
    scanf("%s",m->name);
    Textform(m->name);

    printf("Enter quantity: ");
    scanf("%d",&m->quantity);

    printf("Enter price: ");
    scanf("%f",&m->price);

    printf("Enter manufacturing date: ");
    scanf("%d %d %d",&m->manf.day,&m->manf.month,&m->manf.year);

    printf("Enter expiry date: ");
    scanf("%d %d %d",&m->exp.day,&m->exp.month,&m->exp.year);
}

bool expiring(med m){
    time_t t = time(NULL);
  struct tm tm = *localtime(&t);
    if(m.exp.year<tm.tm_year + 1900){
        return true;
    }
    else if(m.exp.year==tm.tm_year + 1900){
        if(m.exp.month<tm.tm_mon + 1){
            return true;
        }
        else if(m.exp.month==tm.tm_mon + 1){
            if(m.exp.day<tm.tm_mday){
                return true;
            }
        }
    }
    return false;
}

void Textform(char* N){
    N[0] = toupper(N[0]);
    for(int i = 1;N[i]!='\0';i++){
        N[i] = tolower(N[i]);
    }

}

void fill(node** array,med m){
    int i = m.name[0] - 'A';
    if(expiring(m)){
        printf("Medicine is expired\nit won't be added to the stock\n");
        return;
    }else{
        if(array[i]==NULL){
            array[i] = createNode(m);
        }else{
            node* temp = array[i];
            while(temp->next!=NULL){
                temp = temp->next;
            }
            temp->next = createNode(m);
        }
    }
}

void PrintStock(const node** array){
    for(int i = 0;i<26;i++){
        if(array[i]!=NULL){
            node* temp = array[i];
            while(temp!=NULL){
                printf("=====================\n");
                printf("Brand: %s\n",temp->data.brand);
                printf("Name: %s\n",temp->data.name);
                printf("Quantity: %d\n",temp->data.quantity);
                printf("Price: %.2f\n",temp->data.price);
                printf("Manufacturing date: %d/%d/%d\n",temp->data.manf.day,temp->data.manf.month,temp->data.manf.year);
                printf("Expiry date: %d/%d/%d\n",temp->data.exp.day,temp->data.exp.month,temp->data.exp.year);
                printf("=====================\n");
                temp = temp->next;
            }
        }
    }
}