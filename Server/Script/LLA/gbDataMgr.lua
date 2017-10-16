local gbDataMgr = {}

function gbDataMgr:Initialize()
   local accinfo_loader = require('accInfo_loader')
   accinfo_loader:loader()
end

return gbDataMgr



