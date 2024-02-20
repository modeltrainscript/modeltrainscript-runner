/**
 ** This is Public Domain Software.
 **
 ** The author disclaims copyright to this source code.
 ** In place of a legal notice, here is a blessing:
 **
 **    May you do good and not evil.
 **    May you find forgiveness for yourself and forgive others.
 **    May you share freely, never taking more than you give.
 **/

#include "Ecos.hpp"
#include "EcosConst.hpp"

EcosRequest& EcosTranslator::request(uint16_t id, String option) {
    return set(ECoS_Actions::Request, id, option);
}

EcosRequest& EcosTranslator::set(uint16_t id, String option, String value) {
    return set(ECoS_Actions::Set, id, option, value);
}

EcosRequest& EcosTranslator::get(uint16_t id, String option) {
    return set(ECoS_Actions::Get, id, option);
}

EcosRequest& EcosTranslator::queryObjects(uint16_t id, String option) {
    return set(ECoS_Actions::QueryObjects, id, option);
}

EcosRequest& EcosTranslator::set(String action, uint16_t id, String option) {
    ecosRequest.action = action;
    ecosRequest.id = id;
    ecosRequest.option = option; 
    ecosRequest.value = "";
    return ecosRequest;
}

EcosRequest& EcosTranslator::set(String action, uint16_t id, String option, String value) {
    ecosRequest.action = action;
    ecosRequest.id = id;
    ecosRequest.option = option; 
    ecosRequest.value = value;
    return ecosRequest;
}

