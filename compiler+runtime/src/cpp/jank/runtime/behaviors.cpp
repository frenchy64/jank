#include <jank/runtime/object.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/obj/number.hpp>
#include <jank/runtime/obj/cons.hpp>
#include <jank/runtime/obj/persistent_string.hpp>
#include <jank/runtime/obj/keyword.hpp>
#include <jank/runtime/obj/symbol.hpp>
#include <jank/runtime/obj/character.hpp>
#include <jank/runtime/obj/persistent_list.hpp>
#include <jank/runtime/obj/persistent_vector.hpp>
#include <jank/runtime/obj/persistent_hash_set.hpp>
#include <jank/runtime/obj/persistent_sorted_set.hpp>
#include <jank/runtime/obj/persistent_array_map.hpp>
#include <jank/runtime/obj/persistent_array_map_sequence.hpp>
#include <jank/runtime/obj/persistent_hash_map.hpp>
#include <jank/runtime/obj/persistent_hash_map_sequence.hpp>
#include <jank/runtime/obj/persistent_sorted_map.hpp>
#include <jank/runtime/obj/persistent_sorted_map_sequence.hpp>
#include <jank/runtime/obj/transient_hash_map.hpp>
#include <jank/runtime/obj/transient_sorted_map.hpp>
#include <jank/runtime/obj/transient_vector.hpp>
#include <jank/runtime/obj/transient_hash_set.hpp>
#include <jank/runtime/obj/transient_sorted_set.hpp>
#include <jank/runtime/obj/iterator.hpp>
#include <jank/runtime/obj/lazy_sequence.hpp>
#include <jank/runtime/obj/chunk_buffer.hpp>
#include <jank/runtime/obj/array_chunk.hpp>
#include <jank/runtime/obj/chunked_cons.hpp>
#include <jank/runtime/obj/range.hpp>
#include <jank/runtime/obj/integer_range.hpp>
#include <jank/runtime/obj/repeat.hpp>
#include <jank/runtime/obj/ratio.hpp>
#include <jank/runtime/obj/jit_function.hpp>
#include <jank/runtime/obj/jit_closure.hpp>
#include <jank/runtime/obj/multi_function.hpp>
#include <jank/runtime/obj/native_function_wrapper.hpp>
#include <jank/runtime/obj/native_pointer_wrapper.hpp>
#include <jank/runtime/obj/persistent_vector_sequence.hpp>
#include <jank/runtime/obj/persistent_string_sequence.hpp>
#include <jank/runtime/obj/persistent_list_sequence.hpp>
#include <jank/runtime/obj/persistent_hash_set_sequence.hpp>
#include <jank/runtime/obj/persistent_sorted_set_sequence.hpp>
#include <jank/runtime/obj/native_array_sequence.hpp>
#include <jank/runtime/obj/native_vector_sequence.hpp>
#include <jank/runtime/obj/atom.hpp>
#include <jank/runtime/obj/volatile.hpp>
#include <jank/runtime/obj/delay.hpp>
#include <jank/runtime/obj/reduced.hpp>
#include <jank/runtime/obj/tagged_literal.hpp>
#include <jank/runtime/ns.hpp>
#include <jank/runtime/var.hpp>
#include <jank/runtime/rtti.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/behavior/associatively_readable.hpp>
#include <jank/runtime/behavior/associatively_writable.hpp>
#include <jank/runtime/behavior/callable.hpp>
#include <jank/runtime/behavior/chunkable.hpp>
#include <jank/runtime/behavior/collection_like.hpp>
#include <jank/runtime/behavior/conjable.hpp>
#include <jank/runtime/behavior/countable.hpp>
#include <jank/runtime/behavior/derefable.hpp>
#include <jank/runtime/behavior/indexable.hpp>
#include <jank/runtime/behavior/map_like.hpp>
#include <jank/runtime/behavior/metadatable.hpp>
#include <jank/runtime/behavior/nameable.hpp>
#include <jank/runtime/behavior/seqable.hpp>
#include <jank/runtime/behavior/sequential.hpp>
#include <jank/runtime/behavior/set_like.hpp>
#include <jank/runtime/behavior/stackable.hpp>
#include <jank/runtime/behavior/transientable.hpp>
#include <jank/runtime/visit.hpp>
#include <jank/runtime/behaviors.hpp>

