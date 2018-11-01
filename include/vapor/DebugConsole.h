#pragma once

#include <string>

void InitDebugConsole();

void SetFloat(std::string name, float f);
float GetFloat(std::string name, float default_ = 0);
void SetBool(std::string name, bool b);
float GetBool(std::string name, bool default_ = false);
void SetString(std::string name, std::string s);
std::string GetString(std::string name, std::string default_ = "NULL");

template <typename T> void ShowMatrix(std::string name, T m[16]);
