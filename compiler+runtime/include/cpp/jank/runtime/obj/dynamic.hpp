#pragma once

#include <jank/runtime/object.hpp>

namespace jank::runtime::obj
{
  using dynamic_ptr = native_box<struct dynamic>;

  struct dynamic : gc
  {
    static constexpr object_type obj_type{ object_type::dynamic };
    static constexpr native_bool pointer_free{ false };

    dynamic(object_ptr tag, void* wrapped, object_ptr behaviors);

    /* behavior::object_like */
    native_bool equal(object const &) const;
    native_persistent_string to_string() const;
    void to_string(util::string_builder &buff) const;
    native_persistent_string to_code_string() const;
    native_hash to_hash() const;

    object base{ obj_type };

    object_ptr tag{};
    void* wrapped{};
    object_ptr behaviors{};

    mutable native_hash hash{};
  };
}
