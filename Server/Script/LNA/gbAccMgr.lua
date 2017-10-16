local gbAccMgr = {}

local lla = gbLLA
local accInfoAccesserInst = lla.gbAccInfoAccesserInst

function gbAccMgr.Signup(name, password)
   return accInfoAccesserInst:NewAccInfo({szName = name, szPassword = password})
end

return gbAccMgr
