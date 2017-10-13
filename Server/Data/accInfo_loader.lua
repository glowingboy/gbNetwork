local accInfo_loader = {}

data = require("accInfo")
local accInfoAccesserInst = gbLLA.gbAccInfoAccesser.Instance

function accInfo:loader()
   local k,v = next(data)
   while (k ~= nil) do
      if(type(v) == "table") then
	 accInfoAccesserInst:NewAccInfo({szName = v.name, szPassword = v.password})
      else
	 print("err")
      end
      k, v = next(data, k)
   end
end
