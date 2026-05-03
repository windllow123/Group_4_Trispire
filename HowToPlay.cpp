#include "HowToPlay.h"
#include "ConsoleUtils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void HowToPlay() {
    std::ifstream fin("README.txt");
    if (!fin) {
        std::cout << "Unable to open README.txt" << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.find("Battle: Arrow Left/Right to move selection among visible hand slots") != std::string::npos) {
            lines.push_back(line);
            break;
        }
        lines.push_back(line);
    }
    fin.close();

    const int pageSize = 15;
    int pageIndex = 0;
    const int pageCount = std::max(1, static_cast<int>((lines.size() + pageSize - 1) / pageSize));

    while (true) {
        clearScreen();
        int start = pageIndex * pageSize;
        int end = std::min(start + pageSize, static_cast<int>(lines.size()));

        for (int i = start; i < end; ++i) {
            std::cout << lines[i] << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Use the <- or -> to scroll through. Press Esc or Space to exit." << std::endl;
        std::cout << "Page " << (pageIndex + 1) << " / " << pageCount << std::endl;

        int key = getKey();
        if (key == right_key_idx) {
            pageIndex = (pageIndex + 1) % pageCount;
        } else if (key == left_key_idx) {
            pageIndex = (pageIndex + pageCount - 1) % pageCount;
        } else if (key == esc_key_idx || key == space_key_idx) {
            break;
        }
    }
}