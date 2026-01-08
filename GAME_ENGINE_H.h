#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm> // Added for min/max

using namespace std;

// --- STRUCTS ---
struct Wolf {
    int health = 100;
    int hunger = 0;
    int energy = 100;
};

struct Item {
    string name;
    string type;
    int effect;
    Item* next;
};

struct StoryNode {
    int id;
    string description;
    string choiceA;
    string choiceB;
    StoryNode* left;
    StoryNode* right;
    bool isEnding;
};

struct GameState {
    Wolf savedWolf;
    int savedNodeId;
    Item* savedInventory;
    GameState* next;
};

struct GameEvent {
    string description;
    int priority;
    int healthEffect;
};

struct CompareEvent {
    bool operator()(GameEvent const& e1, GameEvent const& e2) {
        return e1.priority > e2.priority;
    }
};

// --- THE ENGINE CLASS ---
struct GameEngine {
    // DATA
    Wolf player;
    Item* inventoryHead = nullptr;
    StoryNode* root = nullptr;
    StoryNode* current = nullptr;
    GameState* stackTop = nullptr;
    priority_queue<GameEvent, vector<GameEvent>, CompareEvent> eventQueue;
    
    string currentMessage = ""; 
    bool eventActive = false;
    GameEvent activeEvent;

    // --- NEW INVENTORY FUNCTIONS ---

