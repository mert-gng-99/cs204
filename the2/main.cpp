// cs204 - the2 Starbucks menu
// Mert Güngör 34159

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <limits>
using namespace std;

// data
struct drink {
    int id;
    string name;
    double rating;
    double price;
};

struct node // cdll node
{
    drink data;
    node* prev;
    node* next;

    node() // constructor
    {
        prev = NULL; // 
        next = NULL;
    }
};


// globals
node* head = nullptr;    // first node (null if empty)
int nextid = 1;          // auto id from 1

string tolowerstr(string s)  // i did not remember the usage of  toLower function so i made this
{  
    // making lower by going one by one 
    for (int i = 0; i < (int)s.length(); i++)
    {
        // cout << "lowering char: " << s[i] << endl; // added for debug
        s[i] = tolower(s[i]);
    }

    return s;
}


// checking if two are equal by lookiggn only charecters
bool equals_nocase(const string& a, const string& b) 
{ 
    // cout << "cmp a=" << a << " b=" << b << endl; // added for debug
    return tolowerstr(a) == tolowerstr(b); 
}

string trim(const string& s) {


    int i = 0; // i is index from start
    int j = s.length(); // j is index after last char

    // skipping the first spaces (left side)
    while ((i < j )&& (isspace(s[i])))
    {
        // cout << "skip left at i=" << i << endl; // added for debug
        i = i + 1;
    }

    // skipping the last spaces (right side)
    while ((j > i) && (isspace(s[j - 1])))
    {
        // cout << "skip right at j=" << j << endl; // added for debug
        j = j - 1;
    }

    // cout << "trim cut from " << i << " len " << (j - i) << endl; // added for debug
    return s.substr(i, j - i);
}


bool parse_add_line(const string& line, string& name, double& rating, double& price) 
{
    // parsing line like: Name rating price (no extra token)
    istringstream iss(line);

    // frist name rating price
    if (!(iss >> name >> rating >> price))
    {
        // cout << "parse fail first three" << endl; // added for debug
        return false; // read fail
    }

    // checking if more than one
    string extra;
    if (iss >> extra)
    {
        // cout << "parse extra token: " << extra << endl; // added for debug
        return false; 
    }

    // cout << "parse ok name=" << name << " rating=" << rating << " price=" << price << endl; // added for debug
    return true; 
}


void print_drink(const drink& d) 
{
    cout << "ID: " << d.id << endl;
    cout << "Name: " << d.name << endl;
    cout << "Rating: " << d.rating << endl;
    cout << "Price: " << d.price << endl;
}




void insert_tail(const string& name, double rating, double price) 
{
    // this puts new node at end (befor head bcz circle)
    node* n = new node();
    n->data.id = nextid++;


    n->data.name = name;
    n->data.rating = rating;

    n->data.price = price;

    // cout << "insert tail: " << name << " id=" << n->data.id << endl; // added for debug

    if (head == nullptr) 
    {
        n->next = n;
        n->prev = n;
        head = n;
    } 
    else 
    {
        node* tail = head->prev;

        n->prev = tail;
        n->next = head;

        tail->next = n;
        head->prev = n;
    }
}

node* find_by_name(const string& name) 
{
    if (head == nullptr) 
    {
        // cout << "find_by_name empty list" << endl; // added for debug
        return nullptr;
    }

    node* cur = head;

    // i do here do while to walk one full round
    do // first time used do-while loop in this course :D
    {
        // cout << "check node name=" << cur->data.name << endl; // added for debug
        if (equals_nocase(cur->data.name, name)) 
        {
            return cur;
        }

        cur = cur->next;
    } 
    while (cur != head);

    return nullptr;
}



int get_size() 
{
    if (head == nullptr) 
    {
        return 0;
    }

    int cnt = 0;
    node* cur = head;

    // loop to count all nodes (stop after one round)
    do 
    {
        cnt = cnt + 1;
        // cout << "size count at id=" << cur->data.id << endl; // added for debug
        cur = cur->next;
    } 
    while (cur != head);

    return cnt;
}

