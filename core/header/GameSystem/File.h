#pragma once

#include <GameSystem/Log.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

//read file function
std::string read_file(std::string path) {
    std::filesystem::path FilePath(path);
    std::ifstream file;
    std::stringstream fileStream;
    if (std::filesystem::exists(FilePath)) {
        file.open(path, std::ios::in);
        fileStream << file.rdbuf();
        file.close();
        LOG_Log("FILE", "READ", path);
        return fileStream.str();
    } else {
        LOG_Error("FILE", "NOT_SUCCESSFULLY_READ", "No such file or directory:  " + path);
        return "";
    }
}