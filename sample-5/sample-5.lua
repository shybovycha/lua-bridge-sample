function applyGravity(entities)
    for i, entity in ipairs(entities) do
        entity.position = entity.position + Vector3(0, -9.8, 0)
    end
end

function findCollisions(entity, radius)
    local results = {}
    local count = 0

    for i, candidate in ipairs(Entity:all()) do
        if not (candidate == entity) then
            local distance = (candidate.position - entity.position).length

            if distance <= radius then
                results[count + 1] = candidate
                count = count + 1
            end
        end
    end

    return results
end
