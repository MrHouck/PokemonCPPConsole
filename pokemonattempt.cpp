#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <Windows.h>
#include <winuser.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

using namespace std;

struct Moveset{
    char type[20];
    char name[20];
};

struct Weakness
{
    char type[20];
};

struct Pokemon
{
    char name[20];
    int hp;
    int atk;
    int def;
    int level;
    int speed;
    int xpRequired;
    int currentXp;
    Moveset moves[4];
};

struct NPC
{
    string name;
    string roomLocation;
    char dialogue[7][4][60];
    int dialogueLines;
    int xPos;
    int yPos;
};

struct Options
{
    bool sound;

};
struct Player
{
    string name;
    char bag[10][20]; //max 10 items
    Pokemon pokemon[6]; //max 6 pokemon

};

/*
Fast = (4n^3)/5
Medium Fast = n^3
Medium Slow = (6/5)n^3 - 15n^2 + 100n - 140
Slow = (5n^3)/4
*/


//FUNCTION PROTOTYPES
//Most of these can be void, since we want to have global variables for most of this project
void CalculateMovement(void);
void interact(char);
void Dialogue(NPC);
void createNPCs(void);
void DrawMap(char [14][70]);
void ShowMenu(void);
void MenuUp(void);
void MenuDown(void);
void MenuInteract(void);
void GetMap(void);
void changeRoom(void);
void getNextRoom(string &);
NPC getNpc(void);

//GLOBAL VARIABLES
bool game = true;
bool npcClose = false, canMove = true, doDialogue = false, dialogueDone = false, drawMenu = false, menuFirstTime = false, devMode = false;
bool talkedToOak = false;
int x=1, y=1, currentRoomNumber = 0, selectedMenuIndex = 0;
string currentRoom = "begin";
string nextRoom = "";
string prevRoom = "begin";
char map[4][14][70]=
{
    {"##########","#o       #","#        #","#    !   #","#        #","#        #","#   __   #","##########"},
    {"##################################################################","#                                 __                             #","#                                                                #","#                                                                #","#                                                                #","#                                                                #","#                                                                #","#       /-\\                             /-\\                      #","#       |_|                             |_|                      #","#        o                                                       #","#                                                                #","##################################################################"},
    {"####################################","#                                  #","#                           @@@    #","#                !          ┬─┬    #","#                                  #","###########              ###########","#                                  #","#                o                 #","#               ___                #","####################################"},
    {"############################","#                          #","#                        _ #","#                          #","#                          #","#                 ---------#","#                          #","#                        ! #","#---------                 #","#                          #","#                          #","#             o            #","############################"}
};
//MENUS
char menu[7][9][18] =
{
    {"+-----------+","| → Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","|   Options |","|   Debug   |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","| → Pokémon |","|   Bag     |","|   Save    |","|   Options |","|   Debug   |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","| → Bag     |","|   Save    |","|   Options |","|   Debug   |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","| → Save    |","|   Options |","|   Debug   |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","| → Options |","|   Debug   |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","|   Options |","| → Debug   |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","|   Options |","|   Debug   |","| → Exit    |","+-----------+" }
};

char saveMenu[2][5][40] =
{
    {
        "+---------------------------------+",
        "| Are you sure you want to save?  |",
        "|    Yes                          |",
        "|  → No                           |",
        "+---------------------------------+"
    },
    {
        "+---------------------------------+",
        "| Are you sure you want to save?  |",
        "|  → Yes                          |",
        "|    No                           |",
        "+---------------------------------+"
    }
};

