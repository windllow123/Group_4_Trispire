#include <iostream>
#include "menu.h"
#include "ConsoleUtils.h"
#include <string>
#include <sstream>
#include <vector>



using namespace std;

struct Difficulty{
    string color;
    string name;
    vector<string> desc;
};

const string menuItems[5]={
    "New Game",
    "Load Game",
    "How to Play",
    "Archive",
    "Quit"
};
const int menuSize=5; //hard coded, change when adding

const string difficultyColors[3]={
    "\x1b[32m", //ANSI foreground green, easy
    "\x1b[33m", //ANSI foreground yellow, normal
    "\x1b[31m" //ANSI foreground red, hard
};
const vector<Difficulty> difficulties ={
    {
        "\x1b[32m",
        "Easy",
        {
            "The easiest difficulty",
            "You have higher starting stats",
            "Enemies have lower stats and their attacks are easier to avoid",
            "Perfect for new players seeking a relaxed experience"
        }
    },
    {
        "\x1b[33m",
        "Normal",
        {
            "The standard difficulty",
            "You have normal starting stats",
            "Enemies have standard stats and accuracy",
            "The classic experience, balanced in difficulty"
        }

    },
    {
        "\x1b[31m",
        "Hard",
        {
            "A challenging mode demanding strategy",
            "You have lower starting stats",
            "Enemies are stronger-higher health, higher damage, higher accuracy",
            "Unforgiving and intense, designed for skilled players"
        }

    }
};

void printCentered(const string &line, int width){
    int leftPadding=(width-line.length())/2;
    int rightPadding=(width-line.length())-leftPadding;
    cout << string(leftPadding, ' ')<<line<<string(rightPadding, ' ')<<endl;
}


void printBoxedText(const string &line){
    int terminalWidth=getTerminalWidth();
        int maxLineSize=14+4; //max menu item length + padding
        int boxWidth=maxLineSize+2;
        int borderSize=boxWidth+2;

        int leftPadding=(terminalWidth-borderSize)/2;
        string indent=string(leftPadding,' ');

        string horizontalLine="";
        for (int i=0; i<boxWidth; i++){
            horizontalLine+="─";
        }

        cout<<indent<<"┌"<<horizontalLine<<"┐"<<endl;
        cout<<indent<<"│"<<string(boxWidth, ' ')<<"│"<<endl;
        int padding=boxWidth-line.length();
        int leftPaddingInside=(padding)/2;
        int rightPaddingInside=padding-leftPaddingInside;
        cout<<indent<<"│"<<string(leftPaddingInside,' ')<<line<<string(rightPaddingInside,' ')<<"│"<<endl;
        cout<<indent<<"│"<<string(boxWidth, ' ')<<"│"<<endl;
        cout<<indent<<"└"<<horizontalLine<<"┘"<<endl;
}

void printSpace(int num){ // helper for printing desired number of newlines
    for (int i=0; i<num; i++){
        cout<<"\n";
    }
}


