#include <iostream>
#include <ncurses.h>
#include <string>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

using namespace std;

#define ENTER 10                    //ASCII codfication for the key 'enter'
#define ESC 27                      //ASCII codfication for the key 'esc'
#define ESC_VALUE -1                //value returned if the key 'esc' is pressed

#define MENU_ST_HEIGHT 3            //height of the first menu level

typedef struct {                    //struct used to contain datas from the configuration file
    int lev,retCode;
    string name,higherLvName;
} data_t;

class menu_t{                       //single item of any menu level

private:

    class menu_t* nextIt;           //pointer to the next item of the same level
    class menu_t* prevIt;           //pointer to the previous item of the same level
    class menu_t* higherLvIt;       //pointer to the item of the higher level
    class menu_t* lowerLvIt;        //pointer to the first item of the lower level

    int lev;                        //menu level of the item
    int retCode;                    //return code of the element. 0 if it has at least a lower level item related
    string name;                    //item name shown on the display

public:
    menu_t(int lev0,int retCode0,string name0,class menu_t* nextIt0,class menu_t* prevIt0,class menu_t* higherLvIt0,class menu_t* lowerLvIt0);

    void addNewItemSmLv(int retCode0,string name0);               //add new item on the same menu level
    void addNewItemLwLv(int lev0, int retCode0,string name0);     //add new item on the lower menu level

    //get functions
    class menu_t* getNextIt();
    class menu_t* getPrevIt();
    class menu_t* getHigherLvIt();
    class menu_t* getLowerLvIt();
    int getLev();
    int getRetCode();
    string getName();

};

typedef menu_t* menuPtr_t;          //pointer to an object of the class menu

class generator_t{                  //class which generate the tree that contains every menu item
private:

    menuPtr_t firstIt;              //pointer which contains the address of the first menu element

public:
    generator_t();

    void create();                          /*create all the menu item reading with the data contained in the configuration file.
                                            It returns a pointer to the first element*/
    void read(vector <data_t> &readIt);
    data_t newIt(string input);             //obtain data about the menu item from the string "input" and return a formatted struct
    void sort(vector <data_t> &readIt);
    void tree(vector <data_t> &readIt, menuPtr_t prevEl);

    menuPtr_t getFirstIt();
};

typedef generator_t *generatorPtr_t;

class handler_t{                    //class which allows the user to handle and create the menu
private:
    menuPtr_t currentIt;            //pointer to the selected menu item
    generatorPtr_t Menu;            //pointer to the class generator_t which creates the menu (it is realized with a tree)
public:
    handler_t();

    void printMenuFirstLv();                                                            //print the first menu level items
    void printMenuLowerLv(int width, menuPtr_t firstLvIt, int startX);      //print menu level items under the first

};

//********************menu_t methods********************

menu_t::menu_t(int lev0,int retCode0,string name0, class menu_t* nextIt0,class menu_t* prevIt0,class menu_t* higherLvIt0,class menu_t* lowerLvIt0){

    lev=lev0;
    retCode=retCode0;
    name=name0;
    nextIt=nextIt0;
    prevIt=prevIt0;
    higherLvIt=higherLvIt0;
    lowerLvIt=lowerLvIt0;
}

void menu_t::addNewItemSmLv(int retCode0,string name0){
    nextIt= new menu_t(lev,retCode0,name0,NULL,this,higherLvIt,NULL);
}

void menu_t::addNewItemLwLv(int lev0, int retCode0,string name0){
    lowerLvIt=new menu_t(lev0,retCode0,name0,NULL,NULL,this,NULL);
}

class menu_t* menu_t::getNextIt(){
    return nextIt;
}

class menu_t* menu_t::getPrevIt(){
    return prevIt;
}

class menu_t* menu_t::getHigherLvIt(){
    return higherLvIt;
}

class menu_t* menu_t::getLowerLvIt(){
    return lowerLvIt;
}

int menu_t::getLev(){
    return lev;
}

int menu_t::getRetCode(){
    return retCode;
}

string menu_t::getName(){
    return name;
}

//*******************************************************

//********************generator_t methods********************
generator_t::generator_t(){
    firstIt=NULL;
}

void generator_t::create(){

    vector <data_t> readIt;         //list used to contain the data read in the configuration file
    
    read(readIt);                   //read the configuration file and create a list with the read data
    
    firstIt=new menu_t(readIt[0].lev,readIt[0].retCode,readIt[0].name,NULL,NULL,NULL,NULL);
    readIt.erase(readIt.begin());
    
    tree(readIt,firstIt);           //creates the tree which contains every item of the menu
}

