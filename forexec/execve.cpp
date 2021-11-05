#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <cstring>

using namespace std;

int main(int argc, char *argv[], char *envp[]) {
    if(argc == 1) {
        cout << "examples of program calls:\n";
        cout << "using parent environment: this_program_name target_program_name arg1 arg2 arg3\n";
        cout << "using custom environment: this_program_name -env environment_vars_file target_program_name arg1 arg2 arg3\n";
        return 0;
    }
    if(argc < 2 || string(argv[1]) == "-env" && argc < 4) {
        cout << "invalid arguments\n";
        return 1;
    }
    int prog_name_idx = 1;
    char** new_envp = envp;
    if(string(argv[1]) == "-env")  {
        prog_name_idx = 3;
        ifstream in(argv[2]);
        if(!in) {
            cout << "can't open file: " << argv[2] << "\n";
            return 1;
        }
        vector<string> lines;
        string line;
        while(getline(in, line)) {
            if(line.find('\n') == line.length() - 1) {
                line.pop_back();
            } else if(line.find('\r') == line.length() - 1) {
                line.pop_back();
            }
            lines.push_back(line);
        }
        new_envp = new char*[lines.size()+1];
        for(int i = 0; i < lines.size(); ++i) {
            char* env = new char[lines[i].length()+1];
            strcpy(env, lines[i].c_str());
            new_envp[i] = env;
        }
        new_envp[lines.size()] = nullptr;
    }
    int args_size = argc - prog_name_idx;
    char** new_args = new char*[args_size+1];
    new_args[0] = argv[prog_name_idx];
    for(int i = 1; i < args_size; ++i) {
        new_args[i] = argv[prog_name_idx+i];
    }
    new_args[args_size] = nullptr;
    execve(new_args[0], new_args, new_envp);


}
