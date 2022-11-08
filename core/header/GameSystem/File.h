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
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if(std::filesystem::exists(FilePath))
    {
        bool success=true;
        try
        {
            file.open(path);
            fileStream << file.rdbuf();
            file.close();
        }
        catch(std::ifstream::failure e)
        {
            LOG_Error("FILE","NOT_SUCCESSFULLY_READ:    "+path+"    ", e.what());
            success= false;
        }
        if(success)
        {
            LOG_Log("FILE", "READ", path);
        }
        return fileStream.str();
    }
    else
    {
        LOG_Error("FILE", "NOT_SUCCESSFULLY_READ", "No such file or directory:  "+path);
    }
}