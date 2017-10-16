local accInfo_loader = {}

data = require("accInfo")
local accInfoAccesserInst = gbLLA.gbAccInfoAccesserInst

function tbl_print(tbl)
   local k,v = next(tbl)
   while (k ~= nil) do
      print("k:" .. tostring(k) .. ";v:" .. tostring(v))
      k, v = next(tbl, k)
   end
end

function accInfo_loader:loader()
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

return accInfo_loader
