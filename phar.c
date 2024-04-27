#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include <ncurses/ncurses.h>

#define ctrl(x) (x & 0x1f)
#define FULL ((void* ) 1)


typedef struct date{
    int day;
    int month;
    int year;
}date;

#include "dates.c"


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
void write_list(node* head,FILE* file);

// input/output functions
void readmed(WINDOW* inputwin,WINDOW* statuswin,med* m);
void fill(node** array,med m);
bool Badnumber(int N);

void print_inputInterface(WINDOW* inputWin, int med_num, int cur);
node* create_menu(WINDOW* menuwin, node* options, bool isSingleton, node** arr);
void reset_searchmenu(WINDOW* menuwin);

// string functions
void Textform(char* N);
bool BadName(char N);

//date functions
bool expiring(med m);
bool BadDate(date d , bool manf);
bool Manfucdate(date manf,date exp);
bool verifyfilename(char* name);

//search functions
node* SearchMedicine(node** array, char* name);

//edit functions
void EditPrice(node* m, WINDOW* inputwin);
void EditQuantity(node* m, WINDOW* inputwin);
void DeleteMed(node* med ,node** array);

//Command Handlers
void handle_create(node** array);
void handle_load(node** arr);
void handle_save(node** arr);
node* load_mainInterface(node** arr);
void edit_med(node* med, node** arr);
void freeList(node* head);

int main(){
    
    /* NCURSES START */
    initscr();
    cbreak();
    start_color();


    //Get Screen size
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);

    int num_save = 0;
    FILE* list = fopen("C:\\Users\\iman5\\Projects Folder\\Phar\\saves\\pharma_list.txt","r");
    fscanf(list,"%d",&num_save);

    WINDOW* menuwin = newwin(5,max_x-98,3,43);
    box(menuwin,0,0);
    refresh();
    wrefresh(menuwin);
    keypad(menuwin,true);

    int choice;
    char* choices[3] = {"Create New Stock","Load Stock","Exit"};
    int highlighted = 0;

    init_pair(1,COLOR_MAGENTA,COLOR_BLACK);

    attron(A_BOLD);
    mvprintw(1,max_x/2-17,"./ Pharma Manager");
    attroff(A_BOLD);

    node** array = init();


    refresh();
    while(1) {

        for (int i = 0; i<3; i++) {
            if (i == highlighted) {
                wattron(menuwin,A_BOLD);
                wattron(menuwin,COLOR_PAIR(1));
            }
            mvwprintw(menuwin,i+1,1,"%d - %s",i+1,choices[i]);
            wattroff(menuwin,COLOR_PAIR(1));
            wattroff(menuwin,A_BOLD);
        }

        choice = wgetch(menuwin);

        switch (choice) { // selction
            case KEY_UP:
            highlighted = (highlighted - 1) % 3;
            if (highlighted < 0) highlighted = 2;
            break;

            case KEY_DOWN:
            highlighted = (highlighted + 1) % 3;
            break;

            default:
            break;
        }

        if (choice == 10) { // handle enter
            break;
        }
    }

    switch(highlighted) {

        case 0:
            delwin(menuwin);
            clear();
            handle_create(array);
        break;
        case 1:
            handle_load(array);
        break;
        case 2:
            return 0;
        break;

    }

    bool isRunning = true;
    node* search_result;
    while (isRunning) {
        search_result = load_mainInterface(array);

        if (search_result != NULL && search_result != FULL) {
            edit_med(search_result,array);
        } else if (search_result == NULL) {
            isRunning = false;
        }

    }

    handle_save(array);


        
    /*NCURSES END*/
    getch();
    endwin();
    for (int i=0; i<26;i++) {
        freeList(array[i]);
    }
    free(array);


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

void freeList(node* head) {
    node* current = head;
    node* next;

    while (current != NULL) {
        next = current->next; 
        free(current);       
        current = next;       
    }
}

