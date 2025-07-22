#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;


// Function to open a file and create a matrix from its contents
// Function to open a file and create a matrix from its contents
bool openandcreate(ifstream &input, int &numRows, int &numWords, vector<vector<char>> &matrix, vector<string> &words) {
    
    string line;
    int j = 0;

    getline(input, line); // Read the first line to determine the number of rows
    numRows = stoi(line); 
  
    // matrix.resize(numRows); // Create a 2D vector to store the matrix → YORUMU SİLMEDEDEN bıraktım

    while (j < numRows && getline(input, line)) { 
        vector<char> row;
        for (int i = 0; i < line.size(); ++i) {
            row.push_back(line[i]); // Store each character in the row
        }
        matrix.push_back(row);
        cout << line << endl; // Added for debug purposes || this note is for me:  bitirince bunu yorum yapmayı unutma !!
        j++;
    }

    /*
    Çalışmazsa buna geri dön yarın
    for (int i = 0; i < numRows; ++i) {
        vector<char> row;
        for (int j = 0; j < numRows; ++j) {
            char ch;
            input >> ch; // Read each character in the row
            row.push_back(ch); 
        }
        matrix.push_back(row);
    }
    */

    getline(input, line); // Read the number of words
    stringstream ss(line);
    ss >> numWords;

    if (numWords == 0) {
        return true; 
    } else {
        // words.resize(numWords);
        int i = 0;
        while (i < numWords && getline(input, line)) { 
            words.push_back(line);
            i++;
        }
    }

    return true;
}



void Spirallreadandprint(vector<vector<char>> &matrix) {
    int top = 0;
    int bottom = matrix.size() - 1;
    int left = 0;
    int right = matrix[0].size() - 1;

    while() // to be continiued...
    
    cout << endl;
}


int main(){
    int numRows;
    int numWords;
    vector<vector<char>> matrix;
    vector<string> words;
    string filename;
    cin >> filename;

    ifstream input;
    input.open(filename);
    if (input.fail()) {
        cout << "Error opening file: " << filename << endl;
        return 0; 
    }

    if (openandcreate(input, numRows, numWords, matrix, words)) {
        Spirallreadandprint(matrix);
    } else {
        
    }

    return 0;
}
