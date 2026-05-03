#include <algorithm>
#include <iostream>
#include "menu.h"
#include "SaveLoad.h"
#include "ConsoleUtils.h"
#include <string>
#include <sstream>
#include <vector>

//menu.cpp-for printing various menu pages in the game, including main menu, difficulty menu, archive, and load menu.



using namespace std;

//struct for supporting the printing of difficulty menu
struct Difficulty{
    string color;
    string name;
    vector<string> desc;
};

//main menu items
const string menuItems[5]={
    "New Game",
    "Load Game",
    "How to Play",
    "Archive",
    "Quit"
};
const int menuSize=5; //hard coded, change when adding

//ANSI escape code for printing colors when printing difficulty menu
const string difficultyColors[3]={
    "\x1b[32m", //ANSI foreground green, easy
    "\x1b[33m", //ANSI foreground yellow, normal
    "\x1b[31m" //ANSI foreground red, hard
};

//vector containing color, name, and description of each difficulty
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


inline int safePadding(int value){
    return value > 0 ? value : 0;
}

//helper function for printing a line of text centered in a specified width, with whitespace padding on the left
//input parameters: const string &line (line of text to be printed, stored as std::string), int width (width of space for text to be centered in, stored as int)
//return: no return value, prints directly on terminal
void printCentered(const string &line, int width){
    int pad = width - static_cast<int>(line.length());
    int leftPadding = safePadding(pad / 2);
    int rightPadding = safePadding(pad - leftPadding);
    cout << string(leftPadding, ' ') << line << string(rightPadding, ' ') << endl;
}