void readmed(WINDOW* inputwin,WINDOW* statuswin,med* m){
    int counter =0;

    wclear(statuswin);
    box(statuswin,0,0);

    init_pair(2,COLOR_RED,COLOR_BLACK); // ERROR MESSAGE COLOUR
    init_pair(3,COLOR_GREEN,COLOR_BLACK); // SUCCESS MESSAGE COLOUR


    // name
    do {

        wclear(statuswin);
        box(statuswin,0,0);

        mvwscanw(inputwin,5,7,"%[^\n]s",m->name);
        Textform(m->name);
 
        
        mvwprintw(inputwin,5,7,"%s",m->name);

        if(BadName(m->name[0])) {

            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"BadName: %s must start with a letter",m->name);
            wattroff(statuswin,COLOR_PAIR(2));

            mvwprintw(inputwin,5,7,"                  ");
        }

        wrefresh(inputwin);
        wrefresh(statuswin);

    } while(BadName(m->name[0]));

    // brand
    {
    mvwscanw(inputwin,6,8,"%[^\n]s",m->brand);

    mvwprintw(inputwin,6,8,"                 ");
    mvwprintw(inputwin,6,8,"%s",m->brand);
    }

    // quantity
    do {
        
        wclear(statuswin);
        box(statuswin,0,0);

        mvwscanw(inputwin,5,37,"%d",&m->quantity);
 

        mvwprintw(inputwin,5,37,"%d",m->quantity);

        if(Badnumber(m->quantity)) {

            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"Bad Quantity: %d (quantity) must be a positive number",m->quantity);
            wattroff(statuswin,COLOR_PAIR(2));
            
            mvwprintw(inputwin,5,37,"             ");
        }

        wrefresh(inputwin);
        wrefresh(statuswin);

    } while(Badnumber(m->quantity));

    //Price
    do {
        
        wclear(statuswin);
        box(statuswin,0,0);

        mvwscanw(inputwin,6,34,"%f",&m->price);
 
        
        mvwprintw(inputwin,6,34,"%.2f",m->price);

        if(Badnumber(m->price)) {

            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"Bad price: %.2f (price) must be a positive number",m->price);
            wattroff(statuswin,COLOR_PAIR(2));

            mvwprintw(inputwin,6,34,"             ");
        }

        wrefresh(inputwin);
        wrefresh(statuswin);

    }while(Badnumber(m->price));

    //Manuf
    do {

        wclear(statuswin);
        box(statuswin,0,0);

        mvwscanw(inputwin,5,55,"%d %d %d",&m->manf.day,&m->manf.month,&m->manf.year);
 

        mvwprintw(inputwin,5,55,"%02d/%02d/%04d",m->manf.day,m->manf.month,m->manf.year);

        if(BadDate(m->manf, true)) {

            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"Bad fabrication date: The fabrication date must be inputted in \"dd mm yyyy\" format");
            mvwprintw(statuswin,2,1,"and set to before today");
            wattroff(statuswin,COLOR_PAIR(2));

            mvwprintw(inputwin,5,55,"                   ");
        }

        wrefresh(inputwin);
        wrefresh(statuswin);

    }while(BadDate(m->manf,true));

    //Exp
    do {
        
        wclear(statuswin);
        box(statuswin,0,0);

        mvwscanw(inputwin,6,55,"%d %d %d",&m->exp.day,&m->exp.month,&m->exp.year);
 

        mvwprintw(inputwin,6,55,"%02d/%02d/%04d",m->exp.day,m->exp.month,m->exp.year);

        if(BadDate(m->exp, false)) {

            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"Bad expiration date: The expiration date must be inputted in \"dd mm yyyy\" format");
            mvwprintw(statuswin,2,1,"and set to after the current date");
            wattroff(statuswin,COLOR_PAIR(2));

            mvwprintw(inputwin,6,55,"                    ");
        }

        if (Manfucdate(m->manf,m->exp)) {
            
            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"Bad expiration date: The expiration date must be after the fabrication date");
            wattroff(statuswin,COLOR_PAIR(2));

            mvwprintw(inputwin,6,55,"                    ");
        }

        if (expiring(*m)) {
            wclear(statuswin);
            box(statuswin,0,0);

            wattron(statuswin,COLOR_PAIR(2));
            mvwprintw(statuswin,1,1,"Bad expiration date: you cannot add expired medicine!");
            wattroff(statuswin,COLOR_PAIR(2));

            mvwprintw(inputwin,6,55,"                    ");
        }

        wrefresh(inputwin);
        wrefresh(statuswin);

    }while(BadDate(m->exp,false) || Manfucdate(m->manf,m->exp) || expiring(*m));

    // added success
    {
        wclear(statuswin);
        box(statuswin,0,0);
        wattron(statuswin,COLOR_PAIR(3));
        mvwprintw(statuswin,1,1,"The medicine %s has been added successfully!",m->name);
        wattroff(statuswin,COLOR_PAIR(3));
    }


    wrefresh(inputwin);
    wrefresh(statuswin);
    getch();

}

