// cs204 - the3 starbucks order management system
// Mert Gungor 34159
//
// NOTE: linked-list ONLY (no arrays/vectors).
// queue (orders) = singly linked list
// stack (drinks) = singly linked list
// menu = circular doubly linked list (cdll) from the2 (only for storing items)



#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

// ===================== tiny helpers =====================

// func: tolowerstr -> make all letters lower-case, i forgat std api name lol
// super basic O(n) pass, not fancy at all
string tolowerstr(string s)
{
    for (size_t i = 0; i < s.length(); i++)
    {
        s[i] = (char)tolower((unsigned char)s[i]);
    }
    return s;
}

// func: equals_nocase -> compare strings w/out caring upper/lower (case ignoor)
bool equals_nocase(const string& a, const string& b)
{
    return tolowerstr(a) == tolowerstr(b);
}

// func: trim -> cut spaces from begin&end (probly works ok)
string trim(const string& s)
{
    int i = 0;
    int j = (int)s.length();

    // // cout << "[DBG] start trim\n";
    // loop: skip head spases (typo ok), move i forward until non-space
    while (i < j && isspace((unsigned char)s[i]))
    {
        // // cout << "[DBG] skip head: " << s[i] << endl;
        i++;
    }
    // loop: skip tail whites (ye i know spelling), move j back
    while (j > i && isspace((unsigned char)s[j - 1]))
    {
        // // cout << "[DBG] skip tail: " << s[j-1] << endl;
        j--;
    }
    return s.substr(i, j - i);
}

// ===================== cdll menu (from the2; only to store items) =====================

struct drink
{
    int id;
    string name;
    double rating;
    double price;
};

struct mnode
{
    drink data;
    mnode* prev;
    mnode* next;

    mnode() : prev(NULL), next(NULL) {}
};

mnode* menu_head = NULL;       // first item
int     menu_next_id = 1;      // auto id

// func: parse_menu_line -> read 3 toks: name rating price ; if extra token then nope
// student style parsing, no exepctions etc
bool parse_menu_line(const string& line, string& nm, double& rt, double& pr)
{
    // // cout << "[DBG] parse line: " << line << endl;
    istringstream iss(line);
    if (!(iss >> nm >> rt >> pr))
    {
        // // cout << "[DBG] parse fail\n";
        return false;
    }
    string extra;
    if (iss >> extra)
    {
        // // cout << "[DBG] extra token: " << extra << endl;
        return false;
    }
    // // cout << "[DBG] parse ok for: " << nm << endl;
    return true;
}

// func: insert_tail_menu -> put new drink to end of cdll (tail), update prev/next links
void insert_tail_menu(const string& name, double rating, double price)
{
    mnode* n = new mnode();
    n->data.id = menu_next_id++;
    n->data.name = name;
    n->data.rating = rating;
    n->data.price = price;

    if (menu_head == NULL)
    {
        n->next = n;
        n->prev = n;
        menu_head = n;
    }
    else
    {
        mnode* tail = menu_head->prev;
        n->prev = tail;
        n->next = menu_head;
        tail->next = n;
        menu_head->prev = n;
    }
}

// func: find_by_name_menu -> go around circle once, case-insens search
mnode* find_by_name_menu(const string& name)
{
    if (menu_head == NULL)
    {
        return NULL;
    }
    mnode* cur = menu_head;
    // loop: traverse cdll one full cycle, chek names (speling? idk)
    do
    {
        if (equals_nocase(cur->data.name, name))
        {
            return cur;
        }
        cur = cur->next;
    }
    while (cur != menu_head);

    return NULL;
}

// func: print_drink -> dump fields as required, simple i/o
void print_drink(const drink& d)
{
    cout << "ID: " << d.id << endl;
    cout << "Name: " << d.name << endl;
    cout << "Rating: " << d.rating << endl;
    cout << "Price: " << d.price << endl;
}

// func: show_menu_forward -> print menu from head forward, stops when loops back
void show_menu_forward()
{
    if (menu_head == NULL)
    {
        cout << "The menu is empty." << endl;
        cout << "---" << endl;
        return;
    }

    mnode* cur = menu_head;
    // loop: classic while(true) beacuse cdll end is when next==head
    // loop: keep taking inputs until user types enough (case-insesitive)
    // loop: main menu loop, reads choice every time (kinda basic ui)
    while (true)
    {
        print_drink(cur->data);
        cout << "---" << endl;

        if (cur->next == menu_head)
        {
            break;
        }
        cur = cur->next;
    }
}

