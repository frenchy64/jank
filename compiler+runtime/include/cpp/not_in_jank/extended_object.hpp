#pragma once

#include <jank/runtime/object.hpp>

namespace not_in_jank
{
  using extended_object_ptr = native_box<struct extended_object>;

  struct extended_object : gc
  {
    //TODO object_type ???
    static constexpr object_type obj_type{ object_type::extended_object };
    static constexpr native_bool pointer_free{ false };

    extended_object() = default;
    extended_object(object_ptr val);

    /* behavior::object_like */
    native_bool equal(object const &) const;
    native_persistent_string to_string() const;
    void to_string(util::string_builder &buff) const;
    native_persistent_string to_code_string() const;
    native_hash to_hash() const;

    /* behavior::derefable */
    object_ptr deref();

    object base{ obj_type };
    object_ptr val{};
  };
}