    void useItem(string itemName) {
        if (!inventoryHead) {
            currentMessage = "Your pack is empty.";
            return;
        }
        Item* curr = inventoryHead;
        Item* prev = nullptr;
        while (curr) {
            if (curr->name == itemName) {
                if (curr->type == "Food") player.hunger = max(0, player.hunger - curr->effect);
                else if (curr->type == "Medical") player.health = min(100, player.health + curr->effect);
                
                if (!prev) inventoryHead = curr->next;
                else prev->next = curr->next;
                delete curr;
                currentMessage = "Used " + itemName;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    string getInventoryString() {
        if (!inventoryHead) return "Pack: Empty";
        string s = "Pack: ";
        Item* t = inventoryHead;
        while (t) {
            s += "[" + t->name + "] ";
            t = t->next;
        }
        return s;
    }

    // --- HELPER FUNCTIONS ---

    StoryNode* createNode(int id, string desc, string a, string b, bool end) {
        StoryNode* node = new StoryNode;
        node->id = id; node->description = desc; node->choiceA = a; node->choiceB = b;
        node->left = nullptr; node->right = nullptr; node->isEnding = end;
        return node;
    }

    void addItem(string n, string t, int e) {
        Item* newItem = new Item;
        newItem->name = n; newItem->type = t; newItem->effect = e; newItem->next = nullptr;
        if (!inventoryHead) inventoryHead = newItem;
        else {
            Item* temp = inventoryHead;
            while (temp->next) temp = temp->next;
            temp->next = newItem;
        }
        currentMessage = "Found: " + n;
    }

    Item* cloneInventory(Item* originalHead) {
        if (!originalHead) return nullptr;
        Item* newHead = new Item(*originalHead);
        newHead->next = nullptr;
        Item* currOrig = originalHead->next;
        Item* currNew = newHead;
        while (currOrig) {
            currNew->next = new Item(*currOrig);
            currNew->next->next = nullptr;
            currNew = currNew->next;
            currOrig = currOrig->next;
        }
        return newHead;
    }

    void saveGame() {
        GameState* newState = new GameState;
        newState->savedWolf = player;
        newState->savedNodeId = current->id;
        newState->savedInventory = cloneInventory(inventoryHead);
        newState->next = stackTop;
        stackTop = newState;
    }

    void undoGame() {
        if (!stackTop) { currentMessage = "Nothing to undo!"; return; }
        GameState* state = stackTop;
        stackTop = stackTop->next;
        player = state->savedWolf;
        inventoryHead = state->savedInventory;
        
        // Manual search logic for your tree
        if (state->savedNodeId == 1) current = root;
        else if (state->savedNodeId == 2) current = root->left;
        else if (state->savedNodeId == 3) current = root->right;
        else if (state->savedNodeId == 4) current = root->left->left;
        else if (state->savedNodeId == 5) current = root->left->right;
        else if (state->savedNodeId == 6) current = root->right->left;
        else if (state->savedNodeId == 7) current = root->right->right;
        // ... (can add more ID checks here)

        delete state;
        currentMessage = "Time rewound!";
    }

    // --- INITIALIZATION (YOUR FULL 20+ NODES) ---
    void init() {

        // -------- MAIN TREE --------
        StoryNode* n1 = new StoryNode(1,
            "Echoes After the Storm\nThe storm has ended.\nSnow covers the forest floor.\nThe wolf stands alone.\nThe pack is gone.\nCold and hunger slowly grow.",
            "Follow blood scent",
            "Howl and search",
            false);

        StoryNode* n2 = new StoryNode(2,
            "The Frozen Stream\nA frozen stream blocks the path.\nThe ice cracks under weight.\nOne mistake could mean death.",
            "Cross the ice",
            "Take long path",
            false);

        StoryNode* n3 = new StoryNode(3,
            "Fading Paw Prints\nOld wolf tracks appear in the snow.\nThey are faint and disappearing.\nHope remains, but danger follows.",
            "Track prints",
            "Rest",
            false);

        StoryNode* n4 = new StoryNode(4,
            "Cold Wounds\nThe ice breaks beneath the wolf.\nFreezing water burns the skin.\nMovement becomes slow and painful.",
            "Heal wounds",
            "Ignore pain",
            false);

        StoryNode* n5 = new StoryNode(5,
            "Hunger Without Reward\nThe hunt fails.\nNo prey is found.\nThe stomach tightens with hunger.",
            "Hunt again",
            "Rest",
            false);

        StoryNode* n6 = new StoryNode(6,
            "See Grey Wolf\nAnother lone wolf appears.\nEyes watch carefully from a distance.\nTrust is uncertain.",
            "Calm approach",
            "Avoid and move",
            false);

        StoryNode* n7 = new StoryNode(7,
            "Night Beneath Open Sky\nDarkness spreads across the land.\nCold grows stronger.\nSleeping could be dangerous.",
            "Stay alert",
            "Sleep",
            false);

        StoryNode* n8 = new StoryNode(8,
            "Slow Healing\nThe wolf rests quietly.\nPain fades slowly.\nStaying too long may attract predators.",
            "Rest longer",
            "Move before predators",
            false);

        StoryNode* n9 = new StoryNode(9,
            "Weak Prey\nA small rabbit runs through the snow.\nIt is fast and alert.\nThe hunt is risky.",
            "Chase prey",
            "Ignore prey",
            false);

        StoryNode* n10 = new StoryNode(10,
            "Beginnings of Pack\nThe grey wolf stays close.\nTrust begins to form.\nLeadership will soon be tested.",
            "Lead calmly",
            "Assert dominance",
            false);

        StoryNode* n11 = new StoryNode(11,
            "Sudden Ambush\nAn enemy attacks from the shadows.\nClaws and teeth clash suddenly.",
            "Fight",
            "Run away",
            false);

        StoryNode* n12 = new StoryNode(12,
            "After Scent of Hunters\nThe air smells of metal and smoke.\nHumans are nearby.\nFew wolves survive this danger.",
            "Avoid area",
            "Investigate area",
            false);

        StoryNode* n13 = new StoryNode(13,
            "ENDING: Death by Hunger\nNo food is found.\nStrength fades.\nThe body finally gives up.",
            "",
            "",
            true);

        StoryNode* n14 = new StoryNode(14,
            "Growing Pack\nMore wolves gather together.\nThey look for guidance and strength.",
            "Expand territory",
            "Feed and stabilize",
            false);

        StoryNode* n15 = new StoryNode(15,
            "Leadership Challenge\nA strong rival steps forward.\nThe pack watches closely.\nBlood may decide leadership.",
            "Fight rival",
            "Step aside",
            false);

        StoryNode* n16 = new StoryNode(16,
            "ENDING: Death in Weak Battle\nWounds slow movement.\nThe wolf falls in combat.",
            "",
            "",
            true);

        StoryNode* n17 = new StoryNode(17,
            "Seek Another Weak Prey\nHunger returns.\nDesperation grows.\nRisk increases with every step.",
            "Chase prey",
            "Ignore and move",
            false);

        StoryNode* n18 = new StoryNode(18,
            "Avoid Area\nThe wolf moves carefully through the land.\nDanger is everywhere.",
            "Chase prey",
            "Ignore prey",
            false);

        StoryNode* n19 = new StoryNode(19,
            "ENDING: Death by Hunters\nA gunshot breaks the silence.\nThe journey ends in blood and snow.",
            "",
            "",
            true);

        StoryNode* n20 = new StoryNode(20,
            "ENDING: Alpha Wolf Survives\nThe pack remains strong.\nThe land is protected.",
            "",
            "",
            true);

        StoryNode* n21 = new StoryNode(21,
            "ENDING: Heroic Sacrifice\nLife is given to protect the pack.\nThe name is remembered.",
            "",
            "",
            true);

        StoryNode* n22 = new StoryNode(22,
            "ENDING: Strong Alpha\nLeadership is earned.\nLoyalty remains.",
            "",
            "",
            true);

        StoryNode* n23 = new StoryNode(23,
            "ENDING: Lone Survivor\nThe wolf survives alone.\nFreedom comes with loneliness.",
            "",
            "",
            true);

        StoryNode* n24 = new StoryNode(24,
            "ENDING: Endless Wanderer\nThe wolf roams the land alone.\nThe journey never truly ends.",
            "",
            "",
            true);

        StoryNode* n25 = new StoryNode(25,
            "ENDING: Fatal Hunt\nWeakness turns the hunt deadly.\nThe wolf does not survive.",
            "",
            "",
            true);

        StoryNode* n26 = new StoryNode(26,
            "ENDING: Frozen Hunger\nCold and hunger overcome the body.\nThe night never ends.",
            "",
            "",
            true);

        // -------- CONNECTIONS (UNCHANGED) --------
        n1->left = n2;   n1->right = n3;
        n2->left = n4;   n2->right = n5;
        n3->left = n6;   n3->right = n7;
        n4->left = n8;   n4->right = n9;
        n5->left = n9;   n5->right = n7;
        n6->left = n10;  n6->right = n11;
        n7->left = n9;   n7->right = n11;
        n8->left = n9;   n8->right = n6;
        n9->left = n12;  n9->right = n13;
        n10->left = n14; n10->right = n15;
        n11->left = n16; n11->right = n17;
        n12->left = n18; n12->right = n19;
        n14->left = n20; n14->right = n21;
        n15->left = n22; n15->right = n23;
        n17->left = n9;  n17->right = n24;
        n18->left = n25; n18->right = n26;

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
    }

    void makeChoice(int choice) {
        if (eventActive) { eventActive = false; return; }
        saveGame();
        if (choice == 1 && current->left) { current = current->left; player.energy -= 10; }
        else if (choice == 2 && current->right) { current = current->right; player.energy -= 5; }
        player.hunger += 5;

        // Inventory Triggers
        if (current->id == 8) addItem("Medical Herbs", "Medical", 30);
        if (current->id == 13) addItem("Fresh Venison", "Food", 40);
        if (current->id == 4) addItem("Scraps", "Food", 10);

        if ((rand() % 100) < 30) {
            GameEvent e = {"Sudden Snowstorm! -10 Health", 2, -10};
            eventQueue.push(e);
        }
        if (!eventQueue.empty()) {
            activeEvent = eventQueue.top();
            eventQueue.pop();
            player.health += activeEvent.healthEffect;
            eventActive = true;
        }
    }

    ~GameEngine() {
        while (inventoryHead) {
            Item* temp = inventoryHead;
            inventoryHead = inventoryHead->next;
            delete temp;
        }
    }
};

#endif#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm> // Added for min/max

using namespace std;

// --- STRUCTS ---
struct Wolf {
    int health = 100;
    int hunger = 0;
    int energy = 100;
};

struct Item {
    string name;
    string type;
    int effect;
    Item* next;
};

struct StoryNode {
    int id;
    string description;
    string choiceA;
    string choiceB;
    StoryNode* left;
    StoryNode* right;
    bool isEnding;
};

struct GameState {
    Wolf savedWolf;
    int savedNodeId;
    Item* savedInventory;
    GameState* next;
};

struct GameEvent {
    string description;
    int priority;
    int healthEffect;
};

struct CompareEvent {
    bool operator()(GameEvent const& e1, GameEvent const& e2) {
        return e1.priority > e2.priority;
    }
};

// --- THE ENGINE CLASS ---
struct GameEngine {
    // DATA
    Wolf player;
    Item* inventoryHead = nullptr;
    StoryNode* root = nullptr;
    StoryNode* current = nullptr;
    GameState* stackTop = nullptr;
    priority_queue<GameEvent, vector<GameEvent>, CompareEvent> eventQueue;
    
    string currentMessage = ""; 
    bool eventActive = false;
    GameEvent activeEvent;

    // --- NEW INVENTORY FUNCTIONS ---

    void useItem(string itemName) {
        if (!inventoryHead) {
            currentMessage = "Your pack is empty.";
            return;
        }
        Item* curr = inventoryHead;
        Item* prev = nullptr;
        while (curr) {
            if (curr->name == itemName) {
                if (curr->type == "Food") player.hunger = max(0, player.hunger - curr->effect);
                else if (curr->type == "Medical") player.health = min(100, player.health + curr->effect);
                
                if (!prev) inventoryHead = curr->next;
                else prev->next = curr->next;
                delete curr;
                currentMessage = "Used " + itemName;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    string getInventoryString() {
        if (!inventoryHead) return "Pack: Empty";
        string s = "Pack: ";
        Item* t = inventoryHead;
        while (t) {
            s += "[" + t->name + "] ";
            t = t->next;
        }
        return s;
    }

    // --- HELPER FUNCTIONS ---

    StoryNode* createNode(int id, string desc, string a, string b, bool end) {
        StoryNode* node = new StoryNode;
        node->id = id; node->description = desc; node->choiceA = a; node->choiceB = b;
        node->left = nullptr; node->right = nullptr; node->isEnding = end;
        return node;
    }

    void addItem(string n, string t, int e) {
        Item* newItem = new Item;
        newItem->name = n; newItem->type = t; newItem->effect = e; newItem->next = nullptr;
        if (!inventoryHead) inventoryHead = newItem;
        else {
            Item* temp = inventoryHead;
            while (temp->next) temp = temp->next;
            temp->next = newItem;
        }
        currentMessage = "Found: " + n;
    }

    Item* cloneInventory(Item* originalHead) {
        if (!originalHead) return nullptr;
        Item* newHead = new Item(*originalHead);
        newHead->next = nullptr;
        Item* currOrig = originalHead->next;
        Item* currNew = newHead;
        while (currOrig) {
            currNew->next = new Item(*currOrig);
            currNew->next->next = nullptr;
            currNew = currNew->next;
            currOrig = currOrig->next;
        }
        return newHead;
    }

    void saveGame() {
        GameState* newState = new GameState;
        newState->savedWolf = player;
        newState->savedNodeId = current->id;
        newState->savedInventory = cloneInventory(inventoryHead);
        newState->next = stackTop;
        stackTop = newState;
    }

    void undoGame() {
        if (!stackTop) { currentMessage = "Nothing to undo!"; return; }
        GameState* state = stackTop;
        stackTop = stackTop->next;
        player = state->savedWolf;
        inventoryHead = state->savedInventory;
        
        // Manual search logic for your tree
        if (state->savedNodeId == 1) current = root;
        else if (state->savedNodeId == 2) current = root->left;
        else if (state->savedNodeId == 3) current = root->right;
        else if (state->savedNodeId == 4) current = root->left->left;
        else if (state->savedNodeId == 5) current = root->left->right;
        else if (state->savedNodeId == 6) current = root->right->left;
        else if (state->savedNodeId == 7) current = root->right->right;
        // ... (can add more ID checks here)

        delete state;
        currentMessage = "Time rewound!";
    }

    // --- INITIALIZATION (YOUR FULL 20+ NODES) ---
    void init() {
        srand(time(0));
        root = createNode(1, "You stand at a fork. Blizzard approaching. LEFT: Cave. RIGHT: River.", "Enter Cave", "Follow River", false);
        root->left = createNode(2, "Warm cave, smells of rot. Go deep or stay shallow?", "Go Deep", "Stay Shallow", false);
        root->right = createNode(3, "River path. Brutal wind (-5 Energy). Thin ice ahead.", "Cross Ice", "Stay on Bank", false);
        root->left->left = createNode(4, "Grizzly sleeping on bones. Backpack nearby.", "Sneak & Steal", "Attack Bear", false);
        root->left->right = createNode(5, "Safe entrance. Strange scratches on the wall.", "Inspect Wall", "Just Sleep", false);
        root->right->left = createNode(6, "Ice creaks. Deer carcass halfway across.", "Grab & Run", "Crawl Slowly", false);
        root->right->right = createNode(7, "Hunter's trap line. Meat in a metal jaw.", "Try to take bait", "Ignore & Hunt", false);
        root->left->left->left = createNode(8, "You grab the bag. It contains Medical Herbs! Run?", "Run out!", "Hide", false);
        root->left->left->right = createNode(9, "The bear wakes up! It swipes at you.", "Dodge Left", "Bite Throat", false);
        root->left->right->left = createNode(10, "Loose rock hides a tunnel to an underground lake.", "Drink Water", "Swim Across", false);
        root->left->right->right = createNode(11, "You wake up hungrier than ever.", "Leave Cave", "Search Deeper", false);
        root->right->left->left = createNode(12, "Ice SHATTERS! You fall in.", "Swim to Deer", "Climb Out", false);
        root->right->left->right = createNode(13, "You reach the deer. Fresh meat!", "Eat Here", "Drag to Shore", false);
        root->right->right->left = createNode(14, "SNAP! Trap closes on your tail (-30 Health).", "Chew leg free", "Howl for help", false);
        root->right->right->right = createNode(15, "You smell a Wolf Pack nearby.", "Approach Pack", "Hide", false);
        root->left->left->left->left = createNode(16, "You escape with loot! (Victory - Survivor)", "", "", true);
        root->left->left->left->right = createNode(17, "The bear smells you. (Death)", "", "", true);
        root->left->left->right->left = createNode(18, "Dodged! You flee bleeding. (Survival - Injured)", "", "", true);
        root->left->left->right->right = createNode(19, "Bear is too strong. (Death)", "", "", true);
        root->left->right->left->left = createNode(20, "Water is magical! Fully Healed! (Victory)", "", "", true);
        root->left->right->left->right = createNode(21, "Something pulls you under... (Death)", "", "", true);

        current = root;
    }

    void makeChoice(int choice) {
        if (eventActive) { eventActive = false; return; }
        saveGame();
        if (choice == 1 && current->left) { current = current->left; player.energy -= 10; }
        else if (choice == 2 && current->right) { current = current->right; player.energy -= 5; }
        player.hunger += 5;

        // Inventory Triggers
        if (current->id == 8) addItem("Medical Herbs", "Medical", 30);
        if (current->id == 13) addItem("Fresh Venison", "Food", 40);
        if (current->id == 4) addItem("Scraps", "Food", 10);

        if ((rand() % 100) < 30) {
            GameEvent e = {"Sudden Snowstorm! -10 Health", 2, -10};
            eventQueue.push(e);
        }
        if (!eventQueue.empty()) {
            activeEvent = eventQueue.top();
            eventQueue.pop();
            player.health += activeEvent.healthEffect;
            eventActive = true;
        }
    }

    ~GameEngine() {
        while (inventoryHead) {
            Item* temp = inventoryHead;
            inventoryHead = inventoryHead->next;
            delete temp;
        }
    }
};

#endif