node* detach(node* x) 
{
    // this unlink x from the circle (but not delete)
    if (head == nullptr || x == nullptr) 
    {
        return x;
    }

    if (x->next == x) // only one
    {
        head = nullptr;
        x->next = NULL;
        x->prev = NULL;
        return x;
    }

    x->prev->next = x->next;
    x->next->prev = x->prev;

    if (x == head) 
    {
        head = x->next;
    }

    x->next = NULL;
    x->prev = NULL;

    // cout << "detached node id=" << x->data.id << endl; // added for debug
    return x;
}



void attach_head(node* x) 
{
    if (x == nullptr) 
    {
        // cout << "attach_head null x" << endl; // added for debug
        return;
    } 

    if (head == nullptr) 
    {
        x->next = x;
        x->prev = x;
        head = x;
        return;
    }

    node* tail = head->prev;

    x->prev = tail;
    x->next = head;

    tail->next = x;
    head->prev = x;

    head = x;

    // cout << "attach_head ok: new head=" << head->data.name << endl; // added for debug
}


// menü operations
void add_drink_interactive() 
{
    cout << "Enter drink details (Name rating price): ";

    string line;
    getline(cin, line);

   

    line = trim(line);

    string name;
    double rating;
    double price;

    // look for input format
    bool ok = parse_add_line(line, name, rating, price);

    if (!ok)
    {
        cout << "Invalid input format." << endl;
        cout << "---" << endl;
        return;
    }

    // checking ifits on the list
    node* ex = find_by_name(name);

    if (ex != NULL)
    {
        cout << "The drink " << name << " is already in the menu." << endl;
        cout << "---" << endl;
    }
    else
    {
        insert_tail(name, rating, price);
        cout << "The drink " << name << " is added to the menu." << endl;
        cout << "---" << endl;
    }
}

void import_from_file_interactive() 
{
    cout << "Enter filename: ";

    string fname;

    getline(cin, fname);

    

    fname = trim(fname);

    ifstream fin(fname);

    if (fin.fail())
    {
        // cout << "file open fail: " << fname << endl; // added for debug
        cout << "File not found: " << fname << endl;
        cout << "---" << endl;
        return;
    }

    string line;

    // read file line by line and add to menu
    while (getline(fin, line))
    {
        line = trim(line);

        if (!line.empty())
        {
            string name;
            double rating;
            double price;

            bool ok = parse_add_line(line, name, rating, price);

            if (ok){
                node* found = find_by_name(name);

                if (found != NULL)
                {
                    cout << "The drink " << name << " is already in the menu." << endl;
                }
                else
                {
                    insert_tail(name, rating, price);
                    cout << "The drink " << name << " is added to the menu." << endl;
                }
            }
            else
            {
                // cout << "skip bad line: " << line << endl; // added for debug
            }
        }
    }

    cout << "---" << endl;
}

void show_menu_forward()
{
    if (head == NULL)
    {
        cout << "The menu is empty." << endl;
        cout << "---" << endl;
        return;
    }

    node* cur = head;

    // loop forward until come back to head
    while (true) // infinite loop to go through the list
    {
        print_drink(cur->data);

        cout << "---" << endl;

        if (cur->next == head)
        {
            break;
        }

        cur = cur->next;
    }
}


void show_menu_backward()
{   
    if (head == NULL)
    {
        cout << "The menu is empty." << endl;
        cout << "---" << endl;
        return;
    }

    node* tail = head->prev;
    node* cur = tail;

    // loop backward until come back to tail
    while (true) // infinite loop to go through the list backwards
    {
        print_drink(cur->data);
        cout << "---" << endl;

        if (cur->prev == tail)
        {
            break;
        }

        cur = cur->prev;
    }
}

