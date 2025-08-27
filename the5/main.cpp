// mert.gungor2_gungor_mert_THE5.cpp
// CS204 THE5 - Multithreaded Starbucks OMS
// Mert Gungor 34159
// email: mert.gungor2 sabanciuniv.edu

#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;


// ========================== helpers from THE3 ==========================
//   taken from the3 source code

string to_lower(string s) {
    for (char& c : s) {
        c = tolower(static_cast<unsigned char>(c));
    }
    return s;
}

bool okey = false;

// ========================== MENU circ double link list ==========================
//   taken from the3 source code

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
//   taken from the3 source code
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
//   taken from the3 source code
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


//   taken from the3 source code
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


//   taken from the3 source code
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

//   taken from the3 source code
void showMenu(node* last) {
    if (last == nullptr) {
        cout << "The menu is empty." << endl << "---" << endl;
        return;
    }
    node* iter = last->next;
    do {
        cout << "---" << endl;
        cout << "ID: " << iter->id << endl;
        cout << "Name: " << iter->name << endl;
        cout << "Rating: " << iter->rating << endl;
        cout << "Price: " << iter->price << endl;
        iter = iter->next;
    } while (iter != last->next);
    cout << "---" << endl;
}

// ========================== Stack drinks in one order ==========================
// taken from the3 source code

struct DrinkNode {
    string drinkName;
    int sugarPacks;
    DrinkNode* next;

    DrinkNode(string name, int sugar, DrinkNode* nxt = nullptr)
        : drinkName(name), sugarPacks(sugar), next(nxt) {}
};



// ========================== Queue (orders) ==========================
//   taken from the3 source code

struct OrderNode {
    int orderID;
    DrinkNode* stackTop;
    double totalPrice;
    OrderNode* next;

    OrderNode(int id, DrinkNode* top, double price, OrderNode* nxt = nullptr)
        : orderID(id), stackTop(top), totalPrice(price), next(nxt) {}
};

void deleteDrinkStack(DrinkNode* top) {
    DrinkNode* current = top;
    while (current != nullptr) {
        DrinkNode* temp = current;
        current = current->next;
        delete temp;
    }
}



// global variables for threads note: sorry for much comments I just want to get full marks :D also I used extra  spaces to read the assigned variables easily
node*      gmenulast   = nullptr;   // circly doulbe link list last pointer

OrderNode* gfront      = nullptr;   // queue front
OrderNode* grear       = nullptr;   // queue rear

int        gcompleted  = 0;         // finished order count
int        gdrinks     = 0;         // how many drinks total


int        gsugars     = 0;         // sugar packs total to just sum
int        grevenue    = 0;         // i keep int coz sample prints like ints

mutex      gqmtx;                   // protects queue
mutex      gstatsmtx;               // protects stats
mutex      gcoutmtx;                // so logs dont overlap like spaghetti

bool       gstop       = false;     // when true workers gonna exit after queue empty



// ========================== take order producer ==========================
//   taken from the3 source code, edited a little bit
//   i kept same input style but added mutex lock when pushing to queue
void takeorder_mt(node* menuLast, int& currentOrderID)

{
    cout << "---" << endl;
    // build a stack top pointer first LIFO
    DrinkNode* stackTop = nullptr;

    double total = 0.0;

    string input;

    cout << "Enter drink details (name sugar) one by one. Type 'enough' to stop." << endl;

    while (true)
    {
        cout << "Enter: ";

        getline(cin, input);

        if (to_lower(input) == "enough")
        {
            break;
        }

        stringstream iss(input);

        string name;

        int sugar;

        if (iss >> name >> sugar)
        {
            node* menuItem = findDrink(menuLast, name);

            if (menuItem != nullptr)
            {
                // push to stack top this is LIFO
                DrinkNode* newDrink = new DrinkNode(name, sugar, stackTop);

                stackTop = newDrink;

                total += menuItem->price;
            }
            else
            {
                cout << "Drink '" << name << "' not found. Try again." << endl;
            }
        }
        else
        {
            cout << "Invalid format. Enter name and sugar." << endl;
        }
    }

    if (stackTop == nullptr)
    {
        // user added nothing sad :D
        cout << "No drinks added." << endl << "---" << endl;

        return;
    }

    // create order node queue element
    OrderNode* newOrder = new OrderNode(currentOrderID, stackTop, total);
    
    // enqueue with lock
    gqmtx.lock();

    if (grear == nullptr){

        gfront = grear = newOrder;
    }
    else{

        grear->next = newOrder;

        grear = newOrder;
    }


    gqmtx.unlock();

    cout << "Order No. " << currentOrderID << " added to the queue." << endl << "---" << endl;

    currentOrderID += 10;  
}



// ========================== queue snapshot ==========================
//   printOrdersPreparationQueue is from THE3
//   here i only lock before calling it so it doesnt race with workers

void printOrdersPreparationQueue(OrderNode* front); // I used this to not forget to add 

void showqueue_snapshot_ts()
{

    gqmtx.lock();
    cout << "---" << endl;
    printOrdersPreparationQueue(gfront);

    gqmtx.unlock();
}



// ========================== report  ==========================
// this is just printing stats with lock
//
void showreport_ts()
{
    cout << "---" << endl;
    cout << "Starbucks Order Report:" << endl;
    
    gstatsmtx.lock();

    cout << "Completed orders: " << gcompleted << endl;

    cout << "Drinks sold: "      << gdrinks    << endl;

    cout << "Sugar packs: "      << gsugars    << endl;

    cout << "Revenue: "          << grevenue   << endl;

    gstatsmtx.unlock();

    cout << "---" << endl;
}



