local gbAccMgr = {}

gbAccMgr.UserInfo = require("accInfo")


function gbAccMgr:Signup(usrName, pwd)
   if(self.UserInfo.get(usrName) == nil) then
      self.UserInfo.set(usrName, pwd)
      return true
   else
      return false
   end
end

function gbAccMgr:Login(usrName, pwd)
   if(self.UserInfo.get(usrName) == pwd then
      return true
   else
      return false
   end
end

return gbAccMgr
