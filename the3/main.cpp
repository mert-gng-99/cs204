// cs204 - the3 starbucks order management system
// Mert Güngör 34159


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <limits>
using namespace std;

// ===================== cdll menu (from the2; only to store items) =====================
struct drink {
    int id;
    string name;
    double rating;
    double price;
};

struct node { // cdll node
    drink data;
    node* prev;
    node* next;
    node(){ prev = NULL; next = NULL; }
};

node* head = nullptr;   // cdll head (first)
int nextid_menu = 1;    // auto inc for menu

// to lower (i forgot exact api so i did loop ver)
string tolowerstr(string s){
    for (int i=0;i<(int)s.length();i++) s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

bool equals_nocase(const string& a, const string& b){ 
    return tolowerstr(a)==tolowerstr(b); 
}

string trim(const string& s){
    int i=0, j=(int)s.length();
    while(i<j && isspace((unsigned char)s[i])) i++;
    while(j>i && isspace((unsigned char)s[j-1])) j--;
    return s.substr(i, j-i);
}

bool parse_menu_line(const string& line, string& name, double& rating, double& price){
    istringstream iss(line);
    if(!(iss>>name>>rating>>price)) return false;
    string extra; if(iss>>extra) return false; // no more toks pls
    return true;
}

void insert_tail_menu(const string& name, double rating, double price){
    node* n = new node();
    n->data.id = nextid_menu++;
    n->data.name = name;
    n->data.rating = rating;
    n->data.price = price;

    // cout << "[dbg] insert " << name << " price="<<price<<"\n";
    if(head==nullptr){
        n->next = n; n->prev = n; head = n;
    }else{
        node* tail = head->prev;
        n->prev = tail; n->next = head;
        tail->next = n; head->prev = n;
    }
}

node* find_by_name_menu(const string& name){
    if(!head) return nullptr;
    node* cur = head;
    do{
        // cout << "[dbg] chk " << cur->data.name << "\n";
        if(equals_nocase(cur->data.name, name)) return cur;
        cur = cur->next;
    }while(cur!=head);
    return nullptr;
}

void print_drink(const drink& d){
    cout << "ID: " << d.id << endl;
    cout << "Name: " << d.name << endl;
    cout << "Rating: " << d.rating << endl;
    cout << "Price: " << d.price << endl;
}

void show_menu_forward(){
    if(head==NULL){
        cout << "The menu is empty." << endl;
        cout << "---" << endl;
        return;
    }
    node* cur = head;
    while(true){
        print_drink(cur->data);
        cout << "---" << endl;
        if(cur->next==head) break;
        cur = cur->next;
    }
}

void clear_menu_all(){
    if(!head) return;
    node* cur = head->next;
    while(cur!=head){
        node* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    delete head;
    head = NULL;
    nextid_menu = 1;
}

// ===================== dynamic stack for drinks (linked list ONLY) =====================
struct drinkitem {
    string name;
    int sugar;
};

struct snode {
    drinkitem data;
    snode* next;
    snode(const string& n, int s): next(NULL){ data.name=n; data.sugar=s; }
};

class drinkstack {
    snode* topnode;
public:
    drinkstack(): topnode(NULL) {}
    ~drinkstack(){ clear(); } // destructor must free evrything or mem leak boom

    bool isempty() const { return topnode==NULL; }

    void push(const string& name, int sugar){
        // cout << "[dbg] push " << name << " sugar=" << sugar << "\n";
        snode* x = new snode(name, sugar);
        x->next = topnode;
        topnode = x;
    }

    bool pop(drinkitem& out){
        if(!topnode) return false;
        snode* x = topnode;
        out = x->data;
        topnode = x->next;
        delete x;
        return true;
    }

    // print from top to bottom (dont mutate, teacher hates that)
    void print_topdown() const {
        snode* cur = topnode;
        while(cur){
            cout << cur->data.name << " with " << cur->data.sugar << " sugar packs" << endl;
            cur = cur->next;
        }
    }

    void stats(int& cnt, long long& sugar_sum) const {
        cnt = 0; sugar_sum = 0;
        for(snode* cur=topnode; cur; cur=cur->next){ cnt++; sugar_sum += cur->data.sugar; }
    }

    void clear(){
        snode* cur = topnode;
        while(cur){
            snode* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        topnode = NULL;
    }
};

// ===================== dynamic queue for orders (linked list ONLY) =====================
struct order {
    int id;
    drinkstack drinks;
    double totalprice;
    order(int i=0): id(i), totalprice(0) {}
};

struct qnode {
    order* ord;
    qnode* next;
    qnode(order* o): ord(o), next(NULL) {}
};

class orderqueue {
    qnode* front;
    qnode* rear;
public:
    orderqueue(): front(NULL), rear(NULL) {}
    ~orderqueue(){ clear_all(); } // destructor yup

    bool isempty() const { return front==NULL; }

    void enqueue(order* o){
        // cout << "[dbg] enq id="<<o->id<<"\n";
        qnode* q = new qnode(o);
        if(!rear){ front = rear = q; }
        else { rear->next = q; rear = q; }
    }

    order* dequeue(){
        if(!front) return NULL;
        qnode* q = front;
        front = front->next;
        if(!front) rear = NULL;
        order* o = q->ord;
        delete q;
        return o;
    }

    // returns: -1 empty, 0 not found, 1 canceled
    int cancel_by_id(int id){
        if(!front) return -1;
        qnode* prev = NULL;
        qnode* cur = front;
        while(cur && cur->ord->id != id){ prev = cur; cur = cur->next; }
        if(!cur) return 0;
        if(prev==NULL){ // front
            front = cur->next;
            if(!front) rear = NULL;
        }else{
            prev->next = cur->next;
            if(cur==rear) rear = prev;
        }
        delete cur->ord;
        delete cur;
        return 1;
    }

    // rush: price *= 1.3 and maybe move to front
    // returns: -1 empty, 0 not found, 1 rushed(no move), 2 moved+rushed
    int rush_by_id(int id){
        if(!front) return -1;
        if(front->ord->id == id){
            front->ord->totalprice *= 1.3;
            return 1;
        }
        qnode* prev = front;
        qnode* cur = front->next;
        while(cur && cur->ord->id!=id){ prev = cur; cur = cur->next; }
        if(!cur) return 0;
        prev->next = cur->next;
        if(cur==rear) rear = prev;
        cur->next = front;
        front = cur;
        front->ord->totalprice *= 1.3;
        return 2;
    }

    void print_queue() const {
        if(!front){
            cout << "No orders in the queue." << endl;
            cout << "---" << endl;
            return;
        }
        cout << "Current orders:" << endl;
        qnode* cur = front;
        while(cur){
            cout << "Order No. " << cur->ord->id << endl;
            cout << "Drinks:" << endl;
            cur->ord->drinks.print_topdown(); // LIFO look
            cout << "Total Price: " << cur->ord->totalprice << endl;
            cout << "---" << endl;
            cur = cur->next;
        }
    }

    void clear_all(){
        while(front){
            qnode* q = front;
            front = front->next;
            delete q->ord;
            delete q;
        }
        rear = NULL;
    }
};

// ===================== helpers / ui =====================
void print_system_menu(){
    cout << "Starbucks Order Management System" << endl;
    cout << "---" << endl;
    cout << "Options:" << endl;
    cout << "1. Display today's Starbucks menu" << endl;
    cout << "2. Place a new order" << endl;
    cout << "3. Prepare the next order" << endl;
    cout << "4. Display current orders in preparation queue" << endl;
    cout << "5. View Starbucks order report" << endl;
    cout << "6. Cancel an order" << endl;
    cout << "7. Rush an order" << endl;
    cout << "8. Exit (prepare all remaining orders first)" << endl;
    cout << "---" << endl;
}

// global stats (ye these are global, i know…)
long long completedorders = 0;
long long totaldrinkssold = 0;
long long totalsugarpacks = 0;
double totalrevenue = 0.0;

// order id generator
int nextorderid = 100;

// load menu at start
void load_menu_startup(){
    cout << "Enter menu filename (e.g., menu.txt): ";
    string fname; getline(cin, fname); fname = trim(fname);
    ifstream fin(fname);
    if(fin.fail()){
        // not printing error here, samples dont show it
        // cout << "[dbg] file open failed: " << fname << "\n";
    }else{
        string line;
        while(getline(fin,line)){
            line = trim(line);
            if(line.empty()) continue;
            string name; double rating, price;
            if(parse_menu_line(line, name, rating, price)){
                if(!find_by_name_menu(name)){
                    insert_tail_menu(name, rating, price);
                    cout << "The drink " << name << " is added to the menu." << endl;
                } // else dup? the doc doesnt ask to warn here
            }else{
                // cout << "[dbg] bad line: " << line << "\n";
            }
        }
        cout << "---" << endl;
    }
    print_system_menu();
}

// place new order (choice 2)
void place_new_order(orderqueue& q){
    cout << "Enter drink details (name sugar) one by one. Type 'enough' to stop." << endl;

    string line;
    order* ord = new order(nextorderid);
    bool added_any = false;

    while(true){
        cout << "Enter: ";
        if(!getline(cin, line)){ delete ord; return; }
        line = trim(line);

        if(line.empty()){
            cout << "Invalid format. Enter name and sugar." << endl;
            continue;
        }

        string low = tolowerstr(line);
        if(low=="enough"){
            break;
        }

        string name; long long sugar; bool ok=false;
        {
            istringstream iss(line);
            if(iss>>name>>sugar){
                string extra;
                if(!(iss>>extra)) ok=true; // exactly two toks
            }
        }

        if(!ok){
            cout << "Invalid format. Enter name and sugar." << endl;
            continue;
        }

        node* m = find_by_name_menu(name);
        if(!m){
            cout << "Drink '" << name << "' not found. Try again." << endl;
            continue;
        }

        // valid add
        ord->drinks.push(m->data.name, (int)sugar);
        ord->totalprice += m->data.price;
        added_any = true;
    }

    if(!added_any){
        delete ord;
        cout << "No drinks added." << endl;
        cout << "---" << endl;
        return;
    }

    q.enqueue(ord);
    cout << "Order No. " << ord->id << " added to the queue." << endl;
    cout << "---" << endl;
    nextorderid += 10;
}

// prepare next (choice 3)
void prepare_next(orderqueue& q){
    if(q.isempty()){
        cout << "No orders to prepare." << endl;
        cout << "---" << endl;
        return;
    }

    order* ord = q.dequeue();

    cout << "Preparing Order No. " << ord->id << endl;
    cout << "Total price: " << ord->totalprice << endl;
    cout << "Drinks (LIFO):" << endl;

    int cnt=0; long long sugars=0;
    drinkitem it;
    while(ord->drinks.pop(it)){
        cout << it.name << " with " << it.sugar << " sugar packs" << endl;
        cnt++;
        sugars += it.sugar;
    }

    completedorders += 1;
    totaldrinkssold += cnt;
    totalsugarpacks += sugars;
    totalrevenue += ord->totalprice;

    cout << "---" << endl;
    delete ord;
}

// display current orders (choice 4)
void display_queue(orderqueue& q){
    q.print_queue();
}

// report (choice 5)
void show_report(){
    cout << "Starbucks Order Report:" << endl;
    cout << "Completed orders: " << completedorders << endl;
    cout << "Drinks sold: " << totaldrinkssold << endl;
    cout << "Sugar packs: " << totalsugarpacks << endl;
    cout << "Revenue: " << totalrevenue << endl;
    cout << "---" << endl;
}

// cancel (choice 6)
void cancel_order(orderqueue& q){
    cout << "Enter Order No. to cancel: ";
    string line; if(!getline(cin,line)) return;
    line = trim(line);

    long long id; bool ok=false;
    {
        istringstream iss(line);
        if(iss>>id){
            string extra; if(!(iss>>extra)) ok=true;
        }
    }
    if(!ok){
        cout << "Invalid input." << endl;
        cout << "---" << endl;
        return;
    }

    int res = q.cancel_by_id((int)id);
    if(res==-1){
        cout << "No orders to cancel." << endl;
        cout << "---" << endl;
    }else if(res==0){
        cout << "Order No. " << id << " not found." << endl;
        cout << "---" << endl;
    }else{
        cout << "Order No. " << id << " canceled." << endl;
        cout << "---" << endl;
    }
}

// rush (choice 7)
void rush_order(orderqueue& q){
    cout << "Enter Order No. to rush: ";
    string line; if(!getline(cin,line)) return;
    line = trim(line);

    long long id; bool ok=false;
    {
        istringstream iss(line);
        if(iss>>id){
            string extra; if(!(iss>>extra)) ok=true;
        }
    }
    if(!ok){
        cout << "Invalid input." << endl;
        cout << "---" << endl;
        return;
    }

    int r = q.rush_by_id((int)id);
    if(r==-1){
        cout << "No orders to rush." << endl;
        cout << "---" << endl;
    }else if(r==0){
        cout << "Order No. " << id << " not found." << endl;
        cout << "---" << endl;
    }else if(r==1){
        cout << "Order No. " << id << " rushed (price +30%)." << endl;
        cout << "---" << endl;
    }else{ // r==2
        cout << "Order No. " << id << " moved to front and rushed (price +30%)." << endl;
        cout << "---" << endl;
    }
}

// exit (choice 8)
void exit_sequence(orderqueue& q){
    if(q.isempty()){
        cout << "No orders left to prepare." << endl;
        cout << "Exiting..." << endl;
        return;
    }
    cout << "Preparing remaining orders..." << endl;
    while(!q.isempty()){
        prepare_next(q);
    }
    cout << "Exiting..." << endl;
}

// ===================== main =====================
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    load_menu_startup();   // reads file, fills cdll, prints system menu

    orderqueue q;

    while(true){
        cout << "Enter choice (1-8): ";
        string line; if(!getline(cin,line)) return 0;
        line = trim(line);

        int choice=-1; bool ok=false;
        {
            istringstream iss(line);
            if(iss>>choice){
                string extra; if(!(iss>>extra)) ok=true;
            }
        }

        if(!ok || choice<1 || choice>8){
            cout << "---" << endl;
            cout << "Invalid choice (1-8)." << endl;
            cout << "---" << endl;
            continue;
        }

        cout << "---" << endl;

        if(choice==1){
            show_menu_forward();
        }else if(choice==2){
            place_new_order(q);
        }else if(choice==3){
            prepare_next(q);
        }else if(choice==4){
            display_queue(q);
        }else if(choice==5){
            show_report();
        }else if(choice==6){
            cancel_order(q);
        }else if(choice==7){
            rush_order(q);
        }else if(choice==8){
            exit_sequence(q);
            clear_menu_all();
            return 0; // cya
        }

        // cout << "[dbg] loop end\n";
    }
}
