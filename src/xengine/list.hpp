#pragma once
#include "luax.h"
#include <iostream>
#include <list>
#include <assert.h>

extern "C"
{
    static int l_list_push(lua_State *L)
    {                                                                                          // Push elements from LUA
        assert(lua_gettop(L) == 2);                                                            // check that the number of args is exactly 2
        std::list<int> **ud = static_cast<std::list<int> **>(luaL_checkudata(L, 1, "ListMT")); // first arg is the list
        int v = luaL_checknumber(L, 2);                                                        // seconds argument is the integer to be pushed to the std::list<int>
        (*ud)->push_back(v);                                                                   // perform the push on C++ object through the pointer stored in user data
        return 0;                                                                              // we return 0 values in the lua stack
    }
    static int l_list_pop(lua_State *L)
    {
        assert(lua_gettop(L) == 1);                                                            // check that the number of args is exactly 1
        std::list<int> **ud = static_cast<std::list<int> **>(luaL_checkudata(L, 1, "ListMT")); // first arg is the userdata
        if ((*ud)->empty())
        {
            lua_pushnil(L);
            return 1; // if list is empty the function will return nil
        }
        lua_pushnumber(L, (*ud)->front()); // push the value to pop in the lua stack
                                           // it will be the return value of the function in lua
        (*ud)->pop_front();                // remove the value from the list
        return 1;                          //we return 1 value in the stack
    }

    struct test_container
    {
    public:
        test_container() {}

    public:
        int add(int val)
        {
            std::cout << "container the list object in lua" << std::endl;
            vals.push_back(val);
            return 1;
        }

        int printAll(){
            for(auto iter = vals.begin();iter != vals.end();iter++){
                std::cout << "printAll the list:"<< *iter << std::endl;
            }
            return 1;
        }

    private:
        std::list<int> vals;
    };

    static int l_container_add(lua_State *L)
    {                                                                                          // Push elements from LUA
        assert(lua_gettop(L) == 2);   
        std::cout << "l_container_add the list object in lua" << std::endl;                                                         // check that the number of args is exactly 2
        test_container **ud = static_cast<test_container **>(luaL_checkudata(L, 1, "Container")); // first arg is the list
        int v = luaL_checknumber(L, 2);                                                        // seconds argument is the integer to be pushed to the std::list<int>
        (*ud)->add(v);                                                                   // perform the push on C++ object through the pointer stored in user data
        return 0;                                                                              // we return 0 values in the lua stack
    }
}



class Main
{
  public:
    Main();
    ~Main();
    void run();

    /* data */
  private:
    lua_State *L;
    std::list<int> theList;
    test_container theContainer;
    void registerListType();
    void runScript();
    void registerContainer();
};

Main::Main()
{
    L = luaL_newstate();
    luaL_openlibs(L);
}

Main::~Main()
{
    lua_close(L);
}

void Main::runScript()
{
    lua_settop(L, 0); //empty the lua stack
    if (luaL_dofile(L, "../scripts/samplescript.lua"))
    {
        fprintf(stderr, "error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        exit(1);
    }
    //   assert(lua_gettop(L) = 0); //empty the lua stack
}

void Main::registerListType()
{
    std::cout << "Set the list object in lua" << std::endl;
    luaL_newmetatable(L, "ListMT");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // ListMT .__index = ListMT
    lua_pushcfunction(L, l_list_push);
    lua_setfield(L, -2, "push"); // push in lua will call l_list_push in C++
    lua_pushcfunction(L, l_list_pop);
    lua_setfield(L, -2, "pop"); // pop in lua will call l_list_pop in C++
}

void Main::registerContainer(){
    std::cout << "Set the container object in lua" << std::endl;
    luaL_newmetatable(L, "Container");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // ListMT .__index = ListMT
    lua_pushcfunction(L, l_container_add);
    lua_setfield(L, -2, "add"); // push in lua will call l_list_push in C++
}

void Main::run()
{
    for (unsigned int i = 0; i < 10; i++) // add some input data to the list
        theList.push_back(i * 100);
    registerListType();
    std::cout << "creating an instance of std::list in lua" << std::endl;
    std::list<int> **ud = static_cast<std::list<int> **>(lua_newuserdata(L, sizeof(std::list<int> *)));
    *(ud) = &theList;
    luaL_setmetatable(L, "ListMT"); // set userdata metatable
    lua_setglobal(L, "the_list");   // the_list in lua points to the new userdata

    registerContainer();
    test_container **ppcontainer = static_cast<test_container **>(lua_newuserdata(L, sizeof(test_container *)));
    *(ppcontainer) = &theContainer;
    luaL_setmetatable(L, "Container"); // set userdata metatable
    lua_setglobal(L, "the_container");   // the_list in lua points to the new userdata

    runScript();

    theContainer.printAll();

    while (!theList.empty())
    { // read the data that lua left in the list
        std::cout << "from C++: pop value " << theList.front() << std::endl;
        theList.pop_front();
    }
}