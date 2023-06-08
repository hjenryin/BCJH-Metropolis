#include "json.hpp"
int getInt(Json::Value &v) {
    try {
        return v.asInt();
    } catch (Json::LogicError &e) {
        return atoi(v.asCString());
    }
}