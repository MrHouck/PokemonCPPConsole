#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <Windows.h>
#include <winuser.h>
#include <mmsystem.h>
#include <fstream>
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
    Weakness weakness;
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
growth rates (in case we decide to use levels)
Fast = (4n^3)/5
Medium Fast = n^3
Medium Slow = (6/5)n^3 - 15n^2 + 100n - 140
Slow = (5n^3)/4
*/


//FUNCTION PROTOTYPES
//Most of these can be void, since we use global variables for most of this project
void CalculateMovement(void); //movement calculation
void interact(char); //interaction

NPC getNpc(void); //get the npc in the room you are in
void Dialogue(NPC); //dialogue system
void createNPCs(void); //npc creation (in progress)

void GetMap(void); //get the room you are in, so you can draw it
void changeRoom(void); //change the room
void getNextRoom(string &); //get the next room
void DrawMap(char [14][70]); //draw the current room

void ShowMenu(void); //show the game menu
void MenuUp(void); //move up in the menu
void MenuDown(void); //move down in the menu
void MenuInteract(void); //interact with the menu

void PlayMusic(void); //play the music (if enabled)
void printDebugStats(void); //print debug stats (if enabled)


//GLOBAL VARIABLES
//boolean reference: https://www.geeksforgeeks.org/bool-data-type-in-c/
bool game = true;
bool npcClose = false, canMove = true, doDialogue = false, dialogueDone = false, drawMenu = false, menuFirstTime = false;
bool devMode = false, soundEnabled = true, musicEnabled = true;
bool talkedToOak = false; //required for moving on
int x=1, y=1, currentRoomNumber = 0, selectedMenuIndex = 0;
string currentRoom = "begin";
string nextRoom = "";
string prevRoom = "begin";
char map[4][14][70]= //all the possible rooms
{
    {"##########","#        #","#        #","#    !   #","#        #","#        #","#   __   #","##########"},
    {"##################################################################","#                                 __                             #","#                                                                #","#                                                                #","#                                                                #","#                                                                #","#                                                                #","#       /-\\                             /-\\                      #","#       |_|                             |_|                      #","#                                                                #","#                                                                #","##################################################################"},
    {"####################################","#                                  #","#                           @@@    #","#                !          ┬─┬    #","#                                  #","###########              ###########","#                                  #","#                                  #","#               ___                #","####################################"},
    {"############################","#                          #","#                        _ #","#                          #","#                          #","#                 ---------#","#                          #","#                        ! #","#---------                 #","#                          #","#                          #","#                          #","############################"}
};
//MENUS
char menu[6][8][18] =
{
    {"+-----------+","| → Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","|   Options |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","| → Pokémon |","|   Bag     |","|   Save    |","|   Options |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","| → Bag     |","|   Save    |","|   Options |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","| → Save    |","|   Options |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","| → Options |","|   Exit    |","+-----------+" },
    {"+-----------+","|   Pokédex |","|   Pokémon |","|   Bag     |","|   Save    |","|   Options |","| → Exit    |","+-----------+" }
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
    "Mom", //name
    "begin", //room npc is in
    { //dialogue
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
    2, //dialogue lines
    6, //x pos
    3 //y pos
};

NPC profOak =
{
    "Prof. Oak", //name
    "prof-oak-house", //room location
    { //dialogue
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
    4, //dialogue lines
    5, //x pos
    10 //y pos
};

//NPC trainerLucas
//{

//}


int main()
{
    ifstream loadFile;
    loadFile.open("saveData.txt");

    loadFile >> musicEnabled;
    loadFile >> soundEnabled;
    loadFile >> devMode;
    loadFile >> talkedToOak;
    loadFile >> x;
    loadFile >> y;
    loadFile >> currentRoom;
    loadFile >> nextRoom;
    loadFile >> prevRoom;

    map[currentRoomNumber][y][x] = 'o';

    if(musicEnabled)
        PlaySound("mom-theme.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC); //https://stackoverflow.com/questions/9961949/playsound-in-c-console-application
    system("chcp 65001  > nul"); //make system support UTF-8 encoding
    Pokemon bulbasaur = {"Bulbasaur", 45, 49, 49, 1, 45, 64, 0}; //to be moved, and changed
    NPC currentNPC; //the current npc in the room
    while(game)
    {
        GetMap(); //getting the room
        getNextRoom(nextRoom); //getting the next room
        system("cls"); //clearing screen to refresh map
        DrawMap(map[currentRoomNumber]); //drawing the map based on the room
        if(drawMenu)
        {
            ShowMenu(); //if the user wants to draw the menu, show the menu
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
            currentNPC = getNpc(); //get the npc in the room
            Dialogue(currentNPC); //do dialogue with that npc
            if(devMode) //draw the debug stats if in dev mode
            {
                printDebugStats();
            }
        }
        else if(devMode) //draw the debug stats if in dev mode
        {
            if(!drawMenu)
            {
                printDebugStats();
            }
        }
        system("pause>nul"); //wait for user input
        if(GetAsyncKeyState(0x58)) //https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        {
            if(!canMove)
            {
                drawMenu = false;
                menuFirstTime = true;
                canMove = true;
                selectedMenuIndex = 0;
            }
            else
            { //draw the menu, make it so the user can't move
                drawMenu = true;
                menuFirstTime = false;
                canMove = false;
                selectedMenuIndex = 0;
            }
        }
        if(canMove)
        { //do movement if allowed
            CalculateMovement();
        }
    }
}

void printDebugStats()
{
    cout << endl << "Debug stats" << endl << "----------------------------";
    cout << endl << "Room: " << endl << "  Current Room: " << currentRoom;
    cout << endl << "  Room Number: " << currentRoomNumber;
    cout << endl << "  Previous room: " << prevRoom;
    cout << endl << "  Next room: " << nextRoom;
    cout << endl << "  Draw Menu: " << drawMenu;
    cout << endl << "  Coords: (" << x << ','<< y << ')';
    cout << endl << "Options: ";
    cout << endl << "  Music Enabled: " << musicEnabled;
    cout << endl << "  Sound Effects Enabled: " << soundEnabled;
}


void GetMap()
{ //can't use switch statements with string unfortuantely
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
{ //just a for loop to draw the map
    map[currentRoomNumber][y][x] = 'o';
    for(int i = 0; i < 14; i++)
    {
        cout << room[i] << endl;
    }
}

void ShowMenu()
{
    //https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    if(GetAsyncKeyState(VK_UP))
    {
        if(soundEnabled)
            mciSendString("play Choose.wav", NULL, 0, NULL); //https://docs.microsoft.com/en-us/previous-versions/dd757161(v%3Dvs.85)
        MenuUp();
    }

    else if(GetAsyncKeyState(VK_DOWN))
    {
        if(soundEnabled)
            mciSendString("play Choose.wav", NULL, 0, NULL);

        MenuDown();
    }

    else if(GetAsyncKeyState(0x5A))
    {
        if(soundEnabled)
            mciSendString("play Select.wav", NULL, 0, NULL);

        MenuInteract();
    }
    else
    {
        for(int i = 0; i < 8; i++)
            cout << menu[selectedMenuIndex][i] << endl;
    }
    Sleep(150); //if this isn't here everything breaks
}

void MenuDown()
{
    if(selectedMenuIndex+1 > 5) //if this isn't here the menu can display weird broken characters because it's accessing indexes that don't exist
        selectedMenuIndex = 0;
    else
        selectedMenuIndex++;
    for(int i = 0; i < 8; i++)
    {
        cout << menu[selectedMenuIndex][i] << endl;
    }
}
void MenuUp()
{
    if(selectedMenuIndex-1 < 0) //if this isn't here the menu can display weird broken characters because it's accessing indexes that don't exist
        selectedMenuIndex = 5;
    else
        selectedMenuIndex--;
    for(int i = 0; i < 8; i++)
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
                if(soundEnabled)
                    mciSendString("play Choose.wav", NULL, 0, NULL);


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
                if(soundEnabled)
                    mciSendString("play Choose.wav", NULL, 0, NULL);


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
                if(soundEnabled)
                    mciSendString("play Select.wav", NULL, 0, NULL);
                if(currentSelection == 1)
                {
                    ofstream write;
                    write.open("saveData.txt");
                    write << musicEnabled << endl << soundEnabled << endl << devMode << endl << talkedToOak << endl << x << endl << y << endl << currentRoom << endl << nextRoom << endl << prevRoom;
                    cout << "Your data was successfully saved, it is now safe to exit." << endl;
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
        Sleep(150); //otherwise the music instantly gets turned off which we don't want
        selectedMenuIndex = 0;
        while(!sequenceFinished)
        {
            system("cls");
            DrawMap(map[currentRoomNumber]);
            string musicEnabledStr;
            string soundEnabledStr;
            string debugModeStr;
            if(devMode)
                debugModeStr = "ON ";
            else
                debugModeStr = "OFF";

            if(soundEnabled)
                soundEnabledStr = "ON ";
            else
                soundEnabledStr = "OFF";

            if(musicEnabled)
                musicEnabledStr = "ON ";
            else
                musicEnabledStr = "OFF";

            switch(selectedMenuIndex)
            {
            case 0:
                cout << "+---------------------------------+" << endl
                     << "|  → Music: " << musicEnabledStr << "                   |" << endl
                     << "|    Sound Effects: " << soundEnabledStr << "           |" << endl
                     << "|    Debug Mode: " << debugModeStr <<  "              |" << endl
                     << "|    Exit                         |" << endl
                     << "+---------------------------------+" << endl;
                break;
            case 1:
                cout << "+---------------------------------+" << endl
                     << "|    Music: " << musicEnabledStr << "                   |" << endl
                     << "|  → Sound Effects: " << soundEnabledStr << "           |" << endl
                     << "|    Debug Mode: " << debugModeStr <<  "              |" << endl
                     << "|    Exit                         |" << endl
                     << "+---------------------------------+" << endl;
                break;
            case 2:
                cout << "+---------------------------------+" << endl
                     << "|    Music: " << musicEnabledStr << "                   |" << endl
                     << "|    Sound Effects: " << soundEnabledStr << "           |" << endl
                     << "|  → Debug Mode: " << debugModeStr <<  "              |" << endl
                     << "|    Exit                         |" << endl
                     << "+---------------------------------+" << endl;
                break;
            case 3:
                cout << "+---------------------------------+" << endl
                     << "|    Music: " << musicEnabledStr << "                   |" << endl
                     << "|    Sound Effects: " << soundEnabledStr << "           |" << endl
                     << "|    Debug Mode: " << debugModeStr <<  "              |" << endl
                     << "|  → Exit                         |" << endl
                     << "+---------------------------------+" << endl;
                break;
            }

            if(GetAsyncKeyState(VK_UP))
            {
                if(selectedMenuIndex <= 0)
                    selectedMenuIndex = 3;
                else
                    selectedMenuIndex--;
                if(soundEnabled)
                    mciSendString("play Choose.wav", NULL, 0, NULL);
            }
            else if(GetAsyncKeyState(VK_DOWN))
            {
                if(selectedMenuIndex >= 3)
                    selectedMenuIndex = 0;
                else
                    selectedMenuIndex++;
                if(soundEnabled)
                    mciSendString("play Choose.wav", NULL, 0, NULL);
            }
            else if(GetAsyncKeyState(0x5A))
            {
                switch(selectedMenuIndex)
                {
                    case 0:
                        musicEnabled = !musicEnabled;
                        if(musicEnabled)
                            PlayMusic();
                        else
                            PlaySound(NULL, 0, 0);
                        break;
                    case 1:
                        soundEnabled = !soundEnabled;
                        break;
                    case 2:
                        devMode = !devMode;
                        break;
                    case 3:
                        sequenceFinished = true;
                        system("cls");
                        DrawMap(map[currentRoomNumber]);
                        break;
                }
                if(soundEnabled)
                    mciSendString("play Select.wav", NULL, 0, NULL);
            }
            if(devMode)
                printDebugStats();
            Sleep(150);
        }
        break;
    case 5:
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
        {
            x=y=5; //starting location
            map[currentRoomNumber][y][x] = 'o';
        }
        else
        {
            x=y=1; //starting location
            map[currentRoomNumber][y][x] = 'o';
        }
        break;
    case 1:
        if(prevRoom == "prof-oak-house")
        {
            x=41;//starting location
            y=9;
            map[currentRoomNumber][y][x] = 'o';
        }
        else
        {
            x=y=9;//starting location
            map[currentRoomNumber][y][x] = 'o';
        }
        break;
    case 2:
        x=17;//starting location
        y=7;
        map[currentRoomNumber][y][x] = 'o';
        break;
    case 3:
        x=14;//starting location
        y=11;
        map[currentRoomNumber][y][x] = 'o';
        break;
    default:
        break;
    }
    if(musicEnabled)
        PlayMusic();
}

void PlayMusic(void)
{
    if(musicEnabled) //double check
    {
        if(currentRoom == "begin")
        {
            PlaySound(NULL, 0, 0);//https://docs.microsoft.com/en-us/previous-versions/dd743680(v%3Dvs.85)
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
}




void CalculateMovement()
{
    //https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
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
        { // _ is the room switcher char
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

    if(GetAsyncKeyState(0x5A)) //z key
    { //if NPC is nearby OR if the pokeballs in prof oaks lab are nearby
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
            Beep(500, 500); //for debugging, it wasn't working
        else if(x==29 && y == 1)
            Beep(500, 500); //for debugging, it wasn't working
        else if(x==30&&y==1)
            Beep(500, 500); //for debugging, it wasn't working
    }

}

NPC getNpc()
{
    NPC errorNPC = {"error","null",{{"An error has occured."}, 1, 0, 0}}; //error npc (if npc doesn't exist or something else happens)
    if(currentRoom == "begin")
        return mom;
    else if(currentRoom == "prof-oak-house")
        return profOak;
    else
        return errorNPC;
    return errorNPC;
}

void Dialogue(NPC npc)
{
    int i = 0;
    while(!dialogueDone) //while dialogue is not done
    {
        if(i > npc.dialogueLines) //if i exceeds dialogue lines, dialogue is done
            dialogueDone = true;
        if(GetAsyncKeyState(0x5A)) //z key
        {
            system("cls");
            DrawMap(map[currentRoomNumber]); //redraw map because otherwise only textbox shows up
            cout << "Press Z to interact." << endl; //we don't have to check npcClose because you're interacting with the npc, it *must* be close
            cout << endl;
            for(int j = 0; j < 4; j++) //cout dialogue
                cout << npc.dialogue[i][j] << endl;
            i++;
        }
        Sleep(150);
    }
    if(npc.name == "Prof. Oak")
    { //make sure we are able to leave the start town
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
        if(x < 15 && x > 5) //multiple checks because there are 3 different places to enter here
            nextRoom = "begin";
        else if(x > 30 && x < 45 && y == 9) //should probably rename outside-begin to palette town, since that's where you start
            nextRoom = "prof-oak-house";
        else if(x > 30 && x < 45 && y < 5 && talkedToOak == true)
            nextRoom = "route-328";
    }
    if(currentRoom == "prof-oak-house")
        nextRoom = "outside-begin";
}
