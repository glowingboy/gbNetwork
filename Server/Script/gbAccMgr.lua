local gbAccMgr = {}

gbAccMgr.UserInfo = {}

function gbAccMgr:Signup(usrName, pwd)
   if(self.UserInfo[usrName] == nil) then
      self.UserInfo[usrName] = pwd
      return true
   else
      return false
   end
end

function gbAccMgr:Login(usrName, pwd)
   if(self.UserInfo[usrName] == pwd) then
      return true
   else
      return false
   end
end

return gbAccMgr
