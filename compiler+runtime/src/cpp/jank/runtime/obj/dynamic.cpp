#include <jank/runtime/obj/dynamic.hpp>
#include <jank/runtime/obj/keyword.hpp>
#include <jank/runtime/rtti.hpp>
#include <jank/runtime/context.hpp>

namespace jank::runtime::obj
{
  dynamic::dynamic(object_ptr const tag, void* const wrapped, object_ptr const behaviors)
    : tag{ tag }
    , wrapped{ wrapped }
    , behaviors{ behaviors }
  {
  }

  native_bool dynamic::equal(object const &o) const
  {
    if(o.type != object_type::dynamic)
    {
      return false;
    }

    //auto const s(expect_object<dynamic>(&o));
    //return runtime::equal(tag, s->tag) && TODO;
    throw "dynamic equal";
  }

  static void
  to_string_impl(object_ptr const &tag, util::string_builder &buff)
  {
    buff('#');
    to_string(tag, buff);
  }

  void dynamic::to_string(util::string_builder &buff) const
  {
    to_string_impl(tag, buff);
  }

  native_persistent_string dynamic::to_string() const
  {
    util::string_builder buff;
    to_string_impl(tag, buff);
    return buff.release();
  }

  native_persistent_string dynamic::to_code_string() const
  {
    return to_string();
  }

  native_hash dynamic::to_hash() const
  {
    if(hash)
    {
      return hash;
    }

    throw "TODO hash dynamic";
    //return hash = hash::combine(hash::visit(tag), hash::visit(form));
  }

  /* behavior::metadatable */
  native_bool is_metadatable()
  {
    return behaviors->is_metadatable;
  }

  native_bool set_meta(object_ptr const m)
  {
    return behaviors->set_meta(this, m);
  }
}
