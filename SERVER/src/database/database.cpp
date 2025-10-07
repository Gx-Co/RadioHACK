#include "database.h"
#include "json.hpp"
#include <bits/stdc++.h>
#include <fstream>
using json = nlohmann::json;
using namespace std;

// ------------------ Вспомогательные функции ------------------
json loadJSON(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) return json::array();
    json j;
    f >> j;
    return j;
}

void saveJSON(const string& filename, const json& j) {
    ofstream f(filename);
    f << setw(2) << j << endl;
}

// ------------------ 5 функций ------------------
void CUDB(const string& login, const string& password, const string& role, const string& level) {
    json users = loadJSON("user.json");
    users.push_back({{"login", login}, {"password", password}, {"role", role}, {"level", level}});
    saveJSON("user.json", users);
}

void CDDB(const string& login, const string& password, const string& role, const string& level) {
    json users = loadJSON("user.json");
    for (auto& user : users) {
        if (user["login"] == login) {
            if (!password.empty()) user["password"] = password;
            if (!role.empty()) user["role"] = role;
            if (!level.empty()) user["level"] = level;
            break;
        }
    }
    saveJSON("user.json", users);
}

void DUDB(const string& login) {
    json users = loadJSON("user.json");
    users.erase(remove_if(users.begin(), users.end(), [&](const json& u){ return u["login"] == login; }), users.end());
    saveJSON("user.json", users);
}

int SUDB(const string& login, const string& password) {
    json users = loadJSON("user.json");
    for (auto& user : users) {
        if (user["login"] == login && user["password"] == password)
            return 1;
    }
    return 0;
}

string SLDB(const string& login) {
    json users = loadJSON("user.json");
    for (auto& user : users) {
        if (user["login"] == login) return user["level"];
    }
    return "";
}
