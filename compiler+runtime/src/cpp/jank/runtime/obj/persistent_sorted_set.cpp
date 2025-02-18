#include <jank/runtime/obj/persistent_sorted_set.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/core/seq.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/obj/persistent_sorted_set_sequence.hpp>
#include <jank/runtime/obj/transient_sorted_set.hpp>
#include <jank/runtime/behavior/metadatable.hpp>

namespace jank::runtime::obj
{
  persistent_sorted_set::persistent_sorted_set(value_type &&d)
    : data{ std::move(d) }
  {
  }

  persistent_sorted_set::persistent_sorted_set(
    runtime::detail::native_persistent_sorted_set const &d)
    : data{ d }
  {
  }

  persistent_sorted_set::persistent_sorted_set(object_ptr const meta, value_type &&d)
    : data{ std::move(d) }
    , meta{ meta }
  {
  }

  persistent_sorted_set::persistent_sorted_set(option<object_ptr> const &meta, value_type &&d)
    : data{ std::move(d) }
    , meta{ meta }
  {
  }

  persistent_sorted_set_ptr persistent_sorted_set::create_from_seq(object_ptr const seq)
  {
    auto const bs(object_behaviors(seq));
    if(!bs.is_seqable)
    {
      throw std::runtime_error{ "not seqable: " + bs.to_code_string(seq) };
    }

    runtime::detail::native_transient_sorted_set transient;
    for(auto it(bs.fresh_seq(seq)); it != nullptr; it = object_behaviors(it).next_in_place(it))
    {
      transient.insert_v(object_behaviors(it).first(it));
    }
    return make_box<persistent_sorted_set>(transient.persistent());
  }

  native_bool persistent_sorted_set::equal(object const &o) const
  {
    if(&o == &base)
    {
      return true;
    }

    auto const bs(object_behaviors(&o));
    if(!bs.is_set)
    {
      return false;
    }

    if(bs.count(&o) != count())
    {
      return false;
    }

    for(auto const entry : data)
    {
      if(!bs.contains(&o, entry))
      {
        return false;
      }
    }

    return true;
  }

  void persistent_sorted_set::to_string(util::string_builder &buff) const
  {
    runtime::to_string(data.begin(), data.end(), "#{", '}', buff);
  }

  native_persistent_string persistent_sorted_set::to_string() const
  {
    util::string_builder buff;
    runtime::to_string(data.begin(), data.end(), "#{", '}', buff);
    return buff.release();
  }

  native_persistent_string persistent_sorted_set::to_code_string() const
  {
    util::string_builder buff;
    runtime::to_code_string(data.begin(), data.end(), "#{", '}', buff);
    return buff.release();
  }

  /* TODO: Cache this. */
  native_hash persistent_sorted_set::to_hash() const
  {
    return hash::unordered(data.begin(), data.end());
  }

  persistent_sorted_set_sequence_ptr persistent_sorted_set::seq() const
  {
    return fresh_seq();
  }

  object_ptr persistent_sorted_set::create_empty() const
  {
    static auto const ret(empty());
    return meta.map_or(ret, [&](auto const m) { return ret->with_meta(m); });
  }

  persistent_sorted_set_sequence_ptr persistent_sorted_set::fresh_seq() const
  {
    if(data.empty())
    {
      return nullptr;
    }
    return make_box<persistent_sorted_set_sequence>(this, data.begin(), data.end(), data.size());
  }

  size_t persistent_sorted_set::count() const
  {
    return data.size();
  }

  persistent_sorted_set_ptr persistent_sorted_set::with_meta(object_ptr const m) const
  {
    auto const meta(behavior::detail::validate_meta(m));
    auto ret(make_box<persistent_sorted_set>(data));
    ret->meta = meta;
    return ret;
  }

  persistent_sorted_set_ptr persistent_sorted_set::conj(object_ptr const head) const
  {
    auto set(data.insert_v(head));
    auto ret(make_box<persistent_sorted_set>(meta, std::move(set)));
    return ret;
  }

  object_ptr persistent_sorted_set::call(object_ptr const o)
  {
    auto const found(data.find(o));
    if(found != data.end())
    {
      return found.get();
    }
    return nil::nil_const();
  }

  transient_sorted_set_ptr persistent_sorted_set::to_transient() const
  {
    return make_box<transient_sorted_set>(data);
  }

  native_bool persistent_sorted_set::contains(object_ptr const o) const
  {
    return data.find(o) != data.end();
  }

  persistent_sorted_set_ptr persistent_sorted_set::disj(object_ptr const o) const
  {
    auto set(data.erase_key(o));
    auto ret(make_box<persistent_sorted_set>(meta, std::move(set)));
    return ret;
  }
}
