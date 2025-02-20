#include <jank/runtime/obj/repeat.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/core.hpp>
#include <jank/runtime/core/math.hpp>
#include <jank/runtime/core/make_box.hpp>
#include <jank/runtime/core/seq.hpp>
#include <jank/runtime/behavior/metadatable.hpp>
#include <jank/runtime/obj/cons.hpp>

namespace jank::runtime::obj
{
  repeat::repeat(object_ptr const value)
    : value{ value }
    , count{ make_box(infinite) }
  {
  }

  repeat::repeat(object_ptr const count, object_ptr const value)
    : value{ value }
    , count{ count }
  {
  }

  object_ptr repeat::create(object_ptr const value)
  {
    return make_box<repeat>(value);
  }

  object_ptr repeat::create(object_ptr const count, object_ptr const value)
  {
    if(lte(count, make_box(0)))
    {
      return persistent_list::empty();
    }
    return make_box<repeat>(count, value);
  }

  repeat_ptr repeat::seq()
  {
    return this;
  }

  repeat_ptr repeat::fresh_seq() const
  {
    return make_box<repeat>(count, value);
  }

  object_ptr repeat::first() const
  {
    return value;
  }

  repeat_ptr repeat::next() const
  {
    if(runtime::equal(count, make_box(infinite)))
    {
      return this;
    }
    if(lt(count, make_box(1)))
    {
      return nullptr;
    }
    return make_box<repeat>(make_box(add(count, make_box(-1))), value);
  }

  repeat_ptr repeat::next_in_place()
  {
    if(runtime::equal(count, make_box(infinite)))
    {
      return this;
    }
    if(lte(count, make_box(1)))
    {
      return nullptr;
    }
    count = add(count, make_box(-1));
    return this;
  }

  cons_ptr repeat::conj(object_ptr const head) const
  {
    return make_box<cons>(head, this);
  }

  native_bool repeat::equal(object const &o) const
  {
    auto const bs(behaviors(&o));
    if(!bs->is_seqable)
    {
      return false;
    }
    else
    {
      auto seq(bs->fresh_seq(&o));
      /* TODO: This is common code; can it be shared? */
      //TODO next_in_place / first perf
      for(auto it(behaviors(seq)->fresh_seq(seq)); it != nullptr;
          it = behaviors(it)->fresh_seq(it), seq = behaviors(seq)->fresh_seq(seq))
      {
        if(seq == nullptr || !runtime::equal(runtime::first(it), runtime::first(seq)))
        {
          return false;
        }
      }
      return true;
    }
  }

  void repeat::to_string(util::string_builder &buff)
  {
    runtime::to_string(seq(), buff);
  }

  native_persistent_string repeat::to_string()
  {
    return runtime::to_string(seq());
  }

  native_persistent_string repeat::to_code_string()
  {
    return runtime::to_code_string(seq());
  }

  native_hash repeat::to_hash() const
  {
    return hash::ordered(&base);
  }

  repeat_ptr repeat::with_meta(object_ptr const m) const
  {
    auto const meta(behavior::detail::validate_meta(m));
    auto ret(fresh_seq());
    ret->meta = meta;
    return ret;
  }
}
