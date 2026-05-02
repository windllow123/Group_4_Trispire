#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

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
    std::cin.ignore();
    std::cin.get();
}

// Sleep in milliseconds
inline void sleepMs(int ms){
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

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


inline int getKey(){
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
}

inline int getTerminalWidth(){
    struct winsize size;
    ioctl(0,TIOCGWINSZ, &size);
    return size.ws_col;
}

#endif
