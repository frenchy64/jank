#include <fmt/core.h>
#include <jank/native_persistent_string/fmt.hpp>

#include <jank/runtime/visit.hpp>

namespace jank::runtime
{
  behaviors object_behaviors(object_ptr type)
  {
    return visit_object(
      [](auto const typed_o) -> behaviors {
        static behaviors const bs{ typed_o };
        return bs;
      },
      type);
  }
}
