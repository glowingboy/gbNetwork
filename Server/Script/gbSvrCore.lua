print("loading gbSvrCore")

-- local scriptPath = "../../Server/Script/?.lua"

-- package.path = string.format("%s;%s", package.path, scriptPath)

gbAccMgr = require("gbAccMgr")

if gbAccMgr:Signup("gb", 123) then
   print("signup ok")
else
   print("signup err")
end

if gbAccMgr:Signup("gb", 123) then
   print("signup ok")
else
   print("signup err")
end


if gbAccMgr:Login("gb", 123) then
   print("login ok")
else
   print("login err")
end