void fill(node** array,med m) {
    int i = m.name[0] - 'A';
    if(expiring(m)){
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

void Textform(char* N){
    N[0] = toupper(N[0]);
    for(int i = 1;N[i]!='\0';i++){
        N[i] = tolower(N[i]);
    }

}



bool Badnumber(int N){
    if(N<0) return true;
    return false;
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



node* SearchMedicine(node** array,char* name){

    // send array of lists, return a list that contains either all med with that letter or a list of length = 1 of that 1 medicine

    // Get Offset of the character A:0, B:1 ...
    int i = toupper(name[0]) - 'A';
    node* result;

    if (i > 25 || i < 0) {
        return NULL;
    }

    if (strlen(name) == 1) { // case of 1 , return head to full list of nodes
        result = array[i];
        return result;

    } else { // case of full name search the letter list and return matches
        result = array[i];

        while(result->next != NULL) {
            if (strncmp(result->data.name,name,20) == 0) {
                return result;
            }

            result = result->next;
        }
    }   

    return NULL;
}

void EditPrice(node* m, WINDOW* inputwin){
    mvwprintw(inputwin,2,34,"             ");
    do {
        mvwscanw(inputwin,2,34,"%f",&m->data.price);
        mvwprintw(inputwin,2,34,"%.2f",m->data.price);
        if(Badnumber(m->data.price)) {
            mvwprintw(inputwin,2,34,"             ");
        }
        wrefresh(inputwin);
    }while(Badnumber(m->data.price));
}

void EditQuantity(node* m, WINDOW* inputwin){
    mvwprintw(inputwin,1,37,"             ");
    do {
        mvwscanw(inputwin,1,37,"%d",&m->data.quantity);
        mvwprintw(inputwin,1,37,"%d",m->data.quantity);
        if(Badnumber(m->data.quantity)) {
            mvwprintw(inputwin,1,37,"             ");
        }
        wrefresh(inputwin);
    } while(Badnumber(m->data.quantity));
}

void DeleteMed(node* med, node** arr){

    int i = toupper(med->data.name[0]) - 'A';
    node* tmp = arr[i];
    node* prev = NULL;

    if (tmp == NULL) {
        return;
    }

    if (tmp == med) {
        arr[i] = tmp->next;
        free(med);
        return;
    }

    while (tmp != NULL && tmp != med) {
        prev = tmp;
        tmp = tmp->next;
    }

    if (tmp == NULL) {
        return;
    }

    prev->next = tmp->next;
    free(med);
}

void handle_create(node** array) {
    clear();
    box(stdscr,0,0);
    int x_max = getmaxx(stdscr);
    move(1,x_max/2-14);
    attron(A_BOLD);
    printw("Create/add New Stock");
    attroff(A_BOLD);

    WINDOW* inputWin = newwin(10,x_max-16,10,8);
    box(inputWin,0,0);
    refresh();
    wrefresh(inputWin);

    WINDOW* statusWin = newwin(4,x_max-24,22,12);
    box(statusWin,0,0);
    refresh();
    wrefresh(statusWin);


    int med_num;

    // print interface
    print_inputInterface(inputWin,-1,0);

    mvwscanw(inputWin,1,22,"%d",&med_num);
    mvwprintw(inputWin,1,22,"%d",med_num);
    wrefresh(inputWin);

    med m;
    for (int i=0; i<med_num; i++) {

        print_inputInterface(inputWin,med_num,i+1);

        readmed(inputWin,statusWin,&m);

        wclear(statusWin);
        box(statusWin,0,0);
        wrefresh(statusWin);

        fill(array,m);

        wrefresh(statusWin);
        wrefresh(inputWin);

    }


    delwin(inputWin);
    delwin(statusWin);
}

void print_inputInterface(WINDOW* inputWin, int med_num, int cur) {
    
    wclear(inputWin);
    box(inputWin,0,0);


    if (med_num > -1) {
        mvwprintw(inputWin,1,1,"Number of medicines: %d",med_num);
        mvwprintw(inputWin,3,1,"%d/%d:",cur,med_num);
    } else {
        mvwprintw(inputWin,1,1,"Number of medicines: ");
        mvwprintw(inputWin,3,1,"0/?:");
    }
    
    
    mvwprintw(inputWin,5,1,"Name: ");
    mvwprintw(inputWin,6,1,"Brand: ");
    mvwprintw(inputWin,5,25,"| Quantity: ");
    mvwprintw(inputWin,6,25,"| Price: ");
    mvwprintw(inputWin,5,50,"| F: ");
    mvwprintw(inputWin,6,50,"| E: ");
    wrefresh(inputWin);
}

void handle_load(node** arr) {
    clear();
    box(stdscr,0,0);
    int x_max = getmaxx(stdscr);
    move(1,x_max/2-14);
    attron(A_BOLD);
    printw("Load a file");
    attroff(A_BOLD);

    FILE* list = fopen("C:\\Users\\iman5\\Projects Folder\\Phar\\saves\\pharma_list.txt","r");


    int n=0;
    char paths[16][64];
    while(fgetc(list) != EOF) {
        fscanf(list,"%[^\n]s",paths[n]);
        n++;
    }



    WINDOW* menuwin = newwin(20,30,3,43);
    box(menuwin,0,0);
    refresh();
    wrefresh(menuwin);
    keypad(menuwin,true);

    int choice;
    int highlighted = 0;
    refresh();
    while(1) {

        for (int i = 0; i<n-1; i++) {
            if (i == highlighted) {
                wattron(menuwin,A_BOLD);
                wattron(menuwin,COLOR_PAIR(1));
            }
            if (paths[i] != NULL) mvwprintw(menuwin,i+1,1,"%d - %s",i+1,paths[i]);

            wattroff(menuwin,COLOR_PAIR(1));
            wattroff(menuwin,A_BOLD);
        }

        choice = wgetch(menuwin);

        switch (choice) { // selction
            case KEY_UP:
            highlighted = (highlighted - 1) % n;
            if (highlighted < 0) highlighted = n-1;
            break;

            case KEY_DOWN:
            highlighted = (highlighted + 1) % n;
            break;

            default:
            break;
        }

        if (choice == 10) { // handle enter
            break;
        }
    }

    FILE* file = fopen(paths[highlighted],"rb");
    node m;
    med medi;

    while(fread(&m,sizeof(node),1,file) != 0 ) {
        medi = m.data;
        fill(arr,medi);
    }
}

void handle_save(node** arr) {
    clear();
    box(stdscr,0,0);
    int x_max = getmaxx(stdscr);
    move(1,x_max/2-14);
    attron(A_BOLD);
    printw("Save to a file");
    attroff(A_BOLD);

    WINDOW* inputWin = newwin(10,x_max-16,10,8);
    box(inputWin,0,0);
    refresh();

    mvwprintw(inputWin,0,2,"Enter the name of the file (without an extension)");
    wrefresh(inputWin);

    char path[32] = {'\0'};
    mvwscanw(inputWin,1,1,"%s",path);

    wrefresh(inputWin);
    FILE * new_save = fopen(path,"wb");

    //save the file for the loading
    bool exists = verifyfilename(path);

    if (!exists) {
        FILE * save_list = fopen("saves/pharma_list.txt","a");
        fprintf(save_list,"%s\n",path);
        fclose(save_list);
    } 


    for (int i=0; i<26; i++) {
        write_list(arr[i],new_save);
    }


    fclose(new_save);
    delwin(inputWin);

}

bool verifyfilename(char* name) {
    char path[64] = {'\0'};

    FILE* list = fopen("saves/pharma_list.txt","r");


    while (fgetc(list) != EOF) {
        fscanf(list,"%[^\n]",path);

        if (strcmp(path,name) == 0) {
            return true;
        }

    }


    fclose(list);
    return false;
}

node* load_mainInterface(node** arr) {

    clear();
        
    box(stdscr,0,0);
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);

    mvwprintw(stdscr,1,(max_x/2)-13,"Search/Edit Medicine");


    WINDOW* inputwin = newwin(5,max_x-24,2,12);
    box(inputwin,0,0);

    WINDOW* resultwin = newwin(max_y-10,max_x-24,8,12);
    box(resultwin,0,0);

    keypad(resultwin,true);

    refresh();
    wrefresh(resultwin);
    wrefresh(inputwin);

    mvwprintw(resultwin,0,3,"CTRL+q: quit");
    mvwprintw(resultwin,0,18,"CTRL+p: page-next");
    mvwprintw(resultwin,0,43,"CTRL+d: add medicine");
    wrefresh(resultwin);


    char name[32] = {'\0'};



    node* res = NULL;
    do {
        mvwscanw(inputwin,1,1,"%[^\n]s",name);
        Textform(name);
        res = SearchMedicine(arr,name);
    } while (res == NULL);

    if (strlen(name) > 1) {
        res = create_menu(resultwin,res,true,arr);
    } else {
        res = create_menu(resultwin,res,false,arr);
    }



    wrefresh(resultwin);
    wrefresh(inputwin);

    delwin(resultwin);
    delwin(inputwin);

    return res;
}

void edit_med(node* med, node** arr) {
    clear();
        
    box(stdscr,0,0);
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);

    mvwprintw(stdscr,1,(max_x/2)-9,"Edit Medicine");


    WINDOW* medwin = newwin(6,max_x-16,5,8);
    box(medwin,0,0);

    WINDOW* menuwin = newwin(6,max_x-98,15,43);
    box(menuwin,0,0);
    keypad(menuwin,true);

    refresh();
    wrefresh(menuwin);
    wrefresh(medwin);

    mvwprintw(medwin,1,1,"Name: %s",med->data.name);
    mvwprintw(medwin,2,1,"Brand: %s",med->data.brand);
    mvwprintw(medwin,1,25,"| Quantity: %d",med->data.quantity);
    mvwprintw(medwin,2,25,"| Price: %f",med->data.price);

    wattron(medwin,COLOR_PAIR(3));
    wprintw(medwin,"$");
    wattroff(medwin,COLOR_PAIR(3));


    mvwprintw(medwin,1,50,"| F: %02d/%02d/%04d",med->data.manf.day,med->data.manf.month,med->data.manf.year);
    mvwprintw(medwin,2,50,"| E: %02d/%02d/%04d (%d d left)",med->data.exp.day,med->data.exp.month,med->data.exp.year,daysLeftToDate(med->data.exp));

    wrefresh(menuwin);
    wrefresh(medwin);

    //menu
    int choice;
    char* choices[4] = {"Edit Quantity","Edit price","Delete med","Exit"};
    int highlighted = 0;
    while(1) {
        for (int i = 0; i<4; i++) {
            if (i == highlighted) {
                wattron(menuwin,A_BOLD);
                wattron(menuwin,COLOR_PAIR(1));
            }
            mvwprintw(menuwin,i+1,1,"%d - %s",i+1,choices[i]);
            wattroff(menuwin,COLOR_PAIR(1));
            wattroff(menuwin,A_BOLD);
        }

        choice = wgetch(menuwin);

        switch (choice) { // selction
            case KEY_UP:
            highlighted = (highlighted - 1) % 4;
            if (highlighted < 0) highlighted = 3;
            break;

            case KEY_DOWN:
            highlighted = (highlighted + 1) % 4;
            break;

            default:
            break;
        }

        if (choice == 10) { // handle enter
            break;        }
    }

    switch(highlighted) {

        case 0:
            EditQuantity(med,medwin);
        break;

        case 1:
            EditPrice(med,medwin);
        break;

        case 2:
            DeleteMed(med,arr);
        break;

        case 3:
            return;
        break;

    }

    delwin(menuwin);
    delwin(medwin);
}

node* create_menu(WINDOW* menuwin, node* options, bool isSingleton, node** arr) {
    int highlighted = 0;
    int i = 0;
    node* cur = options;
    while(1) {
        reset_searchmenu(menuwin);
        cur = options;
        i = 0;

        if (!isSingleton) {

            while(cur != NULL && i < 18) {

                if (i == highlighted) {
                    wattron(menuwin,A_BOLD);
                    wattron(menuwin,COLOR_PAIR(1));
                }

                mvwprintw(menuwin,i+1,1,"%d - %s",i+1,cur->data.name);

                wattroff(menuwin,COLOR_PAIR(1));
                wattroff(menuwin,A_BOLD);

                cur = cur->next;
                i++;
            }

        } else {

            if (i == highlighted) {
                wattron(menuwin,A_BOLD);
                wattron(menuwin,COLOR_PAIR(1));
            }

            mvwprintw(menuwin,i+1,1,"%d - %s",i+1,cur->data.name);

            wattroff(menuwin,COLOR_PAIR(1));
            wattroff(menuwin,A_BOLD);

            i++;
        }


        int choice = wgetch(menuwin);

        switch (choice) { // selction
            case KEY_UP:
            highlighted = (highlighted - 1) % i;
            if (highlighted < 0) highlighted = i-1;
            break;

            case KEY_DOWN:
            highlighted = (highlighted + 1) % i;
            break;

            case ctrl('q'):
            return NULL;
            break;

            case ctrl('p'):
            if (i >= 18 && cur != NULL) {
                options = cur;
            }
            break;

            case ctrl('d'):
                handle_create(arr);
                return FULL;
            break;

            default:
            break;
        }

        if (choice == 10) { // handle enter
            break;
        }
        
        wrefresh(menuwin);
    }

    cur = options;
    for (int i=0; i<highlighted && cur->next != NULL; i++) {
        cur = cur->next;
    }

    wrefresh(menuwin);
    return cur;
}

void reset_searchmenu(WINDOW* menuwin) {
    wclear(menuwin);
    box(menuwin,0,0);
    mvwprintw(menuwin,0,3,"CTRL+q: quit");
    mvwprintw(menuwin,0,18,"CTRL+p: page-next");
    mvwprintw(menuwin,0,43,"CTRL+d: add medicine");
    wrefresh(menuwin);
}

void write_list(node* head,FILE* file) {
    node* tmp = head;
    if (tmp == NULL) {
        return;
    }

    while(tmp != NULL) {
        if (fwrite(tmp,sizeof(node),1,file) != 0) {
            tmp = tmp->next;
        } else {
            return;
        }
    }
}