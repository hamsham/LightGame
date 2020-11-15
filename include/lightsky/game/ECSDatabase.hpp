
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

    template <unsigned componentIter>
    void destroy_entity_internal(const Entity&) noexcept;

    template <>
    void destroy_entity_internal<sizeof...(ComponentTypes)>(const Entity& e) noexcept { (void)e; }

    template <unsigned componentIter>
    bool has_components_internal(const Entity&) const noexcept;

    template <>
    bool has_components_internal<sizeof...(ComponentTypes)>(const Entity&) const noexcept { return false; }

    template <unsigned componentIter>
    size_t num_compnents_internal(const Entity& e) noexcept;

    template <>
    size_t num_compnents_internal<sizeof...(ComponentTypes)>(const Entity& e) noexcept { (void)e; return 0; }

    template <unsigned componentIter>
    void clear_internal() noexcept;

    template <>
    void clear_internal<sizeof...(ComponentTypes)>() noexcept {}

  public:
    ~ECSDatabase() = default;

    ECSDatabase() noexcept;

    Entity create_entity() noexcept;

    inline void destroy_entity(const Entity& e) noexcept { destroy_entity_internal<0>(e); mEntities.erase(e); }

    template <typename ComponentType>
    bool has_component(const Entity&) const noexcept;

    inline bool has_components(const Entity& e) const noexcept { return has_components_internal<0>(e); }

    // get the total number of components used by and entity
    size_t num_compnents(const Entity& e) noexcept { return num_compnents_internal<0>(e); }

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
    void clear() noexcept { clear_internal<0>(); }

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
 *
-------------------------------------*/
template <typename ...ComponentTypes>
ECSDatabase<ComponentTypes...>::ECSDatabase() noexcept :
    mComponents{},
    mEntities{},
    mNextEntityId{0}
{}



/*-------------------------------------
 *
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
 *
-------------------------------------*/
template <typename ...ComponentTypes>
template <unsigned componentIter>
void ECSDatabase<ComponentTypes...>::destroy_entity_internal(const Entity& e) noexcept
{
    mComponents.template element<componentIter>().erase(e);
    destroy_entity_internal<componentIter+1>(e);
}



/*-------------------------------------
 *
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
bool ECSDatabase<ComponentTypes...>::has_component(const Entity& e) const noexcept
{
    return mComponents.template first_of<Component<ComponentType>>().contains(e);
}



/*-------------------------------------
 *
-------------------------------------*/
template <typename ...ComponentTypes>
template <unsigned componentIter>
bool ECSDatabase<ComponentTypes...>::has_components_internal(const Entity& e) const noexcept
{
    return mComponents.template const_element<componentIter>().contains(e) || has_components_internal<componentIter+1>(e);
}



/*-------------------------------------
 * Get the total number of components used by and entity
-------------------------------------*/
template <typename ...ComponentTypes>
template <unsigned componentIter>
size_t ECSDatabase<ComponentTypes...>::num_compnents_internal(const Entity& e) noexcept
{
    return (size_t)mComponents.template element<componentIter>().contains(e) + num_compnents_internal<componentIter+1>(e);
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
template <unsigned componentIter>
void ECSDatabase<ComponentTypes...>::clear_internal() noexcept
{
    mComponents.template element<componentIter>().clear();
    clear_internal<componentIter+1>();
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



/*-------------------------------------
 * Update a specific component.
-------------------------------------*/
template <typename ...ComponentTypes>
template <typename ComponentType>
void ECSDatabase<ComponentTypes...>::update() noexcept
{
    mComponents.template first_of<Component<ComponentType>>().update();
}




} // end game namespace
} // end ls namespace

#endif /* LS_GAME_DATABASE_HPP */