//helper function for printing text contain in a box formed from unicode box drawing characters;
//input parameters: const string &line (line of text to be printed)
//return: no return value, prints directly on terminal
void printBoxedText(const string &line){
    int terminalWidth=getTerminalWidth();
        int maxLineSize=14+4; //max menu item length + padding
        int boxWidth=maxLineSize+2;
        int borderSize=boxWidth+2;

        int leftPadding=safePadding((terminalWidth-borderSize)/2);
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

//helper function for printing a specified number of newline character
//input parameters: int num (number of newlines to print, expressed as int)
//return: no return value, prints directly to terminal
void printSpace(int num){ // helper for printing desired number of newlines
    for (int i=0; i<num; i++){
        cout<<"\n";
    }
}

//function for printing main menu and collecting user input on selected menu choice
//input paramters: none
//return value: int representing the selected menu item: 1) New game, 2) Load Game, 3) How to Play, 4) Archive, 5) Quit
int showMainMenu(){
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
        int leftPadding=safePadding((terminalWidth-bannerWidth)/2);
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
            return selectedIdx + 1; // Return 1-5 for menu choices
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

//helper function for printing difficulty menu and obtaining user input on selected difficulty
//input parameters: none
//return value: int representing selected difficulty. 0: easy, 1: normal, 2: hard
int showDifficultyMenu() {
    int selectedDiff=0; // range 0-2, 2 being hardest
    
    while (true){
        clearScreen();

        int terminalWidth=getTerminalWidth();
        printSpace(5);
        string title="SELECT YOUR DIFFICULTY";
        string instruction="Use left/right arrow key to adjust and Enter to select";
        string backInstruction="Press Esc or Space to return to lobby";
        printCentered(title, terminalWidth);
        string underscore="";
            for (int i=0; i<80; i++){
                underscore+="─";
            }
        int underscoreLeftPad=safePadding((terminalWidth-80)/2);
        cout << string(underscoreLeftPad, ' ')<<underscore<<endl;
        printCentered(instruction, terminalWidth);
        printCentered(backInstruction, terminalWidth);
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
        int barLeftPad=safePadding((terminalWidth-maxBarSize)/2); //cannot use helper as this is unicode
        for (int i=0; i<3; i++){
            cout<<string(barLeftPad, ' ')<<difficultyBar<<endl;
        }
        printSpace(3);
        Difficulty difficultyInfo=difficulties[selectedDiff];
        printBoxedText(difficultyInfo.name);
        printSpace(2);
        for (size_t i=0; i<difficulties[selectedDiff].desc.size(); i++){
            printCentered(difficulties[selectedDiff].desc[i], terminalWidth);
            printSpace(1);
        }
        cout<<"\033[0m";

        int command=getKey();
        if (command==enter_key_idx){
            return selectedDiff; 
        }
        else if (command==esc_key_idx || command==space_key_idx) {
            return -1;
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

//helper function for convert int representation of difficulty level to string
//input parameter: int representing difficulty level
//return value: char*, representing same difficulty level
static const char* saveDifficultyLabel(int d) {
    if (d == 0) return "Easy";
    if (d == 1) return "Normal";
    return "Hard";
}

//helper function for printing boxed text for save slots, wider than printBoxedText()
//input: const string& storing line to be printed), int storing width of terminal), const string& for ANSI escape code for colors), bool selected for selected status
//return: none, print directly to terminal
static void printSaveSlotWideBox(const string& line, int terminalWidth, const string& rowColor, bool selected) {
    const string reset = "\033[0m";
    const string selBg = "\033[30;106m";
    const string selReset = "\033[0m";

    string prefix = selected ? "  > " : "    ";
    string visible = prefix + line;
    int inner = static_cast<int>(visible.length()) + 4;
    int innerMin = (std::min)(72, (std::max)(52, terminalWidth - 8));
    if (inner < innerMin) {
        inner = innerMin;
    }
    if (inner > terminalWidth - 4) {
        inner = (std::max)(40, terminalWidth - 4);
    }

    int border = inner + 2;
    int leftPad = safePadding((terminalWidth - border) / 2);
    string indent(leftPad, ' ');
    string hline;
    hline.reserve(static_cast<size_t>(inner) * 3);
    for (int i = 0; i < inner; ++i) {
        hline += "─";
    }

    cout << indent << "┌" << hline << "┐" << endl;

    int padL = safePadding((inner - static_cast<int>(visible.length())) / 2);
    int padR = inner - static_cast<int>(visible.length()) - padL;

    cout << indent << "│";
    if (selected) {
        cout << selBg << string(static_cast<size_t>(padL), ' ') << visible << string(static_cast<size_t>(padR), ' ')
             << selReset;
    } else {
        cout << rowColor << string(static_cast<size_t>(padL), ' ') << visible << string(static_cast<size_t>(padR), ' ')
             << reset;
    }
    cout << "│" << endl;

    cout << indent << "└" << hline << "┘" << endl;
}

//function for printing save menu
//input paramemter: bool storing whether function call is for a new game or not
//output: int representing selected load file
int showSaveSlotMenu(bool forNewGame) {
    int selectedIdx = 0;

    while (true) {
        clearScreen();
        int terminalWidth = getTerminalWidth();
        printSpace(2);

        string title = forNewGame ? "SAVE DATA — NEW GAME" : "SAVE DATA — LOAD GAME";
        string subtitle =
            forNewGame ? "Choose a slot. Existing data in that slot will be replaced." : "Choose a slot to continue.";

        printCentered(title, terminalWidth);
        string underscore;
        for (int i = 0; i < 72; ++i) {
            underscore += "─";
        }
        int uPad = safePadding((terminalWidth - 72) / 2);
        cout << string(uPad, ' ') << underscore << endl;
        printSpace(1);
        printCentered(subtitle, terminalWidth);
        printSpace(1);
        printCentered("Up / Down: move   Enter: confirm   1-3: jump to slot   Esc / Space: back", terminalWidth);
        printSpace(2);

        for (int s = 1; s <= SaveLoad::SLOT_COUNT; ++s) {
            SaveSlotInfo info;
            bool has = SaveLoad::peekSlot(s, info);
            bool sel = (selectedIdx == s - 1);

            string line;
            if (!has) {
                line = "SLOT " + to_string(s) + "  —  EMPTY";
                if (forNewGame) {
                    line += "  ·  ready for new run";
                } else {
                    line += "  ·  cannot load";
                }
            } else {
                line = "SLOT " + to_string(s) + "  ·  Lv " + to_string(info.currentLevel) + "  " +
                       saveDifficultyLabel(info.difficulty) + "  ·  HP " + to_string(info.playerHp) + "/" +
                       to_string(info.playerMaxHp) + "  ·  Skills " + to_string(info.skillCount);
                if (forNewGame) {
                    line += "  ·  overwrite";
                }
            }

            string rowColor = "\033[90m";
            if (has) {
                int d = info.difficulty;
                if (d >= 0 && d < 3) {
                    rowColor = difficultyColors[d];
                } else {
                    rowColor = "\033[37m";
                }
            }

            if (!has && !sel) {
                rowColor = "\033[90m";
            }

            printSaveSlotWideBox(line, terminalWidth, rowColor, sel);
            printSpace(1);
        }

        cout << "\033[0m";
        printSpace(1);

        int command = getKey();
        if (command == esc_key_idx || command == space_key_idx) {
            return -1;
        }
        if (command == enter_key_idx) {
            return selectedIdx + 1;
        }
        if (command >= '1' && command <= static_cast<char>('0' + SaveLoad::SLOT_COUNT)) {
            return command - '0';
        }
        if (command == up_key_idx) {
            selectedIdx = (selectedIdx + SaveLoad::SLOT_COUNT - 1) % SaveLoad::SLOT_COUNT;
        } else if (command == down_key_idx) {
            selectedIdx = (selectedIdx + 1) % SaveLoad::SLOT_COUNT;
        }
    }
}

//function for printing archive page
//input: none
//output: none
void showArchive(){
    clearScreen();
    printSpace(2);
    int terminalWidth=getTerminalWidth();

    string title="ARCHIVE";
    printCentered(string(title), terminalWidth);

    string underscore="";
        for (int i=0; i<60; i++){
            underscore+="─";
        }
    int underscoreLeftPad=safePadding((terminalWidth-60)/2);
    cout << string(underscoreLeftPad, ' ')<<underscore<<endl;
    printSpace(2);
    
    cout<<"  The following list contains all the skills in the game:"<<endl;
    vector<string> skillList={
        "Steel Cavalry: Your【Strike】always hits; enemy【Dodge】cannot prevent damage.",
        "Warscream: You can use 2 extra【Strike】actions each round.",
        "Sacrifice: Active Skill: Lose 1 hp and claim 3 new cards.",
        "Ambition: When you take damage from an enemy attack, claim 2 cards.",
        "Dragon Gut: You can use【Dodge】as【Strike, use【Strike】as【Dodge】."
    };
    for (size_t i = 0; i < skillList.size(); ++i) {
        cout<<"    "<<skillList[i]<<endl;
    }

    printSpace(6);

    printCentered("HALL OF FAME:", getTerminalWidth());
    cout << string(underscoreLeftPad, ' ')<<underscore<<endl;
    printSpace(1);
    cout<<"      Number of victories in Easy: 0"<<endl;
    cout<<"      Number of victories in Normal: 0"<<endl;
    cout<<"      Number of victories in Hard: 0"<<endl;

    printSpace(2);

    printCentered("Press Enter (Space) to return to lobby...", getTerminalWidth());
    while (true) {
        int key = getKey();
        if (key == enter_key_idx || key == space_key_idx) {
            break;
        }
    }
}

