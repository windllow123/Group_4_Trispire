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

// Clear screen (Mac/Linux)
inline void clearScreen(){
    std::cout << "\033[2J\033[1;1H";
}

// Pause (wait for user)
inline void pauseConsole(){
    std::cout << "Press Enter to continue...";
    //std::cin.ignore();
    std::cin.get();
}

inline void pauseConsole2(){
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

// Sleep in milliseconds
inline void sleepMs(int ms){
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#ifndef _WIN32
struct Terminal{
    termios originalTerminal, raw;

    void enableRawMode(){
        tcgetattr(STDIN_FILENO, &originalTerminal);

        raw=originalTerminal;
        //disable input flags
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

        //disable post-processing, output flag
        raw.c_oflag &= ~(OPOST);

        //for 8 bit chars
        raw.c_cflag |= (CS8);
        
        //for echo, canonical form, signals, and extended input
        raw.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);

        //default timing adjustment, no timeout (vtime=0), wait 1 bit after input (vmin=1)
        //adjustable via setReadMode()
        //setReadMode will assign all changes, so no need to setattr here
        setReadMode(1,0);

    }

    void setReadMode(int vmin, int vtime){
        raw.c_cc[VMIN]=vmin;
        raw.c_cc[VTIME]=vtime;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    void disableRawMode(){
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminal);
    }

    //RAII, restore terminal upon destruction so it is not stuck in raw mode
    ~Terminal() {
        disableRawMode();
    }
};
#endif

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

inline void displayHealthBar(Player& player, Enemy& enemy) {
    std::cout << "\033[0m"; // Reset colors
    int width = getTerminalWidth();
    
    // Create health bar
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
    
    // Print borders and health
    std::cout << std::string(width, '=') << std::endl;
    std::cout << playerBar << std::endl;
    std::cout << enemyBar << std::endl;
    std::cout << std::string(width, '=') << std::endl;
}

#endif
