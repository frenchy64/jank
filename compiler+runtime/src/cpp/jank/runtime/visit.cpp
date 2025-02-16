#include <jank/runtime/visit.hpp>

#include <jank/runtime/behavior/associatively_readable.hpp>
#include <jank/runtime/behavior/associatively_writable.hpp>
#include <jank/runtime/behavior/callable.hpp>
#include <jank/runtime/behavior/conjable.hpp>
#include <jank/runtime/behavior/countable.hpp>
#include <jank/runtime/behavior/seqable.hpp>
#include <jank/runtime/behavior/set_like.hpp>
#include <jank/runtime/behavior/sequential.hpp>
#include <jank/runtime/behavior/collection_like.hpp>
#include <jank/runtime/behavior/transientable.hpp>
#include <jank/runtime/behavior/indexable.hpp>
#include <jank/runtime/behavior/stackable.hpp>
#include <jank/runtime/behavior/chunkable.hpp>
#include <jank/runtime/behavior/metadatable.hpp>

namespace jank::runtime
{
  behaviors object_behaviors(object_ptr type)
  {
    return visit_object(
      [&](auto const typed_o) -> behaviors {
        using T = typename decltype(typed_o)::value_type;

        behaviors bs{};
        if constexpr(behavior::seqable<T>)
        {
          bs.is_seqable = true;
        }
        if constexpr(behavior::sequenceable<T>)
        {
          bs.is_seq = true;
        }
        if constexpr(behavior::collection_like<T>)
        {
          bs.is_collection = true;
        }
        if constexpr(behavior::associatively_readable<T> && behavior::associatively_writable<T>)
        {
          bs.is_associative = true;
        }

        return bs;
      },
      type);
  }
}
