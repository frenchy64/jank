#include <jank/runtime/coz.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/native_persistent_string.hpp>

#include <coz.h>
#include <fmt/format.h>
#include <jank/native_persistent_string/fmt.hpp>

namespace jank::runtime::coz
{
  object_ptr progress()
  {
    COZ_PROGRESS;
    return obj::nil::nil_const();
  }

  object_ptr progress_named(object_ptr const name)
  {
    std::string const s(fmt::format("{}", to_string(name)));
    const char* const c(s.c_str());
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
    COZ_PROGRESS(c);
#pragma clang diagnostic pop
    return obj::nil::nil_const();
  }

  object_ptr begin(object_ptr const name)
  {
    std::string const s(fmt::format("{}", to_string(name)));
    const char* const c(s.c_str());
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
    COZ_BEGIN(c);
#pragma clang diagnostic pop
    return obj::nil::nil_const();
  }

  object_ptr end(object_ptr const name)
  {
    std::string const s(fmt::format("{}", to_string(name)));
    const char* const c(s.c_str());
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
    COZ_END(c);
#pragma clang diagnostic pop
    return obj::nil::nil_const();
  }
}
