def rfibr(x):
    if x>1:
        return rfibr(x-1)+rfibr(x-2)
    else:
        return x
var = "hello world!"
for x in range(100000):
    print(var)