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