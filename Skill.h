#pragma once
#include <string>
#include <cstdlib>
#include <ctime>

struct Skill {
    int id;
    std::string name;
    std::string desc;

    Skill() : id(-1), name(""), desc("") {}
    Skill(int i, std::string n, std::string d) : id(i), name(n), desc(d) {}

    static Skill getRandomSkill() {
        // Temperance (id 3) removed; pool is five skills
        static const int pool[] = {0, 1, 2, 4, 5};
        return getSkillById(pool[rand() % 5]);
    }

    static Skill getSkillById(int id) {
        switch (id) {
            case 0:
                return {0, "Steel Cavalry",
                        "Your【Strike】always hits; enemy【Dodge】cannot prevent damage."};
            case 1:
                return {1, "Warscream", "You can use 2 extra【Strike】actions each round."};
            case 2:
                return {2, "Sacrifice", "Active Skill：Lose 1 hp and claim 3 new cards."};
            case 3:
                // Deprecated (former Temperance); saves may still list id 3 — treated as none
                return {-1, "None", "No effect."};
            case 4:
                return {4, "Ambition", "When you take damage from an enemy attack, claim 2 cards."};
            case 5:
                return {5, "Dragon Gut", "You can use【Dodge】as【Strike】，use【Strike】as【Dodge】."};
            default:
                return {-1, "None", "No effect."};
        }
    }
};
