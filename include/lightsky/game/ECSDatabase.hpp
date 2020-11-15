
#ifndef LS_GAME_DATABASE_HPP
#define LS_GAME_DATABASE_HPP

#include <limits>
#include <unordered_set>

#include "lightsky/utils/Tuple.h"
#include "lightsky/game/Entity.hpp"
#include "lightsky/game/Component.hpp"

namespace ls
{
namespace game
{


enum class ComponentAddStatus;

enum class ComponentRemoveStatus;



/*-----------------------------------------------------------------------------
 * Helper classes to manage partial template specialization in the ECS
 * database.
-----------------------------------------------------------------------------*/
template <unsigned max, unsigned iter>
struct ECSDatabaseHelper;

template <unsigned max>
struct ECSDatabaseHelper<max, max>;



/*-------------------------------------
 * Iteratable helper
-------------------------------------*/
template <unsigned max, unsigned iter>
struct ECSDatabaseHelper
{
    template <typename ...ComponentTypes>
    void destroy_entity_internal(utils::Tuple<Component<ComponentTypes>...>& tuple, const Entity& e) const noexcept
    {
        tuple.template element<iter>().erase(e);

        constexpr ECSDatabaseHelper<max, iter+1> helper;
        helper.template destroy_entity_internal<ComponentTypes...>(tuple, e);
    }

    template <typename ...ComponentTypes>
    bool has_components_internal(const utils::Tuple<Component<ComponentTypes>...>& tuple, const Entity& e) const noexcept
    {
        constexpr ECSDatabaseHelper<max, iter+1> helper;
        return tuple.template const_element<iter>().contains(e) || helper.template has_components_internal<ComponentTypes...>(tuple, e);
    }

    template <typename ...ComponentTypes>
    size_t num_components_internal(const utils::Tuple<Component<ComponentTypes>...>& tuple, const Entity& e) const noexcept
    {
        constexpr ECSDatabaseHelper<max, iter+1> helper;
        return (size_t)tuple.template element<iter>().contains(e) + helper.template num_components_internal<ComponentTypes...>(tuple, e);
    }

    template <typename ...ComponentTypes>
    void clear_internal(utils::Tuple<Component<ComponentTypes>...>& tuple) const noexcept
    {
        tuple.template element<iter>().clear();

        constexpr ECSDatabaseHelper<max, iter+1> helper;
        helper.template clear_internal<ComponentTypes...>(tuple);
    }
};



/*-------------------------------------
 * Iteratable sentinel helper
-------------------------------------*/
template <unsigned max>
struct ECSDatabaseHelper<max, max>
{
    template <typename ...ComponentTypes>
    void destroy_entity_internal(utils::Tuple<Component<ComponentTypes>...>& tuple, const Entity& e) const noexcept
    {
        (void)tuple;
        (void)e;
    }

    template <typename ...ComponentTypes>
    bool has_components_internal(const utils::Tuple<Component<ComponentTypes>...>& tuple, const Entity& e) const noexcept
    {
        (void)tuple;
        (void)e;
        return false;
    }

    template <typename ...ComponentTypes>
    size_t num_components_internal(const utils::Tuple<Component<ComponentTypes>...>& tuple, const Entity& e) const noexcept
    {
        (void)tuple;
        (void)e;
        return 0;
    }

    template <typename ...ComponentTypes>
    void clear_internal(utils::Tuple<Component<ComponentTypes>...>& tuple) const noexcept
    {
        (void)tuple;
    }
};



/*-----------------------------------------------------------------------------
 * ECS database.
 *
 * This is the central manager of all entities and components
-----------------------------------------------------------------------------*/
template <typename ...ComponentTypes>
class ECSDatabase
{
  public:
    enum : EntityIdType
    {
        INVALID_ENTITY = std::numeric_limits<EntityIdType>::max()
    };

    typedef std::unordered_set<Entity> set_type;

    typedef EntityIdType size_type;

    typedef utils::Tuple<Component<ComponentTypes>...> tuple_type;

  private:
    tuple_type mComponents;

    set_type mEntities;

    size_type mNextEntityId;

  public:
    ~ECSDatabase() = default;

    ECSDatabase() noexcept;

    Entity create_entity() noexcept;

    void destroy_entity(const Entity& e) noexcept;

    template <typename ComponentType>
    bool has_component(const Entity&) const noexcept;

    bool has_components(const Entity& e) const noexcept;

    // get the total number of components used by and entity
    size_t num_components(const Entity& e) const noexcept;

    // get a reference to a component container
    template <typename ComponentType>
    const Component<ComponentType>& component() const noexcept;

    // add a component to an entity
    template <typename ComponentType>
    ComponentAddStatus add(const Entity& e) noexcept;

    // add a component to an entity
    template <typename ComponentType, typename... Args>
    ComponentAddStatus add(const Entity& e, Args&&...) noexcept;

    // remove a component from an entity
    template <typename ComponentType>
    ComponentRemoveStatus remove(const Entity& e) noexcept;

    // clear a single component type
    template <typename ComponentType>
    void clear() noexcept;

    // clear all components
    void clear() noexcept;

    // get the size of a component type
    template <typename ComponentType>
    void size() noexcept;