void showMainMenu(){
    int selectedIdx=0;

    while (true){
        clearScreen();
        printSpace(2);

        string banner = R"(
 ███████████            ███   █████████             ███                    
░█░░░███░░░█           ░░░   ███░░░░░███           ░░░                     
░   ░███  ░  ████████  ████ ░███    ░░░  ████████  ████  ████████   ██████ 
    ░███    ░░███░░███░░███ ░░█████████ ░░███░░███░░███ ░░███░░███ ███░░███
    ░███     ░███ ░░░  ░███  ░░░░░░░░███ ░███ ░███ ░███  ░███ ░░░ ░███████ 
    ░███     ░███      ░███  ███    ░███ ░███ ░███ ░███  ░███     ░███░░░  
    █████    █████     █████░░█████████  ░███████  █████ █████    ░░██████ 
   ░░░░░    ░░░░░     ░░░░░  ░░░░░░░░░   ░███░░░  ░░░░░ ░░░░░      ░░░░░░  
                                         ░███                              
                                         █████                             
                                        ░░░░░                                           
)";
        int bannerWidth=76;
        int terminalWidth=getTerminalWidth();
        int leftPadding=(terminalWidth-bannerWidth)/2;
        string indent=string(leftPadding,' ');
        istringstream ss(banner);
        string line;
        while (getline(ss,line)){
            cout<<indent<<line<<endl;
        }

  
        string summary="A Roguelike Deckbuilder Inspired by Slay the Spire and Sanguosha";
        string instructions="Use Arrow Keys to navigate and Enter to select";
        printSpace(2);
        printCentered(summary, terminalWidth);
        printSpace(1);
        printCentered(instructions, terminalWidth);
        printSpace(1);

        for (int i=0; i<menuSize; i++){
            string line=menuItems[i];
            if (selectedIdx==i){
                line="> "+line;
            }
            else{
                line="  "+line;
            }
            printBoxedText(line);
        }

        int command=getKey();
        if (command==enter_key_idx){
            switch (selectedIdx)
            {
            case 0:
                return;//start game
            case 1:
                cout<<"Load Game in progress..."<<endl;
                sleepMs(1000);
                //loadGame();
                break;
            case 2:
                cout<<"How to Play in progress..."<<endl;
                sleepMs(1000);
                //showHowToPlay(); place 玩法介绍 into a page
                break;
            case 3:
                cout<<"Archive in progress..."<<endl;
                sleepMs(1000);
                //showArchive(); desc of all skills and cards, optional
                break;
            case 4:
                exit(0);
            }
        }
        else{
            if (command==up_key_idx){
                selectedIdx=(selectedIdx+menuSize-1)%menuSize;
            }
            else if (command==down_key_idx){
                selectedIdx=(selectedIdx+1)%(menuSize);
            }
        }
    }
}


int showDifficultyMenu() {
    int selectedDiff=0; // range 0-2, 2 being hardest
    
    while (true){
        clearScreen();

        int terminalWidth=getTerminalWidth();
        printSpace(5);
        string title="SELECT YOUR DIFFICULTY";
        string instruction="Use left/right arrow key to adjust and Enter to select";
        printCentered(title, terminalWidth);
        string underscore="";
            for (int i=0; i<80; i++){
                underscore+="─";
            }
        int underscoreLeftPad=(terminalWidth-80)/2;
        cout << string(underscoreLeftPad, ' ')<<underscore<<endl;
        printCentered(instruction, terminalWidth);
        printSpace(4);

        //print bar 
        int maxBarSize=72;
        int currentBarSize=0;
        switch (selectedDiff){
            case 0:
                currentBarSize=maxBarSize/3;
                break;
            case 1:
                currentBarSize=(maxBarSize*2)/3;
                break;
            case 2:
                currentBarSize=maxBarSize;
                break;
        }
        //change color
        cout << difficulties[selectedDiff].color;

        string difficultyBar="";
        for (int i=0; i<72; i++){
            if (i<currentBarSize){
                difficultyBar+="█";
            }
            else{
                difficultyBar+="░";
            }
        }
        int barLeftPad=(terminalWidth-maxBarSize)/2; //cannot use helper as this is unicode
        for (int i=0; i<3; i++){
            cout<<string(barLeftPad, ' ')<<difficultyBar<<endl;
        }
        printSpace(3);
        Difficulty difficultyInfo=difficulties[selectedDiff];
        printBoxedText(difficultyInfo.name);
        printSpace(2);
        for (int i=0; i<difficulties[selectedDiff].desc.size(); i++){
            printCentered(difficulties[selectedDiff].desc[i], terminalWidth);
            printSpace(1);
        }
        cout<<"\033[0m";

        int command=getKey();
        if (command==enter_key_idx){
            return selectedDiff; 
        }
        else{
            if (command==right_key_idx){
                if (selectedDiff<2){
                    selectedDiff+=1;
                }
            }
            else if (command==left_key_idx){
                if (selectedDiff>0){
                    selectedDiff-=1;
                }
            }
        }
    }
    
}
