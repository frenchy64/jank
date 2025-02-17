#include <algorithm>
#include <random>
#include <fmt/core.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/visit.hpp>
#include <jank/runtime/behavior/associatively_readable.hpp>
#include <jank/runtime/behavior/associatively_writable.hpp>
#include <jank/runtime/behavior/callable.hpp>
#include <jank/runtime/behavior/countable.hpp>
#include <jank/runtime/behavior/set_like.hpp>
#include <jank/runtime/behavior/sequential.hpp>
#include <jank/runtime/behavior/collection_like.hpp>
#include <jank/runtime/behavior/transientable.hpp>
#include <jank/runtime/behavior/indexable.hpp>
#include <jank/runtime/behavior/stackable.hpp>
#include <jank/runtime/behavior/chunkable.hpp>
#include <jank/runtime/behavior/metadatable.hpp>
#include <jank/runtime/core.hpp>
#include <jank/runtime/behaviors.hpp>

namespace jank::runtime
{
  native_bool is_empty(object_ptr const o)
  {
    if(is_nil(o))
    {
      return true;
    }
    auto const bs(object_behaviors(o));
    if(bs.is_seqable)
    {
      return bs.seq(o) == nullptr;
    }
    else if(bs.is_countable)
    {
      return bs.count(o) == 0;
    }
    else
    {
      throw std::runtime_error{ fmt::format("cannot check if this is empty: {}", bs.to_string(o)) };
    }
  }

  native_bool is_seq(object_ptr const o)
  {
    return object_behaviors(o).is_sequenceable;
  }

  native_bool is_seqable(object_ptr const o)
  {
    return object_behaviors(o).is_seqable;
  }

  native_bool is_sequential(object_ptr const o)
  {
    return object_behaviors(o).is_sequential;
  }

  native_bool is_collection(object_ptr const o)
  {
    return object_behaviors(o).is_collection;
  }

  native_bool is_list(object_ptr const o)
  {
    /* TODO: Visit and use a behavior for this check instead.
     * It should apply to conses and others. */
    return o->type == object_type::persistent_list;
  }

  native_bool is_vector(object_ptr const o)
  {
    return o->type == object_type::persistent_vector;
  }

  native_bool is_map(object_ptr const o)
  {
    return object_behaviors(o).is_map;
  }

  native_bool is_associative(object_ptr const o)
  {
    return object_behaviors(o).is_associative;
  }

  native_bool is_set(object_ptr const o)
  {
    return object_behaviors(o).is_set;
  }

  native_bool is_countable(object_ptr const o)
  {
    return object_behaviors(o).is_countable;
  }

  native_bool is_transientable(object_ptr const o)
  {
    return object_behaviors(o).is_transientable;
  }

  native_bool is_sorted(object_ptr const o)
  {
    return o->type == object_type::persistent_sorted_map
      || o->type == object_type::persistent_sorted_set;
  }

  object_ptr transient(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_transientable)
    {
      return bs.to_transient(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not transientable: {}", bs.to_string(o)) };
    }
  }

  object_ptr persistent(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_persistentable)
    {
      return bs.to_persistent(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not persistentable: {}", bs.to_string(o)) };
    }
  }

