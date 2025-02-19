#include <fmt/format.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/obj/persistent_hash_map.hpp>
#include <jank/runtime/obj/persistent_vector.hpp>
#include <jank/runtime/obj/transient_hash_map.hpp>
#include <jank/runtime/behavior/seqable.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/core/seq.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/rtti.hpp>

namespace jank::runtime::obj
{
  persistent_hash_map::persistent_hash_map(option<object_ptr> const &meta,
                                           runtime::detail::native_persistent_array_map const &m,
                                           object_ptr const key,
                                           object_ptr const val)
    : parent_type{ meta }
  {
    runtime::detail::native_transient_hash_map transient;
    for(auto const &e : m)
    {
      transient.set(e.first, e.second);
    }
    transient.set(key, val);
    data = transient.persistent();
  }

  persistent_hash_map::persistent_hash_map(value_type &&d)
    : data{ std::move(d) }
  {
  }

  persistent_hash_map::persistent_hash_map(value_type const &d)
    : data{ d }
  {
  }

  persistent_hash_map::persistent_hash_map(object_ptr const meta, value_type &&d)
    : data{ std::move(d) }
  {
    this->meta = meta;
  }

  persistent_hash_map::persistent_hash_map(option<object_ptr> const &meta, value_type &&d)
    : parent_type{ meta }
    , data{ std::move(d) }
  {
  }

  persistent_hash_map_ptr persistent_hash_map::create_from_seq(object_ptr const seq)
  {
    auto const bs(behaviors(seq));
    if(!bs->is_seqable)
    {
      throw std::runtime_error{ fmt::format("Not seqable: {}", bs->to_string(seq)) };
    }

    runtime::detail::native_transient_hash_map transient;
    // TODO next_in_place / first perf
    for(auto it(bs->fresh_seq(seq)); it != nullptr; it = behaviors(it)->next_in_place(it))
    {
      auto const it_bs(behaviors(it));
      auto const key(it_bs->first(it));
      it = it_bs->next_in_place(it);
      if(!it)
      {
        throw std::runtime_error{ fmt::format("Odd number of elements: {}", bs->to_string(seq)) };
      }
      // TODO not confident using it_bs->first here..
      auto const val(runtime::first(it));
      transient.set(key, val);
    }
    return make_box<persistent_hash_map>(transient.persistent());
  }

  object_ptr persistent_hash_map::get(object_ptr const key) const
  {
    auto const res(data.find(key));
    if(res)
    {
      return *res;
    }
    return nil::nil_const();
  }

  object_ptr persistent_hash_map::get(object_ptr const key, object_ptr const fallback) const
  {
    auto const res(data.find(key));
    if(res)
    {
      return *res;
    }
    return fallback;
  }

  object_ptr persistent_hash_map::get_entry(object_ptr const key) const
  {
    auto const res(data.find(key));
    if(res)
    {
      return make_box<persistent_vector>(std::in_place, key, *res);
    }
    return nil::nil_const();
  }

  native_bool persistent_hash_map::contains(object_ptr const key) const
  {
    return data.find(key);
  }

  persistent_hash_map_ptr
  persistent_hash_map::assoc(object_ptr const key, object_ptr const val) const
  {
    auto copy(data.set(key, val));
    return make_box<persistent_hash_map>(meta, std::move(copy));
  }

  persistent_hash_map_ptr persistent_hash_map::dissoc(object_ptr const key) const
  {
    auto copy(data.erase(key));
    return make_box<persistent_hash_map>(meta, std::move(copy));
  }

  object_ptr persistent_hash_map::create_empty() const
  {
    static auto const ret(empty());
    return meta.map_or(ret, [&](auto const m) { return ret->with_meta(m); });
  }

  persistent_hash_map_ptr persistent_hash_map::conj(object_ptr const head) const
  {
    if(head->type == object_type::persistent_array_map
       || head->type == object_type::persistent_hash_map)
    {
      return expect_object<persistent_hash_map>(runtime::merge(this, head));
    }

    if(head->type != object_type::persistent_vector)
    {
      throw std::runtime_error{ fmt::format("invalid map entry: {}", runtime::to_string(head)) };
    }

    auto const vec(expect_object<persistent_vector>(head));
    if(vec->count() != 2)
    {
      throw std::runtime_error{ fmt::format("invalid map entry: {}", runtime::to_string(head)) };
    }

    auto copy(data.set(vec->data[0], vec->data[1]));
    return make_box<persistent_hash_map>(meta, std::move(copy));
  }

  object_ptr persistent_hash_map::call(object_ptr const o) const
  {
    return get(o);
  }

  object_ptr persistent_hash_map::call(object_ptr const o, object_ptr const fallback) const
  {
    return get(o, fallback);
  }

  transient_hash_map_ptr persistent_hash_map::to_transient() const
  {
    return make_box<transient_hash_map>(data);
  }
}
