#include <iostream>
#include <limits>

using namespace std;

void showMenu() {
    cout << "========================\n";
    cout << "       GAME LOBBY\n";
    cout << "========================\n";
    cout << "   1. New Game\n";
    cout << "   2. Load Game\n";
    cout << "   3. How to Play\n";
    cout << "   4. Quit\n";
    cout << "========================\n";
    cout << "Enter your choice: ";
}

void newGame() {
    
}

void loadGame() {
    
}

void howToPlay() {
    
}

void quitGame() {
    cout << "\nGoodbye!\n";
}

int main() {
    int choice;
    
    while (true) {
        showMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter 1-4.\n\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                newGame();
                break;
            case 2:
                loadGame();
                break;
            case 3:
                howToPlay();
                break;
            case 4:
                quitGame();
                return 0;
            default:
                cout << "Invalid choice! Please enter 1-4.\n\n";
                continue;
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
    return 0;
}
