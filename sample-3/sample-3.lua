function vec2str(vec)
    return "luaVector(" .. vec.x .. ", "  .. vec.y .. ", " .. vec.z .. ")"
end

function testVector(vector)
    print("Vector length: " .. vector.length)
    print("Normalized vector: " .. vec2str(vector:normalize()))
end
