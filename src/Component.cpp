
#include <atomic>
#include <utility> // std::move

#include "lightsky/game/Component.hpp"

namespace ls
{
namespace game
{



std::size_t Component::_increment_component_id() noexcept
{
    static std::atomic_size_t idCount{0};
    return idCount.fetch_add(1);
}



Component::~Component() noexcept
{
    clear();
}



Component::Component() noexcept
{
}



Component::Component(Component&& c) noexcept :
    mEntities{std::move(c.mEntities)}
{}



Component& Component::operator=(Component&& c) noexcept
{
    if (this != &c)
    {
        mEntities = std::move(c.mEntities);
    }

    return *this;
}



ComponentAddStatus Component::insert(const Entity& e) noexcept
{
    auto&& iter = mEntities.find(e);
    if (iter == mEntities.end())
    {
        return mEntities.insert(e).second ? ComponentAddStatus::ADD_OK : ComponentAddStatus::ADD_ERR_OTHER;
    }

    return ComponentAddStatus::ADD_ERR_ENTITY_EXISTS;
}



ComponentRemoveStatus Component::erase(const Entity& e) noexcept
{
    auto&& iter = mEntities.find(e);
    if (iter != mEntities.end())
    {
        mEntities.erase(e);
        return ComponentRemoveStatus::REMOVE_OK;
    }

    return ComponentRemoveStatus::REMOVE_ERR_ENTITY_MISSING;
}



void Component::update() noexcept
{
    for (const game::Entity& e : mEntities)
    {
        this->update_entity(e);
    }
}



} // end game namespace
} // end ls namespace
