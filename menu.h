#ifndef MENU_H
#define MENU_H

// Function: Displays main game menu with New Game, Load, How to Play, Archive, Quit
// Output: integer 1-5 representing the selected menu option
int showMainMenu();

// Function: Shows difficulty selection screen with Easy, Normal, Hard options
// Output: 0 for easy, 1 for normal, 2 for hard, -1 if user goes back
int showDifficultyMenu();

// Function: Displays save slot selection menu for new games or loading
// Input: forNewGame - true if selecting slot for new game, false for loading
// Output: slot number 1-3, or -1 if user goes back
int showSaveSlotMenu(bool forNewGame);

// Function: Shows archive screen with skill list and victory counts
void showArchive();

#endif
