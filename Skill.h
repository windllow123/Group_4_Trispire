#pragma once
#include <string>
#include <cstdlib>
#include <ctime>

struct Skill {
    std::string name;
    std::string desc;

    static Skill getRandomSkill() {
        int r = rand() % 6;
        switch (r) {
            case 0: return {"Steel Cavalry", "When enemy use【Dodge】to avoid damage，there is a 50% chance that your【Strike】will hit anyway."};
            case 1: return {"Warscream", "You can use 1 extra【Strike】each round."};
            case 2: return {"Sacrifice", "Active Skill：Lose 1 hp and claim 2 new cards."};
            case 3: return {"Temperance", "If you haven't use any【Strike】in your round，you can skip the discard session for this round."};
            case 4: return {"Ambition", "When you takes 1 damage，you can claim 1 card."};
            case 5: return {"Dragon Gut", "You can use【Dodge】as【Strike】，use【Strike】as【Dodge】."};
            default: return {"None", "No effect."};
        }
    }
};
