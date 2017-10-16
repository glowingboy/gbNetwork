local gbLLA = {}

gbLLA.gbAccInfoAccesserInst = gbAccInfoAccesser.Instance
_G.gbAccInfoAccesser = nil

local dataMgr = require('gbDataMgr')

function gbLLA:Initialize()
   dataMgr:Initialize()   
end



return gbLLA
