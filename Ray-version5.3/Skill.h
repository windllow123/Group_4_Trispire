#pragma once
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

struct Skill {
    int id;
    std::string name;
    std::string desc;

    Skill() : id(-1), name(""), desc("") {}
    Skill(int i, std::string n, std::string d) : id(i), name(n), desc(d) {}

    // 获取一个随机技能（用于兼容旧代码，但主流程将改用三选一）
    static Skill getRandomSkill() {
        std::vector<int> ids = {0,1,2,4,5,6,7,8,9,10,11,12}; // 不含克己(ID3)
        int r = rand() % ids.size();
        return getSkillById(ids[r]);
    }

    // 获取 count 个不重复的随机技能
    static std::vector<Skill> getRandomSkills(int count) {
        std::vector<int> allIds = {0,1,2,4,5,6,7,8,9,10,11,12};
        std::shuffle(allIds.begin(), allIds.end(), std::mt19937{std::random_device{}()});
        std::vector<Skill> result;
        for (int i = 0; i < count && i < (int)allIds.size(); ++i) {
            result.push_back(getSkillById(allIds[i]));
        }
        return result;
    }

    static Skill getSkillById(int id) {
        switch (id) {
            case 0: return {0, "Steel Cavalry", "When enemy use【Dodge】to avoid damage, 50% chance your【Strike】still hits."};
            case 1: return {1, "Warscream", "You can use 1 extra【Strike】each round."};
            case 2: return {2, "Sacrifice", "Active: Lose 1 hp, draw 2 cards."};
            case 4: return {4, "Ambition", "When you take 1 damage, draw 1 card."};
            case 5: return {5, "Dragon Gut", "You can use【Dodge】as【Strike】, and【Strike】as【Dodge】."};
            case 6: return {6, "Vampire", "Each time you deal damage with【Strike】, heal 1 hp (once per turn)."};
            case 7: return {7, "Thorns", "When you take damage, deal 1 unavoidable damage to enemy."};
            case 8: return {8, "Blitz", "On first round of each battle, draw 2 extra cards and gain +1【Strike】this round."};
            case 9: return {9, "Tenacity", "Every 2 HP you lose, draw 1 card."};
            case 10: return {10, "Desperation", "When HP below 30% and hand empty, shuffle discard pile and draw 3 cards (once per battle)."};
            case 11: return {11, "Composure", "Hand size limit increased by 2."};
            case 12: return {12, "Rally", "When you use【Heal】, draw 1 extra card."};
            default: return {-1, "None", "No effect."};
        }
    }
};