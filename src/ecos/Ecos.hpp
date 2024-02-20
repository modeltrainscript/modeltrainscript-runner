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

#ifndef __ECOS_H__
#define __ECOS_H__

#include <Arduino.h>

#include "EcosData.hpp"

class EcosTranslator {
public:
    EcosRequest& request(uint16_t id, String option);
    EcosRequest& get(uint16_t id, String option);
    EcosRequest& set(uint16_t id, String option, String value);
    EcosRequest& queryObjects(uint16_t id, String option);

private:
    EcosRequest& set(String action, uint16_t id, String option);
    EcosRequest& set(String action, uint16_t id, String option, String value);
    
public:
    EcosRequest ecosRequest;
};

#endif 