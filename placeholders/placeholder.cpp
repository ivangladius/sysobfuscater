/*  
    Author: Maximilian Ivan Filipov

    The problem is, reading parameters of systemcalls, replacing them with
    placeholders, e.g syscall(a, b, c, d, ..);, encrypting the data, when
    decrypting knowing where to put the saved parameters is what I will explore
    here. The parameters read from the decrypted file, should be read while the
    program is running, and replacing the placeholders. 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "syscalls.h"
#include <uuid/uuid.h>
#include <iomanip>
#include <filesystem>

#define MAX_LINE_LENGTH 1000

// Update the contains_syscall function
bool contains_syscall(const std::string& line, std::string& syscall_name, SyscallInfo* info) {
    for (const auto& [name, syscall_info] : syscall_map) {
        size_t pos = line.find(name + "(");
        if (pos != std::string::npos) {
            syscall_name = name;
            *info = syscall_info;
            return true;
        }
    }
    return false;
}

// Function to generate a UUID-based variable name
std::string generate_uuid_name() {
    uuid_t uuid;
    uuid_generate(uuid);
    char uuid_str[37];
    uuid_unparse_lower(uuid, uuid_str);
    return "var_" + std::string(uuid_str);
}

// Function to generate a unique variable name based on file name
std::string generate_unique_var_name(const std::string& file_name, int& counter) {
    static std::hash<std::string> hasher;
    size_t file_hash = hasher(file_name);
    return "var_" + std::to_string(file_hash).substr(0, 4) + "_" + std::to_string(++counter);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string input_file_path = argv[1];
    std::ifstream input_file(input_file_path);
    if (!input_file) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    std::string file_name = std::filesystem::path(input_file_path).filename().string();
    int var_counter = 0;

    std::string buffer;
    std::string line;
    std::string syscall_name;
    SyscallInfo info;

    while (std::getline(input_file, line)) {
        if (contains_syscall(line, syscall_name, &info)) {
            std::string indent = line.substr(0, line.find_first_not_of(" \t"));
            std::vector<std::string> var_names;

            // Generate variable declarations
            for (size_t i = 0; i < info.param_types.size(); ++i) {
                std::string var_name = generate_unique_var_name(file_name, var_counter);
                var_names.push_back(var_name);
                buffer += indent + info.param_types[i] + " " + var_name + ";\n";
            }

            // Generate the syscall line
            std::string syscall_line = indent + syscall_name + "(";
            for (size_t i = 0; i < var_names.size(); ++i) {
                syscall_line += var_names[i];
                if (i < var_names.size() - 1) {
                    syscall_line += ", ";
                }
            }
            syscall_line += ")";
            if (line.back() == ';') {
                syscall_line += ";";
            }

            buffer += syscall_line + " [System Call: " + syscall_name + " " + std::to_string(info.number) 
                    + ", Parameters: " + std::to_string(info.param_types.size()) + "]\n";
        } else {
            buffer += line + "\n";
        }
    }

    // Print the buffer contents
    std::cout << buffer;
    return 0;
}