char chooseStarterPokemonMenu[3][4][6][60] =
{
    //squirtle
    //charmander
    //bulbasaur
    {
        {
            "+-------------------------------------------+",
            "| Are you sure you want to choose Squirtle  |",
            "| as your starter pokémon?                  |",
            "|    Yes                                    |",
            "|  → No                                     |",
            "+-------------------------------------------+"
        },
        {
            "+-------------------------------------------+",
            "| Are you sure you want to choose Squirtle  |",
            "| as your starter pokémon?                  |",
            "|  → Yes                                    |",
            "|    No                                     |",
            "+-------------------------------------------+"
        }
    },
    {
        {
            "+-------------------------------------------+",
            "| Are you sure you want to choose Charmander|",
            "| as your starter pokémon?                  |",
            "|    Yes                                    |",
            "|  → No                                     |",
            "+-------------------------------------------+"
        },
        {
            "+-------------------------------------------+",
            "| Are you sure you want to choose Charmander|",
            "| as your starter pokémon?                  |",
            "|  → Yes                                    |",
            "|    No                                     |",
            "+-------------------------------------------+"
        }
    },
    {
        {
            "+-------------------------------------------+",
            "| Are you sure you want to choose Bulbasaur |",
            "| as your starter pokémon?                  |",
            "|    Yes                                    |",
            "|  → No                                     |",
            "+-------------------------------------------+"
        },
        {
            "+-------------------------------------------+",
            "| Are you sure you want to choose Bulbasaur |",
            "| as your starter pokémon?                  |",
            "|  → Yes                                    |",
            "|    No                                     |",
            "+-------------------------------------------+"
        }
    }
};


//NPC DEFINITION
NPC mom =
{
    "Mom",
    "begin",
    {
        {
            "--Mom------------------------------------",
            "|  Right. All boys leave home someday.  |",
            "|  It said so on TV.                    |",
            "-----------------------------------------"
        },
        {
            "--Mom------------------------------------",
            "|  Prof.Oak, next door, is looking for  |",
            "|  you                                  |",
            "-----------------------------------------"
        }
    },
    2,
    6,
    3
};

NPC profOak =
{
    "Prof. Oak",
    "prof-oak-house",
    {
        {
            "--Prof. Oak------------------------------",
            "| Hello there! Welcome to the world of  |",
            "| Pokémon! My name is Oak!              |",
            "-----------------------------------------"
        },
        {
            "--Prof. Oak------------------------------",
            "| People call me the pokémon Prof! This |",
            "| world is inhabited by creatures...    |",
            "-----------------------------------------"
        },
        {
            "--Prof. Oak------------------------------",
            "| called pokémon! For some people,      |",
            "| Pokémon are pets. Others use them...  |",
            "-----------------------------------------"
        },
        {
            "--Prof. Oak------------------------------",
            "| for fights. Myself...I study pokémon  |",
            "| as a profession.                      |",
            "-----------------------------------------"
        }
    },
    4,
    5,
    10
};

//NPC trainerLucas
//{

