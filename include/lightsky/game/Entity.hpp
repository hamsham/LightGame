
#ifndef LS_GAME_ENTITY_HPP
#define LS_GAME_ENTITY_HPP

#include <cstdint> // uint32_t
#include <functional> // std::equal_to

namespace ls
{
namespace game
{

typedef uint32_t EntityIdType;



struct Entity
{
    EntityIdType id;
};



} // end game namespace
} // end ls namespace



namespace std
{

template <>
struct hash<ls::game::Entity>
{
    typedef ls::game::Entity argument_type;
    typedef size_t result_type;

    constexpr size_t operator()(const ls::game::Entity& e) const noexcept
    {
        return (size_t)e.id;
    }
};

template <>
struct equal_to<ls::game::Entity>
{
    typedef bool result_type;
    typedef ls::game::Entity first_argument_type;
    typedef ls::game::Entity second_argument_type;

    constexpr bool operator()(const ls::game::Entity& a, const ls::game::Entity& b) const noexcept
    {
        return a.id == b.id;
    }
};

} // end std namespace

#endif /* LS_GAME_ENTITY_HPP */
