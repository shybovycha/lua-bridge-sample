function testVector(vector)
    local v1 = -vector
    local v2 = vector * -2
    local v3 = v1 + v2
    local v4 = Vector3(v2.x, v2.y, v2.z)

    v4.z = 0

    print(string.format('Negate vector: %s = %s', vector, v1))

    print(string.format('Multiply vector: %s * -2 = %s', vector, v2))

    print(string.format('Add vectors: %s + %s = %s', v1, v2, v3))

    print(string.format('Normalize vector: norm(%s) = %s', v3, v3:normalize()))

    print(string.format('Change vector members: %s (z = 0) = %s', v2, v4))

    print(string.format('Change vector members and normalize: norm(%s) (z = 0) = %s', v2, v4:normalize()))
end
