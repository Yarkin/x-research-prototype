function add(a,b)  
    return a + b + fadd(a,b)  
end

printf= function(s, ...)
    return io.write(s:format(...))
end

function exec()
    sender = "Txxxx1"
    receiver = "Txxxx2"
    balance = 1
    printf("from %s to %s balance:%f\n",sender,receiver,balance)
    userBalance = getBalance(sender)
    printf("user %s balance:%f\n",sender,userBalance)
    balance = transfer(sender,receiver,balance)
    return 1
end
