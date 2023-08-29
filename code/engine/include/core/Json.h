#ifndef Json_h__
#define Json_h__

#include "json/json.hpp"

namespace rush
{
    using Json = nlohmann::basic_json<nlohmann::ordered_map>;
    using JsonException = nlohmann::detail::exception;
}

/*!
@brief macro
@def NLOHMANN_DEFINE_TYPE_INTRUSIVE
@since version 3.9.0
*/
#define RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE(Type, ...)  \
    friend void to_json(rush::Json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const rush::Json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

#define RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE_WITH_DEFAULT(Type, ...)  \
    friend void to_json(rush::Json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const rush::Json& nlohmann_json_j, Type& nlohmann_json_t) { const Type nlohmann_json_default_obj{}; NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) }

/*!
@brief macro
@def NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
@since version 3.9.0
*/
#define RUSH_DEFINE_PROPERTIES_INTRUSIVE(Type, ...)  \
    inline void to_json(rush::Json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const rush::Json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

#define RUSH_DEFINE_PROPERTIES_INTRUSIVE_WITH_DEFAULT(Type, ...)  \
    inline void to_json(rush::Json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const rush::Json& nlohmann_json_j, Type& nlohmann_json_t) { const Type nlohmann_json_default_obj{}; NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) }

#endif // Json_h__
