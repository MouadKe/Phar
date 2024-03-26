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
    char brand[32];
    char name[32];
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

//search functions
node** SearchMedicine(node** array);
int PrintSearch(node** result);
void PrintMedicine(node* result);

//edit functions
void EditPrice(node* result);
void EditQuantity(node* result);
void DeleteMed(node** result , int N , node** array , char name);



int main(){
    
    node** array= init();
    node** result;
    int N;
    med m;
    int choice;
    int choice2;
    int counter=0;
    char trash;
    do{
        printf("1- Add medicine\n2- Print stock\n3- Search medicine\n4- Edit medicine\n5- Delete medicine\n6- Exit\n");
        scanf("%d",&choice);
        scanf("%c",&trash);
        switch(choice){
            case 1:
                printf("Enter the number of medicines you want to add: ");
                scanf("%d",&N);
                for(int i = 0;i<N;i++){
                readmed(&m);
                fill(array,m);
                }
                break;
            case 2:
                PrintStock(array);
                break;
            case 3:
                result = SearchMedicine(array);
                N = PrintSearch(result);
                if(N>0){
                    
                    counter=0;
                    do{
                        if(counter>0){
                            printf("Invalid number!!!\n");
                        }
                        printf("Enter the number of the medicine you want to know more information about: ");
                        scanf("%d",&choice);
                        counter++;
                        }while(choice<1 || choice>N);

                    if(choice<=N){
                        choice--;
                        PrintMedicine(result[choice]);
                    }
                    free(result);
                        }
                    
                
                break;
            case 4:
                result = SearchMedicine(array);
                N = PrintsearchDetails(result);
                if(N>0){
                    counter=0;
                    do{
                        if(counter>0){
                            printf("Invalid number!!!\n");
                        }
                        printf("Enter the number of the medicine you want to edit: ");
                        scanf("%d",&choice);
                        counter++;
                    }while(choice<1 || choice>N);
                    choice--;
                    
                    do{
                        printf("1- Edit price\n2- Edit quantity\n3- Exit\n");
                        scanf("%d",&choice2);
                        switch(choice2){
                            case 1:
                                EditPrice(result[choice]);
                                break;
                            case 2:
                                EditQuantity(result[choice]);
                                break;
                            case 3:
                                free(result);
                                break;
                            default:
                                printf("Invalid choice!!!\n");
                                break;
                        }
                    }while(choice2!=3);
                   
                }
                break;
            case 5:
                result = SearchMedicine(array);
                N = PrintSearch(result);
                if(N>0){
                    
                    counter=0;
                    do{
                        if(counter>0){
                            printf("Invalid number!!!\n");
                        }
                        printf("Enter the number of the medicine you want to delete: ");
                        scanf("%d",&choice);
                        counter++;
                    }while(choice<1 || choice>N);
                    choice--;
                    DeleteMed(result,choice,array,result[choice]->data.name[0]);
                }
                break;
            case 6:
                return 0;
            default:
                printf("Invalid choice!!!\n");
                break;  
        }
    }while(1);





    return 0;
}

//initializing the array of linked lists which each index is a list of medicines that start with the same letter
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
    //The counter here works as a flag to tell us if the user inputed a data we can't use so we can ask him to input it again correctly
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
        printf("Enter manufacturing date (dd mm yyyy): ");
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

//fill array function takes a medicine checks if it's expired and a dd it to the right linked list 
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

// checks if the number is negative
bool Badnumber(int N){
    if(N<0) return true;
    return false;
}

// makes the first letter of the brand and the name capital and the rest of the letters small
void Textform(char* N){
    N[0] = toupper(N[0]);
    for(int i = 1;N[i]!='\0';i++){
        N[i] = tolower(N[i]);
    }

}

// makes sure the name starts with a letter so we don't try to access to an index that doesn't exist
bool BadName(char N){
    if(N>='A' && N<='Z') return false;
    return true;
}


// checks if the expiry date is before the manufacturing date
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

// checks if the medicine is expired
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

// checks if the date is valid
bool BadDate(date d , bool manf){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if(manf){ 
        if(d.year>tm.tm_year + 1900) return true;
        else if(d.year==tm.tm_year + 1900){
            if(d.month>tm.tm_mon + 1) return true;
            else if(d.month==tm.tm_mon + 1){
                if(d.day>tm.tm_mday) return true;
            }
        }
    }
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
    return false;
}



