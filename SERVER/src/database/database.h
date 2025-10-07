#pragma once
#include <string>
using namespace std;

// функции для работы с user.json
void CUDB(const string& login, const string& password, const string& role, const string& level);
void CDDB(const string& login, const string& password="", const string& role="", const string& level="");
void DUDB(const string& login);
int SUDB(const string& login, const string& password);
string SLDB(const string& login);
