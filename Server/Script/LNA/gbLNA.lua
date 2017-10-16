--[[
   lua network api (lua RPC API)
]]--
local gbLNA = {}

gbLLA = require("gbLLA")
gbLLA:Initialize()

gbLNA.AccMgr = require("gbAccMgr")

_G.gbLLA = nil

--gbLNA.Game = require("Game")
return gbLNA
