/*
Static-scoping implementation for a custom language
Using doubly Linked-list like implementation
Built a symbol table from scratch using seperate-chaining hashing technique
*/

// Including necessary libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Maximum indices avaialble for hash-array
#define MAX_CAPACITY 101

typedef enum {
    FAILURE,
    SUCCESS
} status_code;

using namespace std;

// Hash-map node to store key,value and next pointer for Linked-list implementation
class Node{
    public:
        string key;
        int value;
        Node* next;
        // Constructor
        Node(string key = "", int value = 0) : key(key), value(value), next(nullptr){}
};

class Map{
    private:
        Node* array[MAX_CAPACITY];

        int hashfunction(const string& key){
            /*
            Hash function to convert a string to a bucketIndex to allocate via seperate chaining
            */
            int bucketIndex;
            int sum = 0, factor = 31;
            for(int i=0 ; i<key.length() ; i++){
                sum = ((sum % MAX_CAPACITY) + (((int)key[i]) * factor) % MAX_CAPACITY) % MAX_CAPACITY;
                factor = ((factor % 32767) * (31 % 32767)) % 32767;
            }
            bucketIndex = sum;
            return bucketIndex;
        }

    public:
        // Constructor
        Map(){
            for(int i=0; i<MAX_CAPACITY ; i++){
                array[i] = nullptr;
            }
        }

        void insert(const string& key, int value){
            int bucketIndex = hashfunction(key);
            Node* newNode = new Node(key,value);
            if(array[bucketIndex] == nullptr){
                array[bucketIndex] = newNode;
            }
            else{
                Node *prev,*curr = array[bucketIndex];
                int found = 0;
                while(curr && !found){
                    if(curr -> key == key){
                        curr -> value = value;
                        found = 1;
                    }
                    else{
                        prev = curr;
                        curr = curr -> next;
                    }
                }
                if(!found){
                    prev -> next = newNode;
                }
            }
        }

        void deleteKey(const string& key){
            int bucketIndex = hashfunction(key);
            if(array[bucketIndex] != nullptr){
                Node *currNode = array[bucketIndex], *prev;
                int found = 0;
                if(array[bucketIndex] -> key == key){
                    prev = array[bucketIndex];
                    array[bucketIndex] = array[bucketIndex] -> next;
                    delete prev;
                }
                else{
                    prev = array[bucketIndex];
                    currNode = currNode -> next;
                }
                while(!found && currNode != nullptr){
                    if(currNode -> key == key){
                        prev -> next = currNode -> next;
                        delete currNode;
                        found = 1;
                    }
                }
            }
        }

        int find(const string& key, bool& found){
            int bucketIndex = hashfunction(key);
            int value;
            if(array[bucketIndex] == nullptr){
                found = false;
            }
            else{
                Node *curr = array[bucketIndex];
                while(curr && !found){
                    if(curr -> key == key){
                         value = curr -> value;
                         found = 1;
                    }
                    else{
                        curr = curr -> next;
                    }
                }
            }
            return value;
        }

        // Destructor for releasing dynamically allocated memory
        ~Map(){
            for(int i=0; i<MAX_CAPACITY; i++){
                Node *ptr = array[i],*temp;
                while(ptr != nullptr){
                    temp = ptr;
                    ptr = ptr -> next;
                    delete temp;
                }
            }
        }
};

class Scope{
    private:
        Scope* prev;
        Scope* nested_scope;
        Map map;

    public:
        // Constructor
        Scope() : prev(nullptr), nested_scope(nullptr) {}

        // Getter and Setter methods for accessing private data-members of Scope class
        void set_prev(Scope* prev_scope){
            prev = prev_scope;
        }

        void set_nested_scope(Scope* new_scope){
            nested_scope = new_scope;
        }

        Scope* get_prev() const{
            return prev;
        }

        Scope* get_nested_scope() const{
            return nested_scope;
        }

        void assign_value(string var, int val){
            map.insert(var,val);
            cout << "Assigned " << var << " = " << val << endl;
        }

        int print_val(string var, bool& found){
            int ret_val = -1, temp;
            temp = map.find(var,found);
            if(found){
                ret_val = temp;
            }
            return ret_val;
        }

        // Destructor
        ~Scope() {}
};

status_code begin_scope(Scope** curr_scope_ptr){
    status_code sc = FAILURE;
    Scope* new_scope = new Scope();
    if(new_scope != nullptr){
        Scope* curr_scope = *curr_scope_ptr;
        new_scope->set_prev(curr_scope);
        if(curr_scope) curr_scope->set_nested_scope(new_scope);
        curr_scope = new_scope;
        *curr_scope_ptr = curr_scope;
    }
    else{
        cout << "Memory Allocation Failed " << endl;
    }
    return sc;
}

