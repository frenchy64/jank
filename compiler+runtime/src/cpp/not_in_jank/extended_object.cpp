#include <fmt/format.h>

#include <not_in_jank/extended_object.hpp>
#include <jank/runtime/behavior/callable.hpp>
#include <jank/runtime/core/make_box.hpp>

namespace not_in_jank
{
  extended_object::extended_object(object_ptr const v)
    : val{ val }
  {
  }

  native_bool extended_object::equal(object const &o) const
  {
    return &o == &base;
  }

  native_persistent_string extended_object::to_string() const
  {
    util::string_builder buff;
    to_string(buff);
    return buff.release();
  }

  void extended_object::to_string(util::string_builder &buff) const
  {
    fmt::format_to(std::back_inserter(buff), "{}@{}", object_type_str(base.type), fmt::ptr(&base));
  }

  native_persistent_string extended_object::to_code_string() const
  {
    return to_string();
  }

  native_hash extended_object::to_hash() const
  {
    return static_cast<native_hash>(reinterpret_cast<uintptr_t>(this));
  }

  object_ptr extended_object::deref()
  {
    std::lock_guard<std::mutex> const lock{ mutex };
    if(val != nullptr)
    {
      return val;
    }

    if(error != nullptr)
    {
      throw error;
    }

    try
    {
      val = dynamic_call(fn);
    }
    catch(std::exception const &e)
    {
      error = make_box(e.what());
      throw;
    }
    catch(object_ptr const e)
    {
      error = e;
      throw;
    }
    return val;
  }
}
