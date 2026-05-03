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
        int r = rand() % 6;
        switch (r) {
            case 0: return {0, "Steel Cavalry", "When enemy use【Dodge】to avoid damage，there is a 50% chance that your【Strike】will hit anyway."};
            case 1: return {1, "Warscream", "You can use 1 extra【Strike】each round."};
            case 2: return {2, "Sacrifice", "Active Skill：Lose 1 hp and claim 2 new cards."};
            case 3: return {3, "Temperance", "If you haven't use any【Strike】in your round，you can skip the discard session for this round."};
            case 4: return {4, "Ambition", "When you takes 1 damage，you can claim 1 card."};
            case 5: return {5, "Dragon Gut", "You can use【Dodge】as【Strike】，use【Strike】as【Dodge】."};
            default: return {-1, "None", "Not picking up any skills,No effect."};
        }
    }

    static Skill getSkillById(int id) {
        switch (id) {
            case 0: return {0, "Steel Cavalry", "When enemy use【Dodge】to avoid damage，there is a 50% chance that your【Strike】will hit anyway."};
            case 1: return {1, "Warscream", "You can use 1 extra【Strike】each round."};
            case 2: return {2, "Sacrifice", "Active Skill：Lose 1 hp and claim 2 new cards."};
            case 3: return {3, "Temperance", "If you haven't use any【Strike】in your round，you can skip the discard session for this round."};
            case 4: return {4, "Ambition", "When you takes 1 damage，you can claim 1 card."};
            case 5: return {5, "Dragon Gut", "You can use【Dodge】as【Strike】，use【Strike】as【Dodge】."};
            default: return {-1, "None", "Not picking up any skills, No effect."};
        }
    }
};
