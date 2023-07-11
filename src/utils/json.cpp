#include "json.hpp"
int getInt(const Json::Value &v) {
    if (v.isInt()) {
        return v.asInt();
    } else {
        return atoi(v.asCString());
    }
}