//}
int main()
{
    PlaySound("mom-theme.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    system("chcp 65001  > nul");
    Pokemon bulbasaur = {"Bulbasaur", 45, 49, 49, 1, 45, 64, 0};
    NPC currentNPC;
    while(game)
    {
        GetMap();
        getNextRoom(nextRoom);
        system("cls");
        DrawMap(map[currentRoomNumber]);
        if(drawMenu)
        {
            ShowMenu();
        }
        if(npcClose)
        {
            cout << "Press Z to interact." << endl;
        }
        else{
            if(doDialogue)
            {
                doDialogue = false;
                dialogueDone = false;
            }
        }
        if(doDialogue)
        {
            currentNPC = getNpc();
            Dialogue(currentNPC);
            if(devMode)
            {
                cout << endl << "Debug stats" << endl << "----------------------------";
                cout << endl << "Current Room: " << currentRoom;
                cout << endl << "Room Number: " << currentRoomNumber;
                cout << endl << "Previous room: " << prevRoom;
                cout << endl << "Next room: " << nextRoom;
                cout << endl << "NPC Close: " << npcClose;
                cout << endl << "Talked to Prof. Oak: " << talkedToOak;
                cout << endl << "Draw Menu: " << drawMenu;
                cout << endl << "Coords: (" << x << ','<< y << ')';
            }
        }
        else if(devMode)
        {
            if(!drawMenu)
            {
                cout << endl << "Debug stats" << endl << "----------------------------";
                cout << endl << "Current Room: " << currentRoom;
                cout << endl << "Room Number: " << currentRoomNumber;
                cout << endl << "Previous room: " << prevRoom;
                cout << endl << "Next room: " << nextRoom;
                cout << endl << "NPC Close: " << npcClose;
                cout << endl << "Talked to Prof. Oak: " << talkedToOak;
                cout << endl << "Draw Menu: " << drawMenu;
                cout << endl << "Coords: (" << x << ','<< y << ')';
            }
        }
        system("pause>nul");
        if(GetAsyncKeyState(0x58))
        {
            if(!canMove)
            {
                drawMenu = false;
                menuFirstTime = true;
                canMove = true;
                selectedMenuIndex = 0;
            }
            else
            {
                drawMenu = true;
                menuFirstTime = false;
                canMove = false;
                selectedMenuIndex = 0;
            }
        }
        if(canMove)
        {
            CalculateMovement();
        }
    }
}


void GetMap()
{
    if(currentRoom == "begin")
        currentRoomNumber = 0;
    else if(currentRoom == "outside-begin")
        currentRoomNumber = 1;
    else if(currentRoom == "prof-oak-house")
        currentRoomNumber = 2;
    else if(currentRoom == "route-328")
        currentRoomNumber = 3;
}
void DrawMap(char room[14][70])
{
    for(int i = 0; i < 14; i++)
    {
        cout << room[i] << endl;
    }
}

void ShowMenu()
{
    if(GetAsyncKeyState(VK_UP))
    {
        mciSendString("play Choose.wav", NULL, 0, NULL);
        MenuUp();
    }

    else if(GetAsyncKeyState(VK_DOWN))
    {
        mciSendString("play Choose.wav", NULL, 0, NULL);

        MenuDown();
    }

    else if(GetAsyncKeyState(0x5A))
    {
        mciSendString("play Select.wav", NULL, 0, NULL);

        MenuInteract();
    }
    else
    {
        for(int i = 0; i < 9; i++)
            cout << menu[selectedMenuIndex][i] << endl;
    }
    Sleep(150);
}

void MenuDown()
{
    if(selectedMenuIndex+1 > 6)
        selectedMenuIndex = 0;
    else
        selectedMenuIndex++;
    for(int i = 0; i < 9; i++)
    {
        cout << menu[selectedMenuIndex][i] << endl;
    }
}
void MenuUp()
{
    if(selectedMenuIndex-1 < 0)
        selectedMenuIndex = 6;
    else
        selectedMenuIndex--;
    for(int i = 0; i < 9; i++)
    {
        cout << menu[selectedMenuIndex][i] << endl;
    }
}

void MenuInteract()
{
    int currentSelection = 0;
    bool sequenceFinished = false;
    switch(selectedMenuIndex)
    {
    case 0:
        //POKÉDEX
        cout << "pokedex" << endl;
        break;
    case 1:
        //POKÉMON
        cout << "pokemon" << endl;
        break;
    case 2:
        //BAG
        cout << "bag" << endl;
        break;
    case 3:
        //SAVE
        drawMenu=false;
        selectedMenuIndex = 0;
        for(int i = 0; i <= 4; i++)
            cout << saveMenu[currentSelection][i] << endl;
        while(!sequenceFinished)
        {
            system("cls");
            DrawMap(map[currentRoomNumber]);
            if(GetAsyncKeyState(VK_UP))
            {
                Beep(2500, 15);

                if(currentSelection == 0)
                    currentSelection = 1;
                else
                    currentSelection--;
                for(int i = 0; i <= 4; i++)
                {
                    cout << saveMenu[currentSelection][i] << endl;
                }
            }
            else if(GetAsyncKeyState(VK_DOWN))
            {
                Beep(2500, 15);

                if(currentSelection == 1)
                    currentSelection = 0;
                else
                    currentSelection++;
                for(int i = 0; i <= 4; i++)
                {
                    cout << saveMenu[currentSelection][i] << endl;
                }
            }
            else if(GetAsyncKeyState(0x5A))
            {
                Beep(2500, 15);

                if(currentSelection == 0)
                    //don't save
                    cout << "don't save" << endl;
                else if(currentSelection == 1)
                {
                    //save
                    cout << "save" << endl;
                    sequenceFinished = true;
                }
                else
                    cout << "error" << endl;
            }
            else
            {
                for(int i = 0; i <= 4; i++)
                    cout << saveMenu[currentSelection][i] << endl;
            }
            Sleep(150);
        }
        break;
    case 4:
        //OPTIONS
        cout << "options" << endl;
        break;
    case 5:
        //DEBUG
        devMode = !devMode;
        break;
    case 6:
        //EXIT
        exit(0); //same as return 0, but we can't do that since we are in a void function
        break;
    }
    selectedMenuIndex = 0;
    drawMenu = false;
    canMove = true;

}
void changeRoom()
{
    switch(currentRoomNumber)
    {
    case 0:
        if(prevRoom == "outside-begin")
            x=y=5;
        else
            x=y=1;
        break;
    case 1:
        if(prevRoom == "prof-oak-house")
        {
            x=41;
            y=9;
        }
        else
            x=y=9;
        break;
    case 2:
        x=17;
        y=7;
        break;
    case 3:
        x=14;
        y=11;
        break;
    default:
        break;
    }
    if(currentRoom == "begin")
    {
        PlaySound(NULL, 0, 0);
        PlaySound("mom-theme.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    }
    else if(currentRoom == "outside-begin")
    {
        PlaySound(NULL, 0,0);
        PlaySound("palette-town.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    }
    else if(currentRoom == "prof-oak-house")
    {
        PlaySound(NULL, 0,0);
        PlaySound("oak-house.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    }
    else if(currentRoom == "route-328")
    {
        PlaySound(NULL, 0, 0);
        PlaySound("road-from-palette.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    }
}

void CalculateMovement()
{
    if(GetAsyncKeyState(VK_UP))
    {
        int y2 = y-1;
        if(map[currentRoomNumber][y2][x] == ' ')
        {
            map[currentRoomNumber][y][x] = ' ';
            y--;
            map[currentRoomNumber][y][x] = 'o';
        }
        else if(map[currentRoomNumber][y2][x] == '_')
        {
            map[currentRoomNumber][y][x] = ' ';
            prevRoom = currentRoom;
            currentRoom = nextRoom;
            GetMap();
            changeRoom();
        }
    }
    if(GetAsyncKeyState(VK_DOWN))
    {
        int y2 = y+1;
        if(map[currentRoomNumber][y2][x] == ' ')
        {
            map[currentRoomNumber][y][x] = ' ';
            y++;
            map[currentRoomNumber][y][x] = 'o';
        }
        else if(map[currentRoomNumber][y2][x] == '_')
        {
            map[currentRoomNumber][y][x] = ' ';
            prevRoom = currentRoom;
            currentRoom = nextRoom;
            GetMap();
            changeRoom();
        }
    }
    if(GetAsyncKeyState(VK_RIGHT))
    {
        int x2 = x+1;
        if(map[currentRoomNumber][y][x2] == ' ')
        {
            map[currentRoomNumber][y][x] = ' ';
            x++;
            map[currentRoomNumber][y][x] = 'o';
        }
        else if(map[currentRoomNumber][y][x2] == '_')
        {
            map[currentRoomNumber][y][x] = ' ';
            prevRoom = currentRoom;
            currentRoom = nextRoom;
            GetMap();
            changeRoom();
        }
    }
    if(GetAsyncKeyState(VK_LEFT))
    {
        int x2 = x-1;
        if(map[currentRoomNumber][y][x2] == ' ')
        {
            map[currentRoomNumber][y][x] = ' ';
            x--;
            map[currentRoomNumber][y][x] = 'o';
        }
        else if(map[currentRoomNumber][y][x2] == '_')
        {
            map[currentRoomNumber][y][x] = ' ';
            prevRoom = currentRoom;
            currentRoom = nextRoom;
            GetMap();
            changeRoom();
        }
    }
    if(GetAsyncKeyState(0x44))
        devMode = !devMode;

    if(GetAsyncKeyState(0x5A))
    {
        if(map[currentRoomNumber][y-1][x] == '!' || map[currentRoomNumber][y+1][x] == '!' || map[currentRoomNumber][y][x+1] == '!' || map[currentRoomNumber][y][x-1] == '!')
        {
            if(map[currentRoomNumber][y-1][x] == '@' || map[currentRoomNumber][y+1][x] == '@' || map[currentRoomNumber][y][x+1] == '@' || map[currentRoomNumber][y][x-1] == '@')
            {
                interact('@');
            }
            else{
                interact('!');
            }
            return;
        }
    }
    if(map[currentRoomNumber][y-1][x] == '!' || map[currentRoomNumber][y+1][x] == '!' || map[currentRoomNumber][y][x+1] == '!' || map[currentRoomNumber][y][x-1] == '!' || map[currentRoomNumber][y-1][x] == '@' || map[currentRoomNumber][y+1][x] == '@' || map[currentRoomNumber][y][x+1] == '@' || map[currentRoomNumber][y][x-1] == '@')
        npcClose = true;
    else
        npcClose = false;
    return;
}

void interact(char interactedCharacter)
{
    if(interactedCharacter == '!')
    {
        if(!doDialogue)
        {
            if(dialogueDone)
            {
                doDialogue = false;
                dialogueDone = false;
            }
            doDialogue = true;
            dialogueDone = false;
        }
        else
        {
            return;
        }
    } else if(interactedCharacter == '@')
    {
        if((x==27 && y==2) || (x==28||y==1))
            Beep(500, 500);
        else if(x==29 && y == 1)
            Beep(500, 500);
        else if(x==30&&y==1)
            Beep(500, 500);
    }

}

NPC getNpc()
{
    NPC errorNPC = {"error","null",{{"An error has occured."}, 1, 0, 0}};
    if(currentRoom == "begin")
        return mom;
    else if(currentRoom == "prof-oak-house")
        return profOak;
    else
        return errorNPC;

}

void Dialogue(NPC npc)
{
    int i = 0;
    while(!dialogueDone)
    {
        if(i > npc.dialogueLines)
            dialogueDone = true;
        if(GetAsyncKeyState(0x5A))
        {
            system("cls");
            DrawMap(map[currentRoomNumber]);
            if(npcClose)
            {
                cout << "Press Z to interact." << endl;
            }
            cout << endl;
            for(int j = 0; j < 4; j++)
                cout << npc.dialogue[i][j] << endl;
            i++;
        }
        Sleep(150);
    }
    if(npc.name == "Prof. Oak")
    {
        talkedToOak = true;
    }
    doDialogue = false;
}

void getNextRoom(string &nextRoom)
{
    if(currentRoom == "begin")
    {
        nextRoom = "outside-begin";
    }
    if(currentRoom == "outside-begin")
    {
        if(x < 15 && x > 5)
            nextRoom = "begin";
        else if(x > 30 && x < 45 && y == 9)
            nextRoom = "prof-oak-house";
        else if(x > 30 && x < 45 && y < 5 && talkedToOak == true)
            nextRoom = "route-328";
    }
    if(currentRoom == "prof-oak-house")
        nextRoom = "outside-begin";
}
