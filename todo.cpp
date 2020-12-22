//header files declaration

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>
#include <iostream> 
#include <time.h>
using namespace std;

#define SLASH "/"

// Shows Usage 

void help(){
    cout<<"Usage :-"<<endl;
    cout<<"$ ./todo add \"todo item\"  # Add a new todo"<<endl;
    cout<<"$ ./todo ls               # Show remaining todos"<<endl;
    cout<<"$ ./todo del NUMBER       # Delete a todo"<<endl;
    cout<<"$ ./todo done NUMBER      # Complete a todo"<<endl;
    cout<<"$ ./todo help             # Show usage"<<endl;
    cout<<"$ ./todo report           # Statistics";
}


// converts 'argv' arguments to strings using queue

queue<string>* conv_args(char** argv, int argc) {
    queue<string>* res = new queue<string>();
    for(int i = 1; i < argc; i++) {
        res->push(string(argv[i]));
    }
    return res;    
}

// get and reads todo.txt file 

vector<string> get_todo(){

    char path[4096];
    sprintf(path, ".%s.todo", SLASH);

    FILE* fp = fopen(path, "r");
    vector<string> buffer;
    if(fp) {
        do {
            char line[4096];
            char* res = fgets(line, 4096, fp); 
            if(res) {
                buffer.push_back(string(res));
            } else {
                break;
            }
        } while(true);
        fclose(fp);
    }  
    return buffer;
}


// get and reads done.txt file 

vector<string> get_done(){

    char path[4096];
    sprintf(path, ".%s.done", SLASH);

    FILE* fp = fopen(path, "r");
    vector<string> buffer;
    if(fp) {
        do {
            char line[4096];
            char* res = fgets(line, 4096, fp);
            if(res) {
                buffer.push_back(string(res));
            } else {
                break;
            }
        } while(true);
        fclose(fp);
    }  
    return buffer;
}


// gets current date

string get_time(int format){
    time_t curr_time = time(NULL);
    struct tm *tmp;
    char pre_t[80];
    tmp = localtime(&curr_time);
    if(format == 1)
        strftime(pre_t,80,"%d-%m-%Y",tmp);
    else
        strftime(pre_t,80,"%Y-%m-%d",tmp);
    
    return pre_t;
}


// displays list of todos 

void list_todo(){

    auto buffer = get_todo();

    if(buffer.size()==0){
        cout<< "There are no pending todos!";
    }

    for(int i = buffer.size()-1; i>= 0; i--) {
        string todo = buffer[i];
        replace(todo.begin(), todo.end(), '\n', ' ');
        replace(todo.begin(), todo.end(), '\r', ' ');
        cout << "[" << i+1 << "] ";
        if(i ==0 ){
            cout << todo.c_str();
        }else
            cout << todo.c_str() << endl; 
    }
    
}


// converts strings to a total message

string msg(queue<string>* commands) {
    string s;
    while(!commands->empty()) {
        s += commands->front();
        commands->pop();
        if(!commands->empty()) { s += " "; }
    }

    return s;
}

// adds todo's 

void add_todo(string msg){

    char path[4096];
    sprintf(path, ".%s.todo", SLASH);

    if(msg.empty()){
        cout<< "Error: Missing todo string. Nothing added!";
    }
    else {
        FILE* fp = fopen(path, "a");
        if(fp) {
            fseek(fp, 0, SEEK_END);
            fprintf(fp, "%s\n", msg.c_str());
            cout << "Added todo: \"" << msg << "\" ";
            fclose(fp);
        } else {
            cout << "Error adding todo.";
        }
    }
}

// rewrites the todo.txt file after each todo item deleted

void rewrite_todo(vector<string> buffer){
    char path[4096];
    sprintf(path, ".%s.todo", SLASH);
    
    FILE* fp = fopen(path, "w");
    if(fp) {
        for(int i = 0; i < buffer.size(); i++) {
            if(buffer[i].length() == 0) { continue; }
            fprintf(fp, "%s", buffer[i].c_str());
        }        
        fclose(fp);        
    } else {
        cout << "Error updating todos.";
    } 
}


// shifting marked todo item to done.txt file

void mark_todo(vector<string> buffer , unsigned int index){
    char path[4096];
    sprintf(path, ".%s.done", SLASH);
    string curr_date = get_time(2);

    FILE* fp = fopen(path, "a");
    if(fp) {
        fseek(fp, 0, SEEK_END);
        fprintf(fp, "x %s %s",curr_date.c_str() ,buffer[index].c_str());
        cout << "Marked todo #" << index+1 << " as done.";
        fclose(fp);
    } else {
        cout << "Error adding todo.";
    }
}


// deletes an todo item

void delete_todo(unsigned int index) {
    auto todo = get_todo();

    if(todo.size() == 0 || index > todo.size() - 1) {
        cout << "No item with indexError: todo #"<< index+1 <<" does not exist. Nothing deleted.";
    } else {
        todo[index] = "";
        cout << "Deleted todo #" << index+1;
    }

    rewrite_todo(todo); 
}


// marking todo item as done and re-directing to mark_todo() function

void done_todo(unsigned int index){
    
    auto todo = get_todo();

    if(todo.size() == 0 || index > todo.size() - 1) {
        cout << "Error: todo #" << index+1 << " does not exist.";
    } else {
        mark_todo(todo , index);
        todo[index] = "";
    }

    rewrite_todo(todo);
}

// shows Statistics 

void report(){
    vector<string> buffer = get_todo();
    int no_todo = buffer.size();
    buffer = get_done();
    int no_done = buffer.size();
    string curr_date = get_time(1);
    cout<<curr_date<<" Pending: "<<no_todo<<" Completed: "<< no_done;
}


// processing all commands which are arguments 

void process_commands(queue<string>* commands){

    if(commands->empty()){
        cout<< "";
    }

    string command = commands->front();
    transform(command.begin(),command.end(),command.begin(), ::tolower);
    commands->pop();

    if(command.compare("help") == 0){
        help();
    } else 
    if(command.compare("add") == 0){
       add_todo(msg(commands));
    } else
    if(command.compare("del")==0){
        if(commands->empty()){
            cout<<"Error: Missing NUMBER for deleting todo.";
        }else
        {  
            unsigned int idx = atoi(commands->front().c_str());
            commands->pop();
            delete_todo(idx-1);
        }
    }else
    if(command.compare("done")==0){
        if(commands->empty()){
            cout<<"Error: Missing NUMBER for marking todo as done.";
        }else
        {  
            unsigned int id = atoi(commands->front().c_str());
            commands->pop();
            done_todo(id-1);
        }
    }else
    if(command.compare("ls") == 0){
        list_todo();
    }else
    if(command.compare("report") == 0){
        report();
    }else
    {
        cout<<"Invalid command.";
    }
}


// main function of program

int main(int argc, char** argv){

    // conversion from args to strings 
    queue<string>* commands = conv_args(argv,argc);  

    // prints help when no arguments are given 
    if(commands->empty()){
        help();
    }else{

    // process arguments when arguments are given 
    process_commands(commands);
    }
    delete commands;
    return 0;
}