// ========================== worker thread ==========================
void workerfunc(int staffId)
{
    while (true)
    {
        OrderNode* job = nullptr;

        // try to pop from queue with lock
        gqmtx.lock();

        if (gfront != nullptr)
        {
            job    = gfront;

            gfront = gfront->next;

            if (gfront == nullptr)
            {
                grear = nullptr;
            }

            job->next = nullptr;
        }

        gqmtx.unlock();

        // no job found now
        if (job == nullptr) {
            
            // if written exit in main and also queue empty and we go out
            if (gstop){

                return;
            }

            // take a tiny nap otherwise busy spinning like crazy
            this_thread::sleep_for(chrono::milliseconds(100));

           
        }
        else
        {
            // here we have a real job lets print details safe
            int drinksInOrder = 0;

            int sugarsInOrder = 0;

            gcoutmtx.lock();

            cout << endl;

            cout << "Staff " << staffId << " preparing Order No. " << job->orderID << endl;

            cout << "Total price: " << job->totalPrice << endl;

            cout << "Drinks (LIFO):" << endl;

            for (DrinkNode* d = job->stackTop; d != nullptr; d = d->next)
            {
                cout << d->drinkName << " with " << d->sugarPacks << " sugar packs" << endl;

                drinksInOrder++;

                sugarsInOrder += d->sugarPacks;
            }

            gcoutmtx.unlock();

            // simulate real world… coffee takes time :D
            this_thread::sleep_for(chrono::seconds(8));

            // update global stats also with lock
            gstatsmtx.lock();

            gcompleted += 1;

            gdrinks    += drinksInOrder;

            gsugars    += sugarsInOrder;

            grevenue   += (job->totalPrice); // int like samples

            gstatsmtx.unlock();

            // say we finished so user see message
            gcoutmtx.lock();

            cout << "Staff " << staffId << " completed Order No. " << job->orderID << endl;

            cout << "---" << endl;

            gcoutmtx.unlock();

            // finally clean memory of that order
            deleteDrinkStack(job->stackTop);

            delete job;
        }
    }
}



// ========================== THE3 queue printer unchanged ==========================
//   taken from the3 source code

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



// ========================== main ==========================
// flow is like sample runs:
// - ask file
// - import menu  from THE3
// - start 5 worker threads
// - on exit set gstop and join everyone

int main(){
    

    

    // 1) ask file name first
    cout << "Enter menu filename (e.g., menu.txt): ";

    string fname;

    if (!getline(cin, fname)) {

        return 0;
    }
        



    // very tiny existence check if no file program just stop
    ifstream test(fname);

    if (!test){

        return 0;
    }
    



    // load menu using the3 function exactly same
    importFromFile(gmenulast, fname);



    cout << "Multithreaded Starbucks Order Management System" << endl;

    cout << "Staff threads: 5 workers started" << endl;

    cout << "---" << endl;

    // >>> Options once (moved out of loop)
    cout << "Options:" << endl;
    cout << "1. Display today's Starbucks menu" << endl;
    cout << "2. Place a new order" << endl;
    cout << "3. Display current orders in preparation queue" << endl;
    cout << "4. View Starbucks order report" << endl;
    cout << "5. Exit" << endl;
    cout << "---" << endl;


    // 2) start workers
    const int W = 5;

    thread workers[W];

    for (int i = 0; i < W; i++)
    {
        workers[i] = thread(workerfunc, i + 1);
    }



    // 3) menu loop
    int currentOrderID = 100;

    while (true)
    {
       
        if (!okey){
            cout << "Enter choice (1-5): ";
        }



        string ch;

        if (!getline(cin, ch)) {

            break;
        }


        // check numeric
        bool oknum = ch.size() > 0;

        for (int i = 0; i < (int)ch.size(); i = i + 1){


            if (!isdigit(ch[i])) {

                oknum = false;
                break; // no need to check more if one is found
            }
        }

        if (!oknum)
        {

            cout << "---" << endl;
            cout << "Invalid choice (1-5)." << endl;
            cout << "---" << endl;
        }
        else
        {
            int opt = stoi(ch);



            if (opt == 1)
            {   
                okey = true;
                // print menu with THE3 function 
                showMenu(gmenulast);
                okey = false;
            }
            else if (opt == 2)
            {
                okey = true;
                // user enters drinks we push order safely
                takeorder_mt(gmenulast, currentOrderID);
                okey = false;

                
            }
            else if (opt == 3)
            {
                okey = true;
                // show queue snapshot in a safe way
                showqueue_snapshot_ts();
                okey = false;
            }
            else if (opt == 4)
            {
                okey = true;
                // show report safely
                showreport_ts();
                okey = false;
            }
            else if (opt == 5)
            {
                okey = true;
                // exit time… tell workers to stop soon
                cout << "---" << endl;
                cout << "Stopping staff threads..." << endl;

                gstop = true;   // they will see it on next poll
                


                // join them all 
                for (int i = 0; i < W; i++){

                    if (workers[i].joinable()) workers[i].join();
                }

                cout << "Exiting..." << endl;



                // just in case free any leftover orders in queue :)
                gqmtx.lock();

                while (gfront != nullptr)
                {
                    OrderNode* t = gfront;
                    gfront = gfront->next;

                    deleteDrinkStack(t->stackTop);
                    delete t;
                }

                grear = nullptr;
                gqmtx.unlock();

                // free menu same logic as THE3 source
                if (gmenulast != nullptr)
                {
                    node* head = gmenulast->next;
                    node* current = head;
                    node* nextNode;
                    do {

                        nextNode = current->next;
                        delete current;
                        current = nextNode;

                    } while (current != head);
                    gmenulast = nullptr;
                }
                okey = false;
                break; // out of main loop
            }
            else {
                
                cout << "---" << endl;
                cout << "Invalid choice (1-5)." << endl;
                cout << "---" << endl;
            }
        }
    }



    return 0;
}
