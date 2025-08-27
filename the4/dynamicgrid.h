// cs204 - THE4 DynamicGrid
// Mert Güngör 34159 

#ifndef DYNAMICGRID_H
#define DYNAMICGRID_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
using namespace std;


// template for any T 
template <typename T>
class DynamicGrid {
private:
    // singly linked list: each row owns its own dynamic array
    struct gridrow {
        T* elems;        // cells
        int capacity;    // allocated size
        int used;        // how many cells are actually used (from left)
        gridrow* next;   // next row

        // ctor: make a row. default cap=4, used=0
        
        gridrow(int cap = 4, int use = 0) {

            elems    = nullptr;
            capacity = cap;
            
            used     = use;
            next     = nullptr;
        
            // tiny safety: capacity should be at least 1 so even if someone pass 0 or negative, we still have memory
            if (capacity < 1){

                capacity = 1;
            }
        
            // allocate memory for cells
            elems = new T[capacity];
        
            
            //  fill with default values so printing is clean because we only print used count but still nice to have defaults 
            for (int i = 0; i < capacity; i = i + 1) {

                // default construct value of T 
                elems[i] = T();
            }
        }

    };

    gridrow* first;   // head row nullptr if empty
    int rowcount;     // number of rows
    int maxcols;      // widest row ever seen 

  
   
    gridrow* get_row_ptr(int r) const {


        if ((r < 0) || (r >= rowcount)) {
            // outside  return null so caller can handle default behavior
            return nullptr;
        }

        gridrow* cur = first;

        for (int i = 0; i < r; i = i + 1)
        {

            cur = cur->next;
        }

        return cur;
    }

    
    // capacity=width used=widt all cells pre filled with T
    gridrow* append_row_default(int width){


        if (width < 1)
        {
            width = 1;
        }

        // make a row with width cells all default initialized
        gridrow* node = new gridrow(width, width);

        if (first == nullptr){

            // first ever node becomes head
            first = node;
        }
        else{

            // go tail and chain to end
            gridrow* cur = first;

            while (cur->next != nullptr)
            {
                cur = cur->next;
            }

            cur->next = node;
        }

        // grid got one more row
        rowcount = rowcount + 1;

        // keep track of widest row we ever had 
        if (width > maxcols)
        {
            maxcols = width;
        }

        return node;
    }

    
    gridrow* append_row_filled(const T& v, int width){


        gridrow* node = append_row_default(width);

        // overwrite all used cells with v
        for (int i = 0; i < node->used; i = i + 1)
        {
            node->elems[i] = v;
        }

        return node;
    }

    // ensure a row can hold at least need cells / capacity grow.
   
    void ensure_row_capacity(gridrow* row, int need){


        if (row == nullptr)

        {
            // nothing to do if row not exists 
            return;
        }

        if (need <= row->capacity)
        {
            // already enough chill :D
            return;
        }

        
        // this avoids re alloc all the time when user create big index
        int newcap = row->capacity * 2;


        if (newcap < need){

            // if doubling not enough go directly to need 
            newcap = need;
        }

        // make new bigger array
        T* arr = new T[newcap];

        // prefill with default to avoid garbage prints
        for (int i = 0; i < newcap; i = i + 1){

            arr[i] = T();
        }

        // copy old values
        for (int i = 0; i < row->used; i = i + 1){

            arr[i] = row->elems[i];
        }

        // free old memory / the memory that we own it
        delete [] row->elems;

        // switch pointers and update cap
        row->elems    = arr;
        row->capacity = newcap;
    }

    
    // ensure used count reaches newused fill new part with defaults also pushes maxcols if we got wider
    void ensure_row_used(gridrow* row, int newused){


        if (row == nullptr){


            return;
        }

        if (newused < 0){

            newused = 0;
        }

        if (newused > row->capacity){

            // need more memory to store new cells
            ensure_row_capacity(row, newused);
        }

        // fill the newly exposed part with defaults
        if (newused > row->used){

            for (int i = row->used; i < newused; i = i + 1){

                row->elems[i] = T();
            }
        }

        // finally this row logically has that many cells now
        row->used = newused;

        // record the widest row we ever had
        if (row->used > maxcols){


            maxcols = row->used;
        }
    }

    // delete whole grid all rows + arrays used by destructor and operator>>
    void clear_all()
    {
        gridrow* cur = first;

        while (cur != nullptr){

            gridrow* nxt = cur->next;

            delete [] cur->elems;
            delete cur;

            cur = nxt;
        }

        // reset everything to empty state
        first    = nullptr;
        rowcount = 0;
        maxcols  = 0;
    }

    // make sure we have rows up to r 0..r if not append default rows width=4
    void ensure_row_exists(int r)
    {
        if (r < 0)
        {
            // negative row makes no sense, we make it  in caller anyway
            return;
        }

        if (r < rowcount)
        {
            // already exists
            return;
        }

        int need = r + 1;

        // if grid is empty, create the first 4-wide row
        if (first == nullptr){


            first    = new gridrow(4, 4);
            rowcount = 1;

            if (maxcols < 4){

                maxcols = 4;
            }
        }

        // keep appending default rows until index r exists
        while (rowcount < need){

            append_row_default(4);
        }
    }

public:
    // ==*** ctor/dtor/assign ***===
    DynamicGrid(): first(nullptr), rowcount(0), maxcols(0)
    {
        // empty grid, nothing to do
     
    }

    DynamicGrid(int rows, int cols): first(nullptr), rowcount(0), maxcols(0){

        if (rows < 0){

            rows = 0;
        }

        if (cols < 1){

            cols = 1;
        }

        // create rectangular grid rows x cols
        for (int i = 0; i < rows; i = i + 1)
        {
            append_row_default(cols);
        }
    }

