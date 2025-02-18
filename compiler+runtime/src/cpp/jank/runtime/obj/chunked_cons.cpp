#include <fmt/format.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/obj/chunked_cons.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/core/make_box.hpp>
#include <jank/runtime/core/seq.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/core.hpp>
#include <jank/runtime/behavior/chunkable.hpp>
#include <jank/runtime/behavior/metadatable.hpp>
#include <jank/runtime/obj/array_chunk.hpp>
#include <jank/runtime/obj/cons.hpp>
#include <jank/runtime/obj/chunk_buffer.hpp>

namespace jank::runtime::obj
{
  chunked_cons::chunked_cons(object_ptr const head, object_ptr const tail)
    : head{ head }
    , tail{ tail == nil::nil_const() ? nullptr : tail }
  {
    assert(head);
  }

  chunked_cons::chunked_cons(object_ptr const meta, object_ptr const head, object_ptr const tail)
    : head{ head }
    , tail{ tail == nil::nil_const() ? nullptr : tail }
    , meta{ meta }
  {
    assert(head);
    assert(meta);
  }

  chunked_cons_ptr chunked_cons::seq() const
  {
    return const_cast<chunked_cons *>(this);
  }

  chunked_cons_ptr chunked_cons::fresh_seq() const
  {
    return make_box<chunked_cons>(head, tail);
  }

  object_ptr chunked_cons::first() const
  {
    auto const bs(object_behaviors(head));

    if(!bs.is_chunk_like)
    {
      throw std::runtime_error{ fmt::format("invalid chunked cons head: {}", bs.to_string(head)) };
    }
    return bs.nth(head, make_box(0));
  }

  object_ptr chunked_cons::next() const
  {
    auto const bs(object_behaviors(head));

    if(!bs.is_chunk_like)
    {
      throw std::runtime_error{ fmt::format("invalid chunked cons head: {}", bs.to_string(head)) };
    }

    if(1 < bs.count(head))
    {
      return make_box<chunked_cons>(bs.chunk_next(head), tail);
    }
    return tail;
  }

  static chunked_cons_ptr next_in_place_non_chunked(chunked_cons_ptr const o)
  {
    if(!o->tail)
    {
      return nullptr;
    }

    auto const tail(o->tail);
    auto const bs(object_behaviors(tail));

    if(!bs.is_sequenceable)
    {
      throw std::runtime_error{ fmt::format("invalid sequence: {}", bs.to_string(tail)) };
    }

    o->head = bs.first(tail);
    o->tail = bs.next(tail);
    if(o->tail == nil::nil_const())
    {
      o->tail = nullptr;
    }
    return o;
  }

  chunked_cons_ptr chunked_cons::next_in_place()
  {
    auto const bs(object_behaviors(head));

    if(!bs.is_chunk_like)
    {
      return next_in_place_non_chunked(this);
    }

    if(1 < bs.count(head))
    {
      head = bs.chunk_next(head);
      return this;
    }
    return next_in_place_non_chunked(this);
  }

  object_ptr chunked_cons::chunked_first() const
  {
    if(object_behaviors(head).is_chunk_like)
    {
      return head;
    }

    auto const buffer(make_box<chunk_buffer>(static_cast<size_t>(1)));
    buffer->append(head);
    return buffer->chunk();
  }

  object_ptr chunked_cons::chunked_next() const
  {
    return tail;
  }

  native_bool chunked_cons::equal(object const &o) const
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

  void chunked_cons::to_string(util::string_builder &buff) const
  {
    runtime::to_string(seq(), buff);
  }

  native_persistent_string chunked_cons::to_string() const
  {
    return runtime::to_string(seq());
  }

  native_persistent_string chunked_cons::to_code_string() const
  {
    return runtime::to_code_string(seq());
  }

  native_hash chunked_cons::to_hash() const
  {
    return hash::ordered(&base);
  }

  cons_ptr chunked_cons::conj(object_ptr const head) const
  {
    return make_box<cons>(head, this);
  }

  chunked_cons_ptr chunked_cons::with_meta(object_ptr const m) const
  {
    auto const meta(behavior::detail::validate_meta(m));
    auto ret(fresh_seq());
    ret->meta = meta;
    return ret;
  }
}
