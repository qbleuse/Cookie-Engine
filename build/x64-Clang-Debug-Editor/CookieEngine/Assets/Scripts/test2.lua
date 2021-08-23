
function MyInc()
	return -2
end

function Construct(myObject)
	print('Construct2')
	myObject.test = 0
end

function Update(myObject)
	myObject.test = myObject.test + MyInc()
	print('Update2 test = ' .. myObject.test)
end

function Entity:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
	self.test = 0
    return o
end

function Entity:update()
	self.test = self.test - 10
end