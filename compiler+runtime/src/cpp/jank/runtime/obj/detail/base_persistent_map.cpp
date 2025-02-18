#include <jank/runtime/obj/detail/base_persistent_map.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/obj/persistent_array_map.hpp>
#include <jank/runtime/obj/persistent_sorted_map.hpp>
#include <jank/runtime/obj/persistent_sorted_map_sequence.hpp>
#include <jank/runtime/obj/persistent_hash_map.hpp>
#include <jank/runtime/obj/persistent_hash_map_sequence.hpp>

namespace jank::runtime::obj::detail
{
  template <typename PT, typename ST, typename V>
  base_persistent_map<PT, ST, V>::base_persistent_map(option<object_ptr> const &meta)
    : meta{ meta }
  {
  }

  template <typename PT, typename ST, typename V>
  native_bool base_persistent_map<PT, ST, V>::equal(object const &o) const
  {
    if(&o == &base)
    {
      return true;
    }

    auto const bs(object_behaviors(&o));
    if(!bs.is_map)
    {
      return false;
    }

    if(bs.count(&o) != count())
    {
      return false;
    }

    for(auto const &entry : static_cast<PT const *>(this)->data)
    {
      auto const found(bs.contains(&o, entry.first));

      if(!found || !runtime::equal(entry.second, bs.get(&o, entry.first)))
      {
        return false;
      }
    }

    return true;
  }

  template <typename PT, typename ST, typename V>
  void base_persistent_map<PT, ST, V>::to_string_impl(typename V::const_iterator const &begin,
                                                      typename V::const_iterator const &end,
                                                      util::string_builder &buff,
                                                      native_bool const to_code)
  {
    auto inserter(std::back_inserter(buff));
    inserter = '{';
    for(auto i(begin); i != end; ++i)
    {
      auto const pair(*i);
      if(to_code)
      {
        runtime::to_code_string(pair.first, buff);
      }
      else
      {
        runtime::to_string(pair.first, buff);
      }
      inserter = ' ';

      if(to_code)
      {
        runtime::to_code_string(pair.second, buff);
      }
      else
      {
        runtime::to_string(pair.second, buff);
      }
      auto n(i);
      if(++n != end)
      {
        inserter = ',';
        inserter = ' ';
      }
    }
    inserter = '}';
  }

  template <typename PT, typename ST, typename V>
  void base_persistent_map<PT, ST, V>::to_string(util::string_builder &buff) const
  {
    to_string_impl(static_cast<PT const *>(this)->data.begin(),
                   static_cast<PT const *>(this)->data.end(),
                   buff,
                   false);
  }

  template <typename PT, typename ST, typename V>
  native_persistent_string base_persistent_map<PT, ST, V>::to_string() const
  {
    util::string_builder buff;
    to_string_impl(static_cast<PT const *>(this)->data.begin(),
                   static_cast<PT const *>(this)->data.end(),
                   buff,
                   false);
    return buff.release();
  }

  template <typename PT, typename ST, typename V>
  native_persistent_string base_persistent_map<PT, ST, V>::to_code_string() const
  {
    util::string_builder buff;
    to_string_impl(static_cast<PT const *>(this)->data.begin(),
                   static_cast<PT const *>(this)->data.end(),
                   buff,
                   true);
    return buff.release();
  }

  template <typename PT, typename ST, typename V>
  native_hash base_persistent_map<PT, ST, V>::to_hash() const
  {
    if(hash)
    {
      return hash;
    }

    return hash = hash::unordered(static_cast<PT const *>(this)->data.begin(),
                                  static_cast<PT const *>(this)->data.end());
  }

  template <typename PT, typename ST, typename V>
  native_box<ST> base_persistent_map<PT, ST, V>::seq() const
  {
    if(static_cast<PT const *>(this)->data.empty())
    {
      return nullptr;
    }
    return make_box<ST>(static_cast<PT const *>(this),
                        static_cast<PT const *>(this)->data.begin(),
                        static_cast<PT const *>(this)->data.end());
  }

  template <typename PT, typename ST, typename V>
  native_box<ST> base_persistent_map<PT, ST, V>::fresh_seq() const
  {
    if(static_cast<PT const *>(this)->data.empty())
    {
      return nullptr;
    }
    return make_box<ST>(static_cast<PT const *>(this),
                        static_cast<PT const *>(this)->data.begin(),
                        static_cast<PT const *>(this)->data.end());
  }

  template <typename PT, typename ST, typename V>
  size_t base_persistent_map<PT, ST, V>::count() const
  {
    return static_cast<PT const *>(this)->data.size();
  }

  template <typename PT, typename ST, typename V>
  native_box<PT> base_persistent_map<PT, ST, V>::with_meta(object_ptr const m) const
  {
    auto const meta(behavior::detail::validate_meta(m));
    auto ret(make_box<PT>(static_cast<PT const *>(this)->data));
    ret->meta = meta;
    return ret;
  }

  template struct base_persistent_map<persistent_array_map,
                                      persistent_array_map_sequence,
                                      runtime::detail::native_persistent_array_map>;
  template struct base_persistent_map<persistent_hash_map,
                                      persistent_hash_map_sequence,
                                      runtime::detail::native_persistent_hash_map>;
  template struct base_persistent_map<persistent_sorted_map,
                                      persistent_sorted_map_sequence,
                                      runtime::detail::native_persistent_sorted_map>;
}
