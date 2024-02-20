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

#ifndef __ECOS_CONST_H__
#define __ECOS_CONST_H__

class ECoS_Actions {
public:
    static constexpr const char* Set = "set";
    static constexpr const char* Get = "get";
    static constexpr const char* Request = "request";
    static constexpr const char* QueryObjects = "queryObjects";
};

class ECoS_Ids {
public:
    static const uint8_t ECoS               = 1;
    static const uint8_t LokManager         = 10;
    static const uint8_t AccessoryManager   = 11;
    static const uint8_t FeedbackManager    = 26;
};

class ECoS_Options {
public:
    static constexpr const char* State = "state";
    static constexpr const char* View = "view";
    static constexpr const char* Address = "addr";
    static constexpr const char* Ports = "ports";
};

class ECoS_Values {
public:
    static constexpr const char* On = "1";
    static constexpr const char* Off = "0";
};

#endif