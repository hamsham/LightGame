
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
    mEntities{},
    mMinEntityId{0}
{}



ECSDatabase::ECSDatabase(ECSDatabase&& db) noexcept :
    mComponents{std::move(db.mComponents)},
    mEntities{std::move(db.mEntities)},
    mMinEntityId{db.mMinEntityId}
{
    db.mMinEntityId = 0;
}



ECSDatabase& ECSDatabase::operator=(ECSDatabase&& db) noexcept
{
    if (this != &db)
    {
        mComponents = std::move(db.mComponents);
        mEntities = std::move(db.mEntities);
        mMinEntityId = db.mMinEntityId;

        db.mMinEntityId = 0;
    }

    return *this;
}



/*-------------------------------------
 * Spawn an entity with a unique ID
-------------------------------------*/
Entity ECSDatabase::create_entity() noexcept
{
    if (mMinEntityId == std::numeric_limits<std::unordered_set<Entity>::size_type>::max())
    {
        return Entity{(EntityIdType)INVALID_ENTITY};
    }

    // new entities always get the lowest free index in our set. This will help
    // both get a unique ID and enable us to check if we're out of memory.
    Entity newEntity{mMinEntityId};
    std::unordered_set<Entity>::iterator&& iter = mEntities.find(newEntity);

    // make sure we're not creating a previously generated entity
    LS_ASSERT(iter == mEntities.end()); // insurance

    mEntities.insert(newEntity);

    while (true)
    {
        iter = mEntities.find(Entity{++mMinEntityId});
        if (iter == mEntities.end())
        {
            break;
        }
    }

    return newEntity;
}



/*-------------------------------------
 * Spawn an entity with a unique ID
-------------------------------------*/
void ECSDatabase::destroy_entity(Entity& e) noexcept
{
    const std::unordered_set<Entity>::iterator&& iter = mEntities.find(e);
    LS_DEBUG_ASSERT(iter != mEntities.end()); // no double-freeing
    mEntities.erase(iter);

    for (utils::Pointer<Component>& component : mComponents)
    {
        if (component->contains(e))
        {
            component->erase(e);
        }
    }

    if (e.id < mMinEntityId)
    {
        mMinEntityId = e.id;
    }

    e.id = (EntityIdType)INVALID_ENTITY;
}


/*-------------------------------------
 * Get the number of components for an entity
-------------------------------------*/
size_t ECSDatabase::num_components(Entity& e) const noexcept
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
