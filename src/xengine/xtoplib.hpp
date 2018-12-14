#pragma once
#include <stdio.h>
#include <iostream> 
#include "xengine.hpp"
#include "time.h"

using namespace std; 
class xtoplib{
    public:
    static const char szClassName[];
    static const xengine<xtoplib>::RegType RegisterInfo[];

    int transfer(lua_State* L){
        int n = lua_gettop(L);
        const char* sender = lua_tostring(L,1);
        const char* receive = lua_tostring(L,2);
        int64_t balance = lua_tointeger(L,n);
        printf("transfer %s to %s %lld\n", sender,receive,balance);
        lua_pushnumber(L, 1L);
        return 1;
    }

    int receive(lua_State* L){
        lua_pushstring(L,"lua call receive");
        return 1;
    }

    int sign(lua_State* L){
        lua_pushstring(L,"lua call sign");
        return 1;
    }

    int random(lua_State* L){
        lua_pushstring(L,"lua call random");
        return 1;
    }
};