node** SearchMedicine(node** array){
    char name;
    char trash;
    printf("Enter the first letter of the medicine: ");
    scanf("%c",&name);
    scanf("%c",&trash);
    name = toupper(name);

    int i = name - 'A';
    int counter = 2;
    node* temp = array[i];
    //we use a dynamic array to store the result of the search
    // so we can return it and use it later and have O(1) access to the elements
    node** result = (node**)calloc(sizeof(node*),2);
    *result = temp;

    //if the list is empty we return the result array with the first element being NULL
    while(temp->next!=NULL){
        counter++;
        temp = temp->next;
        //we use realloc to add the new element to the array and keep one extra index Null to keep track of the array later
        result = (node**)realloc(result,sizeof(node*)*counter);
        *(result+counter-2) = temp;
        *(result+counter-1)=NULL;
    }

    return result;
}

// this function prints the result of the search and returns the size of the search array
int PrintSearch(node** result){
    int counter = 0;
    if(result[0]==NULL){
        printf("No medicine found!!!\n");
        return -1;
    }else if(result[1]==NULL){
        // if there is one element there one element we print all the details
        PrintMedicine(result[0]);
        return 0;
    }
    for(int i = 0;result[i]!=NULL;i++){
        // if there is more than one element we print the brand and the name only and ask the user to choose which medice they want to know more about
        counter++;
        printf("=====================\n");
        printf("Medecine %d\n",counter);
        printf("Brand: %s\n",result[i]->data.brand);
        printf("Name: %s\n",result[i]->data.name);
        printf("=====================\n");
    }
    return counter;
}

// this function prints the details of a medicine and returns the size of the search array
// we use this function when the user wants to edit a medicine so we can print the details of the medicine they want to edit
int PrintsearchDetails(node** result){
    int counter = 0;
    if(result[0]==NULL){
        printf("No medicine found!!!\n");
        return -1;
    }else if(result[1]==NULL){
        PrintMedicine(result[0]);
        return 0;
    }
    for(int i = 0;result[i]!=NULL;i++){
        counter++;
        printf("=====================\n");
        printf("Medecine %d\n",counter);
        printf("Brand: %s\n",result[i]->data.brand);
        printf("Name: %s\n",result[i]->data.name);
        printf("Quantity: %d\n",result[i]->data.quantity);
        printf("Price: %.2f\n",result[i]->data.price);
        printf("Manufacturing date: %02d/%02d/%d\n",result[i]->data.manf.day,result[i]->data.manf.month,result[i]->data.manf.year);
        printf("Expiry date: %02d/%02d/%d\n",result[i]->data.exp.day,result[i]->data.exp.month,result[i]->data.exp.year);
        printf("=====================\n");
    }
    return counter;
}

// this function prints the details of a medicine
void PrintMedicine(node* result){
    printf("=====================\n");
    printf("Brand: %s\n",result->data.brand);
    printf("Name: %s\n",result->data.name);
    printf("Quantity: %d\n",result->data.quantity);
    printf("Price: %.2f\n",result->data.price);
    printf("Manufacturing date: %02d/%02d/%d\n",result->data.manf.day,result->data.manf.month,result->data.manf.year);
    printf("Expiry date: %02d/%02d/%d\n",result->data.exp.day,result->data.exp.month,result->data.exp.year);
    printf("=====================\n");
}



void EditPrice(node* result){
    int counter = 0;    
    do{
        if(counter>0){
            printf("Invalid price!!!\nPrice should be a positive number\n");
        }
        printf("Enter the new price: ");
        scanf("%f",&result->data.price);
        counter++;
    }while(Badnumber(result->data.price));
}

void EditQuantity(node* result){
    int counter = 0;
    do{
        if(counter>0){
            printf("Invalid quantity!!!\nQuantity should be a positive number\n");
        }
        printf("Enter the new quantity: ");
        scanf("%d",&result->data.quantity);
        counter++;
    }while(Badnumber(result->data.quantity));
}


void DeleteMed(node** result , int N , node** array , char name){
    node* temp = result[N];
    int i = name - 'A';
    // we check if the node we want to remove is in the start of the search array 
    // if true we directly delete it from the original array
    if(N==0){
        array[i] = result[N]->next;
    }else{
        result[N-1]->next = temp->next;
    }
    
    //we free both the deleted node and the search array because we aren't going to use them again
    free(temp);
    free(result);
}
