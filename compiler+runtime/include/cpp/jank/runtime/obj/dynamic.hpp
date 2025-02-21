#pragma once

#include <jank/runtime/object.hpp>
#include <jank/runtime/behaviors.hpp>

namespace jank::runtime::obj
{
  using dynamic_ptr = native_box<struct dynamic>;

  struct dynamic : gc
  {
    static constexpr object_type obj_type{ object_type::dynamic };
    static constexpr native_bool pointer_free{ false };

    dynamic(object_ptr tag, object_behaviors_ptr behaviors, void* impl);

    /* behavior::object_like */
    native_bool equal(object const &) const;
    native_persistent_string to_string() const;
    void to_string(util::string_builder &buff) const;
    native_persistent_string to_code_string() const;
    native_hash to_hash() const;
    
    /* behavior::metadatable */
    native_bool is_metadatable();
    //std::function<object_ptr(object_ptr const, object_ptr const)> with_meta{};
    //std::function<option<object_ptr>(object_ptr const)> meta{};
    //std::function<object_ptr(object_ptr const)> get_meta{}; //convenient, could be a member function
    object_ptr set_meta(object_ptr const, object_ptr const);

    object base{ obj_type };

    /* Tag must be a qualified keyword under a namespace you control. */
    keyword_ptr tag{} const;
    object_behaviors_ptr behaviors{} const;
    /* The implementation details for this dynamic object.
     * Only relevant for C++ implementors.
     *
     * equal = [](object_ptr const lhs, object_ptr const rhs) {
     *   auto const lhs_dyn(expect_object<dynamic> lhs);
     *   auto const lhs_impl(<reinterpret_cast<a_cpp_class *>(lhs_dyn->impl));
     *   auto const rhs_dyn(dyn_cast<dynamic>(rhs));
     *   if(!rhs_dyn || !equal(lhs_dyn->tag, rhs_dyn->tag))
     *   {
     *     return false;
     *   }
     *   auto const rhs_impl(<reinterpret_cast<a_cpp_class *>(rhs_dyn->impl));
     *   // call method on wrapped class
     *   return lhs_impl->equivalent(rhs_impl);
     * }
     * */
    void* impl{} const;

    mutable native_hash hash{};
  };
}
