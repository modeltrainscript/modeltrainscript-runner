#include "EcosData.hpp"


String EcosRequest::message() {
    if (value.length() > 0){
        return action + String("(") + id + String(",") + option + String("[") + value + String("])");    
    }
    return action + String("(") + id + String(",") + option + String(")");
}