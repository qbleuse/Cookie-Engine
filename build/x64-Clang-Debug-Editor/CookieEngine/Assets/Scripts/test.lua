
function MyInc()
	return 2
end

function Construct(myObject)
	print('Construct')
	myObject.test = 0

	local test = 1
end

function Update(myObject)
	myObject.test = myObject.test + MyInc()
	print('Update test = ' .. myObject.test)
end

function Entity:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
	self.test = 0
    return o
end

function Entity:update()
	self.test = self.test + 1
end