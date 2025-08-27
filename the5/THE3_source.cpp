#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <fstream>

using namespace std;


string to_lower(string s) {
    for (char& c : s) {
        c = tolower(static_cast<unsigned char>(c));
    }
    return s;
}

// Structure for menu items (CDLL node)
struct node {
    int id;
    string name;
    double rating;
    double price;
    node* next;
    node* prev;
    
    node(int i = 0, string n = "", double r = 0.0, double p = 0.0, node* nx = nullptr, node* pr = nullptr)
        : id(i), name(n), rating(r), price(p), next(nx), prev(pr) {}
};

// Function to insert at end of CDLL
void insertAtEnd(node*& last, int id, const string& name, double rating, double price) {
    node* newNode = new node(id, name, rating, price);
    
    if (last == nullptr) {
        last = newNode;
        last->next = last;
        last->prev = last;
    } else {
        node* first = last->next;
        newNode->next = first;
        newNode->prev = last;
        first->prev = newNode;
        last->next = newNode;
        last = newNode;
    }
}

// Function to get next ID
int getNextID(node* last) {
    if (last == nullptr) {
        return 1;
    }
    int max_id = 0;
    node* iter = last->next;
    do {
        if (iter->id > max_id) {
            max_id = iter->id;
        }
        iter = iter->next;
    } while (iter != last->next);
    return max_id + 1;
}

// Function to import menu from file (removed duplicate check as per user note)
void importFromFile(node*& last, const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "File not found: " << filename << endl << "---" << endl;
        return;
    }
    
    string line;
    while (getline(file, line)) {
        stringstream iss(line);
        string name;
        double rating, price;
        if (iss >> name >> rating >> price) {
            int id = getNextID(last);
            insertAtEnd(last, id, name, rating, price);
            cout << "The drink " << name << " is added to the menu." << endl;
        } else {
            cout << "Invalid line in file: " << line << endl;
        }
    }
    file.close();
    cout << "---" << endl;
}

// Function to find a drink by name (reused in takeOrder)
node* findDrink(node* last, const string& name) {
    if (last == nullptr) {
        return nullptr;
    }
    string norm_name = to_lower(name);
    node* iter = last->next;
    do {
        if (to_lower(iter->name) == norm_name) {
            return iter;
        }
        iter = iter->next;
    } while (iter != last->next);
    return nullptr;
}

// Function to show today's Starbucks menu
void showMenu(node* last) {
    if (last == nullptr) {
        cout << "The menu is empty." << endl << "---" << endl;
        return;
    }
    node* iter = last->next;
    do {
        cout << "ID: " << iter->id << endl;
        cout << "Name: " << iter->name << endl;
        cout << "Rating: " << iter->rating << endl;
        cout << "Price: " << iter->price << endl;
        cout << "---" << endl;
        iter = iter->next;
    } while (iter != last->next);
}

// Structure for individual drink items in an order (stack)
struct DrinkNode {
    string drinkName;
    int sugarPacks;
    DrinkNode* next;
    
    DrinkNode(string name, int sugar, DrinkNode* nxt = nullptr)
        : drinkName(name), sugarPacks(sugar), next(nxt) {}
};

// Structure for orders (queue)
struct OrderNode {
    int orderID;
    DrinkNode* stackTop;
    double totalPrice;
    OrderNode* next;
    
    OrderNode(int id, DrinkNode* top, double price, OrderNode* nxt = nullptr)
        : orderID(id), stackTop(top), totalPrice(price), next(nxt) {}
};

// Helper to delete a drink stack (reused in cancel and exit)
void deleteDrinkStack(DrinkNode* top) {
    DrinkNode* current = top;
    while (current != nullptr) {
        DrinkNode* temp = current;
        current = current->next;
        delete temp;
    }
}

