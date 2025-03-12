# Nested-Scope-Handler
Written a translator for a custom language to handle nested scopes for the language. 

The scopes for the languge are handled using static scoping which has been implemented using a Doubly linked-list structure. 

## Custom Language format

```
begin
    assign a 5
    assign b 10
    assign c 60
    begin
        assign a 50
        assign b 70
        begin
            assign d 60
            print d
            assign a 1000
            print a
            print b
        end
        print a
        print c
    end
    begin
        assign c 80
        print c
        print b
    end
    print a
end
```

The language consists of the following 4 commands only :-
1. ```begin``` Used for initializing the scope
2. ```end``` Used for marking the end of a scope
3. ```assign <variable-name> <value (int only)>``` Used for assigning a value to a variable (if it exists), or otherwise create a new variable having that value.
4. ```print <variable-name>``` Used for printing the value stored by the variable. If the variable doesnt exists in that scope, then it looks for its value in its surrounding/ parent scopes.

## Approach
Here is the prototype of the Scope class used for the implementation
```
class Scope{
    private:
        Scope* prev;
        Scope* nested_scope;
        unordered_map<string, int> map;
    public:
        // Constructor
        Scope() : prev(nullptr), nested_scope(nullptr) {}

        // Getter and Setter methods for accessing private data-members of Scope class
        void set_prev(Scope* prev_scope);
        void set_nested_scope(Scope* new_scope);
        Scope* get_prev() const;
        Scope* get_nested_scope() const;
        void assign_value(string var, int val);
        int print_val(string var, bool* found);

        // Destructor
        ~Scope() {}
};
```
1. The scopes have been handled using a doubly-linked list like structure, where each scope (a seperate Node) has its own symbol table (implemented using a ```unordered_map<string,int>``` in C++).
2. Whenever a new-scope is created (during a ```begin``` command)a new ```Scope``` node is created and is inserted as next node after the node the current scope pointer points to.
3. Whenever an ```end``` command is found it deletes the current scope node and traverses to the parent scope.
4. For assginement it simply adds the variable as a key and its value in the hashmap in the scope node.
5. For printing , the program searches if the value is present in the current scope, and if not iteratively traverses to the parent nodes to search if the value exist. 
