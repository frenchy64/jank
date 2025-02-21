#include <jank/runtime/behaviors.hpp>

#include <jank/runtime/visit.hpp>
#include <jank/runtime/rtti.hpp>
#include <jank/runtime/convert.hpp>
#include <jank/runtime/behavior/associatively_readable.hpp>
#include <jank/runtime/behavior/associatively_writable.hpp>
#include <jank/runtime/behavior/callable.hpp>
#include <jank/runtime/behavior/comparable.hpp>
#include <jank/runtime/behavior/chunkable.hpp>
#include <jank/runtime/behavior/collection_like.hpp>
#include <jank/runtime/behavior/countable.hpp>
#include <jank/runtime/behavior/derefable.hpp>
#include <jank/runtime/behavior/indexable.hpp>
#include <jank/runtime/behavior/map_like.hpp>
#include <jank/runtime/behavior/metadatable.hpp>
#include <jank/runtime/behavior/nameable.hpp>
#include <jank/runtime/behavior/number_like.hpp>
#include <jank/runtime/behavior/sequential.hpp>
#include <jank/runtime/behavior/set_like.hpp>
#include <jank/runtime/behavior/stackable.hpp>
#include <jank/runtime/behavior/transientable.hpp>

namespace jank::runtime
{
  /* wrap an object */
  template <typename T>
  requires behavior::object_like<T>
  object_behaviors::object_behaviors(native_box<T>)
  {
    if constexpr(behavior::object_like<T>)
    {
      this->is_object_like = true;
      this->to_string = [](object_ptr const o) { return expect_object<T>(o)->to_string(); };
      this->to_string_builder = [](object_ptr const o, util::string_builder &buff) {
        return expect_object<T>(o)->to_string(buff);
      };
      this->to_code_string = [](object_ptr const o) { return expect_object<T>(o)->to_code_string(); };
      this->to_hash = [](object_ptr const o) { return expect_object<T>(o)->to_hash(); };
      this->equal = [](object_ptr const lhs, object_ptr const rhs) {
        return expect_object<T>(lhs)->equal(*rhs);
      };
      this->base = [](object_ptr const o) { return expect_object<T>(o)->base; };
    }
    if constexpr(behavior::seqable<T>)
    {
      this->is_seqable = true;
      this->seq = [](object_ptr const o) { return expect_object<T>(o)->seq(); };
      this->fresh_seq = [](object_ptr const o) { return expect_object<T>(o)->fresh_seq(); };
    }
    if constexpr(behavior::sequential<T>)
    {
      this->is_sequential = true;
    }
    if constexpr(behavior::sequenceable<T>)
    {
      this->is_sequenceable = true;
      this->first = [](object_ptr const o) { return expect_object<T>(o)->first(); };
      this->next = [](object_ptr const o) { return expect_object<T>(o)->next(); };
    }
    if constexpr(behavior::sequenceable_in_place<T>)
    {
      this->is_sequenceable_in_place = true;
      this->next_in_place = [](object_ptr const o) { return expect_object<T>(o)->next_in_place(); };
    }
    if constexpr(behavior::collection_like<T>)
    {
      this->is_collection = true;
      this->empty = [](object_ptr const c) { return expect_object<T>(c)->create_empty(); };
    }
    if constexpr(behavior::associatively_readable<T> && behavior::associatively_writable<T>)
    {
      this->is_associative = true;
    }
    if constexpr(behavior::associatively_writable<T>)
    {
      this->is_associatively_writable = true;
      this->assoc = [](object_ptr const m, object_ptr const k, object_ptr const v) {
        return expect_object<T>(m)->assoc(k, v);
      };
      this->dissoc
        = [](object_ptr const m, object_ptr const k) { return expect_object<T>(m)->dissoc(k); };
    }
    if constexpr(behavior::associatively_readable<T>)
    {
      this->is_associatively_readable = true;
      this->get = [](object_ptr const m, object_ptr const k) { return expect_object<T>(m)->get(k); };
      this->get_default = [](object_ptr const m, object_ptr const k, object_ptr const d) {
        return expect_object<T>(m)->get(k, d);
      };
      this->get_entry
        = [](object_ptr const m, object_ptr const k) { return expect_object<T>(m)->get_entry(k); };
    }
    if constexpr(behavior::associatively_writable_in_place<T>)
    {
      this->is_associatively_writable_in_place = true;
      this->assoc_in_place = [](object_ptr const m, object_ptr const k, object_ptr const v) {
        return expect_object<T>(m)->assoc_in_place(k, v);
      };
      this->dissoc_in_place = [](object_ptr const m, object_ptr const k) {
        return expect_object<T>(m)->dissoc_in_place(k);
      };
    }
    if constexpr(behavior::countable<T>)
    {
      this->is_countable = true;
      this->count = [](object_ptr const o) { return expect_object<T>(o)->count(); };
    }
    if constexpr(behavior::transientable<T>)
    {
      this->is_transientable = true;
      this->to_transient = [](object_ptr const o) { return expect_object<T>(o)->to_transient(); };
    }
    if constexpr(behavior::persistentable<T>)
    {
      this->is_persistentable = true;
      this->to_persistent = [](object_ptr const o) { return expect_object<T>(o)->to_persistent(); };
    }
    if constexpr(behavior::chunk_like<T>)
    {
      this->is_chunk_like = true;
      this->chunk_next = [](object_ptr const o) { return expect_object<T>(o)->chunk_next(); };
      this->chunk_next_in_place
        = [](object_ptr const o) { return expect_object<T>(o)->chunk_next_in_place(); };
    }
    if constexpr(behavior::chunkable<T>)
    {
      this->is_chunkable = true;
      this->chunked_first = [](object_ptr const o) { return expect_object<T>(o)->chunked_first(); };
      this->chunked_next = [](object_ptr const o) { return expect_object<T>(o)->chunked_next(); };
    }
    if constexpr(behavior::metadatable<T>)
    {
      this->is_metadatable = true;
      this->with_meta
        = [](object_ptr const o, object_ptr const m) { return expect_object<T>(o)->with_meta(m); };
      this->meta = [](object_ptr const o) { return expect_object<T>(o)->meta; };
      this->get_meta = [](object_ptr const o) {
        return expect_object<T>(o)->meta.unwrap_or(obj::nil::nil_const());
      };
      this->set_meta = [](object_ptr const o, object_ptr const meta_obj) {
        expect_object<T>(o)->meta = behavior::detail::validate_meta(meta_obj);
        return meta_obj;
      };
    }
    if constexpr(std::is_base_of_v<behavior::callable, T>)
    {
      this->is_callable = true;
    }
    if constexpr(behavior::comparable<T>)
    {
      this->is_comparable = true;
      this->compare = [](object_ptr const lhs, object_ptr const rhs) {
        return expect_object<T>(lhs)->compare(*rhs);
      };
    }
    if constexpr(behavior::function_like<T>)
    {
      this->is_function_like = true;
      //TODO start arguments from 1 instead of 0
      this->call0 = [](object_ptr const o) { return expect_object<T>(o)->call(); };
      this->call1
        = [](object_ptr const o, object_ptr const a0) { return expect_object<T>(o)->call(a0); };
      this->call2 = [](object_ptr const o, object_ptr const a0, object_ptr const a1) {
        return expect_object<T>(o)->call(a0, a1);
      };
      this->call3
        = [](object_ptr const o, object_ptr const a0, object_ptr const a1, object_ptr const a2) {
            return expect_object<T>(o)->call(a0, a1, a2);
          };
      this->call4 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3) { return expect_object<T>(o)->call(a0, a1, a2, a3); };
      this->call5 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3,
                       object_ptr const a4) { return expect_object<T>(o)->call(a0, a1, a2, a3, a4); };
      this->call6
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4,
             object_ptr const a5) { return expect_object<T>(o)->call(a0, a1, a2, a3, a4, a5); };
      this->call7
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4,
             object_ptr const a5,
             object_ptr const a6) { return expect_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6); };
      this->call8 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3,
                       object_ptr const a4,
                       object_ptr const a5,
                       object_ptr const a6,
                       object_ptr const a7) {
        return expect_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6, a7);
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
        return expect_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6, a7, a8);
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
        return expect_object<T>(o)->call(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      };
      this->get_arity_flags
        = [](object_ptr const o) { return expect_object<T>(o)->get_arity_flags(); };
    }
    //TODO test for call1 arity
    if constexpr(std::same_as<T, obj::persistent_hash_set>
                 || std::same_as<T, obj::persistent_hash_map>
                 || std::same_as<T, obj::persistent_array_map>
                 || std::same_as<T, obj::transient_vector>
                 || std::same_as<T, obj::transient_hash_set> || std::same_as<T, obj::keyword>)
    {
      this->call1
        = [](object_ptr const o, object_ptr const a0) { return expect_object<T>(o)->call(a0); };
    }
    //TODO test for call2 arity
    if constexpr(std::same_as<T, obj::persistent_hash_map>
                 || std::same_as<T, obj::persistent_array_map>
                 || std::same_as<T, obj::transient_hash_set> || std::same_as<T, obj::keyword>)
    {
      this->call2 = [](object_ptr const o, object_ptr const a0, object_ptr const a1) {
        return expect_object<T>(o)->call(a0, a1);
      };
    }
    if constexpr(behavior::nameable<T>)
    {
      this->is_named = true;
      this->get_name = [](object_ptr const o) { return expect_object<T>(o)->get_name(); };
      this->get_namespace = [](object_ptr const o) { return expect_object<T>(o)->get_namespace(); };
    }
    if constexpr(behavior::derefable<T>)
    {
      this->is_derefable = true;
      this->deref = [](object_ptr const o) { return expect_object<T>(o)->deref(); };
    }
    if constexpr(behavior::indexable<T>)
    {
      this->is_indexable = true;
      this->nth = [](object_ptr const o, object_ptr const i) { return expect_object<T>(o)->nth(i); };
      this->nth_default = [](object_ptr const o, object_ptr const i, object_ptr const d) {
        return expect_object<T>(o)->nth(i, d);
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
    if constexpr(behavior::stackable<T>)
    {
      this->is_stackable = true;
      this->peek = [](object_ptr const o) { return expect_object<T>(o)->peek(); };
      this->pop = [](object_ptr const o) { return expect_object<T>(o)->pop(); };
    }
    if constexpr(behavior::number_like<T>)
    {
      this->is_number_like = true;
      this->to_integer = [](object_ptr const o) { return expect_object<T>(o)->to_integer(); };
      this->to_real = [](object_ptr const o) { return expect_object<T>(o)->to_real(); };
    }
    if constexpr(behavior::set_like<T> || behavior::associatively_readable<T>)
    {
      this->contains
        = [](object_ptr const m, object_ptr const k) { return expect_object<T>(m)->contains(k); };
    }
    if constexpr(behavior::conjable<T>)
    {
      this->is_conjable = true;
      this->conj = [](object_ptr const o, object_ptr const v) { return expect_object<T>(o)->conj(v); };
    }
    if constexpr(behavior::conjable_in_place<T>)
    {
      this->is_conjable_in_place = true;
      this->conj_in_place
        = [](object_ptr const o, object_ptr const v) { return expect_object<T>(o)->conj_in_place(v); };
    }
  }

  /* wrap a persistent map
   *
   * {:object_like {:to_string (fn [this] ...)
   *                :to_code_string (fn [this] ...)
   *                :to_hash (fn [this] ...)
   *                :equal (fn [this that] ...)}
   *  :seqable {:seq (fn [this] ...)
   *            :fresh_seq (fn [this] ...)}
   *  :sequential {:first (fn [this] ...)
   *               :next (fn [this] ..)}
   *  :sequential_in_place {:next_in_place (fn [this] ..)}
   *  :associatively_writeable {:assoc (fn [this k v] ..)
   *                            :dissoc (fn [this k] ..)}
   *  :associatively_readable {:get (fn [this k] ..)
   *                           :get_default (fn [this k fallback] ..)
   *                           :get_entry (fn [this k] ..)
   *                           :contains (fn [this k] ..)}
   *  :associatively_writeable_in_place {:assoc_in_place (fn [this k v] ..)
   *                                     :dissoc_in_place (fn [this k] ..)}
   *  :countable {:count (fn [this] ..)}
   *  :transientable {:to_transient (fn [this] ..)}
   *  :persistentable {:to_persistent (fn [this] ..)}
   *  :chunk_like {:chunk_next (fn [this] ..)}
   *  :chunkable {:chunked_first (fn [this] ..)
   *              :chunked_next (fn [this] ..)}
   *  :metadatable {:with_meta (fn [this m] ..)
   *                :get_meta (fn [this] ..)
   *                :set_meta (fn [this m] ..)}
   *  :callable {:call0 (fn [this])
   *             :call1 (fn [this a0])
   *             :call2 (fn [this a0 a1])
   *             :call3 (fn [this a0 a1 a2])
   *             :call4 (fn [this a0 a1 a2 a3])
   *             :call5 (fn [this a0 a1 a2 a3 a4])
   *             :call6 (fn [this a0 a1 a2 a3 a4 a5])
   *             :call7 (fn [this a0 a1 a2 a3 a4 a5 a6])
   *             :call8 (fn [this a0 a1 a2 a3 a4 a5 a6 a7])
   *             :call9 (fn [this a0 a1 a2 a3 a4 a5 a6 a7 a8])
   *             :call10 (fn [this a0 a1 a2 a3 a4 a5 a6 a7 a8 a9])
   *             :get_arity_flags (fn [..])} ;TODO
   *  :comparable {:compare (fn [this that] ...)}
   *  :nameable {:get_name (fn [this])
   *             :get_namespace (fn [this])}
   *  :derefable {:deref (fn [this])}
   *  :indexable {:nth (fn [this i])
   *              :nth_default (fn [this i d])}
   *  :map_like {}
   *  :set_like {}
   *  :stackable {:peek (fn [this])
   *              :pop (fn [this])}
   *  :number_like {:to_integer (fn [this])
   *                :to_real (fn [this])}
   *  :conjable {:conj (fn [this v])}
   *  :conjable_in_place {:conj_in_place (fn [this v])}}
   *  */
  object_behaviors::object_behaviors(persistent_hash_map * const mp)
  {
    auto const object_like(runtime::get(m, __rt_ctx->intern_keyword("object_like").expect_ok()));
    if(truthy(object_like))
    {
      auto const to_string(runtime::get(object_like, __rt_ctx->intern_keyword("to_string").expect_ok()));
      auto const to_code_string(runtime::get(object_like, __rt_ctx->intern_keyword("to_code_string").expect_ok()));
      auto const to_hash(runtime::get(object_like, __rt_ctx->intern_keyword("to_hash").expect_ok()));
      auto const equal(runtime::get(object_like, __rt_ctx->intern_keyword("equal").expect_ok()));
      this->is_object_like = true;
      this->to_string = [&](object_ptr const o) { return dynamic_call(to_string, o); };
      this->to_string_builder = [&](object_ptr const o, util::string_builder &buff) {
        runtime::to_code_string(to_string(o), buff);
        return buff.release;
      };
      this->to_code_string = [&](object_ptr const o) { return dynamic_call(to_code_string, o); };
      this->to_hash = [&](object_ptr const o) { return dynamic_call(to_hash, o); };
      this->equal = [&](object_ptr const lhs, object const rhs) {
        return dynamic_call(equal, lhs, make_box(rhs));
      };
      //this->base = [&](object_ptr const o) { return "dynamic"; };
    }
    auto const seqable(runtime::get(m, __rt_ctx->intern_keyword("seqable").expect_ok()));
    if(truthy(seqable))
    {
      auto const seq(runtime::get(seqable, __rt_ctx->intern_keyword("seq").expect_ok()));
      auto const fresh_seq(runtime::get(seqable, __rt_ctx->intern_keyword("fresh_seq").expect_ok()));
      this->is_seqable = true;
      this->seq = [&](object_ptr const o) {
        auto const s(dynamic_call(seq, o));
        if(is_nil(s))
        {
          return nullptr;
        }
        return s;
      };
      this->fresh_seq = [](object_ptr const o) {
        auto const s(dynamic_call(fresh_seq, o));
        if(is_nil(s))
        {
          return nullptr;
        }
        return s;
      };
    }
    auto const sequential(runtime::get(m, __rt_ctx->intern_keyword("sequential").expect_ok()));
    if(truthy(sequential))
    {
      this->is_sequential = true;
    }
    auto const sequenceable(runtime::get(m, __rt_ctx->intern_keyword("sequential").expect_ok()));
    if(truthy(sequenceable))
    {
      auto const first(runtime::get(sequenceable, __rt_ctx->intern_keyword("first").expect_ok()));
      auto const next(runtime::get(sequenceable, __rt_ctx->intern_keyword("next").expect_ok()));
      this->is_sequenceable = true;
      this->first = [&](object_ptr const o) { return dynamic_call(first, o); };
      this->next = [&](object_ptr const o) {
        auto const s(dynamic_call(next, o));
        if(is_nil(s))
        {
          return nullptr;
        }
        return s;
      };
    }
    auto const sequenceable_in_place(runtime::get(m, __rt_ctx->intern_keyword("sequential").expect_ok()));
    if(truthy(sequenceable_in_place))
    {
      auto const next_in_place(runtime::get(sequenceable_in_place, __rt_ctx->intern_keyword("next_in_place").expect_ok()));
      this->is_sequenceable_in_place = true;
      this->next_in_place = [&](object_ptr const o) {
        auto const s(dynamic_call(next, o));
        if(is_nil(s))
        {
          return nullptr;
        }
        return s;
      };
    }
    auto const collection_like(runtime::get(m, __rt_ctx->intern_keyword("collection_like").expect_ok()));
    if(truthy(collection_like))
    {
      auto const empty(runtime::get(collection_like, __rt_ctx->intern_keyword("empty").expect_ok()));
      this->is_collection = true;
      this->empty = [&](object_ptr const c) { return dynamic_call(empty, o); };
    }
    auto const associatively_readable(runtime::get(m, __rt_ctx->intern_keyword("associatively_readable").expect_ok()));
    auto const associatively_writeable(runtime::get(m, __rt_ctx->intern_keyword("associatively_writeable").expect_ok()));
    if(truthy(associatively_readable) && truthy(associatively_writeable))
    {
      this->is_associative = true;
    }
    if(truthy(associatively_writeable))
    {
      auto const assoc(runtime::get(associatively_writeable, __rt_ctx->intern_keyword("assoc").expect_ok()));
      auto const dissoc(runtime::get(associatively_writeable, __rt_ctx->intern_keyword("dissoc").expect_ok()));
      this->is_associatively_writable = true;
      this->assoc = [&](object_ptr const m, object_ptr const k, object_ptr const v) {
        return dynamic_call(assoc, m, k v);
      };
      this->dissoc
        = [&](object_ptr const m, object_ptr const k) {
          return dynamic_call(dissoc, m, k);
        };
    }
    if(truthy(associatively_readable)
    {
      auto const get(runtime::get(associatively_readable, __rt_ctx->intern_keyword("get").expect_ok()));
      auto const get_default(runtime::get(associatively_readable, __rt_ctx->intern_keyword("get_default").expect_ok()));
      auto const get_entry(runtime::get(associatively_readable, __rt_ctx->intern_keyword("get_entry").expect_ok()));
      this->is_associatively_readable = true;
      this->get = [&](object_ptr const m, object_ptr const k) { return dynamic_call(get, m, k); };
      this->get_default = [&](object_ptr const m, object_ptr const k, object_ptr const d) {
        return dynamic_call(get_default, m, k, d);
      };
      this->get_entry
        = [&](object_ptr const m, object_ptr const k) {
        return dynamic_call(get_entry, m, k);
      };
    }
    auto const associatively_writeable_in_place(runtime::get(m, __rt_ctx->intern_keyword("associatively_writeable_in_place").expect_ok()));
    if(truthy(associatively_writeable_in_place))
    {
      auto const assoc_in_place(runtime::get(associatively_writeable_in_place, __rt_ctx->intern_keyword("assoc_in_place").expect_ok()));
      auto const dissoc_in_place(runtime::get(associatively_writeable_in_place, __rt_ctx->intern_keyword("dissoc_in_place").expect_ok()));
      this->is_associatively_writable_in_place = true;
      this->assoc_in_place = [&](object_ptr const m, object_ptr const k, object_ptr const v) {
        return dynamic_call(assoc_in_place, m, k, v);
      };
      this->dissoc_in_place = [&](object_ptr const m, object_ptr const k) {
        return dynamic_call(dissoc_in_place, m, k);
      };
    }
    auto const countable(runtime::get(m, __rt_ctx->intern_keyword("countable").expect_ok()));
    if(truthy(countable))
    {
      auto const count(runtime::get(countable, __rt_ctx->intern_keyword("count").expect_ok()));
      this->is_countable = true;
      this->count = [](object_ptr const o) { return dynamic_call(count, o); };
    }
    auto const transientable(runtime::get(m, __rt_ctx->intern_keyword("transientable").expect_ok()));
    if(truthy(transientable))
    {
      auto const to_transient(runtime::get(transientable, __rt_ctx->intern_keyword("to_transient").expect_ok()));
      this->is_transientable = true;
      this->to_transient = [&](object_ptr const o) { return dynamic_call(to_transient, o) };
    }
    auto const persistentable(runtime::get(m, __rt_ctx->intern_keyword("persistentable").expect_ok()));
    if(truthy(persistentable))
    {
      auto const to_persistent(runtime::get(persistentable, __rt_ctx->intern_keyword("to_persistent").expect_ok()));
      this->is_persistentable = true;
      this->to_persistent = [&](object_ptr const o) { return dynamic_call(to_persistent, o); };
    }
    auto const chunk_like(runtime::get(m, __rt_ctx->intern_keyword("chunk_like").expect_ok()));
    if(truthy(chunk_like))
    {
      auto const chunk_next(runtime::get(chunk_like, __rt_ctx->intern_keyword("chunk_next").expect_ok()));
      auto const chunk_next_in_place(runtime::get(chunk_like, __rt_ctx->intern_keyword("chunk_next_in_place").expect_ok()));
      this->is_chunk_like = true;
      this->chunk_next = [&](object_ptr const o) { return dynamic_call(chunk_next, o); };
      this->chunk_next_in_place
        = [](object_ptr const o) { return dynamic_call(chunk_next_in_place, o); };
    }
    auto const chunkable(runtime::get(m, __rt_ctx->intern_keyword("chunkable").expect_ok()));
    if(truthy(chunkable))
    {
      auto const chunked_first(runtime::get(chunkable, __rt_ctx->intern_keyword("chunked_first").expect_ok()));
      auto const chunked_next(runtime::get(chunkable, __rt_ctx->intern_keyword("chunked_next").expect_ok()));
      this->is_chunkable = true;
      this->chunked_first = [](object_ptr const o) { return dynamic_call(chunked_first, o); };
      this->chunked_next = [](object_ptr const o) { return dynamic_call(chunked_next, o); };
    }
    auto const metadatable(runtime::get(m, __rt_ctx->intern_keyword("metadatable").expect_ok()));
    if(truthy(metadatable))
    {
      auto const with_meta(runtime::get(metadatable, __rt_ctx->intern_keyword("with_meta").expect_ok()));
      auto const get_meta(runtime::get(metadatable, __rt_ctx->intern_keyword("get_meta").expect_ok()));
      auto const set_meta(runtime::get(metadatable, __rt_ctx->intern_keyword("set_meta").expect_ok()));
      this->is_metadatable = true;
      this->with_meta
        = [&](object_ptr const o, object_ptr const m) { return dynamic_call(with_meta, o, m); };
      //this->meta = [](object_ptr const o) { return expect_object<T>(o)->meta; };
      this->get_meta = [&](object_ptr const o) {
        return dynamic_call(get_meta, o);
      };
      this->set_meta = [&](object_ptr const o, object_ptr const meta_obj) {
        return dynamic_call(set_meta, o, meta_obj);
      };
    }
    auto const callable(runtime::get(m, __rt_ctx->intern_keyword("callable").expect_ok()));
    if(truthy(callable))
    {
      this->is_callable = true;
    }
    auto const comparable(runtime::get(m, __rt_ctx->intern_keyword("comparable").expect_ok()));
    if(truthy(comparable))
    {
      auto const compare(runtime::get(comparable, __rt_ctx->intern_keyword("compare").expect_ok()));
      this->is_comparable = true;
      this->compare = [](object_ptr const lhs, object const rhs) {
        return dynamic_call(compare, lhs, make_box(rhs));
      };
    }
    auto const function_like(runtime::get(m, __rt_ctx->intern_keyword("function_like").expect_ok()));
    if(behavior::function_like<T>)
    {
      auto const call0(runtime::get(comparable, __rt_ctx->intern_keyword("call0").expect_ok()));
      auto const call1(runtime::get(comparable, __rt_ctx->intern_keyword("call1").expect_ok()));
      auto const call2(runtime::get(comparable, __rt_ctx->intern_keyword("call2").expect_ok()));
      auto const call3(runtime::get(comparable, __rt_ctx->intern_keyword("call3").expect_ok()));
      auto const call4(runtime::get(comparable, __rt_ctx->intern_keyword("call4").expect_ok()));
      auto const call5(runtime::get(comparable, __rt_ctx->intern_keyword("call5").expect_ok()));
      auto const call6(runtime::get(comparable, __rt_ctx->intern_keyword("call6").expect_ok()));
      auto const call7(runtime::get(comparable, __rt_ctx->intern_keyword("call7").expect_ok()));
      auto const call8(runtime::get(comparable, __rt_ctx->intern_keyword("call8").expect_ok()));
      auto const call9(runtime::get(comparable, __rt_ctx->intern_keyword("call9").expect_ok()));
      auto const call10(runtime::get(comparable, __rt_ctx->intern_keyword("call10").expect_ok()));
      auto const get_arity_flags(runtime::get(comparable, __rt_ctx->intern_keyword("get_arity_flags").expect_ok()));
      this->is_function_like = true;
      //TODO start arguments from 1 instead of 0
      this->call0 = [](object_ptr const o) { return dynamic_call(call0, o); };
      this->call1
        = [](object_ptr const o, object_ptr const a0) { return dynamic_call(call1, o, a0); };
      this->call2 = [](object_ptr const o, object_ptr const a0, object_ptr const a1) {
        return dynamic_call(call2, o, a0, a1);
      };
      this->call3
        = [](object_ptr const o, object_ptr const a0, object_ptr const a1, object_ptr const a2) {
          return dynamic_call(call3, o, a0, a1, a2);
          };
      this->call4 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3) {

        return dynamic_call(call4, o, a0, a1, a2, a3);
      };
      this->call5 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3,
                       object_ptr const a4) {
        return dynamic_call(call5, o, a0, a1, a2, a3, a4);
      };
      this->call6
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4,
             object_ptr const a5) {
          return dynamic_call(call6, o, a0, a1, a2, a3, a4, a5);
        };
      this->call7
        = [](object_ptr const o,
             object_ptr const a0,
             object_ptr const a1,
             object_ptr const a2,
             object_ptr const a3,
             object_ptr const a4,
             object_ptr const a5,
             object_ptr const a6) {
          return dynamic_call(call7, o, a0, a1, a2, a3, a4, a5, a6);
        };
      this->call8 = [](object_ptr const o,
                       object_ptr const a0,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const a3,
                       object_ptr const a4,
                       object_ptr const a5,
                       object_ptr const a6,
                       object_ptr const a7) {
        return dynamic_call(call8, o, a0, a1, a2, a3, a4, a5, a6, a7);
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
        return dynamic_call(call9, o, a0, a1, a2, a3, a4, a5, a6, a7, a8);
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
        return dynamic_call(call10, o, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      };
      this->get_arity_flags
        = [](object_ptr const o) { throw "TODO get_arity_flags" };
    }

    auto const nameable(runtime::get(m, __rt_ctx->intern_keyword("nameable").expect_ok()));
    if(truthy(nameable))
    {
      auto const get_name(runtime::get(nameable, __rt_ctx->intern_keyword("get_name").expect_ok()));
      auto const get_namespace(runtime::get(nameable, __rt_ctx->intern_keyword("get_namespace").expect_ok()));
      this->is_named = true;
      this->get_name = [&](object_ptr const o) { return dynamic_call(get_name, o); };
      this->get_namespace = [&](object_ptr const o) { return dynamic_call(get_namespace, o); };
    }
    auto const derefable(runtime::get(m, __rt_ctx->intern_keyword("derefable").expect_ok()));
    if(truthy(derefable))
    {
      auto const deref(runtime::get(derefable, __rt_ctx->intern_keyword("deref").expect_ok()));
      this->is_derefable = true;
      this->deref = [&](object_ptr const o) { return dynamic_call(deref, o); };
    }
    auto const indexable(runtime::get(m, __rt_ctx->intern_keyword("indexable").expect_ok()));
    if(truthy(indexable))
    {
      auto const nth(runtime::get(indexable, __rt_ctx->intern_keyword("nth").expect_ok()));
      this->is_indexable = true;
      this->nth = [](object_ptr const o, object_ptr const i) { return dynamic_call(nth, o, i); };
      this->nth_default = [](object_ptr const o, object_ptr const i, object_ptr const d) {
        return dynamic_call(nth_default, o, i, d);
      };
    }
    auto const map_like(runtime::get(m, __rt_ctx->intern_keyword("map_like").expect_ok()));
    if(truthy(map_like))
    {
      this->is_map = true;
    }
    auto const set_like(runtime::get(m, __rt_ctx->intern_keyword("set_like").expect_ok()));
    if(truthy(set_like))
    {
      this->is_set = true;
    }
    auto const stackable(runtime::get(m, __rt_ctx->intern_keyword("stackable").expect_ok()));
    if(truthy(stackable))
    {
      auto const peek(runtime::get(stackable, __rt_ctx->intern_keyword("peek").expect_ok()));
      auto const pop(runtime::get(stackable, __rt_ctx->intern_keyword("pop").expect_ok()));
      this->is_stackable = true;
      this->peek = [&](object_ptr const o) { return dynamic_call(peek, o); };
      this->pop = [&](object_ptr const o) { return dynamic_call(pop, o); };
    }
    auto const number_like(runtime::get(m, __rt_ctx->intern_keyword("number_like").expect_ok()));
    if(truthy(number_like))
    {
      auto const to_integer(runtime::get(number_like, __rt_ctx->intern_keyword("to_integer").expect_ok()));
      auto const to_real(runtime::get(number_like, __rt_ctx->intern_keyword("to_real").expect_ok()));
      this->is_number_like = true;
      this->to_integer = [&](object_ptr const o) { return dynamic_call(to_integer, o); };
      this->to_real = [&](object_ptr const o) { return dynamic_call(to_real, o); };
    }
    //if(behavior::set_like<T> || behavior::associatively_readable<T>)
    if(truthy(associatively_readable))
    {
      auto const contains(runtime::get(associatively_readable, __rt_ctx->intern_keyword("contains").expect_ok()));
      this->contains
        = [&](object_ptr const m, object_ptr const k) { return dynamic_call(contains, m, k) };
    }
    auto const conjable(runtime::get(m, __rt_ctx->intern_keyword("conjable").expect_ok()));
    if(truthy(conjable))
    {
      auto const conj(runtime::get(conjable, __rt_ctx->intern_keyword("conj").expect_ok()));
      this->is_conjable = true;
      this->conj = [&](object_ptr const o, object_ptr const v) {
        return dynamic_call(conj, o, v);
      };
    }
    auto const conjable_in_place(runtime::get(m, __rt_ctx->intern_keyword("conjable_in_place").expect_ok()));
    if(truthy(conjable_in_place))
    {
      auto const conj_in_place(runtime::get(conjable_in_place, __rt_ctx->intern_keyword("conj_in_place").expect_ok()));
      this->is_conjable_in_place = true;
      this->conj_in_place
        = [&](object_ptr const o, object_ptr const v) { return dynamic_call(conj_in_place, o, v); };
    }
  }

  object_behaviors const *behaviors(object_ptr type)
  {
    return visit_object(
      [](auto const typed_o) -> object_behaviors const * {
        static object_behaviors const bs{ typed_o };
        return &bs;
      },
      type);
  }
}