    // iterate a specific component. Each component type should specify its
    // own iterator (const).
    template <typename ComponentType, typename IterCallback>
    void iterate(const IterCallback& cb) const noexcept;

    // iterate a specific component. Each component type should specify its
    // own iterator
    template <typename ComponentType, typename IterCallback>
    void iterate(const IterCallback& cb) noexcept;

    // invoke a specific update method
    template <typename ComponentType>
    void update() noexcept;
};



/*-------------------------------------
 * Constructor
-------------------------------------*/
template <typename ...ComponentTypes>
ECSDatabase<ComponentTypes...>::ECSDatabase() noexcept :
    mComponents{},
    mEntities{},
    mNextEntityId{0}
{}



/*-------------------------------------
 * Spawn an entity with a unique ID
-------------------------------------*/
template <typename ...ComponentTypes>
Entity ECSDatabase<ComponentTypes...>::create_entity() noexcept
{
    if (mEntities.size() == (size_t)std::numeric_limits<size_type>::max())
    {
        return Entity{(EntityIdType)INVALID_ENTITY};
    }

    Entity newEntity;
    set_type::iterator iter;

    do
    {
        newEntity.id = mNextEntityId++;
        iter = mEntities.find(newEntity);
    }
    while (iter != mEntities.end());

    mEntities.insert(newEntity);
    return newEntity;
}



/*-------------------------------------
 * Destroy an entity
-------------------------------------*/
template <typename ...ComponentTypes>
void ECSDatabase<ComponentTypes...>::destroy_entity(const Entity& e) noexcept
{
    constexpr ECSDatabaseHelper<sizeof...(ComponentTypes), 0> helper;
    helper.template destroy_entity_internal<ComponentTypes...>(mComponents, e);
    mEntities.erase(e);
}




/*-------------------------------------
 * Check if an entity has a specific component
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
bool ECSDatabase<ComponentTypes...>::has_component(const Entity& e) const noexcept
{
    return mComponents.template first_of<Component<ComponentType>>().contains(e);
}



/*-------------------------------------
 * Check if an entity has any components
-------------------------------------*/
template <typename ...ComponentTypes>
bool ECSDatabase<ComponentTypes...>::has_components(const Entity& e) const noexcept
{
    constexpr ECSDatabaseHelper<sizeof...(ComponentTypes), 0> helper;
    return helper.template has_components_internal<ComponentTypes...>(mComponents, e);
}



/*-------------------------------------
 * Get the number of components used by an entity
-------------------------------------*/
template <typename ...ComponentTypes>
size_t ECSDatabase<ComponentTypes...>::num_components(const Entity& e) const noexcept
{
    constexpr ECSDatabaseHelper<sizeof...(ComponentTypes), 0> helper;
    return helper.template num_components_internal<ComponentTypes...>(mComponents, e);
}



/*-------------------------------------
 * Get a reference to a component container
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
const Component<ComponentType>& ECSDatabase<ComponentTypes...>::component() const noexcept
{
    return mComponents.template first_of<Component<ComponentType>>();
}



/*-------------------------------------
 * Add a component to an entity
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
ComponentAddStatus ECSDatabase<ComponentTypes...>::add(const Entity& e) noexcept
{
    return mComponents.template first_of<Component<ComponentType>>().insert(e);
}



/*-------------------------------------
 * Add a component to an entity (with arguments)
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType, typename... Args>
ComponentAddStatus ECSDatabase<ComponentTypes...>::add(const Entity& e, Args&&... args) noexcept
{
    return mComponents.template first_of<Component<ComponentType>>().insert(e, std::forward<Args>(args)...);
}



/*-------------------------------------
 * Remove a component from an entity
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
ComponentRemoveStatus ECSDatabase<ComponentTypes...>::remove(const Entity& e) noexcept
{
    return mComponents.template first_of<Component<ComponentType>>().erase(e);
}



/*-------------------------------------
 * Clear a single component type
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
void ECSDatabase<ComponentTypes...>::clear() noexcept
{
    mComponents.template first_of<Component<ComponentType>>().clear();
}



/*-------------------------------------
 * Clear all components
-------------------------------------*/
template <typename ...ComponentTypes>
void ECSDatabase<ComponentTypes...>::clear() noexcept
{
    constexpr ECSDatabaseHelper<sizeof...(ComponentTypes), 0> helper;
    return helper.template clear_internal<ComponentTypes...>(mComponents);
}



/*-------------------------------------
 * Get the size of a component type
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
void ECSDatabase<ComponentTypes...>::size() noexcept
{
    return mComponents.template first_of<Component<ComponentType>>().size();
}



/*-------------------------------------
 * Iterate a specific component (const).
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType, typename IterCallback>
void ECSDatabase<ComponentTypes...>::iterate(const IterCallback& cb) const noexcept
{
    mComponents.template first_of<Component<ComponentType>>().iterate(cb);
}



/*-------------------------------------
 * Iterate a specific component.
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType, typename IterCallback>
void ECSDatabase<ComponentTypes...>::iterate(const IterCallback& cb) noexcept
{
    mComponents.template first_of<Component<ComponentType>>().iterate(cb);
}




} // end game namespace
} // end ls namespace

#endif /* LS_GAME_DATABASE_HPP */