void generator_t::read(vector <data_t> &readIt){
    char str[30];               //string used containt temporarily every line of the configuration
    FILE* fpConf;               //file pointer used to read the configuration file       
    string path="/home/";       //path contains the absolute path of the configuration file
    path+=getenv("USER");
    path+="/.menu.conf"; 

    fpConf=fopen(path.c_str(),"r");
    
    if(fpConf!=NULL){
            while((fscanf(fpConf,"%s",str))>0)
                readIt.push_back(newIt(str));

            sort(readIt);       //sort the vector passed in the suitable way to create the tree for the menu
    }else{
        cout << "error: configuration file not found!" << endl;
        exit(-1);
    }
    
    fclose(fpConf);
}

data_t generator_t::newIt(string input){
    vector <string> split;          //temporary vector of string to contain data from the string "input"
    data_t temp;
    
    boost::split(split, input, boost::is_any_of(";"));  /*every string of split will contain a piece of the string input 
                                                        (it is separated by the char ';')*/

    temp.lev=stoi(split[0]);
    temp.name=split[1];
    temp.higherLvName=split[2];
    if(split.size()==4)                 //assign a return code only if the menu item has one in the configuration file
        temp.retCode=stoi(split[3]);
    
    return temp;
}

void generator_t::sort(vector <data_t> &readIt){
    int i,ord;
    data_t temp;
    
    //the first item put in the file will be the first element of the menu
    for(i=2; i<readIt.size(); i++) {        //scan all the list readIt
    temp=readIt[i];                         //element which will be ordered
    ord=i-1;

    //find the right position of the element only if the index ord(used to scan the ordered part of the list) is > 0 and 
    //the element doesn't belong to the first menu level
    while( (readIt[ord].name!=temp.higherLvName) && (ord>0) && (temp.higherLvName!="NULL")) {
        readIt[ord+1]=readIt[ord];
      ord--;
    }
    
    readIt[ord+1]=temp;

  }
}

void generator_t::tree(vector <data_t> &readIt,menuPtr_t prevEl){//prevEl: previous element in the menu
    int next;       //will contain the next element level if exists to control the recursion
    
    do{
        if(!readIt.empty()){    //true if the list is empty

            if(prevEl->getLev()==readIt[0].lev){        //if the new element belongs to the same menu level

                prevEl->addNewItemSmLv(readIt[0].retCode,readIt[0].name);
                readIt.erase(readIt.begin());
                tree(readIt,prevEl->getNextIt());
                
            }else if(prevEl->getLev()+1==readIt[0].lev){ //if the new element belongs to the lower menu level
                
                prevEl->addNewItemLwLv(readIt[0].lev,readIt[0].retCode,readIt[0].name);
                readIt.erase(readIt.begin());
                tree(readIt,prevEl->getLowerLvIt());
                
            }
        }
        
    if(!readIt.empty())  //if exists a following element, it's saved in next its level
        next=readIt[0].lev;
    //the statement checks that the following item is on the same level of the current one
    //if it's not, the following element doensn't exist or belongs to an higher menu level
    }while((!readIt.empty()) && (next==prevEl->getLev())); 
}

menuPtr_t generator_t::getFirstIt(){
    return firstIt;
}
//*******************************************************

//********************handler_t methods********************

handler_t::handler_t(){
    Menu=new generator_t();
    Menu->create();
    currentIt=Menu->getFirstIt(); //contains the first element of the menu
}

