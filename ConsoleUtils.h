#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

const int up_key_idx=1001;
const int down_key_idx=1002;
const int right_key_idx=1003;
const int left_key_idx=1004;
const int enter_key_idx=1005;
const int esc_key_idx=27;
const int space_key_idx=32;
const int backspace_key_idx=8;
const int delete_key_idx=127;

// Function: Checks if a key code is a pause action
// Input: key - the key code to check
// Output: true if key is Escape or Space
inline bool isPauseKey(int key) {
    return key == esc_key_idx || key == space_key_idx;
}

// Function: Clears the console screen
inline void clearScreen(){
    std::cout << "\033[2J\033[1;1H";
}

// Function: Pauses execution and waits for user to press Enter
inline void pauseConsole(){
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

// Function: Clears input buffer then pauses, waits for Enter
inline void pauseConsole2(){
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

// Function: Suspends execution for specified milliseconds
// Input: ms - number of milliseconds to sleep
inline void sleepMs(int ms){
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#ifndef _WIN32
// RAII terminal manager for raw input mode on Unix systems
struct Terminal{
    termios originalTerminal, raw;

    // Function: Enables raw terminal mode for immediate key input without echo
    void enableRawMode(){
        tcgetattr(STDIN_FILENO, &originalTerminal);

        raw=originalTerminal;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
        setReadMode(1,0);
    }

    // Function: Configures read timeout behavior for raw mode
    // Input: vmin - minimum characters to read
    // Input: vtime - timeout in deciseconds
    void setReadMode(int vmin, int vtime){
        raw.c_cc[VMIN]=vmin;
        raw.c_cc[VTIME]=vtime;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    // Function: Restores original terminal settings
    void disableRawMode(){
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminal);
    }

    // Function: Destructor ensures terminal is restored to normal mode
    ~Terminal() {
        disableRawMode();
    }
};
#endif

// Function: Reads a single key press from console, handling arrow keys
// Output: key code integer for the pressed key
inline int getKey(){
#ifdef _WIN32
    int c = _getch();
    if (c == 0 || c == 224) {
        int dir = _getch();
        switch (dir) {
            case 72: return up_key_idx;
            case 80: return down_key_idx;
            case 75: return left_key_idx;
            case 77: return right_key_idx;
        }
    } else if (c == '\r' || c == '\n') {
        return enter_key_idx;
    }
    return c;
#else
    Terminal terminal;
    terminal.enableRawMode();
    int c;
    c = getchar();
    if (c=='\x1b'){
        getchar();
        int dir=getchar();
        switch (dir)
        {
        case 'A':
            return up_key_idx;
        case 'B':
            return down_key_idx;
        case 'C':
            return right_key_idx;
        case 'D':
            return left_key_idx;
        }
    }
    else if (c=='\r'||c=='\n'){
        return enter_key_idx;
    }
    return c;
#endif
}

// Function: Reads a line of numeric input with pause support
// Input: out - reference to store the input string
// Input: paused - reference to set if user pressed pause key
// Output: true if Enter pressed, false if paused
inline bool readLineWithPauseSupport(std::string& out, bool& paused) {
    out.clear();
    paused = false;
    while (true) {
        int key = getKey();
        if (key == enter_key_idx) {
            std::cout << std::endl;
            return true;
        }
        if (isPauseKey(key)) {
            paused = true;
            return false;
        }
        if (key == backspace_key_idx || key == delete_key_idx) {
            if (!out.empty()) {
                out.pop_back();
                std::cout << "\b \b";
            }
            continue;
        }
        if ((key >= '0' && key <= '9') || key == '-') {
            out.push_back(static_cast<char>(key));
            std::cout << static_cast<char>(key);
            continue;
        }
    }
}

// Function: Gets the current terminal window width
// Output: terminal width in characters, defaults to 80
inline int getTerminalWidth(){
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
#else
    struct winsize size;
    ioctl(0,TIOCGWINSZ, &size);
    return size.ws_col;
#endif
}

#include "Player.h"
#include "Enemy.h"

// Function: Displays health bars for both player and enemy with borders
// Input: player - reference to player object for HP display
// Input: enemy - reference to enemy object for HP display
inline void displayHealthBar(Player& player, Enemy& enemy) {
    std::cout << "\033[0m";
    int width = getTerminalWidth();
    
    std::string playerBar = "Player: ";
    for (int i = 0; i < player.max_hp; i++) {
        playerBar += (i < player.hp) ? "# " : "_ ";
    }
    playerBar += " [" + std::to_string(player.hp) + "/" + std::to_string(player.max_hp) + "]";
    
    std::string enemyBar = "Enemy:  ";
    for (int i = 0; i < enemy.max_hp; i++) {
        enemyBar += (i < enemy.hp) ? "# " : "_ ";
    }
    enemyBar += " [" + std::to_string(enemy.hp) + "/" + std::to_string(enemy.max_hp) + "]";
    
    std::cout << std::string(width, '=') << std::endl;
    std::cout << playerBar << std::endl;
    std::cout << enemyBar << std::endl;
    std::cout << std::string(width, '=') << std::endl;
}

#endif
