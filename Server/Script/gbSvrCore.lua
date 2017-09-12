print("loading gbSvrCore")

--local gbAccountMgrPath = "../AccMgr/?.lua"

--package.path = string.format("%s;%s", package.path, gbAccountMgrPath)

gbAccMgr = require(gbAccMgr)

gbAccMgr:Signup("gb", 123)


