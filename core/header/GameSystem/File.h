#pragma once

#include <GameSystem/Log.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//read file function
std::string read_file(std::string path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream fileStream;
        fileStream << file.rdbuf();
        file.close();
        LOG_Log("FILE", "READ", path);
        return fileStream.str();
    }
    catch (std::ifstream::failure &e) {
        LOG_Error("FILE", "NOT_SUCCESSFULLY_READ", e.what());
    }
}