// Function: Take an order
void takeOrder(node* menuLast, OrderNode*& front, OrderNode*& rear, int& currentOrderID) {
    DrinkNode* stackTop = nullptr;
    double total = 0.0;
    string input;
    
    cout << "Enter drink details (name sugar) one by one. Type 'enough' to stop." << endl;
    
    while (true) {
        cout << "Enter: ";
        getline(cin, input);
        if (to_lower(input) == "enough") {
            break;
        }
        stringstream iss(input);
        string name;
        int sugar;
        if (iss >> name >> sugar) {
            node* menuItem = findDrink(menuLast, name);
            if (menuItem != nullptr) {
                DrinkNode* newDrink = new DrinkNode(name, sugar, stackTop);
                stackTop = newDrink;
                total += menuItem->price;
            } else {
                cout << "Drink '" << name << "' not found. Try again." << endl;
            }
        } else {
            cout << "Invalid format. Enter name and sugar." << endl;
        }
    }
    
    if (stackTop == nullptr) {
        cout << "No drinks added." << endl << "---" << endl;
        return;
    }
    
    OrderNode* newOrder = new OrderNode(currentOrderID, stackTop, total);
    if (rear == nullptr) {
        front = rear = newOrder;
    } else {
        rear->next = newOrder;
        rear = newOrder;
    }
    
    cout << "Order No. " << currentOrderID << " added to the queue." << endl << "---" << endl;
    currentOrderID += 10;
}

// Function: Prepare an order (handles stats update)
void prepareOrder(OrderNode*& front, OrderNode*& rear, int& completedOrders, int& totalDrinksSold, int& totalSugarPacks, double& totalRevenue) {
    if (front == nullptr) {
        cout << "No orders to prepare." << endl << "---" << endl;
        return;
    }
    
    OrderNode* toPrepare = front;
    front = front->next;
    if (front == nullptr) {
        rear = nullptr;
    }
    
    cout << "Preparing Order No. " << toPrepare->orderID << endl;
    cout << "Total price: " << toPrepare->totalPrice << endl;
    cout << "Drinks (LIFO):" << endl;
    
    DrinkNode* drink = toPrepare->stackTop;
    int drinksInOrder = 0;
    int sugarInOrder = 0;
    while (drink != nullptr) {
        cout << drink->drinkName << " with " << drink->sugarPacks << " sugar packs" << endl;
        sugarInOrder += drink->sugarPacks;
        drinksInOrder++;
        DrinkNode* temp = drink;
        drink = drink->next;
        delete temp;
    }
    totalDrinksSold += drinksInOrder;
    totalSugarPacks += sugarInOrder;
    totalRevenue += toPrepare->totalPrice;
    completedOrders++;
    
    delete toPrepare;
    cout << "---" << endl;
}

// Function: Print orders preparation queue (traverses queue and stacks)
void printOrdersPreparationQueue(OrderNode* front) {
    if (front == nullptr) {
        cout << "No orders in the queue." << endl << "---" << endl;
        return;
    }
    
    OrderNode* iter = front;
    cout << "Current orders:" << endl;
    while (iter != nullptr) {
        cout << "Order No. " << iter->orderID << endl;
        cout << "Drinks:" << endl;
        DrinkNode* drink = iter->stackTop;
        while (drink != nullptr) {
            cout << drink->drinkName << " with " << drink->sugarPacks << " sugar packs" << endl;
            drink = drink->next;
        }
        cout << "Total Price: " << iter->totalPrice << endl;
        cout << "---" << endl;
        iter = iter->next;
    }
}

// Function: Starbucks order report
void starbucksOrderReport(int completedOrders, int totalDrinksSold, int totalSugarPacks, double totalRevenue) {
    cout << "Starbucks Order Report:" << endl;
    cout << "Completed orders: " << completedOrders << endl;
    cout << "Drinks sold: " << totalDrinksSold << endl;
    cout << "Sugar packs: " << totalSugarPacks << endl;
    cout << "Revenue: " << totalRevenue << endl;
    cout << "---" << endl;
}

