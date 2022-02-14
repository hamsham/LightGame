
#ifndef LS_GAME_DATABASE_HPP
#define LS_GAME_DATABASE_HPP

#include <unordered_set>
#include <utility> // std::forward
#include <vector>

#include "lightsky/utils/Pointer.h"
#include "lightsky/utils/Tuple.h"

#include "lightsky/game/Entity.hpp"
#include "lightsky/game/Component.hpp"

namespace ls
{
namespace game
{



struct Entity;
class Component;



enum class ComponentCreateStatus
{
    REGISTER_ERR_COMPONENT_EXISTS,
    REGISTER_ERR_NO_MEMORY,
    REGISTER_OK
};



/*-----------------------------------------------------------------------------
 * ECS database.
 *
 * This is the central manager of all entities and components
-----------------------------------------------------------------------------*/
class ECSDatabase
{
  public:
    enum : EntityIdType
    {
        INVALID_ENTITY = ~(EntityIdType)0
    };

  private:
    std::vector<utils::Pointer<Component>> mComponents;

    std::unordered_set<Entity> mEntities;

    std::size_t mMinEntityId;

  public:
    ~ECSDatabase() noexcept;

    ECSDatabase() noexcept;

    ECSDatabase(const ECSDatabase&) = delete;

    ECSDatabase(ECSDatabase&&) noexcept;

    ECSDatabase& operator=(const ECSDatabase&) = delete;

    ECSDatabase& operator=(ECSDatabase&&) noexcept;

    template <typename ComponentType>
    ComponentCreateStatus construct_component();

    template <typename ComponentType, typename... Args>
    ComponentCreateStatus construct_component(Args&&... args);

    template <typename ComponentType>
    void destroy_component();

    // get a reference to a component container
    template <typename ComponentType>
    const ComponentType* component() const noexcept;

    // get a reference to a component container
    template <typename ComponentType>
    ComponentType* component() noexcept;

    Entity create_entity() noexcept;

    void destroy_entity(Entity& e) noexcept;

    size_t num_components(Entity& e) const noexcept;
};



/*-------------------------------------
 * Construct a component with no arguments
-------------------------------------*/
template <typename ComponentType>
ComponentCreateStatus ECSDatabase::construct_component()
{
    const std::size_t componentId = Component::registration_id<ComponentType>();
    if (mComponents.size() <= componentId)
    {
        mComponents.resize(componentId+1);
    }
    else
    {
        if (mComponents[componentId])
        {
            return ComponentCreateStatus::REGISTER_ERR_COMPONENT_EXISTS;
        }
    }

    mComponents[componentId].reset(new(std::nothrow) ComponentType{});
    if (!mComponents[componentId])
    {
        return ComponentCreateStatus::REGISTER_ERR_NO_MEMORY;
    }

    return ComponentCreateStatus::REGISTER_OK;
}



/*-------------------------------------
 * Construct a component
-------------------------------------*/
template <typename ComponentType, typename... Args>
ComponentCreateStatus ECSDatabase::construct_component(Args&&... args)
{
    const std::size_t componentId = Component::registration_id<ComponentType>();
    if (mComponents.size() <= componentId)
    {
        mComponents.resize(componentId+1, ls::utils::Pointer<Component>{nullptr});
    }
    else
    {
        if (mComponents[componentId])
        {
            return ComponentCreateStatus::REGISTER_ERR_COMPONENT_EXISTS;
        }
    }

    mComponents[componentId].reset(new(std::nothrow) ComponentType{std::forward<Args>(args)...});
    if (!mComponents[componentId])
    {
        return ComponentCreateStatus::REGISTER_ERR_NO_MEMORY;
    }

    return ComponentCreateStatus::REGISTER_OK;
}



/*-------------------------------------
 * Destroy a Component
-------------------------------------*/
template <typename ComponentType>
void ECSDatabase::destroy_component()
{
    const std::size_t componentId = Component::registration_id<ComponentType>();
    if (mComponents.size() <= componentId)
    {
        return;
    }

    if ((mComponents.size()-1) == componentId)
    {
        mComponents.pop_back();
    }
    else
    {
        mComponents[componentId].reset();
    }
}



/*-------------------------------------
 * Retrieve a component (const)
-------------------------------------*/
template <typename ComponentType>
const ComponentType* ECSDatabase::component() const noexcept
{
    // obtain an index to the component. Use a static const so this unique ID
    // is initialized once and re-used throughout the lifetime of the program.
    const std::size_t componentId = Component::registration_id<ComponentType>();
    return static_cast<const ComponentType*>(mComponents[componentId].get());
}



/*-------------------------------------
 * Retrieve a component
-------------------------------------*/
template <typename ComponentType>
ComponentType* ECSDatabase::component() noexcept
{
    // obtain an index to the component. Use a static const so this unique ID
    // is initialized once and re-used throughout the lifetime of the program.
    const std::size_t componentId = Component::registration_id<ComponentType>();
    return static_cast<ComponentType*>(mComponents[componentId].get());
}



} // end game namespace
} // end ls namespace

#endif /* LS_GAME_DATABASE_HPP */