    DynamicGrid(const DynamicGrid& other): first(nullptr), rowcount(0), maxcols(0)
    {
        // deep copy row by row own storage no sharing
        gridrow* cur = other.first;

        while (cur != nullptr){


            gridrow* n = append_row_default(cur->used);

            for (int j = 0; j < cur->used; j = j + 1){

                n->elems[j] = cur->elems[j];
            }

            cur = cur->next;
        }

        // keep the widest info too 
        if (other.maxcols > maxcols){


            maxcols = other.maxcols;
        }
    }

    ~DynamicGrid(){

        
        clear_all();
    }

    DynamicGrid& operator=(const DynamicGrid& rhs){

        if (this == &rhs) {

            
            return *this; // self-assign, do nothing
        }

        clear_all();

        // deep copy again same as copy ctor
        gridrow* cur = rhs.first;

        while (cur != nullptr) {

            gridrow* n = append_row_default(cur->used);

            for (int j = 0; j < cur->used; j = j + 1)
            {

                n->elems[j] = cur->elems[j];
            }

            cur = cur->next;
        }

        if (rhs.maxcols > maxcols) {


            maxcols = rhs.maxcols;
        }

        return *this;
    }

    // ====== tiny getters ======

    int GetRowCount() const { 
        return rowcount; 
    }
    int GetMaxCols()  const {
        return maxcols;
    }

    // read without auto grow outof range - default T

    T GetIndex(int r, int c) const {


        gridrow* row = get_row_ptr(r);

        if (row == nullptr){

            return T();
        }

        if ((c < 0) || (c >= (row->used))){

            return T();
        }

        return row->elems[c];
    }

    // write without auto grow  outof range -> ignore 
    
    void SetIndex(int r, int c, const T& value) {

        gridrow* row = get_row_ptr(r);

        if (row == nullptr){

            return; // no auto expand here on purpose
        }

        if (c < 0 || c >= row->used)
        {
            return;
        }

        row->elems[c] = value;
    }

    // access: auto grow both rows and columns and return a reference
    T& operator()(int r, int c){

        if (r < 0){

            r = 0;
        }

        if (c < 0)
        {
            c = 0;
        }

        // make sure the row exist append default rows if needed
        ensure_row_exists(r);

        gridrow* row = get_row_ptr(r);

        // if column too big, enlarge used and maybe capacity to c+1
        if (c >= (row->used)) {

            ensure_row_used(row, c + 1);
        }

        return row->elems[c];
    }

    // ^ : append a new row filled with v
    // width: use maxcols if known else 4 
   
    DynamicGrid& operator^(const T& v) {

        int width = (maxcols > 0 ? maxcols : 4);

        append_row_filled(v, width);

        return *this;
    }

    // %  in each row, remove all target values and left shift the rest
    DynamicGrid& operator%(const T& target){


        gridrow* cur = first;


        while (cur != nullptr){

            int keep = 0;

            for (int i = 0; i < cur->used; i = i + 1){

                if (!(cur->elems[i] == target)){

                    cur->elems[keep] = cur->elems[i];
                    keep = keep + 1;
                }
            }

            // clean the tail for nice printing
            // i think it is not necessary but it makes my code cleaner i think :D
            for (int i = keep; i < cur->used; i = i + 1){

                cur->elems[i] = T();
            }

            // new logical length is keep
            cur->used = keep;

            cur = cur->next;
        }

        return *this;
    }

    // ++ : pad all rows to maxcols using default values
    DynamicGrid& operator++() { // prefix
        int target = maxcols;

        gridrow* cur = first;

        while (cur != nullptr)
        {
            if (cur->used < target)
            {
                // grow to target width auto capacity if needed
                ensure_row_used(cur, target);
            }

            cur = cur->next;
        }

        return *this;
    }

    // ~ : total number of used cells sum of row.used
    // not capacity only meaningful cells count
    int operator~() const{

        int total = 0;

        gridrow* cur = first;

        while (cur != nullptr){


            total = total + cur->used;
            cur   = cur->next;

        }

        return total;
    }

    // ====== stream İ/O ======
   
    // operator<< prints each row on its own line, up to used elements

    friend ostream& operator<<(ostream& os, const DynamicGrid& g)
    {
        const gridrow* cur = g.first;

        while (cur != nullptr)
        {

            for (int i = 0; i < cur->used; i = i + 1){

                os << setw(10) << cur->elems[i];
            }

            os << '\n';

            cur = cur->next;
        }

        return os;
    }

    // operator>> builds a rectangular grid from input:
    // width = token count of first line if first empty  4
    // clearing old grid first and rebuild fresh
    friend istream& operator>>(istream& is, DynamicGrid& g){

        // drop old content first
        g.clear_all();

        string line;
        int width = -1;
        bool got_any = false;

        while (getline(is, line)){

            got_any = true;

            // decide width from the first line
            if (width == -1)
            {
                int cnt = 0;
                {
                    istringstream tmp(line);
                    T tmpv;

                    // count tokens in first line
                    while (tmp >> tmpv){

                        cnt = cnt + 1;
                    }
                }

                // if first line is empty, default width = 4
                if (cnt > 0){

                    width = cnt;
                }

                else{

                    width = 4;
                }
            }

            // add a row with fixed width
            gridrow* row = g.append_row_default(width);

            // fill tokens up to width
            istringstream iss(line);
            int filled = 0;
            T val;

            while ((filled < width) && (iss >> val)){

                row->elems[filled] = val;
                filled = filled + 1;
            }
        }

        // update maxcols if we actually read something
        if ((got_any) && (width > 0 )&& (g.maxcols) < width) {
            
            g.maxcols = width;
        }

        return is;
    }
};

#endif 
