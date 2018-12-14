local api = xtoplib()
if type(api) == "table" then
    local ret = api.transfer("TA1","TA2",3)
    print(tostring(ret))
    ret = api.receive("xxx")
    print(tostring(ret))
    ret = api.sign("xxx")
    print(tostring(ret))
    ret = api.random("xxx")
    print(tostring(ret))
end
api = nil
collectgarbage("collect")