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

// Function prototypes
// data structure functions
node** init();
node* createNode(med data);

// input/output functions
void readmed(med* m);
void fill(node** array,med m);
void PrintStock(const node** array);
bool Badnumber(int N);

// string functions
void Textform(char* N);
bool BadName(char N);

//date functions
bool expiring(med m);
bool BadDate(date d , bool manf);
bool Manfucdate(date manf,date exp);



int main(){
    
    node** array= init(26);
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
    int counter =0;


    printf("Enter brand: ");
    scanf("%s",m->brand);
    Textform(m->brand);

    do{
        if(counter>0){
            printf("Invalid name!!!\nThe name should start with a letter\n");
        }
        printf("Enter name: ");
        scanf("%s",m->name);
        Textform(m->name);
        counter++;

    }while(BadName(m->name[0]));
    counter = 0;
    
    do{
        if(counter>0){
            printf("Invalid quantity!!!\nQuantity should be a positive number\n");
        }
        printf("Enter quantity: ");
        scanf("%d",&m->quantity);
        counter++;
    }while(Badnumber(m->quantity));
    counter = 0;

    do{
        if(counter>0){
            printf("Invalid price!!!\nPrice should be a positive number\n");
        }
        printf("Enter price: ");
        scanf("%f",&m->price);
        counter++;
    }while(Badnumber(m->price));
    counter = 0;

    do{
        if(counter>0){
            printf("Invalid manufacturing date!!!\n");
        }
        printf("Enter manufacturing date: ");
        scanf("%d %d %d",&m->manf.day,&m->manf.month,&m->manf.year);
        counter++;
    }while(BadDate(m->manf,true));
    counter = 0;

    do{
        if(counter>0){
            printf("Invalid expiry date!!!\n");
            if(Manfucdate(m->manf,m->exp)){
                printf("Expiry date should be after the manufacturing date\n");
            }
        }
        printf("Enter expiry date (dd mm yyyy): ");
        scanf("%d %d %d",&m->exp.day,&m->exp.month,&m->exp.year);
        counter++;
    }while(BadDate(m->exp,false) || Manfucdate(m->manf,m->exp));
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
            array[i] = createNode(m);
            array[i]->next = temp;
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
                printf("Manufacturing date: %02d/%02d/%d\n",temp->data.manf.day,temp->data.manf.month,temp->data.manf.year);
                printf("Expiry date: %02d/%02d/%d\n",temp->data.exp.day,temp->data.exp.month,temp->data.exp.year);
                printf("=====================\n");
                temp = temp->next;
            }
        }
    }
}

bool Badnumber(int N){
    if(N<0) return true;
    return false;
}


void Textform(char* N){
    N[0] = toupper(N[0]);
    for(int i = 1;N[i]!='\0';i++){
        N[i] = tolower(N[i]);
    }

}

bool BadName(char N){
    if(N>='A' && N<='Z') return false;
    return true;
}



bool Manfucdate(date manf,date exp){
    if(exp.year<manf.year) return true;
    else if(exp.year==manf.year){
        if(exp.month<manf.month) return true;
        else if(exp.month==manf.month){
            if(exp.day<manf.day) return true;
        }
    }
    return false;
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

bool BadDate(date d , bool manf){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if(manf && d.year>tm.tm_year + 1900) return true;
    if(d.year<1) return true;
    if (d.month < 1 || d.month > 12) return true;
    if (d.day < 1 || d.day > 31) return true;
    switch(d.month){
        case 4:case 6:case 9:case 11:
            if(d.day>30) return true;
            break;
        case 2:
            if(d.year%4==0){
                if(d.day>29) return true;
            }else{
                if(d.day>28) return true;
            }
            break;
    }
}

node** SearchMedicine(node** array,char name){
    int i = name - 'A';
    int counter = 2;
    node* temp = array[i];
    node** result = (node**)calloc(sizeof(node*),2);
    *result = temp;
    *(result+1) = NULL;

    while(temp!=NULL){
        counter++;
        temp = temp->next;
        result = (node**)realloc(result,sizeof(node*)*counter);
        *(result+counter-1) = temp;
    }

    return result;
}

int PrintSearch(node** result){
    int counter = 0;
    for(int i = 0;result[i]!=NULL;i++){
        counter++;
        printf("=====================\n");
        printf("Medecine %d\n",counter+1);
        printf("Brand: %s\n",result[i]->data.brand);
        printf("Name: %s\n",result[i]->data.name);
        printf("=====================\n");
    }
    return counter;
}