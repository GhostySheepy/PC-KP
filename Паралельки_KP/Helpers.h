#pragma once
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <windows.h>


namespace Helpers
{
	inline std::string readfile(std::string& path_to_file) {
        std::ifstream file(path_to_file);

        if (!file.is_open()) {
            std::cout<< "FILE ' " << path_to_file <<  "' CAN`T OPEN!!" << std::endl;
        }

        std::stringstream buffer;

        buffer << file.rdbuf();

        file.close();

        return buffer.str();
	}

    inline std::wstring string_to_wstring(std::string& in) {

        /*std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(in);*/
        size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
            in.data(), (int)in.size(), NULL, 0);

        std::vector<wchar_t> buffer(charsNeeded);
        int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
            in.data(), (int)in.size(), &buffer[0], buffer.size());
   

        return std::wstring(&buffer[0], charsConverted);

    }

    inline void print_index(std::unordered_map<std::wstring, std::set<std::string>>& ind, std::string title) {
        std::cout << "Index " << title << " out:" << std::endl;
        for (auto i : ind) {
            std::wcout << std::endl << L"Key '" << i.first << L"':\t";
            for (auto j : i.second) {
                std::cout << "\t" << j << " ;  ";
            }
        }
        std::cout << std::endl;
    }
};

