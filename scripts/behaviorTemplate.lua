local test = {}

--Set variables

function test:start()
    --Called once at start
end

function test:update(delta)
    --Called once per frame
end

function test::onCollision()
    --called on collision
end

function test:rayIntersect()
    --called when grounded
end

function test:reset()
    --Called once at end
end

return test