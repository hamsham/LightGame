
#include <atomic>
#include <utility> // std::move

#include "lightsky/utils/Assertions.h"

#include "lightsky/game/ECSDatabase.hpp"

namespace ls
{
namespace game
{



ECSDatabase::~ECSDatabase() noexcept
{
}



ECSDatabase::ECSDatabase() noexcept :
    mComponents{},
    mEntities{}
{}



ECSDatabase::ECSDatabase(ECSDatabase&& db) noexcept :
    mComponents{std::move(db.mComponents)},
    mEntities{std::move(db.mEntities)}
{}



ECSDatabase& ECSDatabase::operator=(ECSDatabase&& db) noexcept
{
    if (this != &db)
    {
        mComponents = std::move(db.mComponents);
        mEntities = std::move(db.mEntities);
    }

    return *this;
}



/*-------------------------------------
 * Spawn an entity with a unique ID
-------------------------------------*/
Entity ECSDatabase::create_entity() noexcept
{
    if (mEntities.size() == (size_t)std::numeric_limits<size_t>::max())
    {
        return Entity{(EntityIdType)INVALID_ENTITY};
    }

    Entity newEntity{mEntities.size()};
    std::unordered_set<Entity>::iterator iter;

    // first check we didn't run out of memory
    LS_ASSERT(newEntity.id != std::numeric_limits<decltype(newEntity.id)>::max());

    while (true)
    {
        iter = mEntities.find(newEntity);
        if (iter == mEntities.end())
        {
            break;
        }

        newEntity.id++;
    }

    mEntities.insert(newEntity);
    return newEntity;
}



/*-------------------------------------
 * Spawn an entity with a unique ID
-------------------------------------*/
void ECSDatabase::destroy_entity(Entity& e) noexcept
{
    std::unordered_set<Entity>::iterator&& iter = mEntities.find(e);
    if (iter != mEntities.end())
    {
        mEntities.erase(iter);
    }

    e.id = std::numeric_limits<decltype(e.id)>::max();
}


/*-------------------------------------
 * Get the number of components for an entity
-------------------------------------*/
size_t ECSDatabase::num_components(Entity& e) noexcept
{
    size_t numComponents = 0;

    for (const ls::utils::Pointer<Component>& c : mComponents)
    {
        if (c && c->contains(e))
        {
            ++numComponents;
        }
    }

    return numComponents;
}



} // end game namespace
} // end ls namespace
