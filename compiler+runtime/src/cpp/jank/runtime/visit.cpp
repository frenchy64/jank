#include <fmt/core.h>
#include <jank/native_persistent_string/fmt.hpp>

#include <jank/runtime/visit.hpp>

namespace jank::runtime
{
  behaviors* object_behaviors(object_ptr type)
  {
    //fmt::println("bool {} {}", runtime::to_string(type), object_type_str(type->type));
    return visit_object(
      [&](auto const typed_o) -> behaviors* {
        using T = typename decltype(typed_o)::value_type;
        static auto const bs(specific_object_behaviors<T>(typed_o));
        return bs;
      },
      type);
  }
}