namespace jank::runtime
{
  template <typename T>
  requires behavior::object_like<T>
  behaviors::behaviors(native_box<T>)
  {
    if constexpr(behavior::object_like<T>)
    {
      this->is_object_like = true;
      this->to_string = [](object_ptr const o) { return try_object<T>(o)->to_string(); };
      this->to_code_string
        = [](object_ptr const o) { return try_object<T>(o)->to_code_string(); };
      this->to_hash = [](object_ptr const o) { return try_object<T>(o)->to_hash(); };
      this->equal = [](object_ptr const lhs, object_ptr const rhs) {
        return try_object<T>(lhs)->equal(*rhs);
      };
    }
    if constexpr(behavior::seqable<T>)
    {
      this->is_seqable = true;
      this->seq = [](object_ptr const o) { return try_object<T>(o)->seq(); };
      this->fresh_seq = [](object_ptr const o) { return try_object<T>(o)->fresh_seq(); };
    }
    if constexpr(behavior::sequenceable<T>)
    {
      this->is_sequenceable = true;
      this->first = [](object_ptr const o) { return try_object<T>(o)->first(); };
      this->next = [](object_ptr const o) { return try_object<T>(o)->next(); };
    }
    if constexpr(behavior::sequenceable_in_place<T>)
    {
      this->is_sequenceable_in_place = true;
      this->next_in_place = [](object_ptr const o) { return try_object<T>(o)->next_in_place(); };
    }
    if constexpr(behavior::collection_like<T>)
    {
      this->is_collection = true;
    }
    if constexpr(behavior::associatively_readable<T> && behavior::associatively_writable<T>)
    {
      this->is_associative = true;
      this->is_associatively_writable = true;
      this->is_associatively_readable = true;
      this->assoc = [](object_ptr const m, object_ptr const k, object_ptr const v) {
        return try_object<T>(m)->assoc(k, v);
      };
      this->dissoc
        = [](object_ptr const m, object_ptr const k) { return try_object<T>(m)->dissoc(k); };
      this->get = [](object_ptr const m, object_ptr const k) { return try_object<T>(m)->get(k); };
      this->get_default = [](object_ptr const m, object_ptr const k, object_ptr const d) {
        return try_object<T>(m)->get(k, d);
      };
      this->get_entry
        = [](object_ptr const m, object_ptr const k) { return try_object<T>(m)->get_entry(k); };
    }
    if constexpr(behavior::associatively_writable_in_place<T>)
    {
      this->is_associatively_writable_in_place = true;
      this->assoc_in_place = [](object_ptr const m, object_ptr const k, object_ptr const v) {
        return try_object<T>(m)->assoc_in_place(k, v);
      };
      this->dissoc_in_place = [](object_ptr const m, object_ptr const k) {
        return try_object<T>(m)->dissoc_in_place(k);
      };
    }
    if constexpr(behavior::countable<T>)
    {
      this->is_countable = true;
    }
    if constexpr(behavior::transientable<T>)
    {
      this->is_transientable = true;
      this->to_transient = [](object_ptr const o) { return try_object<T>(o)->to_transient(); };
    }
    if constexpr(behavior::persistentable<T>)
    {
      this->is_persistentable = true;
      this->to_persistent = [](object_ptr const o) { return try_object<T>(o)->to_persistent(); };
    }
    if constexpr(behavior::chunk_like<T>)
    {
      this->is_chunk_like = true;
      this->chunk_next = [](object_ptr const o) { return try_object<T>(o)->chunk_next(); };
      this->chunk_next_in_place
        = [](object_ptr const o) { return try_object<T>(o)->chunk_next_in_place(); };
    }
    if constexpr(behavior::chunkable<T>)
    {
      this->is_chunkable = true;
      this->chunked_first = [](object_ptr const o) { return try_object<T>(o)->chunked_first(); };
      this->chunked_next = [](object_ptr const o) { return try_object<T>(o)->chunked_next(); };
    }
    if constexpr(behavior::metadatable<T>)
    {
      this->is_metadatable = true;
      this->with_meta
        = [](object_ptr const o, object_ptr const m) { return try_object<T>(o)->with_meta(m); };
      this->get_meta = [](object_ptr const o) {
        return try_object<T>(o)->meta.unwrap_or(obj::nil::nil_const());
      };
      this->set_meta = [](object_ptr const o, object_ptr const meta_obj) {
        try_object<T>(o)->meta = behavior::detail::validate_meta(meta_obj);
        return meta_obj;
      };
    }
    if constexpr(std::is_base_of_v<behavior::callable, T>)
    {
      this->is_callable = true;
    }
    if constexpr(behavior::function_like<T>)
    {
      this->is_function_like = true;
      //TODO start arguments from 1 instead of 0
      this->call0 = [](object_ptr const o) { return try_object<T>(o)->call(); };
      this->call1
        = [](object_ptr const o, object_ptr const a0) { return try_object<T>(o)->call(a0); };
      this->call2 = [](object_ptr const o, object_ptr const a0, object_ptr const a1) {
        return try_object<T>(o)->call(a0, a1);
      };
      this->call3
        = [](object_ptr const o, object_ptr const a0, object_ptr const a1, object_ptr const a2) {
            return try_object<T>(o)->call(a0, a1, a2);
          };
      this->call4 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3) { return try_object<T>(o)->call(a0, a1, a2, a3); };
      this->call5
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4) { return try_object<T>(o)->call(a0, a1, a2, a3, a4); };
      this->call6
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4,
             object_ptr const a5) { return try_object<T>(o)->call(a0, a1, a2, a3, a4, a5); };
      this->call7
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4,
             object_ptr const a5,
             object_ptr const a6) { return try_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6); };
      this->call8 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3,
                       object_ptr const a4,
                       object_ptr const a5,
                       object_ptr const a6,
                       object_ptr const a7) {
        return try_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6, a7);
      };
      this->call9 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3,
                       object_ptr const a4,
                       object_ptr const a5,
                       object_ptr const a6,
                       object_ptr const a7,
                       object_ptr const a8) {
        return try_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6, a7, a8);
      };
      this->call10 = [](object_ptr const o,
                        object_ptr const a0,
                        object_ptr const a1,
                        object_ptr const a2,
                        object_ptr const a3,
                        object_ptr const a4,
                        object_ptr const a5,
                        object_ptr const a6,
                        object_ptr const a7,
                        object_ptr const a8,
                        object_ptr const a9) {
        return try_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      };
      this->get_arity_flags
        = [](object_ptr const o) { return try_object<T>(o)->get_arity_flags(); };
    }
    //TODO test for call1 arity
    if constexpr(std::same_as<T, obj::persistent_hash_set>
                 || std::same_as<T, obj::persistent_hash_map>
                 || std::same_as<T, obj::persistent_array_map>
                 || std::same_as<T, obj::transient_vector>
                 || std::same_as<T, obj::transient_hash_set> || std::same_as<T, obj::keyword>)
    {
      this->call1
        = [](object_ptr const o, object_ptr const a0) { return try_object<T>(o)->call(a0); };
    }
    //TODO test for call2 arity
    if constexpr(std::same_as<T, obj::persistent_hash_map>
                 || std::same_as<T, obj::persistent_array_map>
                 || std::same_as<T, obj::transient_hash_set> || std::same_as<T, obj::keyword>)
    {
      this->call2 = [](object_ptr const o, object_ptr const a0, object_ptr const a1) {
        return try_object<T>(o)->call(a0, a1);
      };
    }
    if constexpr(behavior::nameable<T>)
    {
      this->is_named = true;
      this->get_name = [](object_ptr const o) { return try_object<T>(o)->get_name(); };
      this->get_namespace = [](object_ptr const o) { return try_object<T>(o)->get_namespace(); };
    }
    if constexpr(behavior::derefable<T>)
    {
      this->is_derefable = true;
      this->deref = [](object_ptr const o) { return try_object<T>(o)->deref(); };
    }
    if constexpr(behavior::indexable<T>)
    {
      this->is_indexable = true;
      this->nth = [](object_ptr const o, object_ptr const i) { return try_object<T>(o)->nth(i); };
      this->nth_default = [](object_ptr const o, object_ptr const i, object_ptr const d) {
        return try_object<T>(o)->nth(i, d);
      };
    }
    if constexpr(behavior::map_like<T>)
    {
      this->is_map = true;
    }
    if constexpr(behavior::set_like<T>)
    {
      this->is_set = true;
    }
    if constexpr(behavior::set_like<T> || behavior::associatively_readable<T>)
    {
      this->contains
        = [](object_ptr const m, object_ptr const k) { return try_object<T>(m)->contains(k); };
    }
    if constexpr(behavior::conjable<T>)
    {
      this->is_conjable = true;
      this->conj
        = [](object_ptr const o, object_ptr const v) { return try_object<T>(o)->conj(v); };
    }
    if constexpr(behavior::conjable_in_place<T>)
    {
      this->is_conjable_in_place = true;
      this->conj_in_place = [](object_ptr const o, object_ptr const v) {
        return try_object<T>(o)->conj_in_place(v);
      };
    }
  }

  behaviors object_behaviors(object_ptr type)
  {
    return visit_object(
      [](auto const typed_o) -> behaviors {
        static behaviors const bs{ typed_o };
        return bs;
      },
      type);
  }
}
