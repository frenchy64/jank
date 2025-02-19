#pragma once

#include <jank/runtime/object.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/detail/type.hpp>

namespace jank::runtime
{
  template <typename It>
  native_bool equal(object const &o, It const begin, It const end)
  {
    /* nil is seqable, but we don't want it to be equal to an empty collection.
       An empty seq itself is nil, but that's different. */
    if(&o == obj::nil::nil_const())
    {
      return false;
    }
    auto const bs(behaviors(&o));
    if(!bs->is_seqable)
    {
      return false;
    }
    auto seq(bs->fresh_seq(&o));
    auto it(begin);
    //TODO next_in_place / first perf
    for(; it != end; ++it, seq = behaviors(seq)->next_in_place(seq))
    {
      if(seq == nullptr || !runtime::equal(*it, behaviors(seq)->first(seq)))
      {
        return false;
      }
    }
    return seq == nullptr && it == end;
  }
}
