#include <fmt/format.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/obj/cons.hpp>
#include <jank/runtime/core.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/behavior/metadatable.hpp>

namespace jank::runtime::obj
{
  cons::cons(object_ptr const head, object_ptr const tail)
    : head{ head }
    , tail{ tail == nil::nil_const() ? nullptr : tail }
  {
    assert(head);
  }

  cons_ptr cons::seq() const
  {
    return const_cast<cons *>(this);
  }

  cons_ptr cons::fresh_seq() const
  {
    return make_box<cons>(head, tail);
  }

  object_ptr cons::first() const
  {
    return head;
  }

  object_ptr cons::next() const
  {
    if(!tail)
    {
      return nullptr;
    }

    return runtime::seq(tail);
  }

  cons_ptr cons::next_in_place()
  {
    if(!tail)
    {
      return nullptr;
    }

    auto const bs(object_behaviors(tail));
    if(!bs.is_sequenceable)
    {
      throw std::runtime_error{ fmt::format("invalid sequence: {}", bs.to_string(tail)) };
    }
    head = bs.first(tail);
    tail = bs.next(tail);
    if(tail == nil::nil_const())
    {
      tail = nullptr;
    }

    return this;
  }

  native_bool cons::equal(object const &o) const
  {
    auto const bs(object_behaviors(&o));
    if(!bs.is_seqable)
    {
      return false;
    }

    auto seq(bs.fresh_seq(&o));
    //TODO next_in_place / first perf
    for(object_ptr it(fresh_seq()); it != nullptr;
        it = object_behaviors(it).next_in_place(it), seq = object_behaviors(seq).next_in_place(seq))
    {
      if(seq == nullptr
         || !runtime::equal(object_behaviors(it).first(it), object_behaviors(seq).first(seq)))
      {
        return false;
      }
    }
    return true;
  }

  void cons::to_string(util::string_builder &buff) const
  {
    runtime::to_string(seq(), buff);
  }

  native_persistent_string cons::to_string() const
  {
    return runtime::to_string(seq());
  }

  native_persistent_string cons::to_code_string() const
  {
    return runtime::to_code_string(seq());
  }

  native_hash cons::to_hash() const
  {
    if(hash != 0)
    {
      return hash;
    }

    return hash = hash::ordered(&base);
  }

  cons_ptr cons::conj(object_ptr const head) const
  {
    return make_box<cons>(head, this);
  }

  cons_ptr cons::with_meta(object_ptr const m) const
  {
    auto const meta(behavior::detail::validate_meta(m));
    auto ret(fresh_seq());
    ret->meta = meta;
    return ret;
  }
}
