#include "BattleUI.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ConsoleUtils.h"

namespace {
std::string healthBar(int maxHealth, int currentHealth) {
    if (maxHealth <= 0) return "";
    currentHealth = std::max(0, std::min(currentHealth, maxHealth));

    std::string bar;
    for (int i = 0; i < maxHealth; ++i) {
        if (i < currentHealth) {
            bar += "██";
        } else {
            bar += "░░";
        }
    }
    return bar;
}

std::string cardType(const Card& card) {
    if (card.type == CardType::SHA) return "ATK";
    if (card.type == CardType::SHAN) return "DEF";
    if (card.type == CardType::TAO) return "HEAL";
    if (card.type == CardType::TOTEM) return "BUFF";
    return "UNK";
}

std::string clipText(const std::string& text, int width) {
    if (width <= 0) return "";
    if (static_cast<int>(text.size()) <= width) return text;
    if (width <= 3) return text.substr(0, width);
    return text.substr(0, width - 3) + "...";
}

std::string centerText(const std::string& text, int width) {
    if (width <= 0) return "";

    std::string clippedText = clipText(text, width);
    int leftPadding = std::max(0, (width - static_cast<int>(clippedText.size())) / 2);
    int rightPadding = std::max(0, width - static_cast<int>(clippedText.size()) - leftPadding);

    return std::string(leftPadding, ' ') + clippedText + std::string(rightPadding, ' ');
}

std::vector<std::string> wrapText(const std::string& text, int width) {
    std::vector<std::string> lines;
    if (width <= 0) {
        lines.push_back("");
        return lines;
    }

    std::istringstream iss(text);
    std::string word;
    std::string currentLine;

    while (iss >> word) {
        if (static_cast<int>(word.size()) > width) {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
            }

            size_t start = 0;
            while (start < word.size()) {
                lines.push_back(word.substr(start, static_cast<size_t>(width)));
                start += static_cast<size_t>(width);
            }
            continue;
        }

        if (currentLine.empty()) {
            currentLine = word;
            continue;
        }

        int nextLength = static_cast<int>(currentLine.size() + 1 + word.size());
        if (nextLength <= width) {
            currentLine += " " + word;
        } else {
            lines.push_back(currentLine);
            currentLine = word;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    if (lines.empty()) {
        lines.push_back("");
    }

    return lines;
}

std::string padRight(const std::string& text, int width) {
    if (static_cast<int>(text.size()) >= width) return text;
    return text + std::string(width - static_cast<int>(text.size()), ' ');
}

std::string drawHorizontalLine(int width, const std::string& horizontalChar) {
    if (width <= 0) return "";

    std::string line;
    for (int i = 0; i < width; ++i) {
        line += horizontalChar;
    }
    return line;
}

std::vector<std::string> makePanel(int height, int innerWidth) {
    std::vector<std::string> panel(height, "│" + std::string(innerWidth, ' ') + "│");
    panel[0] = "┌" + drawHorizontalLine(innerWidth, "─") + "┐";
    panel[height - 1] = "└" + drawHorizontalLine(innerWidth, "─") + "┘";
    return panel;
}

void setPanelRow(std::vector<std::string>& panel,
                 int row,
                 int height,
                 int innerWidth,
                 const std::string& text) {
    if (row < 1 || row >= height - 1) return;
    panel[row] = "│" + padRight(clipText(text, innerWidth), innerWidth) + "│";
}

void printSpace(int num) {
    for (int i = 0; i < num; ++i) {
        std::cout << "\n";
    }
}

std::vector<std::string> cardBoxes(int slotNumber, const Card* card, bool selected, int boxWidth) {
    std::vector<std::string> box;
    int innerWidth = std::max(8, boxWidth - 2);

    std::string borderHorizontalChar = selected ? "═" : "─";
    std::string borderVerticalChar = selected ? "║" : "│";
    std::string cornerTopLeft = selected ? "╔" : "┌";
    std::string cornerTopRight = selected ? "╗" : "┐";
    std::string cornerBottomLeft = selected ? "╚" : "└";
    std::string cornerBottomRight = selected ? "╝" : "┘";

    std::string slotLabel = "SLOT " + std::to_string(slotNumber);
    if (selected) {
        slotLabel = ">" + slotLabel;
    }

    std::string horizontalLine = drawHorizontalLine(innerWidth, borderHorizontalChar);

    box.push_back(cornerTopLeft + horizontalLine + cornerTopRight);
    box.push_back(borderVerticalChar + std::string(innerWidth, ' ') + borderVerticalChar);
    box.push_back(borderVerticalChar + centerText(slotLabel, innerWidth) + borderVerticalChar);

    if (card != nullptr) {
        box.push_back(borderVerticalChar + centerText(clipText(card->getName(), innerWidth), innerWidth) + borderVerticalChar);
        box.push_back(borderVerticalChar + centerText("TYPE: " + cardType(*card), innerWidth) + borderVerticalChar);
    } else {
        box.push_back(borderVerticalChar + centerText("EMPTY CARD", innerWidth) + borderVerticalChar);
        box.push_back(borderVerticalChar + "  " + drawHorizontalLine(innerWidth - 4, "─") + "  " + borderVerticalChar);
    }

    box.push_back(borderVerticalChar + std::string(innerWidth, ' ') + borderVerticalChar);
    box.push_back(cornerBottomLeft + horizontalLine + cornerBottomRight);
    return box;
}
}  // namespace

void BattleUI::addLog(const std::string& message) {
    logs.push_back(message);
    if (static_cast<int>(logs.size()) > maxLogs) {
        logs.erase(logs.begin());
    }
}

void BattleUI::render(const Player& player,
                      const Enemy& enemy,
                      int level,
                      int round,
                      int selectedCard,
                      const std::string& phase,
                      const std::string& helper) const {
    clearScreen();

    int terminalWidth = std::max(100, getTerminalWidth());
    const int columnGap = 4;
    int topLeftWidth = std::max(40, terminalWidth * 45 / 100);
    int topRightWidth = terminalWidth - topLeftWidth - columnGap;
    if (topRightWidth < 38) {
        topRightWidth = 38;
        topLeftWidth = terminalWidth - topRightWidth - columnGap;
    }

    const int topSectionLineHeight = 16;
    const int bottomSectionLineHeight = 20;

    const int topLeftInner = topLeftWidth - 2;
    const int topRightInner = topRightWidth - 2;
    const int bottomInner = terminalWidth - 2;

    std::vector<std::string> topLeftSection = makePanel(topSectionLineHeight, topLeftInner);
    std::vector<std::string> topRightSection = makePanel(topSectionLineHeight, topRightInner);
    std::vector<std::string> bottomSection = makePanel(bottomSectionLineHeight, bottomInner);

    std::string skillList = "SKILLS: ";
    if (player.skills.empty()) {
        skillList += "None";
    } else {
        for (size_t i = 0; i < player.skills.size(); ++i) {
            if (i > 0) skillList += ", ";
            skillList += player.skills[i].name;
        }
    }

    const int playerBarIndent = 12;
    const std::string playerBarLine = std::string(playerBarIndent, ' ') + healthBar(player.max_hp, player.hp);
    const std::string enemyBarLine = std::string(playerBarIndent, ' ') + healthBar(enemy.max_hp, enemy.hp);

    for (int row = 1; row < topSectionLineHeight - 1; ++row) {
        switch (row) {
            case 1:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner,
                            centerText("Battle Status", topLeftInner));
                break;
            case 3:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner,
                            "LEVEL: " + std::to_string(level) + "  ROUND: " + std::to_string(round));
                break;
            case 4:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner, "PHASE: " + phase);
                break;
            case 5:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner, "ENEMY: " + enemy.name);
                break;
            case 7:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner,
                            "PLAYER HP: " + std::to_string(player.hp) + "/" + std::to_string(player.max_hp));
                break;
            case 8: {
                int visibleLen = playerBarIndent + (2 * player.max_hp);
                int trailingSpaces = std::max(0, topLeftInner - visibleLen);
                topLeftSection[row] = "│" + playerBarLine + std::string(trailingSpaces, ' ') + "│";
                break;
            }
            case 9:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner,
                            "ENEMY  HP: " + std::to_string(enemy.hp) + "/" + std::to_string(enemy.max_hp));
                break;
            case 10: {
                int visibleLen = playerBarIndent + (2 * enemy.max_hp);
                int trailingSpaces = std::max(0, topLeftInner - visibleLen);
                topLeftSection[row] = "│" + enemyBarLine + std::string(trailingSpaces, ' ') + "│";
                break;
            }
            case 12:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner,
                            "HAND: " + std::to_string(player.hand.size()) + " cards");
                break;
            case 13:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner,
                            "STRIKE REMAINING: " +
                                std::to_string(std::max(0, player.max_sha_per_turn - player.current_sha_used)));
                break;
            case 14:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner, skillList);
                break;
            default:
                setPanelRow(topLeftSection, row, topSectionLineHeight, topLeftInner, "");
                break;
        }
    }

    setPanelRow(topRightSection, 1, topSectionLineHeight, topRightInner, centerText("BATTLE LOG", topRightInner));

    std::vector<std::string> fittedLogLines;
    for (size_t i = 0; i < logs.size(); ++i) {
        std::vector<std::string> wrappedLines = wrapText("- " + logs[i], topRightInner);
        for (size_t j = 0; j < wrappedLines.size(); ++j) {
            fittedLogLines.push_back(wrappedLines[j]);
        }
    }

    int logStartLine = 2;
    int visibleLogRows = topSectionLineHeight - 3;
    int fittingStartIndex = std::max(0, static_cast<int>(fittedLogLines.size()) - visibleLogRows);
    for (int i = fittingStartIndex; i < static_cast<int>(fittedLogLines.size()) && logStartLine < topSectionLineHeight - 1; ++i) {
        setPanelRow(topRightSection, logStartLine, topSectionLineHeight, topRightInner, fittedLogLines[i]);
        ++logStartLine;
    }

    std::string cardSectionTitle = "Cards: Left/Right Select | Enter to Play | E to End Turn | SPACE to Pause";
    if (player.hasSkill("Sacrifice")) {
        cardSectionTitle += " | Q to Sacrifice";
    }
    setPanelRow(bottomSection, 1, bottomSectionLineHeight, bottomInner, cardSectionTitle);

    int bRow = 2;
    const int slotCount = 8;
    const int slotCols = 4;
    const int slotRows = 2;
    const int slotGap = 2;
    const int slotHeight = 7;
    int slotWidth = std::max(12, (bottomInner - (slotCols - 1) * slotGap) / slotCols);

    setPanelRow(bottomSection, bRow++, bottomSectionLineHeight, bottomInner, "Your Hand:");

    if (player.hand.empty()) {
        setPanelRow(bottomSection, bRow++, bottomSectionLineHeight, bottomInner, "Empty Hand");
    } else {
        for (int gridRow = 0; gridRow < slotRows; ++gridRow) {
            std::vector<std::vector<std::string>> boxRow;
            for (int col = 0; col < slotCols; ++col) {
                int slotIndex = gridRow * slotCols + col;
                const Card* cardPtr = (slotIndex < static_cast<int>(player.hand.size())) ? player.hand[slotIndex] : nullptr;
                bool selected = (slotIndex == selectedCard);
                boxRow.push_back(cardBoxes(slotIndex + 1, cardPtr, selected, slotWidth));
            }

            for (int line = 0; line < slotHeight && bRow < bottomSectionLineHeight - 2; ++line) {
                std::string rowLine;
                for (int col = 0; col < slotCols; ++col) {
                    rowLine += boxRow[col][line];
                    if (col < slotCols - 1) {
                        rowLine += std::string(slotGap, ' ');
                    }
                }
                int expectedDisplayWidth = slotCols * slotWidth + (slotCols - 1) * slotGap;
                if (expectedDisplayWidth < bottomInner) {
                    rowLine += std::string(bottomInner - expectedDisplayWidth, ' ');
                }
                bottomSection[bRow++] = "│" + rowLine + "│";
            }

            if (gridRow + 1 < slotRows && bRow < bottomSectionLineHeight - 2) {
                setPanelRow(bottomSection, bRow++, bottomSectionLineHeight, bottomInner, "");
            }
        }
    }

    if (static_cast<int>(player.hand.size()) > slotCount && bRow < bottomSectionLineHeight - 2) {
        setPanelRow(bottomSection, bRow++, bottomSectionLineHeight, bottomInner, "Only first 8 cards shown.");
    }

    setPanelRow(bottomSection, bottomSectionLineHeight - 2, bottomSectionLineHeight, bottomInner, helper);

    printSpace(2);

    for (int i = 0; i < topSectionLineHeight; ++i) {
        std::cout << topLeftSection[i] << std::string(columnGap, ' ') << topRightSection[i] << "\n";
    }
    std::cout << "\n";
    for (int i = 0; i < bottomSectionLineHeight; ++i) {
        std::cout << bottomSection[i] << "\n";
    }
}
