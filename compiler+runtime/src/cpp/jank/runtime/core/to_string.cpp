#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/obj/character.hpp>

namespace jank::runtime
{
  native_persistent_string to_string(object const * const o)
  {
    return behaviors(o)->to_string(o);
  }

  void to_string(char const ch, util::string_builder &buff)
  {
    obj::character{ ch }.to_string(buff);
  }

  void to_string(object_ptr const o, util::string_builder &buff)
  {
    return behaviors(o)->to_string_builder(o, buff);
  }

  native_persistent_string to_code_string(object const * const o)
  {
    return behaviors(o)->to_code_string(o);
  }

  void to_code_string(char const ch, util::string_builder &buff)
  {
    buff(obj::character{ ch }.to_code_string());
  }

  void to_code_string(object_ptr const o, util::string_builder &buff)
  {
    buff(behaviors(o)->to_code_string(o));
  }
}