// Function: Cancel an order by ID
void cancelOrder(OrderNode*& front, OrderNode*& rear, int orderID) {
    if (front == nullptr) {
        cout << "No orders to cancel." << endl << "---" << endl;
        return;
    }
    
    OrderNode* prev = nullptr;
    OrderNode* curr = front;
    while (curr != nullptr) {
        if (curr->orderID == orderID) {
            if (prev == nullptr) {
                front = curr->next;
                if (front == nullptr) {
                    rear = nullptr;
                }
            } else {
                prev->next = curr->next;
                if (curr == rear) {
                    rear = prev;
                }
            }
            cout << "Order No. " << orderID << " canceled." << endl << "---" << endl;
            deleteDrinkStack(curr->stackTop);
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    cout << "Order No. " << orderID << " not found." << endl << "---" << endl;
}

// Function: Rush an order by ID (moves to front and adjusts price)
void rushOrder(OrderNode*& front, OrderNode*& rear, int orderID) {
    if (front == nullptr) {
        cout << "No orders to rush." << endl << "---" << endl;
        return;
    }
    
    if (front->orderID == orderID) {
        front->totalPrice *= 1.3;
        cout << "Order No. " << orderID << " rushed (price +30%)." << endl << "---" << endl;
        return;
    }
    
    OrderNode* prev = nullptr;
    OrderNode* curr = front;
    while (curr != nullptr) {
        if (curr->orderID == orderID) {
            if (prev != nullptr) {
                prev->next = curr->next;
            }
            if (curr == rear) {
                rear = prev;
            }
            curr->next = front;
            front = curr;
            curr->totalPrice *= 1.3;
            cout << "Order No. " << orderID << " moved to front and rushed (price +30%)." << endl << "---" << endl;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    cout << "Order No. " << orderID << " not found." << endl << "---" << endl;
}

int main() {
    node* menuLast = nullptr;
    OrderNode* front = nullptr;
    OrderNode* rear = nullptr;
    int currentOrderID = 100;
    int completedOrders = 0;
    int totalDrinksSold = 0;
    int totalSugarPacks = 0;
    double totalRevenue = 0.0;
    string choice;
    
    // Load menu with retry on failure
    string filename;
    bool loaded = false;
    while (!loaded) {
        cout << "Enter menu filename (e.g., menu.txt): ";
        getline(cin, filename);
        ifstream file(filename);
        if (file) {
            loaded = true;
            file.close();
            importFromFile(menuLast, filename);
        } else {
            cout << "File not found. Try again." << endl << "---" << endl;
        }
    }
    

    cout << "Starbucks Order Management System" << endl << "---" << endl;
    cout << "Options:" << endl;
    cout << "1. Display today's Starbucks menu" << endl;
    cout << "2. Place a new order" << endl;
    cout << "3. Prepare the next order" << endl;
    cout << "4. Display current orders in preparation queue" << endl;
    cout << "5. View Starbucks order report" << endl;
    cout << "6. Cancel an order" << endl;
    cout << "7. Rush an order" << endl;
    cout << "8. Exit (prepare all remaining orders first)" << endl << "---" << endl;
    
    // Loop for choices
    do {
        cout << "Enter choice (1-8): ";
        cin >> choice;
        cout << "---" << endl;
        cin.ignore();
        
        if (choice == "1") {
            showMenu(menuLast);
        } else if (choice == "2") {
            takeOrder(menuLast, front, rear, currentOrderID);
        } else if (choice == "3") {
            prepareOrder(front, rear, completedOrders, totalDrinksSold, totalSugarPacks, totalRevenue);
        } else if (choice == "4") {
            printOrdersPreparationQueue(front);
        } else if (choice == "5") {
            starbucksOrderReport(completedOrders, totalDrinksSold, totalSugarPacks, totalRevenue);
        } else if (choice == "6") {
            cout << "Enter Order No. to cancel: ";
            int orderID;
            if (cin >> orderID) {
                cin.ignore();
                cancelOrder(front, rear, orderID);
            } else {
                cout << "Invalid input." << endl << "---" << endl;
                cin.clear();
                cin.ignore();
            }
        } else if (choice == "7") {
            cout << "Enter Order No. to rush: ";
            int orderID;
            if (cin >> orderID) {
                cin.ignore();
                rushOrder(front, rear, orderID);
            } else {
                cout << "Invalid input." << endl << "---" << endl;
                cin.clear();
                cin.ignore();
            }
        } else if (choice == "8") {
            if (front == nullptr) {
                cout << "No orders left to prepare." << endl;
            } else {
                cout << "Preparing remaining orders..." << endl;
                while (front != nullptr) {
                    prepareOrder(front, rear, completedOrders, totalDrinksSold, totalSugarPacks, totalRevenue);
                }
            }
            cout << "Exiting..." << endl;
            // Cleanup menu
            if (menuLast != nullptr) {
                node* head = menuLast->next;
                node* current = head;
                node* nextNode;
                do {
                    nextNode = current->next;
                    delete current;
                    current = nextNode;
                } while (current != head);
                menuLast = nullptr;
            }
            break;
        } else {
            cout << "Invalid choice (1-8)." << endl << "---" << endl;
        }
    } while (true);
    
    return 0;
}