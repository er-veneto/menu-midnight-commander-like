The program can handle a menu on not more than 3 levels

in order to compile the program execute (in the folder where the main is saved) the command: g++ main.cpp -o main.out -lncurses

required libraries:

1. iostream
2. ncurses.h
3. string
4. bits
5. boost

***************CREATING THE CONFIGURATION FILE***************
the program configuration file has to be created in ~/ and has to be called .menu.conf
this file contains all the data of every menu item in this order:

*number of the menu level*;*name of the item*;*higher menu level name*;*return code of the item*

**!!! BE CAREFUL !!!**
1. The return code can be omitted if the item has not got any submenu item
2. The first level menu item has NULL as higher menu level name
3. do NOT put any space in the name of the item
4. the program IS CASE SENSITIVE so be careful while you are writing the configuration file
5. do NOT leave any space before,between and after the lines of the configuration file
6. ALWAYS write a menu level BEFORE its sublevels (on the top al the element of the first level; then all the elements of the second
   level; then the elements of the third level). the elements will be sorted during the executio of the program

example:

Command
        User_Menu 
                    open_File   (return code: 10)
Item
        ItemSubMenu1
                        ItemSubMenu2    (return code: 4)
        ItemSubMenu3
                        ItemSubMenu4    (return code: 8)

// configuration file
1;Command;NULL
1;Item;NULL
2;ItemSubMenu1;Item
2;User_Menu;Command
2;ItemSubMenu3;Item
3;ItemSubMenu2;ItemSubMenu1;4
3;ItemSubMenu4;ItemSubMenu3;8
3;open_File;User_menu;10


***************HOW TO USE THE PROGRAM***************
use the arrow keys to move between the menu items

commands of the first menu level:
    1. lef arrow:       move to the previous menu item
    2. right arrow:     move to the next menu item
    3. down arrow:      show the lower level items of the selected menu element

commands of the second and the third menu level:

    1. up arrow:        move to the previous menu item
    2. down arrow:      move to the next menu item
    3. left arrow:      show the higher level items of the selected menu element
    4. right arrow:     show the lower level items of the selected menu element
[![](https://www.paypal.com/en_IT/i/scr/pixel.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=98D7V3XCTU9LS&source=url)
