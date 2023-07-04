#include "json.hpp"
int getInt(Json::Value &v) {
    if (v.isInt()) {
        return v.asInt();
    } else {
        return atoi(v.asCString());
    }
}