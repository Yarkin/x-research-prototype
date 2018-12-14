#pragma once
#include "luax.h"

template <typename T>
class xengine{
public:
    struct RegType{
        const char* szFunName;
        int (T::*mFun)(lua_State*);
    };

    static void RegisterClass(lua_State* L)
    {
        lua_register(L, T::szClassName, &xengine::constructor);
    }

private:
    static int thunk(lua_State* L){
        int objUpValueIndex = lua_upvalueindex(1);
        int funIndexUpValueIndex = lua_upvalueindex(2);

        T* obj = (T*)lua_topointer(L, objUpValueIndex);
        int funIndex = (int)lua_tonumber(L, funIndexUpValueIndex);

        return (obj->*(T::RegisterInfo[funIndex].mFun))(L);
    }

    static int constructor(lua_State* L){
        T* obj = new T();
        lua_newtable(L);
        for(int i=0; T::RegisterInfo[i].szFunName; ++i){
            lua_pushlightuserdata(L, obj);
            lua_pushnumber(L, i);
            lua_pushcclosure(L, thunk, 2);
            lua_setfield(L, -2, T::RegisterInfo[i].szFunName);
        }
        return 1;
    }
};