void end_scope(Scope** curr_scope_ptr){
    Scope* curr_scope = *curr_scope_ptr;
    Scope* temp = curr_scope;
    curr_scope = curr_scope -> get_prev();
    *curr_scope_ptr = curr_scope;
    // Freeing allocated memory
    delete temp;
}

void assign_variable(string variable, int value, Scope* curr_scope){
    if(curr_scope != nullptr){
        curr_scope -> assign_value(variable,value);
    }
    else{
        cerr << "No scope defined for given variable" << endl;
    }
}

void print_var(string variable, Scope* curr_scope){
    bool found = false;
    Scope* ptr = curr_scope;
    int value;
    while(ptr != nullptr && !found){
        value = ptr->print_val(variable, found);
        if(found){
            cout << "Value of variable " << variable << " is " << value << endl;
        }
        else{
            ptr = ptr -> get_prev();
        }
    }
    if(!found){
        cerr << "Variable (" << variable << ") doesnt exist in any scope" << endl; 
    }
}

// Function to remove left-trailing spaces from a string
string removeLeftSpaces(const string& line) {
    string leftRemoved = "";
    int i = 0;
    for(i ; i<line.length() && line[i] == ' ' ; i++);
    for(i ; i<line.length() ; i++){
        leftRemoved += line[i];
    }
    return leftRemoved;
}

// Temporary function to check file contnets being printed
void printFileContents(vector<string>& lines_of_code){
    for(const string& line : lines_of_code){
        cout << line << " size of line : " << line.length() << endl;
    }
}

bool checkSyntax(const string& file_name) {
    ifstream input_file(file_name);
    if (!input_file.is_open()) {
        cerr << "File couldn't be opened" << endl;
        return false;
    }

    string line;
    vector<string> lines_of_code;
    int begin_count = 0;
    int end_count = 0;
    bool syntax_valid = true;

    while (getline(input_file, line)) {
        lines_of_code.push_back(removeLeftSpaces(line));
    }
    input_file.close();

    for (const string& line : lines_of_code) {
        stringstream ss(line);
        string word;
        vector<string> words;
        while (ss >> word) {
            words.push_back(word);
        }

        if (words.empty()) {
            continue;
        }

        string command = words[0];
        if (command == "begin") {
            if (words.size() != 1) {
                cerr << "Syntax error: 'begin' command should not have arguments" << endl;
                syntax_valid = false;
            }
            begin_count++;
        } 
        else if (command == "end") {
            if (words.size() != 1) {
                cerr << "Syntax error: 'end' command should not have arguments" << endl;
                syntax_valid = false;
            }
            end_count++;
        } 
        else if (command == "assign") {
            if (words.size() != 3) {
                cerr << "Syntax error: 'assign' command should have exactly 2 arguments" << endl;
                syntax_valid = false;
            }
        } 
        else if (command == "print") {
            if (words.size() != 2) {
                cerr << "Syntax error: 'print' command should have exactly 1 argument" << endl;
                syntax_valid = false;
            }
        } 
        else {
            cerr << "Syntax error: Invalid command '" << command << "'" << endl;
            syntax_valid = false;
        }
        if(end_count > begin_count){
            cerr << "Syntax error: No 'begin' specified for 'end' command" << endl;
            syntax_valid = false;
        }
    }

    if (begin_count != end_count) {
        cerr << "Syntax error: Mismatched 'begin' and 'end' commands" << endl;
        syntax_valid = false;
    }

    return syntax_valid;
}

void Parser(const string& file_name, vector<string>& lines_of_code) {
    ifstream input_file(file_name);

    if (!input_file.is_open()) {
        cerr << "File Error: File couldn't be opened" << endl;
    } 
    else {
        string line;
        while (getline(input_file, line)) {
            string trimmed_line = removeLeftSpaces(line);
            if (!trimmed_line.empty()) {
                lines_of_code.push_back(trimmed_line);
            }
        }
    }
    input_file.close();
}

int main() {
    Scope* curr_scope = nullptr;
    string input_file = "input.txt";
    vector<string> lines_of_code;

    if (!checkSyntax(input_file)) {
        cerr << "Syntax errors found. Exiting." << endl;
        return 1;
    }

    Parser(input_file, lines_of_code);

    for (int i = 0; i < lines_of_code.size(); i++) {
        stringstream ss(lines_of_code[i]);
        string word;
        vector<string> words;
        while (ss >> word) {
            words.push_back(word);
        }

        // Command Handler
        if (words[0] == "begin") {
            begin_scope(&curr_scope);
        } 
        else if (words[0] == "end") {
            end_scope(&curr_scope);
        } 
        else if (words[0] == "assign") {
            assign_variable(words[1], stoi(words[2]), curr_scope);
        } 
        else if (words[0] == "print") {
            print_var(words[1], curr_scope);
        } 
        else {
            cout << "Invalid Command" << endl;
        }
    }
    return 0;
}
