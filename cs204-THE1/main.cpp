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


// Function to read the matrix in spiral order and print it
void Spirallreadandprint(vector<vector<char>> &matrix) {
    int top = 0;
    int bottom = matrix.size() - 1;
    int left = 0;
    int right = matrix[0].size() - 1;

    while (top <= bottom && left <= right) {
        // Print the top row
        for (int i = left; i <= right; ++i) {
            cout << matrix[top][i] << " ";
        }
        cout << "---"; // ne yazılacağını unuttum internetim yok bunu unutma değiştirmeyi
        top = top +1; // preyle postu karıştırdım bu kodu okuyunca slayta bak

        // Print the right column
        for (int i = top; i <= bottom; ++i) {
            cout << matrix[i][right] << " ";
        }
        right = right - 1;
        cout << "---";
        // Print the bottom row if still within bounds
        if (top <= bottom) {
            for (int i = right; i >= left; --i) {
                cout << matrix[bottom][i] << " ";
            }
            bottom = bottom -1 ;
        }
        cout << "---";
        // Print the left column if still within bounds
        if (left <= right) {
            for (int i = bottom; i >= top; --i) {
                cout << matrix[i][left] << " ";
            }
            left = left + 1;
        }
        cout << "---";
    }
    
    // Yarın test etmeyi unutma 
    
    
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
