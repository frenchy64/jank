#include <fmt/format.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/obj/persistent_vector.hpp>
#include <jank/runtime/obj/transient_vector.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/rtti.hpp>
#include <jank/runtime/core/seq.hpp>
#include <jank/runtime/core/seq_ext.hpp>
#include <jank/runtime/obj/persistent_vector_sequence.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/obj/number.hpp>
#include <jank/runtime/behavior/metadatable.hpp>
#include <jank/runtime/behavior/sequential.hpp>

namespace jank::runtime::obj
{
  persistent_vector::persistent_vector(value_type &&d)
    : data{ std::move(d) }
  {
  }

  persistent_vector::persistent_vector(value_type const &d)
    : data{ d }
  {
  }

  persistent_vector::persistent_vector(object_ptr const meta, value_type &&d)
    : data{ std::move(d) }
    , meta{ meta }
  {
  }

  persistent_vector::persistent_vector(option<object_ptr> const &meta, value_type &&d)
    : data{ std::move(d) }
    , meta{ meta }
  {
  }

  persistent_vector_ptr persistent_vector::create(object_ptr const s)
  {
    if(s == nullptr)
    {
      return make_box<persistent_vector>();
    }

    auto const bs(behaviors(s));
    if(!bs->is_sequenceable)
    {
      throw std::runtime_error{ fmt::format("invalid sequence: {}", bs->to_string(s)) };
    }

    runtime::detail::native_transient_vector v;
    for(auto i(bs->fresh_seq(s)); i != nullptr; i = behaviors(i)->next_in_place(i))
    {
      v.push_back(runtime::first(i));
    }
    return make_box<persistent_vector>(v.persistent());
  }

  object_ptr persistent_vector::create_empty() const
  {
    static auto const ret(empty());
    return meta.map_or(ret, [&](auto const m) { return ret->with_meta(m); });
  }

  persistent_vector_ptr persistent_vector::empty()
  {
    static auto const ret(make_box<persistent_vector>());
    return ret;
  }

  native_bool persistent_vector::equal(object const &o) const
  {
    if(&o == &base)
    {
      return true;
    }
    if(auto const v = dyn_cast<persistent_vector>(&o))
    {
      if(data.size() != v->data.size())
      {
        return false;
      }
      for(size_t i{}; i < data.size(); ++i)
      {
        if(!runtime::equal(data[i], v->data[i]))
        {
          return false;
        }
      }
      return true;
    }
    else
    {
      auto const bs(behaviors(&o));
      if(!bs->is_sequential)
      {
        return false;
      }
      size_t i{};
      auto e(bs->fresh_seq(&o));
      for(; e != nullptr; e = behaviors(e)->next_in_place(e))
      {
        if(!runtime::equal(data[i], runtime::first(e)))
        {
          return false;
        }

        if(++i == data.size())
        {
          e = behaviors(e)->next_in_place(e);
          break;
        }
      }
      return e == nullptr && i == data.size();
    }
  }

  void persistent_vector::to_string(util::string_builder &buff) const
  {
    runtime::to_string(data.begin(), data.end(), "[", ']', buff);
  }

  native_persistent_string persistent_vector::to_string() const
  {
    util::string_builder buff;
    runtime::to_string(data.begin(), data.end(), "[", ']', buff);
    return buff.release();
  }

  native_persistent_string persistent_vector::to_code_string() const
  {
    util::string_builder buff;
    runtime::to_code_string(data.begin(), data.end(), "[", ']', buff);
    return buff.release();
  }

  native_hash persistent_vector::to_hash() const
  {
    if(hash != 0)
    {
      return hash;
    }

    return hash = hash::ordered(data.begin(), data.end());
  }

  native_integer persistent_vector::compare(object const &o) const
  {
    return compare(*try_object<persistent_vector>(&o));
  }

  native_integer persistent_vector::compare(persistent_vector const &v) const
  {
    auto const size(data.size());
    auto const v_size(v.data.size());

    if(size < v_size)
    {
      return -1;
    }
    if(size > v_size)
    {
      return 1;
    }

    for(size_t i{}; i < size; ++i)
    {
      auto const res(runtime::compare(data[i], v.data[i]));
      if(res != 0)
      {
        return res;
      }
    }

    return 0;
  }

  persistent_vector_sequence_ptr persistent_vector::seq() const
  {
    if(data.empty())
    {
      return nullptr;
    }
    return make_box<persistent_vector_sequence>(const_cast<persistent_vector *>(this));
  }

  persistent_vector_sequence_ptr persistent_vector::fresh_seq() const
  {
    if(data.empty())
    {
      return nullptr;
    }
    return make_box<persistent_vector_sequence>(const_cast<persistent_vector *>(this));
  }

  size_t persistent_vector::count() const
  {
    return data.size();
  }

  persistent_vector_ptr persistent_vector::conj(object_ptr head) const
  {
    auto vec(data.push_back(head));
    auto ret(make_box<persistent_vector>(meta, std::move(vec)));
    return ret;
  }

  transient_vector_ptr persistent_vector::to_transient() const
  {
    return make_box<transient_vector>(data);
  }

  persistent_vector_ptr persistent_vector::with_meta(object_ptr const m) const
  {
    auto const meta(behavior::detail::validate_meta(m));
    auto ret(make_box<persistent_vector>(data));
    ret->meta = meta;
    return ret;
  }

  object_ptr persistent_vector::get(object_ptr const key) const
  {
    return get(key, nil::nil_const());
  }

  object_ptr persistent_vector::get(object_ptr const key, object_ptr const fallback) const
  {
    if(key->type == object_type::integer)
    {
      auto const i(expect_object<integer>(key)->data);
      if(i < 0 || data.size() <= static_cast<size_t>(i))
      {
        return fallback;
      }
      return data[i];
    }
    else
    {
      return fallback;
    }
  }

  object_ptr persistent_vector::get_entry(object_ptr const key) const
  {
    if(key->type == object_type::integer)
    {
      auto const i(expect_object<integer>(key)->data);
      if(i < 0 || data.size() <= static_cast<size_t>(i))
      {
        return nil::nil_const();
      }
      /* TODO: Map entry type? */
      return make_box<persistent_vector>(std::in_place, key, data[i]);
    }
    else
    {
      return nil::nil_const();
    }
  }

  native_bool persistent_vector::contains(object_ptr const key) const
  {
    if(key->type == object_type::integer)
    {
      auto const i(expect_object<integer>(key)->data);
      return i >= 0 && static_cast<size_t>(i) < data.size();
    }
    else
    {
      return false;
    }
  }

  object_ptr persistent_vector::peek() const
  {
    if(data.empty())
    {
      return nil::nil_const();
    }

    return data[data.size() - 1];
  }

  persistent_vector_ptr persistent_vector::pop() const
  {
    if(data.empty())
    {
      throw std::runtime_error{ "cannot pop an empty vector" };
    }

    return make_box<persistent_vector>(meta, data.take(data.size() - 1));
  }

  object_ptr persistent_vector::nth(object_ptr const index) const
  {
    if(index->type == object_type::integer)
    {
      auto const i(expect_object<integer>(index)->data);
      if(i < 0 || data.size() <= static_cast<size_t>(i))
      {
        throw std::runtime_error{
          fmt::format("out of bounds index {}; vector has a size of {}", i, data.size())
        };
      }
      return data[i];
    }
    else
    {
      throw std::runtime_error{ fmt::format("nth on a vector must be an integer; found {}",
                                            runtime::to_string(index)) };
    }
  }

  object_ptr persistent_vector::nth(object_ptr const index, object_ptr const fallback) const
  {
    return get(index, fallback);
  }
}
