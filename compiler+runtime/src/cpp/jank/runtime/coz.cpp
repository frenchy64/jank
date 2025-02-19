#include <jank/runtime/coz.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/native_persistent_string.hpp>

#include <coz.h>
#include <fmt/format.h>
#include <jank/native_persistent_string/fmt.hpp>

namespace jank::runtime::coz
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
  object_ptr progress()
  {
    COZ_PROGRESS;
    return obj::nil::nil_const();
  }

  object_ptr progress_named(object_ptr const name)
  {
    std::string const s(fmt::format("{}", to_string(name)));
    const char* const c(s.c_str());
    COZ_PROGRESS(c);
    return obj::nil::nil_const();
  }

  object_ptr begin(object_ptr const name)
  {
    std::string const s(fmt::format("{}", to_string(name)));
    const char* const c(s.c_str());
    COZ_BEGIN(c);
    return obj::nil::nil_const();
  }

  object_ptr end(object_ptr const name)
  {
    std::string const s(fmt::format("{}", to_string(name)));
    const char* const c(s.c_str());
    COZ_END(c);
    return obj::nil::nil_const();
  }
#pragma clang diagnostic pop
}
