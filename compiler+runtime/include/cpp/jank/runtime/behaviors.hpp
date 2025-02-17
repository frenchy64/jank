#pragma once

#include <jank/runtime/object.hpp>
#include <jank/option.hpp>

namespace jank::runtime
{
  // TODO try splitting up
  //   all_behaviors
  //   -> countable_behaviors
  //      -> count
  // TODO behaviors<T>
  struct behaviors
  {
    template <typename T>
    requires behavior::object_like<T>
    behaviors(native_box<T>);

    //native_bool is_list{};
    //native_bool is_sorted{};
    native_bool is_associative{}; // redundant?
    native_bool is_associatively_readable{};
    native_bool is_associatively_writable{};
    native_bool is_associatively_writable_in_place{};
    native_bool is_callable{};
    native_bool is_chunk_like{};
    native_bool is_chunkable{};
    native_bool is_conjable{};
    native_bool is_conjable_in_place{};
    native_bool is_collection{};
    native_bool is_countable{};
    native_bool is_derefable{};
    native_bool is_indexable{};
    native_bool is_function_like{};
    native_bool is_map{};
    native_bool is_metadatable{};
    native_bool is_named{};
    native_bool is_object_like{}; //redundant?
    native_bool is_seqable{};
    native_bool is_sequenceable_in_place{};
    native_bool is_sequenceable{};
    native_bool is_sequential{};
    native_bool is_set{};
    native_bool is_transientable{};
    native_bool is_persistentable{};
    native_bool is_vector{};

    /* behavior::object_like */
    std::function<native_persistent_string(object_ptr const)> to_string{};
    std::function<native_persistent_string(object_ptr const)> to_code_string{};
    std::function<native_hash(object_ptr const)> to_hash{};
    std::function<native_bool(object_ptr const, object_ptr const)> equal{};

    /* behavior::associatively_writable_in_place */
    std::function<object_ptr(object_ptr const m, object_ptr const k, object_ptr const v)>
      assoc_in_place{};
    std::function<object_ptr(object_ptr const m, object_ptr const k)> dissoc_in_place{};

    /* behavior::associatively_writable */
    std::function<object_ptr(object_ptr const m, object_ptr const k, object_ptr const v)> assoc{};
    std::function<object_ptr(object_ptr const m, object_ptr const k)> dissoc{};

    /* behavior::associatively_readable */
    std::function<object_ptr(object_ptr const m, object_ptr const k)> get{};
    std::function<object_ptr(object_ptr const m, object_ptr const k, object_ptr const d)>
      get_default{};
    std::function<object_ptr(object_ptr const m, object_ptr const k)> get_entry{};

    /* behavior::associatively_readable || behavior::set_like */
    std::function<native_bool(object_ptr const m, object_ptr const k)> contains{};

    /* behavior::sequenceable */
    std::function<object_ptr(object_ptr const)> first{};
    std::function<object_ptr(object_ptr const)> next{};

    /* behavior::countable */
    std::function<size_t(object_ptr const)> count{};

    /* behavior::sequenceable_in_place */
    std::function<object_ptr(object_ptr const)> next_in_place{};

    /* behavior::derefable */
    std::function<object_ptr(object_ptr const)> deref{};

    /* behavior::indexable */
    std::function<object_ptr(object_ptr const, object_ptr const)> nth{};
    std::function<object_ptr(object_ptr const, object_ptr const, object_ptr const)> nth_default{};

    /* behavior::nameable */
    std::function<native_persistent_string(object_ptr const)> get_name{};
    std::function<native_persistent_string(object_ptr const)> get_namespace{};

    /* behavior::seqable */
    std::function<object_ptr(object_ptr const)> seq{};
    std::function<object_ptr(object_ptr const)> fresh_seq{};

    /* behavior::metadatable */
    std::function<object_ptr(object_ptr const, object_ptr const)> with_meta{};
    std::function<option<object_ptr>(object_ptr const)> meta{};
    std::function<object_ptr(object_ptr const)> get_meta{}; //convenient, could be a member function
    std::function<object_ptr(object_ptr const, object_ptr const meta_obj)> set_meta{};

    /* behavior::transientable */
    std::function<object_ptr(object_ptr const)> to_transient{};

    /* behavior::persistentable */
    std::function<object_ptr(object_ptr const)> to_persistent{};

    /* behavior::chunk_like */
    std::function<object_ptr(object_ptr const)> chunk_next{};
    std::function<object_ptr(object_ptr const)> chunk_next_in_place{};

    /* behavior::conjable */
    std::function<object_ptr(object_ptr const, object_ptr const)> conj{};

    /* behavior::conjable_in_place */
    std::function<object_ptr(object_ptr const, object_ptr const)> conj_in_place{};

    /* behavior::chunkable */
    std::function<object_ptr(object_ptr const)> chunked_first{};
    std::function<object_ptr(object_ptr const)> chunked_next{};

    /* behavior::function_like */
    std::function<object_ptr(object_ptr const)> call0{};
    std::function<object_ptr(object_ptr const, object_ptr const)> call1{};
    std::function<object_ptr(object_ptr const, object_ptr const, object_ptr const)> call2{};
    std::function<
      object_ptr(object_ptr const, object_ptr const, object_ptr const, object_ptr const)>
      call3{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call4{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call5{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call6{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call7{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call8{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call9{};
    std::function<object_ptr(object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const,
                             object_ptr const)>
      call10{};
    std::function<size_t(object_ptr const)> get_arity_flags{};

    //object_ptr to_runtime_data() const
    //{
    //  return obj::persistent_array_map::create_unique();
    //}
  };

  behaviors object_behaviors(object_ptr type);
}