// func: clear_menu_all -> free whole cdll memmory, reset head/id
void clear_menu_all()
{
    if (menu_head == NULL)
    {
        return;
    }

    mnode* cur = menu_head->next;
    // loop: walk nodes until we come back to head, delete each (careful)
    while (cur != menu_head)
    {
        mnode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    delete menu_head;

    menu_head = NULL;
    menu_next_id = 1;
}

// func: get_price_from_menu -> helper to fetch price by name (case-insens)
bool get_price_from_menu(const string& name, double& out_price)
{
    mnode* f = find_by_name_menu(name);
    if (!f)
    {
        return false;
    }
    out_price = f->data.price;
    return true;
}

// ===================== dynamic stack for drinks (linked list ONLY) =====================

struct drinkitem
{
    string name;
    int    sugar;
};

struct snode
{
    drinkitem data;
    snode*    next;

    snode(const string& n, int s) : next(NULL)
    {
        data.name  = n;
        data.sugar = s;
    }
};

// class: drinkstack -> LIFO for drinks, single linked list, very basic implemant
// has destructor to avoid mem leak (i hope)
class drinkstack
{
    snode* topnode;

public:

    drinkstack() : topnode(NULL) { }

    ~drinkstack()
    {
        clear();
    }

    bool isempty() const
    {
        return topnode == NULL;
    }

    void push(const string& name, int sugar)
    {
        // // cout << "[DBG] push " << name << " sugar=" << sugar << endl;
        snode* x = new snode(name, sugar);
        x->next = topnode;
        topnode = x;
    }

    bool pop(drinkitem& out)
    {
        if (topnode == NULL)
        {
            return false;
        }

        snode* x = topnode;
        out = x->data;
        topnode = x->next;
        delete x;
        return true;
    }
    void print_topdown() const
    {
        // loop: walk from top to bottom and print (dont mutate stack)
        snode* cur = topnode;
        // loop: iterate queue from front to rear and print all
        while (cur)
        {
            cout << cur->data.name << " with " << cur->data.sugar << " sugar packs" << endl;
            cur = cur->next;
        }
    }
    void stats(int& cnt, long long& sugar_sum) const
    {
        // for-loop: count items & sum sugars, very straigth forward
        cnt = 0;
        sugar_sum = 0;

        for (snode* cur = topnode; cur; cur = cur->next)
        {
            cnt++;
            sugar_sum += cur->data.sugar;
        }
    }
    void clear()
    {
        // loop: free every node, set top to null at end
        snode* cur = topnode;
        while (cur)
        {
            snode* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        topnode = NULL;
    }
};

// ===================== dynamic queue for orders (linked list ONLY) =====================

struct order
{
    int        id;
    drinkstack drinks;
    double     totalprice;

    order(int i = 0) : id(i), totalprice(0.0) { }
};

struct qnode
{
    order* ord;
    qnode* next;

    qnode(order* o) : ord(o), next(NULL) { }
};

// class: orderqueue -> FIFO for orders, single linked, with front/rear ptrs
// rush moves node to front and also 1.3x price
class orderqueue
{
    qnode* front;
    qnode* rear;

public:

    orderqueue() : front(NULL), rear(NULL) { }

    ~orderqueue()
    {
        clear_all();
    }

    bool isempty() const
    {
        return front == NULL;
    }

    void enqueue(order* o)
    {
        // // cout << "[DBG] enqueue id=" << o->id << endl;
        qnode* q = new qnode(o);

        if (rear == NULL)
        {
            front = q;
            rear  = q;
        }
        else
        {
            rear->next = q;
            rear = q;
        }
    }

    order* dequeue()
    {
        if (front == NULL)
        {
            return NULL;
        }

        qnode* q = front;
        front = front->next;

        if (front == NULL)
        {
            rear = NULL;
        }

        order* o = q->ord;
        delete q;
        return o;
    }

    // returns: -1 empty, 0 not found, 1 canceled
    int cancel_by_id(int id)
    {
        if (front == NULL)
        {
            return -1;
        }

        qnode* prev = NULL;
        qnode* cur  = front;
        // loop: search by id, keep prev to unlink later, kind of clasic
        // loop: walk until target id for rushing
        while (cur && cur->ord->id != id)
        {
            prev = cur;
            cur  = cur->next;
        }

        if (cur == NULL)
        {
            return 0;
        }

        if (prev == NULL)
        {
            front = cur->next;
            if (front == NULL)
            {
                rear = NULL;
            }
        }
        else
        {
            prev->next = cur->next;
            if (cur == rear)
            {
                rear = prev;
            }
        }

        delete cur->ord;
        delete cur;
        return 1;
    }

    // rush: price *= 1.3 and maybe move to front
    // returns: -1 empty, 0 not found, 1 rushed(no move), 2 moved+rushed
    int rush_by_id(int id)
    {
        if (front == NULL)
        {
            return -1;
        }

        if (front->ord->id == id)
        {
            front->ord->totalprice = front->ord->totalprice * 1.3;
            return 1;
        }

        qnode* prev = front;
        qnode* cur  = front->next;

        while (cur && cur->ord->id != id)
        {
            prev = cur;
            cur  = cur->next;
        }

        if (cur == NULL)
        {
            return 0;
        }

        prev->next = cur->next;
        if (cur == rear)
        {
            rear = prev;
        }

        cur->next = front;
        front = cur;

        front->ord->totalprice = front->ord->totalprice * 1.3;
        return 2;
    }

    void print_queue() const
    {
        if (front == NULL)
        {
            cout << "No orders in the queue." << endl;
            cout << "---" << endl;
            return;
        }

        cout << "Current orders:" << endl;

        qnode* cur = front;
        while (cur)
        {
            cout << "Order No. " << cur->ord->id << endl;
            cout << "Drinks:" << endl;
            cur->ord->drinks.print_topdown();   // LIFO look
            cout << "Total Price: " << cur->ord->totalprice << endl;
            cout << "---" << endl;

            cur = cur->next;
        }
    }

    void clear_all()
    {
        // loop: destroy whole queue, delete order then node
        while (front)
        {
            qnode* q = front;
            front = front->next;
            delete q->ord;
            delete q;
        }
        rear = NULL;
    }
};

// ===================== ui & globals =====================

void print_system_menu()
{
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

// stats
long long completed_orders = 0;
long long drinks_sold      = 0;
long long sugar_packs      = 0;
double    revenue          = 0.0;

// id generator
int next_order_id = 100;

// ===================== flows =====================

// func: load_menu_startup -> ask file, read menu lines, print system menu at end
void load_menu_startup()
{
    cout << "Enter menu filename (e.g., menu.txt): ";

    string fname;
    getline(cin, fname);
    fname = trim(fname);

    ifstream fin(fname);

    if (fin.fail())
    {
        // samples do not show an explicit error line here, so keep silent
        // // cout << "[DBG] cannot open: " << fname << endl;
    }
    else
    {
        string line;
        // loop: read each line from file, parse and insert to cdll
        while (getline(fin, line))
        {
            line = trim(line);
            if (line.empty())
            {
                continue;
            }

            string name;
            double rating;
            double price;

            if (parse_menu_line(line, name, rating, price))
            {
                if (find_by_name_menu(name))
                {
                    cout << "The drink " << name << " is already in the menu." << endl;
                }
                else
                {
                    insert_tail_menu(name, rating, price);
                    cout << "The drink " << name << " is added to the menu." << endl;
                }
            }
            else
            {
                // // cout << "[DBG] bad line: " << line << endl;
            }
        }
        cout << "---" << endl;
    }

    print_system_menu();
}

// func: place_new_order -> read drink lines until 'enough', validate & push to stack
void place_new_order(orderqueue& q)
{
    cout << "Enter drink details (name sugar) one by one. Type 'enough' to stop." << endl;

    order* ord = new order(next_order_id);
    bool   added_any = false;

    while (true)
    {
        cout << "Enter: ";

        string line;
        if (!getline(cin, line))
        {
            delete ord;
            return;
        }

        line = trim(line);

        if (line.empty())
        {
            cout << "Invalid format. Enter name and sugar." << endl;
            continue;
        }

        if (equals_nocase(line, "enough"))
        {
            break;
        }

        string     name;
        long long  sugar;
        bool       ok = false;

        {
            istringstream iss(line);
            if (iss >> name >> sugar)
            {
                string extra;
                if (!(iss >> extra))
                {
                    ok = true;   // exactly two tokens
                }
            }
        }

        if (!ok)
        {
            cout << "Invalid format. Enter name and sugar." << endl;
            continue;
        }

        mnode* m = find_by_name_menu(name);

        if (!m)
        {
            cout << "Drink '" << name << "' not found. Try again." << endl;
            continue;
        }

        ord->drinks.push(m->data.name, (int)sugar);
        ord->totalprice += m->data.price;
        added_any = true;
    }

    if (!added_any)
    {
        delete ord;
        cout << "No drinks added." << endl;
        cout << "---" << endl;
        return;
    }

    q.enqueue(ord);

    cout << "Order No. " << ord->id << " added to the queue." << endl;
    cout << "---" << endl;

    next_order_id += 10;
}

// func: prepare_next -> pop front order, print drinks in LIFO, update stats
void prepare_next(orderqueue& q)
{
    if (q.isempty())
    {
        cout << "No orders to prepare." << endl;
        cout << "---" << endl;
        return;
    }

    order* ord = q.dequeue();

    cout << "Preparing Order No. " << ord->id << endl;
    cout << "Total price: " << ord->totalprice << endl;
    cout << "Drinks (LIFO):" << endl;

    int        cnt    = 0;
    long long  sugars = 0;

    drinkitem it;
    // loop: pop from stack until empty (LIFO), also update local counts
    while (ord->drinks.pop(it))
    {
        cout << it.name << " with " << it.sugar << " sugar packs" << endl;
        cnt++;
        sugars += it.sugar;
    }

    cout << "---" << endl;

    completed_orders += 1;
    drinks_sold      += cnt;
    sugar_packs      += sugars;
    revenue          += ord->totalprice;

    delete ord;
}

// func: display_queue -> just delegates to queue printer
void display_queue(orderqueue& q)
{
    q.print_queue();
}

// func: show_report -> dump global stats, nothing crazy
void show_report()
{
    cout << "Starbucks Order Report:" << endl;
    cout << "Completed orders: " << completed_orders << endl;
    cout << "Drinks sold: " << drinks_sold << endl;
    cout << "Sugar packs: " << sugar_packs << endl;
    cout << "Revenue: " << revenue << endl;
    cout << "---" << endl;
}

// func: cancel_order -> parse id, find & remove from queue (no stat change)
void cancel_order(orderqueue& q)
{
    cout << "Enter Order No. to cancel: ";

    string line;
    if (!getline(cin, line))
    {
        return;
    }

    line = trim(line);

    long long id;
    bool ok = false;

    {
        istringstream iss(line);
        if (iss >> id)
        {
            string extra;
            if (!(iss >> extra))
            {
                ok = true;
            }
        }
    }

    if (!ok)
    {
        cout << "Invalid input." << endl;
        cout << "---" << endl;
        return;
    }

    int res = q.cancel_by_id((int)id);

    if (res == -1)
    {
        cout << "No orders to cancel." << endl;
        cout << "---" << endl;
    }
    else if (res == 0)
    {
        cout << "Order No. " << id << " not found." << endl;
        cout << "---" << endl;
    }
    else
    {
        cout << "Order No. " << id << " canceled." << endl;
        cout << "---" << endl;
    }
}

// func: rush_order -> parse id, move to front if found, price *= 1.3
void rush_order(orderqueue& q)
{
    cout << "Enter Order No. to rush: ";

    string line;
    if (!getline(cin, line))
    {
        return;
    }

    line = trim(line);

    long long id;
    bool ok = false;

    {
        istringstream iss(line);
        if (iss >> id)
        {
            string extra;
            if (!(iss >> extra))
            {
                ok = true;
            }
        }
    }

    if (!ok)
    {
        cout << "Invalid input." << endl;
        cout << "---" << endl;
        return;
    }

    int r = q.rush_by_id((int)id);

    if (r == -1)
    {
        cout << "No orders to rush." << endl;
        cout << "---" << endl;
    }
    else if (r == 0)
    {
        cout << "Order No. " << id << " not found." << endl;
        cout << "---" << endl;
    }
    else if (r == 1)
    {
        cout << "Order No. " << id << " rushed (price +30%)." << endl;
        cout << "---" << endl;
    }
    else
    {
        cout << "Order No. " << id << " moved to front and rushed (price +30%)." << endl;
        cout << "---" << endl;
    }
}

// func: exit_sequence -> prepare all left orders (so stats update), then say bye :)
void exit_sequence(orderqueue& q)
{
    if (q.isempty())
    {
        cout << "No orders left to prepare." << endl;
        cout << "Exiting..." << endl;
        return;
    }

    cout << "Preparing remaining orders..." << endl;
    // loop: keep preparing until queue is empty (kapanis turu)
    while (!q.isempty())
    {
        prepare_next(q);
    }

    cout << "Exiting..." << endl;
}

// ===================== main =====================

// func: main -> boot, load menu, then classic command loop, ends with cleanup
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    load_menu_startup();

    orderqueue q;

    while (true)
    {
        cout << "Enter choice (1-8): ";

        string line;
        if (!getline(cin, line))
        {
            clear_menu_all();
            return 0;
        }

        line = trim(line);

        int  choice = -1;
        bool ok = false;

        {
            istringstream iss(line);
            if (iss >> choice)
            {
                string extra;
                if (!(iss >> extra))
                {
                    ok = true;
                }
            }
        }

        cout << "---" << endl;

        if (!ok || choice < 1 || choice > 8)
        {
            cout << "Invalid choice (1-8)." << endl;
            cout << "---" << endl;
            continue;
        }

        if (choice == 1)
        {
            show_menu_forward();
        }
        else if (choice == 2)
        {
            place_new_order(q);
        }
        else if (choice == 3)
        {
            prepare_next(q);
        }
        else if (choice == 4)
        {
            display_queue(q);
        }
        else if (choice == 5)
        {
            show_report();
        }
        else if (choice == 6)
        {
            cancel_order(q);
        }
        else if (choice == 7)
        {
            rush_order(q);
        }
        else if (choice == 8)
        {
            exit_sequence(q);
            clear_menu_all();
            return 0;
        }
    }
}
