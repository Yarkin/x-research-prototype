#include "stdio.h"
#include "luax.h"
#include "xengine.hpp"
#include "xtoplib.hpp"
#include "list.hpp"

const char xtoplib::szClassName[] = "xtoplib";
const xengine<xtoplib>::RegType xtoplib::RegisterInfo[] = {
    {"transfer",  &xtoplib::transfer},
    {"receive", &xtoplib::receive},
    {"sign", &xtoplib::sign},
    {"random", &xtoplib::random},
    {0}
};

extern "C" int fadd(lua_State *L)
{
    int n = lua_gettop(L);
    printf("fadd len:%d\n",n);
    double op1 = 0.0f;
    for(int i=0;i<n;i++){
      double  op2 = lua_tonumber(L, lua_gettop(L));
      op1 += op2;
      lua_pop(L, 1);
    }
    printf("fadd res:%f\n",op1);
    lua_pushnumber(L, op1);
    return 1;
}

extern "C" int getBalance(lua_State *L){
    int n = lua_gettop(L);
    printf("getBalance len:%d\n",n);
    const char* account = lua_tostring(L,1);
    printf("balance accout :%s\n",account);
    lua_pushnumber(L, 1000000000L);
    return 1;
}

extern "C" int transfer(lua_State *L){
    int n = lua_gettop(L);
    printf("transfer len:%d\n",n);
    lua_assert(n > 0);
    const char* from = lua_tostring(L,1);
    const char* to = lua_tostring(L,2);
    double  balance = lua_tonumber(L, 0);
    printf("tranfer %s to %s balance: %f\n",from,to,balance);
    lua_pushnumber(L, 1);
    return 1;
}

const char* p_script ="local api = xtoplib()\n\
if type(api) == \"table\" then\n\
    local ret = api.transfer(\"TA1\",\"TA2\",3)\n\
    print(tostring(ret))\n\
    ret = api.receive(\"xxx\")\n\
    print(tostring(ret))\n\
    ret = api.sign(\"xxx\")\n\
    print(tostring(ret))\n\
    ret = api.random(\"xxx\")\n\
    print(tostring(ret))\n\
end\n\
api = nil\n\
collectgarbage(\"collect\")";

int main(int argc, char **argv)
{
    lua_State *m_pState = luaL_newstate();
    luaopen_base(m_pState);
    luaopen_table(m_pState);
    luaopen_io(m_pState);
    luaopen_string(m_pState);
    luaopen_math(m_pState);
    luaopen_debug(m_pState);
    luaL_openlibs(m_pState);
    lua_register(m_pState, "fadd", fadd);
    lua_register(m_pState, "getBalance", getBalance);
    lua_register(m_pState, "transfer", transfer);
    int bRet = luaL_loadfile(m_pState, "../scripts/xx.lua");
    bRet = lua_pcall(m_pState, 0, 0, 0);

    lua_getglobal(m_pState, "add");
    lua_pushnumber(m_pState, 5);
    lua_pushnumber(m_pState, 1);
    bRet = lua_pcall(m_pState, 2, 1, 0);
    double dVal = lua_tonumber(m_pState, -1);


    Main main;
    main.run();

    Main main2;
    main2.run();

    lua_getglobal(m_pState, "exec");
    bRet = lua_pcall(m_pState, 0, 1, 0);

    xengine<xtoplib>::RegisterClass(m_pState);
    luaL_dofile(m_pState,"../scripts/f.lua");
    printf("script:%s\n",p_script);
    
    lua_close(m_pState);
    return 1;
}