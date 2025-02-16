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
  template <typename T>
  behaviors<T> object_behaviors(native_box<T> type)
  {
    return visit_object(
      [&](auto const typed_o) -> behaviors<T> {
        using S = typename decltype(typed_o)::value_type;

        // TODO: cache
        behaviors<S const> bs{};
        if constexpr(behavior::seqable<S>)
        {
          bs.is_seqable = true;
        }
        if constexpr(behavior::sequenceable<S>)
        {
          bs.is_seq = true;
        }
        if constexpr(behavior::collection_like<S>)
        {
          bs.is_collection = true;
        }
        if constexpr(behavior::associatively_readable<S> && behavior::associatively_writable<S>)
        {
          bs.is_associative = true;
        }
        if constexpr(behavior::countable<S>)
        {
          bs.is_counter = true;
        }
        if constexpr(behavior::transientable<S>)
        {
          bs.is_transientable = true;
          bs.to_transient = [](native_box<S const> const o) { o->to_transient(); };
        }

        return reinterpret_cast<native_box<T>>(bs);
      },
      type);
  }
}
