#include <iostream>
#include <string>
using namespace std;

// ---------------- WOLF STATS ----------------
struct Wolf {
    int health;
    int hunger;
    int energy;

    Wolf() {
        health = 100;
        hunger = 0;
        energy = 100;
    }
};

// ---------------- STORY NODE ----------------
struct StoryNode {
    int id;
    string description;
    string choiceA;
    string choiceB;
    bool isEnding;

    StoryNode* left;   // Choice A
    StoryNode* right;  // Choice B

    StoryNode(int i, string d, string a, string b, bool end) {
        id = i;
        description = d;
        choiceA = a;
        choiceB = b;
        isEnding = end;
        left = NULL;
        right = NULL;
    }
};

// ---------------- GAME ENGINE ----------------
struct GameEngine {
    Wolf player;
    StoryNode* root;
    StoryNode* current;

    GameEngine() {
        root = NULL;
        current = NULL;
    }

    void init() {

        // -------- EARLY GAME --------
        StoryNode* n1 = new StoryNode(1,
            "Echoes After the Storm\nSnow buries the forest. Your pack is gone.",
            "Follow blood scent",
            "Howl & search territory",
            false);

        StoryNode* n2 = new StoryNode(2,
            "Frozen Stream\nIce cracks under your paws.",
            "Cross ice",
            "Take longer path",
            false);

        StoryNode* n3 = new StoryNode(3,
            "Fading Paw Prints\nOld wolf tracks barely remain.",
            "Track prints",
            "Rest briefly",
            false);

        StoryNode* n4 = new StoryNode(4,
            "Cold Wounds\nFreezing water numbs your limbs.",
            "Treat wounds",
            "Ignore pain",
            false);

        StoryNode* n5 = new StoryNode(5,
            "Hunger Without Reward\nThe prey is gone.",
            "Search for prey",
            "Rest",
            false);

        StoryNode* n6 = new StoryNode(6,
            "Lone Gray Wolf\nAnother wolf watches you.",
            "Approach calmly",
            "Avoid",
            false);

        StoryNode* n7 = new StoryNode(7,
            "Night Beneath Open Sky\nSleep is dangerous.",
            "Stay alert",
            "Sleep",
            false);

        StoryNode* n8 = new StoryNode(8,
            "Slow Healing\nRecovery is painful and slow.",
            "Rest longer",
            "Move on",
            false);

        StoryNode* n9 = new StoryNode(9,
            "Weak Prey\nA rabbit darts through the snow.",
            "Chase prey",
            "Ignore",
            false);

        StoryNode* n10 = new StoryNode(10,
            "Pack Beginnings\nTrust begins to form.",
            "Lead patiently",
            "Assert dominance",
            false);

        StoryNode* n11 = new StoryNode(11,
            "Sudden Ambush\nClaws strike from darkness.",
            "Fight",
            "Flee",
            false);

        StoryNode* n12 = new StoryNode(12,
            "Hunter's Scent\nMetal and smoke fill the air.",
            "Avoid area",
            "Investigate",
            false);

        StoryNode* n13 = new StoryNode(13,
            "Growing Pack\nMore wolves join you.",
            "Expand territory",
            "Focus on feeding",
            false);

        StoryNode* n14 = new StoryNode(14,
            "Leadership Challenge\nA rival steps forward.",
            "Fight for leadership",
            "Step aside",
            false);

        StoryNode* n15 = new StoryNode(15,
            "Quiet Crossing\nSafer land, but hunger remains.",
            "Keep moving",
            "Rest",
            false);

        // -------- ENDINGS --------
        StoryNode* e16 = new StoryNode(16,
            "ENDING: Killed by Hunters\nA gunshot ends everything.",
            "",
            "",
            true);

        StoryNode* e17 = new StoryNode(17,
            "ENDING: Alpha of the North\nYou rule with your pack.",
            "",
            "",
            true);

        StoryNode* e18 = new StoryNode(18,
            "ENDING: Lone Wanderer\nYou survive alone.",
            "",
            "",
            true);

        StoryNode* e19 = new StoryNode(19,
            "ENDING: Death by Starvation\nHunger wins.",
            "",
            "",
            true);

        StoryNode* e20 = new StoryNode(20,
            "ENDING: Heroic Sacrifice\nYou die defending the pack.",
            "",
            "",
            true);

        StoryNode* e21 = new StoryNode(21,
            "ENDING: Death in Battle\nBlood stains the snow.",
            "",
            "",
            true);

        StoryNode* e22 = new StoryNode(22,
            "ENDING: Broken Alpha\nVictory without loyalty.",
            "",
            "",
            true);

        StoryNode* e23 = new StoryNode(23,
            "ENDING: Peaceful Survival\nStability over dominance.",
            "",
            "",
            true);

        StoryNode* e24 = new StoryNode(24,
            "ENDING: Exhausted Collapse\nYour body fails.",
            "",
            "",
            true);

        StoryNode* e25 = new StoryNode(25,
            "ENDING: Fatal Hunt\nWeakness costs your life.",
            "",
            "",
            true);

        StoryNode* e26 = new StoryNode(26,
            "ENDING: Frozen Night\nYou never wake up.",
            "",
            "",
            true);

        // -------- TREE CONNECTIONS (MATCHES SKETCH) --------
        n1->left = n2;     n1->right = n3;
        n2->left = n4;     n2->right = n5;
        n3->left = n6;     n3->right = n7;
        n4->left = n8;     n4->right = n9;
        n5->left = n9;     n5->right = n7;
        n6->left = n10;    n6->right = n7;
        n7->left = n11;    n7->right = e26;
        n8->left = n9;     n8->right = n6;
        n9->left = n12;    n9->right = e19;
        n10->left = n13;  n10->right = n14;
        n11->left = e21;  n11->right = n9;
        n12->left = n15;  n12->right = e16;
        n13->left = e17;  n13->right = e23;
        n14->left = e22;  n14->right = e18;
        n15->left = e18;  n15->right = n9;

        root = n1;
        current = root;
    }

    void makeChoice(int choice) {
        if (choice == 1 && current->left)
            current = current->left;
        else if (choice == 2 && current->right)
            current = current->right;

        player.hunger += 5;
        player.energy -= 5;

        if (player.hunger >= 100 || player.energy <= 0)
            current = new StoryNode(99, "ENDING: You collapse from exhaustion.", "", "", true);
    }
};

// ---------------- MAIN ----------------
int main() {
    GameEngine game;
    game.init();

    while (!game.current->isEnding) {
        cout << "\n----------------------------\n";
        cout << game.current->description << endl;

        cout << "\nHealth: " << game.player.health
             << " | Hunger: " << game.player.hunger
             << " | Energy: " << game.player.energy << endl;

        cout << "\n1. " << game.current->choiceA << endl;
        cout << "2. " << game.current->choiceB << endl;
        cout << "> ";

        int choice;
        cin >> choice;

        game.makeChoice(choice);
    }

    cout << "\n----------------------------\n";
    cout << game.current->description << endl;
    cout << "\nGAME OVER\n";
    return 0;
}
