#include <jank/runtime/visit.hpp>

namespace jank::runtime
{
  behaviors* object_behaviors(object_ptr type)
  {
    return visit_object(
      [&](auto const typed_o) -> behaviors* {
        using T = typename decltype(typed_o)::value_type;
        static auto const bs(specific_object_behaviors<T>(typed_o));
        return bs;
      },
      type);
  }
}
