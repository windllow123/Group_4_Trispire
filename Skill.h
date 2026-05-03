#pragma once
#include <string>
#include <cstdlib>
#include <ctime>

// Represents a skill that can be acquired by the player
struct Skill {
    int id;
    std::string name;
    std::string desc;

    // Function: Default constructor, creates an empty skill with no effect
    Skill() : id(-1), name(""), desc("") {}
    
    // Function: Constructs a skill with given id, name, and description
    // Input: i - skill id number
    // Input: n - skill name
    // Input: d - skill description text
    Skill(int i, std::string n, std::string d) : id(i), name(n), desc(d) {}

    // Function: Returns a random skill from the available pool (5 skills, Temperance removed)
    // Output: a randomly selected Skill object
    static Skill getRandomSkill() {
        static const int pool[] = {0, 1, 2, 4, 5};
        return getSkillById(pool[rand() % 5]);
    }

    // Function: Returns a skill by its ID number
    // Input: id - the skill ID to look up (0-5, id 3 is deprecated Temperance)
    // Output: Skill object matching the ID, or empty skill with "None" name if not found
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
