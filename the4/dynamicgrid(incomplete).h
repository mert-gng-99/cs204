#ifndef _dynamicgrid
#define _dynamicgrid

class DynamicGrid {
private:
    struct GridRow {
        int* elements;
        int  capacity;
        int  actualSize;
        GridRow* next;
        // add constructor
    };
    GridRow* firstRow;
    // consider what other members you might need

public:
    DynamicGrid(const int &, const int &);
    int  GetIndex(const int &, const int &) const;
    void SetIndex(const int &, const int &, const int &) const;
    // add missing constructors, destructor, and operators
};

DynamicGrid::DynamicGrid(const int & r, const int & c) : firstRow(nullptr) {
    for (int i = 0; i < r; i++) {
        GridRow* row = new GridRow;
        row->capacity   = c;
        row->actualSize = c;
        row->elements   = new int[c];
        row->next       = firstRow;
        firstRow        = row;
    }
}

int DynamicGrid::GetIndex(const int & i, const int & j) const {
    GridRow* cur = firstRow;
    for (int k = 0; k < i && cur != nullptr; k++)
        cur = cur->next;
    if (cur != nullptr && j < cur->actualSize)
        return cur->elements[j];
    return 0;
}

void DynamicGrid::SetIndex(const int & i, const int & j, const int & value) const {
    GridRow* cur = firstRow;
    for (int k = 0; k < i && cur != nullptr; k++)
        cur = cur->next;
    if (cur != nullptr && j < cur->actualSize)
        cur->elements[j] = value;
}

#endif