#include "json.hpp"
int getInt(const Json::Value &v) {
    try {
        return v.asInt();
    } catch (Json::LogicError &e) {
        return atoi(v.asCString());
    }
}