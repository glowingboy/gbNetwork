local gbAccMgr = {}

local accInfoAccesserInst = gbLLA.gbAccInfoAccesser.Instance

function gbAccMgr.Signup(name, password)
   return accInfoAccesserInst:NewAccInfo({szName = name, szPassword = password})
end

return gbAccMgr