void show_size_interactive()
{
    // just print size (count nodes)
    int n = get_size();
    cout << "Total drinks in menu: " << n << endl;
    cout << "---" << endl;
}


void remove_by_name_interactive()
{
    cout << "Enter name to remove: ";

    string name;
    getline(cin, name);

   

    name = trim(name);

    // cout << "remove_by_name ask for=" << name << endl; // added for debug
    node* x = find_by_name(name);

    if (x == NULL)
    {
        cout << "The drink " << name << " could not be found in the menu." << endl;
        cout << "---" << endl;
    }
    else
    {
        detach(x);
        delete x;
        cout << "The drink " << name << " is deleted from the menu." << endl;
        cout << "---" << endl;
    }
}


// remove by index 
void remove_by_index_interactive() 
{
    cout << "Enter index to remove: "; // ask user for index

    string line; 
    if (!getline(cin , line)) return; // if fail reading, just quit function
    line = trim(line); // remove spaces from start and end

    long long idx; 
    bool oknum = false; // check if number valid

    {  
        ////cout << "[DEBUG] user input line: " << line << endl; 
        istringstream iss(line);
        oknum = (bool)(iss >> idx); // try to convert to number
        if (oknum) 
        { 
            string extra; 
            if (iss >> extra) oknum = false; // if extra word after num, wrong
        }
        ////cout << "[DEBUG] idx=" << idx << " oknum=" << oknum << endl; 
    }   

    if (head == NULL) 
    {
        cout << "The menu is empty." << endl << "---" << endl; 
        return;
    }

    if (!oknum) 
    {
        cout << "Invalid index." << endl << "---" << endl;
        return;
    }

    int n = get_size(); // get total drinks count
    ////cout << "[DEBUG] total drinks: " << n << endl; // debug

    if ((idx < 1 )|| (idx > n)) {
        cout << "Invalid index." << endl << "---" << endl;
        return;
    }

    node* cur = head; 

    for (int i = 1; i < idx; i++) {
        cur = cur->next; // move to next node

    }

    string nm = cur->data.name; // name of drink we delete

    detach(cur); // cut from list
    delete cur;  // free from memory
    cout << "The drink " << nm << " is deleted from the menu." << endl << "---" << endl;


}


bool comes_before_desc(const drink& a, const drink& b) 
{
    if (a.price != b.price)
    {
        if (a.price > b.price)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (a.rating != b.rating){


            if (a.rating > b.rating){
                return true;
            }
            else {
                return false;
            }
        }
        else
        {
            // same values, keep old order (stable)
            return false;
        }
    }
}


// this functoin sort the menu by price big to smal
// if price same then sort by rating big to smal
void order_by_price_desc()
{
    // if menu is emty or have 1 drink dont need sort
    if (head == NULL || head->next == head){

        cout << "The menu is sorted by price in descending order." << endl;
        cout << "---" << endl;
        return;
    }

    int n = get_size(); // get how meny drinks
    node* newhead = NULL; // new list start

    node* cur = head;

    // loop all drinks, take from old list and put in new list
    for (int i = 0; i < n; i = i + 1)
    {
        node* nxt = cur->next; // save next
        cur = detach(cur); // remove from old list, now cur alone

        if (newhead == NULL){


            // first drink in new list, point to itself
            cur->next = cur;
            cur->prev = cur;
            newhead = cur;
        }
        else {
            node* p = newhead;
            bool inserted = false;

            // find the place to put cur
            while (true){
                if (comes_before_desc(cur->data, p->data))
                {
                    // put cur before p
                    node* left = p->prev;

                    cur->prev = left;
                    cur->next = p;

                    left->next = cur;
                    p->prev = cur;

                    if (p == newhead)
                    {
                        newhead = cur; // if insert at start update head
                    }

                    inserted = true;
                    break;
                }

                if (p->next == newhead)
                {
                    // reach end, break
                    break;
                }

                p = p->next;
            }

            if (inserted == false){
                // put cur at end (tail)
                node* tail = newhead->prev;

                cur->prev = tail;
                cur->next = newhead;

                tail->next = cur;
                newhead->prev = cur;
            }
        }

        cur = nxt; // go to next drink in old list
    }

    head = newhead; // update head to new list

    cout << "The menu is sorted by price in descending order." << endl;
    cout << "---" << endl;
}

