--[[
********************************
*                              *
*      The Moon Project        *
*                              *
********************************

This software is provided as free and open source by the
staff of The Moon Project, in accordance with 
the GPL license. This means we provide the software we have 
created freely and it has been thoroughly tested to work for 
the developers, but NO GUARANTEE is made it will work for you 
as well. Please give credit where credit is due, if modifying,
redistributing and/or using this software. Thank you.

Staff of Moon Project, Feb 2008
~~End of License Agreement
--Moon April 2008]]
local Done_85 = nil
local Done_70 = nil
local Done_55 = nil
local Done_25 = nil
function Vexallus_Combat(Unit, Event)
--Unit:RegisterEvent("Vexallus_Chain", 5400, 0)
	Unit:RegisterEvent("Vexallus_SpawnPhases", 1000, 1)
	Unit:RegisterEvent("Vexallus_ArcaneShock", 4000, 0)
	Unit:RegisterEvent("Vexallus_Overload", math.random(2000, 3000), 0)
end

--[[
Not working hits himself.

function Vexallus_Chain(Unit, Event)
local Flip = math.random(1,3)
if Flip == 1 then
Unit:CastSpellOnTarget(44318, Unit:GetRandomPlayer(7))
else
end
end--]]

function Vexallus_ArcaneShock(Unit, Event)
	local Flip = math.random(1,2)
	if Flip == 1 then
		Unit:FullCastSpellOnTarget(46381, Unit:GetRandomPlayer(0))
	end
end

function Vexallus_Overload(Unit, Event)
	if Unit:GetHealthPct() <= 10 then
		Unit:CastSpell(44353)
	end
end


--wowhead: 85%, 70%, 55%, 40%, and 25% Spawns.
function Vexallus_SpawnPhases(Unit, Event)
	Unit:RegisterEvent("Vexallus_85", 1000, 0)
	Unit:RegisterEvent("Vexallus_70", 1000, 0)
	Unit:RegisterEvent("Vexallus_55", 1000, 0)
	Unit:RegisterEvent("Vexallus_40", 1000, 0)
	Unit:RegisterEvent("Vexallus_25", 1000, 0)
end 


--85
function Vexallus_85(Unit, Event)
	if Unit:GetHealthPct() <= 85 and Done_85 ~= 1 then
		Unit:SpawnCreature(24745, Unit:GetX(), Unit:GetY(), Unit:GetZ(), Unit:GetO(), 16, 0)
		Done_85 = 1
	end
end
--end

--70
function Vexallus_70(Unit, Event)
	if Unit:GetHealthPct() <= 70 and  Done_70 ~= 1 then
		Unit:SpawnCreature(24745, Unit:GetX(), Unit:GetY(), Unit:GetZ(), Unit:GetO(), 16, 0)Done_70 = 1
	end
end
--end

--55
function Vexallus_55(Unit, Event)
	if Unit:GetHealthPct() <= 55 and Done_55 ~= 1 then
		Unit:SpawnCreature(24745, Unit:GetX(), Unit:GetY(), Unit:GetZ(), Unit:GetO(), 16, 0)
		Done_55 = 1
	end
end
--end
--40
function Vexallus_40(Unit, Event)
	if Unit:GetHealthPct() <= 40 and Done_40 ~= 1 then
	Unit:SpawnCreature(24745, Unit:GetX(), Unit:GetY(), Unit:GetZ(), Unit:GetO(), 16, 0)
	Done_40 = 1
	end
end
--end
--25
function Vexallus_25(Unit, Event)
	if Unit:GetHealthPct() <= 25 and Done_25 ~= 1 then
	Unit:SpawnCreature(24745, Unit:GetX(), Unit:GetY(), Unit:GetZ(), Unit:GetO(), 16, 0)
	Done_25 = 1
	end
end
function Vexallus_LeaveCombat(Unit, Event)
	Unit:RemoveEvents()
end
function Vexallus_Died(Unit, Event)
	Unit:RemoveEvents()
end


RegisterUnitEvent(24744, 1, "Vexallus_Combat")
RegisterUnitEvent(24744, 2, "Vexallus_LeaveCombat")
RegisterUnitEvent(24744, 4, "Vexallus_Died")