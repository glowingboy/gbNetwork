local accInfo = {}

accInfo.gb = "123"

accInfo.fs = "abc"

local get = function (k)
   return accInfo[k]
end

local set = function(k, v)
   --lock
   accInfo[k] = v
   --unlock
end


return {get = get, set = set}