// ask user to favoritize a drink, increse rating by 1 and move to head
void favoritize_drink_interactive()
{
    cout << "Enter name to favoritize: ";

    string name;
    getline(cin, name);

   

    name = trim(name);

    // cout << "favoritizing: " << name << endl; // added for debug
    node* x = find_by_name(name);

    if (x == NULL)
    {
        cout << "The drink " << name << " could not be found in the menu." << endl;
        cout << "---" << endl;
        return;
    }

    x->data.rating = x->data.rating + 1.0; // increse rating

    if (x != head)
    {
        detach(x);      // remove from list
        attach_head(x); // put to start
    }

    cout << "The drink " << name << " is favoritized." << endl;
    cout << "---" << endl;
}

// help func to swap 2 nodes in doubly circ list
void swap_nodes(node* a, node* b) 
{
    if (a == b || a == NULL || b == NULL || head == NULL)
    {
        return; // if same or null dont do
    }

   

    // if they next to each other
    if ((a->next == b) || (b->next == a))
    {
        node* left;
        node* right;

     
        if (a->next == b)
        {
            left = a;
            right = b;
        }
        else
        {
            left = b;
            right = a;
        }

        node* lprev = left->prev;
        node* rnext = right->next;

        //cout << "swap: " << left->data.name << " <-> " << right->data.name << endl; // added for debug

        lprev->next = right;
        right->prev = lprev;

        //cout << "left prev set to right" << endl; // added for debug

        right->next = left;
        left->prev  = right;

        //cout << "right next set to left" << endl; // added for debug

        left->next  = rnext;
        rnext->prev = left;

        //cout << "left next set to right next" << endl; // added for debug
        // if head was swapped, update it
        if (head == left) {
            head = right;
        }
        else if (head == right){
            head = left;
        }
            
        return;
    }

    // if they far from each other, swap links around both
    node* ap = a->prev;
    node* an = a->next;
    node* bp = b->prev;
    node* bn = b->next;
    //cout << "swap far: " << a->data.name << " <-> " << b->data.name << endl; // added for debug
    ap->next = b;
    b->prev = ap;

    an->prev = b;
    b->next = an;

    // cout << "a links set to b" << endl; // added for debug

    bp->next = a;
    a->prev = bp;

    bn->prev = a;
    a->next = bn;
    // cout << "b links set to a" << endl; // added for debug
    if (head == a) {

        head = b;
    }
    else if (head == b) {

        head = a;

    }

}

// ask user to swap two drinks by name
void swap_two_drinks_interactive()
{
    cout << "Enter first drink name: ";

    string n1;
    getline(cin, n1);

    

    n1 = trim(n1);

    cout << "Enter second drink name: ";

    string n2;
    getline(cin, n2);

  
    n2 = trim(n2);

    if (equals_nocase(n1, n2))
    {
        cout << "The drinks are the same, no swap needed." << endl;
        cout << "---" << endl;
        return;
    }

    node* a = find_by_name(n1);
    node* b = find_by_name(n2);

    if (a == NULL || b == NULL)
    {
        cout << "One or both drinks could not be found in the menu." << endl;
        cout << "---" << endl;
        return;
    }

    // cout << "swap: " << n1 << " <-> " << n2 << endl; // added for debug
    swap_nodes(a, b);

    cout << "The drinks " << n1 << " and " << n2 << " are swapped." << endl;
    cout << "---" << endl;
}

