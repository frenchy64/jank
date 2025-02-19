#include <jank/runtime/obj/detail/base_persistent_map_sequence.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/obj/cons.hpp>
#include <jank/runtime/obj/persistent_array_map_sequence.hpp>
#include <jank/runtime/obj/persistent_hash_map_sequence.hpp>
#include <jank/runtime/obj/persistent_sorted_map_sequence.hpp>

namespace jank::runtime::obj::detail
{
  template <typename PT, typename IT>
  base_persistent_map_sequence<PT, IT>::base_persistent_map_sequence(object_ptr const c,
                                                                     IT const &b,
                                                                     IT const &e)
    : coll{ c }
    , begin{ b }
    , end{ e }
  {
    assert(begin != end);
  }

  template <typename PT, typename IT>
  native_bool base_persistent_map_sequence<PT, IT>::equal(object const &o) const
  {
    auto const bs(behaviors(&o));
    if(!bs->is_seqable)
    {
      return false;
    }

    auto seq(bs->fresh_seq(&o));
    //TODO next_in_place / first perf
    for(object_ptr it(fresh_seq()); it != nullptr;
        it = behaviors(it)->next_in_place(it), seq = behaviors(seq)->next_in_place(seq))
    {
      if(seq == nullptr
         || !runtime::equal(behaviors(it)->first(it), behaviors(seq)->first(seq)))
      {
        return false;
      }
    }
    return true;
  }

  template <typename PT, typename IT>
  void base_persistent_map_sequence<PT, IT>::to_string_impl(util::string_builder &buff,
                                                            native_bool const to_code) const
  {
    buff('(');
    for(auto i(begin); i != end; ++i)
    {
      buff('[');
      if(to_code)
      {
        runtime::to_code_string((*i).first, buff);
      }
      else
      {
        runtime::to_string((*i).first, buff);
      }
      buff(' ');
      if(to_code)
      {
        runtime::to_code_string((*i).second, buff);
      }
      else
      {
        runtime::to_string((*i).second, buff);
      }
      buff(']');
      auto n(i);
      if(++n != end)
      {
        buff(' ');
      }
    }
    buff(')');
  }

  template <typename PT, typename IT>
  void base_persistent_map_sequence<PT, IT>::to_string(util::string_builder &buff) const
  {
    return to_string_impl(buff, false);
  }

  template <typename PT, typename IT>
  native_persistent_string base_persistent_map_sequence<PT, IT>::to_string() const
  {
    util::string_builder buff;
    to_string_impl(buff, false);
    return buff.release();
  }

  template <typename PT, typename IT>
  native_persistent_string base_persistent_map_sequence<PT, IT>::to_code_string() const
  {
    util::string_builder buff;
    to_string_impl(buff, true);
    return buff.release();
  }

  template <typename PT, typename IT>
  native_hash base_persistent_map_sequence<PT, IT>::to_hash() const
  {
    return hash::unordered(&static_cast<PT const *>(this)->base);
  }

  template <typename PT, typename IT>
  size_t base_persistent_map_sequence<PT, IT>::count() const
  {
    return std::distance(begin, end);
  }

  template <typename PT, typename IT>
  native_box<PT> base_persistent_map_sequence<PT, IT>::seq()
  {
    return static_cast<PT *>(this);
  }

  template <typename PT, typename IT>
  native_box<PT> base_persistent_map_sequence<PT, IT>::fresh_seq() const
  {
    return make_box<PT>(coll, begin, end);
  }

  template <typename PT, typename IT>
  obj::persistent_vector_ptr base_persistent_map_sequence<PT, IT>::first() const
  {
    auto const pair(*begin);
    return make_box<obj::persistent_vector>(
      runtime::detail::native_persistent_vector{ pair.first, pair.second });
  }

  template <typename PT, typename IT>
  native_box<PT> base_persistent_map_sequence<PT, IT>::next() const
  {
    auto n(begin);
    ++n;

    if(n == end)
    {
      return nullptr;
    }

    return make_box<PT>(coll, n, end);
  }

  template <typename PT, typename IT>
  native_box<PT> base_persistent_map_sequence<PT, IT>::next_in_place()
  {
    ++begin;

    if(begin == end)
    {
      return nullptr;
    }

    return static_cast<PT *>(this);
  }

  template <typename PT, typename IT>
  obj::cons_ptr base_persistent_map_sequence<PT, IT>::conj(object_ptr const head)
  {
    return make_box<obj::cons>(head, static_cast<PT *>(this));
  }

  template struct base_persistent_map_sequence<
    persistent_hash_map_sequence,
    runtime::detail::native_persistent_hash_map::const_iterator>;
  template struct base_persistent_map_sequence<
    persistent_array_map_sequence,
    runtime::detail::native_persistent_array_map::const_iterator>;
  template struct base_persistent_map_sequence<
    persistent_sorted_map_sequence,
    runtime::detail::native_persistent_sorted_map::const_iterator>;
}