  object_ptr conj_in_place(object_ptr const coll, object_ptr const o)
  {
    auto const bs(object_behaviors(coll));
    if(bs.is_conjable_in_place)
    {
      return bs.conj_in_place(coll, o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not conjable_in_place: {}", bs.to_string(coll)) };
    }
  }

  object_ptr disj_in_place(object_ptr const coll, object_ptr const o)
  {
    /* TODO: disjoinable_in_place */
    if(coll->type == object_type::transient_hash_set)
    {
      return expect_object<obj::transient_hash_set>(coll)->disjoin_in_place(o);
    }
    else if(coll->type == object_type::transient_sorted_set)
    {
      return expect_object<obj::transient_sorted_set>(coll)->disjoin_in_place(o);
    }

    throw std::runtime_error{ fmt::format("not disjoinable_in_place: {}",
                                          runtime::to_string(coll)) };
  }

  object_ptr assoc_in_place(object_ptr const coll, object_ptr const k, object_ptr const v)
  {
    auto const bs(object_behaviors(coll));
    if(bs.is_associatively_writable_in_place)
    {
      return bs.assoc_in_place(coll, k, v);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not associatively_writable_in_place: {}",
                                            bs.to_string(coll)) };
    }
  }

  object_ptr dissoc_in_place(object_ptr const coll, object_ptr const k)
  {
    auto const bs(object_behaviors(coll));
    if(bs.is_associatively_writable_in_place)
    {
      return bs.dissoc_in_place(coll, k);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not associatively_writable_in_place: {}",
                                            bs.to_string(coll)) };
    }
  }

  object_ptr pop_in_place(object_ptr const coll)
  {
    auto const trans(try_object<obj::transient_vector>(coll));
    return trans->pop_in_place();
  }

  object_ptr seq(object_ptr const s)
  {
    if(is_nil(s))
    {
      return s;
    }
    auto const bs(object_behaviors(s));
    if(bs.is_seqable)
    {
      auto const ret(bs.seq(s));
      if(!ret)
      {
        return obj::nil::nil_const();
      }

      return ret;
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
  }

  object_ptr fresh_seq(object_ptr const s)
  {
    if(is_nil(s))
    {
      return s;
    }
    auto const bs(object_behaviors(s));
    if(bs.is_seqable)
    {
      auto const ret(bs.fresh_seq(s));
      if(!ret)
      {
        return obj::nil::nil_const();
      }
      return ret;
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
  }

  object_ptr first(object_ptr const s)
  {
    if(is_nil(s))
    {
      return s;
    }
    auto const bs(object_behaviors(s));
    if(bs.is_sequenceable)
    {
      return bs.first(s);
    }
    else if(bs.is_seqable)
    {
      auto const ret(bs.seq(s));
      if(!ret)
      {
        return obj::nil::nil_const();
      }

      return object_behaviors(ret).first(ret);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
  }

  object_ptr second(object_ptr const s)
  {
    return first(next(s));
  }

  object_ptr next(object_ptr const s)
  {
    if(is_nil(s))
    {
      return s;
    }
    auto const bs(object_behaviors(s));
    if(bs.is_sequenceable)
    {
      return bs.next(s) ?: obj::nil::nil_const();
    }
    else if(bs.is_seqable)
    {
      auto const seq(bs.seq(s));
      if(!seq)
      {
        return obj::nil::nil_const();
      }

      return runtime::next(seq);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
  }

  object_ptr next_in_place(object_ptr const s)
  {
    if(is_nil(s))
    {
      return s;
    }
    auto const bs(object_behaviors(s));
    if(bs.is_sequenceable_in_place)
    {
      return bs.next_in_place(s) ?: obj::nil::nil_const();
    }
    else if(bs.is_sequenceable)
    {
      return bs.next(s) ?: obj::nil::nil_const();
    }
    else if(bs.is_seqable)
    {
      auto const ret(bs.seq(s));
      if(!ret)
      {
        return obj::nil::nil_const();
      }

      return runtime::next_in_place(ret) ?: obj::nil::nil_const();
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
  }

  object_ptr rest(object_ptr const s)
  {
    if(!s || s == obj::nil::nil_const())
    {
      return obj::persistent_list::empty();
    }
    auto const bs(object_behaviors(s));
    auto const seq(bs.seq(s));
    if(!seq)
    {
      return obj::persistent_list::empty();
    }
    //TODO specialize is_sequenceable case to save visit (like next above)
    auto const ret(runtime::next(s));
    if(is_nil(ret)) //TODO update condition with above change ^^
    {
      return obj::persistent_list::empty();
    }
    return ret;
  }

  object_ptr cons(object_ptr const head, object_ptr const tail)
  {
    auto const bs(object_behaviors(tail));
    if(bs.is_seqable)
    {
      return make_box<jank::runtime::obj::cons>(head, bs.seq(tail));
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(tail)) };
    }
  }

  object_ptr conj(object_ptr const s, object_ptr const o)
  {
    if(is_nil(s))
    {
      return make_box<obj::persistent_list>(std::in_place, o);
    }
    auto const bs(object_behaviors(s));
    if(bs.is_conjable_in_place)
    {
      return bs.conj_in_place(s, o);
    }
    else if(bs.is_conjable)
    {
      return bs.conj(s, o);
    }
    else if(bs.is_seqable)
    {
      // TODO save extra visit?
      return runtime::conj(bs.seq(s) ?: obj::nil::nil_const(), o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
  }

  object_ptr disj(object_ptr const s, object_ptr const o)
  {
    if(s->type == object_type::nil)
    {
      return s;
    }
    else if(s->type == object_type::persistent_hash_set)
    {
      auto const set(expect_object<obj::persistent_hash_set>(s));
      return set->disj(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not disjoinable: {}", runtime::to_string(s)) };
    }
  }

  object_ptr assoc(object_ptr const m, object_ptr const k, object_ptr const v)
  {
    auto const bs(object_behaviors(m));
    if(bs.is_associatively_writable)
    {
      return bs.assoc(m, k, v);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not associatively writable: {}", bs.to_string(m)) };
    }
  }

  object_ptr dissoc(object_ptr const m, object_ptr const k)
  {
    auto const bs(object_behaviors(m));
    if(bs.is_associatively_writable)
    {
      return bs.dissoc(m, k);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not associatively writable: {}", bs.to_string(m)) };
    }
  }

  object_ptr get(object_ptr const m, object_ptr const key)
  {
    return get(m, key, obj::nil::nil_const());
  }

  object_ptr get(object_ptr const m, object_ptr const key, object_ptr const fallback)
  {
    return visit_object(
      [&](auto const typed_m) -> object_ptr {
        using T = typename decltype(typed_m)::value_type;

        if constexpr(behavior::associatively_readable<T>)
        {
          return typed_m->get(key, fallback);
        }
        else
        {
          return fallback;
        }
      },
      m);
  }

  /*
  {
    auto const bs(object_behaviors(m));
    if(bs.is_associatively_readable)
    {
      return bs.get_default(m, key, fallback);
    }
    else
    {
      return fallback;
    }
  }
  */

  object_ptr get_in(object_ptr m, object_ptr keys)
  {
    return get_in(m, keys, obj::nil::nil_const());
  }

  object_ptr get_in(object_ptr m, object_ptr keys, object_ptr fallback)
  {
    return visit_object(
      [&](auto const typed_m) -> object_ptr {
        using T = typename decltype(typed_m)::value_type;

        if constexpr(behavior::associatively_readable<T>)
        {
          return visit_object(
            [&](auto const typed_keys) -> object_ptr {
              using T = typename decltype(typed_keys)::value_type;

              if constexpr(behavior::seqable<T>)
              {
                object_ptr ret{ typed_m };
                for(auto seq(typed_keys->fresh_seq()); seq != nullptr; seq = next_in_place(seq))
                {
                  ret = get(ret, seq->first());
                }

                if(ret == obj::nil::nil_const())
                {
                  return fallback;
                }
                return ret;
              }
              else
              {
                throw std::runtime_error{ fmt::format("not seqable: {}", typed_keys->to_string()) };
              }
            },
            keys);
        }
        else
        {
          return obj::nil::nil_const();
        }
      },
      m);
  }

  object_ptr find(object_ptr const s, object_ptr const key)
  {
    auto const nil(obj::nil::nil_const());
    if(s == nullptr || s == nil)
    {
      return nil;
    }

    auto const bs(object_behaviors(s));
    if(bs.is_associatively_readable)
    {
      return bs.get_entry(s, key);
    }
    else
    {
      return nil;
    }
  }

  native_bool contains(object_ptr const s, object_ptr const key)
  {
    if(s == nullptr || s == obj::nil::nil_const())
    {
      return false;
    }

    auto const bs(object_behaviors(s));
    if(bs.is_associatively_readable || bs.is_set)
    {
      return bs.contains(s, key);
    }
    else
    {
      return false;
    }
  }

  object_ptr merge(object_ptr const m, object_ptr const other)
  {
    return visit_object(
      [&](auto const typed_m) -> object_ptr {
        using T = typename decltype(typed_m)::value_type;

        if constexpr(behavior::associatively_writable<T>)
        {
          return visit_object(
            [&](auto const typed_other) -> object_ptr {
              using O = typename decltype(typed_other)::value_type;

              if constexpr(std::same_as<O, obj::persistent_hash_map>
                           || std::same_as<O, obj::persistent_array_map>
                           || std::same_as<O, obj::transient_hash_map>
                           //|| std::same_as<O, obj::transient_array_map>
              )
              {
                object_ptr ret{ m };
                for(auto const &pair : typed_other->data)
                {
                  ret = assoc(ret, pair.first, pair.second);
                }
                return ret;
              }
              else
              {
                throw std::runtime_error{ fmt::format("not associatively readable: {}",
                                                      typed_m->to_string()) };
              }
            },
            other);
        }
        else
        {
          throw std::runtime_error{ fmt::format("not associatively writable: {}",
                                                typed_m->to_string()) };
        }
      },
      m);
  }

  object_ptr subvec(object_ptr const o, native_integer const start, native_integer const end)
  {
    if(o->type != object_type::persistent_vector)
    {
      throw std::runtime_error{ "not a vector" };
    }

    auto const v(expect_object<obj::persistent_vector>(o));

    if(end < start || start < 0 || static_cast<size_t>(end) > v->count())
    {
      throw std::runtime_error{ "index out of bounds" };
    }
    else if(start == end)
    {
      return obj::persistent_vector::empty();
    }
    return make_box<obj::persistent_vector>(
      detail::native_persistent_vector{ v->data.begin() + start, v->data.begin() + end });
  }

  object_ptr nth(object_ptr const o, object_ptr const idx)
  {
    return nth(o, idx, obj::nil::nil_const());
  }

  object_ptr nth(object_ptr const o, object_ptr const idx, object_ptr const fallback)
  {
    if(is_nil(o))
    {
      return fallback;
    }

    auto const index(to_int(idx));
    if(index < 0)
    {
      return fallback;
    }

    auto const bs(object_behaviors(o));
    if(bs.is_indexable)
    {
      return bs.nth_default(o, idx, fallback);
    }
    else if(bs.is_seqable)
    {
      native_integer i{};
      // TODO next_in_place / first without visit
      for(auto it(bs.fresh_seq(o)); it != nullptr && !is_nil(it); it = next_in_place(it), ++i)
      {
        if(i == index)
        {
          return first(it);
        }
      }
      return fallback;
    }
    else
    {
      throw std::runtime_error{ fmt::format("not indexable: {}", object_type_str(o->type)) };
    }
  }

  object_ptr peek(object_ptr const o)
  {
    if(o == obj::nil::nil_const())
    {
      return o;
    }

    return visit_object(
      [&](auto const typed_o) -> object_ptr {
        using T = typename decltype(typed_o)::value_type;

        if constexpr(behavior::stackable<T>)
        {
          return typed_o->peek();
        }
        else
        {
          throw std::runtime_error{ fmt::format("not stackable: {}", object_type_str(o->type)) };
        }
      },
      o);
  }

  object_ptr pop(object_ptr const o)
  {
    if(o == obj::nil::nil_const())
    {
      return o;
    }

    return visit_object(
      [&](auto const typed_o) -> object_ptr {
        using T = typename decltype(typed_o)::value_type;

        if constexpr(behavior::stackable<T>)
        {
          return typed_o->pop();
        }
        else
        {
          throw std::runtime_error{ fmt::format("not stackable: {}", object_type_str(o->type)) };
        }
      },
      o);
  }

  object_ptr empty(object_ptr const o)
  {
    return visit_object(
      [=](auto const typed_o) -> object_ptr {
        using T = typename decltype(typed_o)::value_type;

        if constexpr(behavior::collection_like<T>)
        {
          return T::empty();
        }
        else
        {
          return obj::nil::nil_const();
        }
      },
      o);
  }

  native_persistent_string str(object_ptr const o, object_ptr const args)
  {
    return visit_seqable(
      [=](auto const typed_args) -> native_persistent_string {
        util::string_builder buff;
        buff.reserve(16);
        runtime::to_string(o, buff);
        if(0 < sequence_length(typed_args))
        {
          auto const fresh(typed_args->fresh_seq());
          runtime::to_string(fresh->first(), buff);
          for(auto it(fresh->next_in_place()); it != nullptr; it = it->next_in_place())
          {
            runtime::to_string(it->first(), buff);
          }
        }
        return buff.release();
      },
      args);
  }

  obj::persistent_list_ptr list(object_ptr const s)
  {
    return visit_seqable(
      [](auto const typed_s) -> obj::persistent_list_ptr {
        return obj::persistent_list::create(typed_s);
      },
      s);
    /*
    auto const bs(object_behaviors(s));
    if(bs.is_seqable)
    {
      return obj::persistent_list::create(s);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
    */
  }

  obj::persistent_vector_ptr vec(object_ptr const s)
  {
    return visit_seqable(
      [](auto const typed_s) -> obj::persistent_vector_ptr {
        return obj::persistent_vector::create(typed_s);
      },
      s);
    /*
    auto const bs(object_behaviors(s));
    if(bs.is_seqable)
    {
      return obj::persistent_vector::create(s);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not seqable: {}", bs.to_string(s)) };
    }
    */
  }

  size_t sequence_length(object_ptr const s)
  {
    return sequence_length(s, std::numeric_limits<size_t>::max());
  }

  size_t sequence_length(object_ptr const s, size_t const max)
  {
    if(s == nullptr)
    {
      return 0;
    }

    return visit_object(
      [&](auto const typed_s) -> size_t {
        using T = typename decltype(typed_s)::value_type;

        if constexpr(std::same_as<T, obj::nil>)
        {
          return 0;
        }
        else if constexpr(behavior::countable<T>)
        {
          return typed_s->count();
        }
        else if constexpr(behavior::seqable<T>)
        {
          size_t length{ 0 };
          for(auto i(typed_s->fresh_seq()); i != nullptr && length < max; i = next_in_place(i))
          {
            ++length;
          }
          return length;
        }
        else
        {
          throw std::runtime_error{ fmt::format("not seqable: {}", typed_s->to_string()) };
        }
      },
      s);
  }

  native_bool sequence_equal(object_ptr const l, object_ptr const r)
  {
    if(l == r)
    {
      return true;
    }

    /* TODO: visit_sequence. */
    return visit_seqable(
      [](auto const typed_l, object_ptr const r) -> native_bool {
        return visit_seqable(
          [](auto const typed_r, auto const typed_l) -> native_bool {
            auto r_it(typed_r->fresh_seq());
            auto l_it(typed_l->fresh_seq());
            if(!r_it)
            {
              return l_it == nullptr;
            }
            if(!l_it)
            {
              return r_it == nullptr;
            }

            for(; l_it != nullptr; l_it = l_it->next_in_place(), r_it = r_it->next_in_place())
            {
              if(!r_it)
              {
                return false;
              }
              if(!runtime::equal(l_it->first(), r_it->first()))
              {
                return false;
              }
            }
            return r_it == nullptr;
          },
          r,
          typed_l);
      },
      l,
      r);
  }

  object_ptr reduce(object_ptr const f, object_ptr const init, object_ptr const s)
  {
    return visit_seqable(
      [](auto const typed_coll, object_ptr const f, object_ptr const init) -> object_ptr {
        object_ptr res{ init };
        for(auto it(typed_coll->fresh_seq()); it != nullptr; it = it->next_in_place())
        {
          res = dynamic_call(f, res, it->first());
          if(res->type == object_type::reduced)
          {
            res = expect_object<obj::reduced>(res)->val;
            break;
          }
        }
        return res;
      },
      s,
      f,
      init);
  }

  object_ptr reduced(object_ptr const o)
  {
    return make_box<obj::reduced>(o);
  }

  native_bool is_reduced(object_ptr const o)
  {
    return o->type == object_type::reduced;
  }

  object_ptr chunk_buffer(object_ptr const capacity)
  {
    return make_box<obj::chunk_buffer>(capacity);
  }

  object_ptr chunk_append(object_ptr const buff, object_ptr const val)
  {
    auto const buffer(try_object<obj::chunk_buffer>(buff));
    buffer->append(val);
    return obj::nil::nil_const();
  }

  object_ptr chunk(object_ptr const buff)
  {
    auto const buffer(try_object<obj::chunk_buffer>(buff));
    return buffer->chunk();
  }

  object_ptr chunk_first(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_chunkable)
    {
      return bs.chunked_first(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not chunkable: {}", bs.to_string(o)) };
    }
  }

  object_ptr chunk_next(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_chunkable)
    {
      return bs.chunked_next(o) ?: obj::nil::nil_const();
    }
    else
    {
      throw std::runtime_error{ fmt::format("not chunkable: {}", bs.to_string(o)) };
    }
  }

  object_ptr chunk_rest(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_chunkable)
    {
      return bs.chunked_next(o) ?: obj::persistent_list::empty();
    }
    else
    {
      throw std::runtime_error{ fmt::format("not chunkable: {}", bs.to_string(o)) };
    }
  }

  object_ptr chunk_cons(object_ptr const chunk, object_ptr const rest)
  {
    return make_box<obj::chunked_cons>(chunk, seq(rest));
  }

  native_bool is_chunked_seq(object_ptr const o)
  {
    return object_behaviors(o).is_chunkable;
  }

  object_ptr iterate(object_ptr const fn, object_ptr const o)
  {
    return make_box<obj::iterator>(fn, o);
  }

  object_ptr repeat(object_ptr const val)
  {
    return make_box<obj::repeat>(val);
  }

  object_ptr repeat(object_ptr const n, object_ptr const val)
  {
    return make_box<obj::repeat>(n, val);
  }

  object_ptr sort(object_ptr const coll)
  {
    return visit_seqable(
      [](auto const typed_coll) -> object_ptr {
        native_vector<object_ptr> vec;
        for(auto it(typed_coll->fresh_seq()); it != nullptr; it = it->next_in_place())
        {
          vec.push_back(it->first());
        }

        std::stable_sort(vec.begin(), vec.end(), [](object_ptr const a, object_ptr const b) {
          return runtime::compare(a, b) < 0;
        });

        using T = typename decltype(typed_coll)::value_type;

        if constexpr(behavior::metadatable<T>)
        {
          return make_box<obj::native_vector_sequence>(typed_coll->meta, std::move(vec));
        }
        else
        {
          return make_box<obj::native_vector_sequence>(std::move(vec));
        }
      },
      coll);
  }

  object_ptr shuffle(object_ptr const coll)
  {
    return visit_seqable(
      [](auto const typed_coll) -> object_ptr {
        native_vector<object_ptr> vec;
        for(auto it(typed_coll->fresh_seq()); it != nullptr; it = it->next_in_place())
        {
          vec.push_back(it->first());
        }

        static std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(vec.begin(), vec.end(), g);

        return make_box<obj::persistent_vector>(
          runtime::detail::native_persistent_vector{ vec.begin(), vec.end() });
      },
      coll);
  }

}