// delete all drinks from menu
void clear_menu_interactive()
{
    if (head == NULL) 
    {
        cout << "The menu is empty." << endl;
        cout << "---" << endl;
        return;
    }

    node* cur = head->next;

    // go around and delete all except head, then delete head
    while (cur != head)
    {
        node* nxt = cur->next;
        // cout << "delete node id=" << cur->data.id << endl; // added for debug
        delete cur;
        cur = nxt;
    }

    delete head;
    head = NULL;
    nextid = 1;   
    cout << "The menu is cleared." << endl;
    cout << "---" << endl;
}

// exit the progrm and clear menu if exist
void exit_program_interactive()
{
    cout << "Exiting the program..." << endl;

    if (head == NULL)
    {
        cout << "The menu is empty." << endl;
        cout << "---" << endl;
    }
    else
    {
        node* cur = head->next;

        // delete all nodes safe
        while (cur != head)
        {
            node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }

        delete head;
        head = NULL;

        cout << "The menu is cleared." << endl;
        cout << "---" << endl;
    }
    nextid = 1;
}

void print_menu_options(){
    cout << "Starbucks Menu Manager" << endl;
    cout << "---" << endl;
    cout << "MENU" << endl;
    cout << "1. Add a drink" << endl;
    cout << "2. Import from file" << endl;
    cout << "3. Show menu (forward)" << endl;
    cout << "4. Show menu (backward)" << endl;
    cout << "5. Show size" << endl;
    cout << "6. Remove by name" << endl;
    cout << "7. Remove by index" << endl;
    cout << "8. Order by price (descending)" << endl;
    cout << "9. Favoritize a drink" << endl;
    cout << "10. Swap two drinks" << endl;
    cout << "11. Clear menu" << endl;
    cout << "12. Exit" << endl;
    cout << "13. Show this menu again" << endl;
    cout << "---" << endl;

}

// main functoin, run the progrm and ask user what to do

// main functoin, run the progrm and ask user what to do
int main()
{
    print_menu_options(); // first show menu options

    while (true) // run forevr until exit
    {
        cout << "Enter your choice: "; // ask user choice

        string line;

        
        if (!getline(cin, line)) { 
            // cout << "[DEBUG] getline fail in main" << endl; // added for debug
            return 0; 
        }

        line = trim(line); // remove spaces from both side

        int choice = -1; 
        bool ok = false; // will be true if parse int success

        {   // debug block (student style)
            // cout << "[DEBUG] raw choice str: '" << line << "'" << endl; // added for debug
            istringstream iss(line);
            ok = (bool)(iss >> choice); // try parse number
            if (ok) {
                string extra;
                if (iss >> extra) ok = false; 
            }
            // cout << "[DEBUG] choice=" << choice << " ok=" << ok << endl; // added for debug
        }

        if ((!ok) || (choice < 1) || (choice > 13)) // wrong number or parse fail
        {
            cout << "---" << endl;
            cout << "Not a valid option." << endl;
            cout << "---" << endl;
        }
        else // valid option, go do staff
        {
            cout << "---" << endl;

            if (choice == 1){
                add_drink_interactive();
            }
            else if (choice == 2){
                import_from_file_interactive();
            }
            else if (choice == 3){
                show_menu_forward();
            }
            else if (choice == 4){
                show_menu_backward();
            }
            else if (choice == 5){
                show_size_interactive();
            }
            else if (choice == 6){
                remove_by_name_interactive();
            }
            else if (choice == 7){
                remove_by_index_interactive();
            }
            else if (choice == 8){
                order_by_price_desc();
            }
            else if (choice == 9){
                favoritize_drink_interactive();
            }
            else if (choice == 10){
                swap_two_drinks_interactive();
            }
            else if (choice == 11){
                clear_menu_interactive();
            }
            else if (choice == 12){
                exit_program_interactive();
                return 0; // quit progrm
            }
            else if (choice == 13){
                print_menu_options();
            }
        }
    }
}

// sonunda
