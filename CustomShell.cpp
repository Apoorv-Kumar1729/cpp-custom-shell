#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

// Function to split the input string into tokens based on whitespace
vector<string> tokenize(const string& input) {
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to execute external commands using fork, execvp, and waitpid
void execute_command(vector<string>& tokens) {
    if (tokens.empty()) return;

    // Check for output redirection '>'
    string redirect_file = "";
    size_t redirect_index = 0;
    bool has_redirection = false;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == ">") {
            if (i + 1 < tokens.size()) {
                has_redirection = true;
                redirect_file = tokens[i + 1];
                redirect_index = i;
                break;
            } else {
                cerr << "myshell: syntax error near unexpected token 'newline'\n";
                return;
            }
        }
    }

    // Prepare arguments for execvp. It expects a null-terminated array of char* (C-strings)
    vector<char*> args;
    size_t limit = has_redirection ? redirect_index : tokens.size();
    for (size_t i = 0; i < limit; ++i) {
        args.push_back(const_cast<char*>(tokens[i].c_str()));
    }
    args.push_back(nullptr); // Null-terminate the arguments array

    // OS Magic: Fork a child process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        cerr << "myshell: fork failed\n";
    } 
    else if (pid == 0) {
        // --- CHILD PROCESS EXECUTION ---
        
        if (has_redirection) {
            // Open the target file for writing (create if missing, truncate if exists)
            int fd = open(redirect_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("myshell: open failed");
                exit(EXIT_FAILURE);
            }
            // Overwrite standard output descriptor (1) with our file descriptor
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("myshell: dup2 failed");
                exit(EXIT_FAILURE);
            }
            close(fd); // Close the duplicate descriptor as it's no longer needed
        }

        // Replace the child process image with the new program
        if (execvp(args[0], args.data()) < 0) {
            perror(("myshell: " + tokens[0]).c_str());
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // --- PARENT PROCESS EXECUTION ---
        // Wait for the specific child process to terminate to avoid creating zombies
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    string input;

    while (true) {
        // Print custom prompt (you can customize this to show current path later)
        cout << "myshell> " << flush;
        
        if (!getline(cin, input)) {
            // Handle Ctrl+D (EOF) gracefully
            cout << "\n";
            break;
        }

        vector<string> tokens = tokenize(input);
        if (tokens.empty()) continue;

        // Handle Built-in Commands (Must run in parent process context)
        if (tokens[0] == "exit") {
            break;
        } 
        else if (tokens[0] == "cd") {
            if (tokens.size() < 2) {
                cerr << "myshell: cd: missing argument\n";
            } else {
                if (chdir(tokens[1].c_str()) != 0) {
                    perror("myshell: cd failed");
                }
            }
            continue;
        }

        // Execute external commands
        execute_command(tokens);
    }

    return 0;
}