void handler_t::printMenuFirstLv(){
    int stdscrHeight,stdscrWidth,lengthPrev, input=0;   /*input: will contain keyboard input; 
                                                        lengthPrev: length of the previously printed string*/
    menuPtr_t iterator;                                 //iterator used in order to scan all the menu level item
    
    initscr();                                          //setup memory and clears the screen
    
    noecho();
    getmaxyx(stdscr,stdscrHeight,stdscrWidth);          //gets the dimension of the screen to create a window of the right dimension
    WINDOW* menuWin=newwin(MENU_ST_HEIGHT, stdscrWidth, 0 , 0);     //window used to display the first level of the menu
    box(menuWin,0,0);
    refresh();
    wrefresh(menuWin);
    
    keypad(menuWin,TRUE);
    
    while(1){
        iterator=Menu->getFirstIt();
        lengthPrev=1;
        while(iterator!=NULL){         //print all the first menu level items
           if(iterator==currentIt)     //controls if the iterator is pointing the selected menu element
                    wattron(menuWin,A_REVERSE);
           mvwprintw(menuWin,1,lengthPrev,iterator->getName().c_str());
           wattroff(menuWin,A_REVERSE);
           lengthPrev+=iterator->getName().length()+1;
           iterator=iterator->getNextIt();
        }
        wrefresh(menuWin);
        
        input=wgetch(menuWin);
        
        switch(input){
            case KEY_DOWN:  //go to the lower menu level if it exists
                if(currentIt->getLowerLvIt()!=NULL){
                    currentIt=currentIt->getLowerLvIt();
                    printMenuLowerLv(stdscrWidth/2, currentIt,0);
                }
                break;
            case KEY_LEFT:  //go to the previous menu element if it exists
                if(currentIt->getPrevIt()!=NULL)
                    currentIt=currentIt->getPrevIt();
                break;
            case KEY_RIGHT: //go to the following menu element if it exists
                if(currentIt->getNextIt()!=NULL)
                    currentIt=currentIt->getNextIt();
                break;
            case ENTER:     //quit from the program if the element hasn't a lower element with the return code of the item
                if(currentIt->getLowerLvIt()==NULL){
                    endwin();
                    exit(currentIt->getRetCode());
                }
                break;
            case ESC:       //quit from the program returning -1
                 endwin();
                 exit(ESC_VALUE);
        }
    }

    endwin();   //deallocate the memory and ends ncurses
}

void handler_t::printMenuLowerLv(int width, menuPtr_t firstLvIt, int startX){   //height:height of the box, width: width of the box, firstLvIt: first item of that level, x coordinates of the top left box corner 
    menuPtr_t iterator;                    //iterator to scan all the menu level item
    int input=0, prevItems,height=2;         //input: keyboard input, prevItems: elemtens already printed on the screen
    
    initscr();        //setup memory and clears the screen
    
    noecho();

    iterator=firstLvIt;
    while(iterator!=NULL){
        height++;
        iterator=iterator->getNextIt();
    }

    WINDOW* menuWin=newwin(height, width, MENU_ST_HEIGHT+1 , startX);     //window used to display the first level of the menu
    box(menuWin,0,0);
    refresh();
    wrefresh(menuWin);
    
    keypad(menuWin,TRUE);
    
    while(input!=KEY_LEFT){
        iterator=firstLvIt;
        prevItems=0;
        while(iterator!=NULL){          //print all the first menu level items
            if(iterator==currentIt)     //controls if the iterator is pointing the menu element selected
                    wattron(menuWin,A_REVERSE);
           mvwprintw(menuWin,1+prevItems,1,iterator->getName().c_str());
            wattroff(menuWin,A_REVERSE);
            prevItems+=1;
            iterator=iterator->getNextIt();
        }
        wrefresh(menuWin);
        
        input=wgetch(menuWin);
        
        switch(input){
            case  KEY_LEFT: //go to the higher menu level if it exists
                if(currentIt->getHigherLvIt()!=NULL){
                    currentIt=currentIt->getHigherLvIt();
                    wclear(menuWin);
                    wrefresh(menuWin);
                }
                break;
            case KEY_RIGHT: //go to the lower menu level if it exists
                if(currentIt->getLowerLvIt()!=NULL){
                    currentIt=currentIt->getLowerLvIt();
                    printMenuLowerLv(width, currentIt, width+startX);
                }
                break;
            case KEY_UP:    //go to the previous menu element if it exists
                if(currentIt->getPrevIt()!=NULL)
                    currentIt=currentIt->getPrevIt();
                break;
            case KEY_DOWN:  //go to the following menu element if it exists
                if(currentIt->getNextIt()!=NULL)
                    currentIt=currentIt->getNextIt();
                break;
            case ENTER:     //quit from the program if the element hasn't a lower element with the return code of the item
                if(currentIt->getLowerLvIt()==NULL){
                    endwin();
                    exit(currentIt->getRetCode());
                }
                break;
            case ESC:       //quit from the program returning -1
                endwin();
                exit(ESC_VALUE);
        }
    }
    endwin();   //deallocate the memory and ends ncurses
}
//*******************************************************

int main(){
    handler_t handler;
    
    handler.printMenuFirstLv();
}
