#include <fmt/format.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/core.hpp>
#include <jank/runtime/rtti.hpp>
#include <jank/runtime/obj/atom.hpp>
#include <jank/runtime/obj/character.hpp>
#include <jank/runtime/obj/cons.hpp>
#include <jank/runtime/obj/delay.hpp>
#include <jank/runtime/obj/keyword.hpp>
#include <jank/runtime/obj/nil.hpp>
#include <jank/runtime/obj/number.hpp>
#include <jank/runtime/obj/persistent_hash_map.hpp>
#include <jank/runtime/obj/persistent_string.hpp>
#include <jank/runtime/obj/symbol.hpp>
#include <jank/runtime/obj/tagged_literal.hpp>
#include <jank/runtime/obj/volatile.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/behavior/callable.hpp>
#include <jank/runtime/behavior/comparable.hpp>
#include <jank/runtime/behavior/nameable.hpp>
#include <jank/runtime/behavior/derefable.hpp>
#include <jank/runtime/context.hpp>

namespace jank::runtime
{
  native_integer compare(object_ptr const l, object_ptr const r)
  {
    if(l == r)
    {
      return 0;
    }

    if(l != obj::nil::nil_const())
    {
      if(r == obj::nil::nil_const())
      {
        return 1;
      }

      auto const bs(object_behaviors(l));
      if(!bs.is_comparable)
      {
        throw std::runtime_error{ fmt::format("not comparable: {}", bs.to_string(l)) };
      }
      return bs.compare(l, r);
    }

    return -1;
  }

  native_bool is_identical(object_ptr const lhs, object_ptr const rhs)
  {
    return lhs == rhs;
  }

  native_persistent_string type(object_ptr const o)
  {
    return object_type_str(o->type);
  }

  native_bool is_nil(object_ptr const o)
  {
    return o == obj::nil::nil_const();
  }

  native_bool is_true(object_ptr const o)
  {
    return o == obj::boolean::true_const();
  }

  native_bool is_false(object_ptr const o)
  {
    return o == obj::boolean::false_const();
  }

  native_bool is_some(object_ptr const o)
  {
    return o != obj::nil::nil_const();
  }

  native_bool is_var(object_ptr const o)
  {
    return o->type == object_type::var;
  }

  native_bool is_string(object_ptr const o)
  {
    return o->type == object_type::persistent_string;
  }

  native_bool is_char(object_ptr const o)
  {
    return o->type == object_type::character;
  }

  native_bool is_symbol(object_ptr const o)
  {
    return o->type == object_type::symbol;
  }

  native_bool is_simple_symbol(object_ptr const o)
  {
    return o->type == object_type::symbol && expect_object<obj::symbol>(o)->ns.empty();
  }

  native_bool is_qualified_symbol(object_ptr const o)
  {
    return o->type == object_type::symbol && !expect_object<obj::symbol>(o)->ns.empty();
  }

  object_ptr print_helper(object_ptr const args,
                          std::function<void(object_ptr, util::string_builder &)> into_builder)
  {
    if(is_nil(args))
    {
      return obj::nil::nil_const();
    }

    auto const bs(object_behaviors(args));
    if(!bs.is_sequenceable)
    {
      throw std::runtime_error{ fmt::format("expected a sequence: {}", bs.to_string(args)) };
    }
    util::string_builder buff;
    into_builder(bs.first(args), buff);
    // TODO next_in_place / first perf
    for(auto it(bs.next_in_place(args)); it != nullptr; it = object_behaviors(it).next_in_place(it))
    {
      buff(' ');
      runtime::to_string(first(it), buff);
    }
    std::fwrite(buff.data(), 1, buff.size(), stdout);
    return obj::nil::nil_const();
  }

  object_ptr print(object_ptr const args)
  {
    return print_helper(args, [](object_ptr o, util::string_builder &b) {
      return runtime::to_string(o, b);
    });
  }

  object_ptr println(object_ptr const args)
  {
    auto const res(print(args));
    std::putc('\n', stdout);
    return res;
  }

  object_ptr pr(object_ptr const args)
  {
    return print_helper(args, [](object_ptr o, util::string_builder &b) {
      return runtime::to_code_string(o, b);
    });
  }

  object_ptr prn(object_ptr const args)
  {
    auto const res(pr(args));
    std::putc('\n', stdout);
    return res;
  }

  native_real to_real(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_number_like)
    {
      return bs.to_real(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not a number: {}", bs.to_string(o)) };
    }
  }

  native_bool equal(char const lhs, object_ptr const rhs)
  {
    if(!rhs || rhs->type != object_type::character)
    {
      return false;
    }

    auto const typed_rhs = expect_object<obj::character>(rhs);
    return typed_rhs->to_hash() == static_cast<native_hash>(lhs);
  }

  native_bool equal(object_ptr const lhs, object_ptr const rhs)
  {
    if(!lhs)
    {
      return !rhs;
    }
    else if(!rhs)
    {
      return !lhs;
    }

    return object_behaviors(lhs).equal(lhs, rhs);
  }

  object_ptr meta(object_ptr const m)
  {
    if(m == nullptr || m == obj::nil::nil_const())
    {
      return obj::nil::nil_const();
    }

    auto const bs(object_behaviors(m));
    if(bs.is_metadatable)
    {
      return bs.meta(m).unwrap_or(obj::nil::nil_const());
    }
    else
    {
      return obj::nil::nil_const();
    }
  }

  object_ptr with_meta(object_ptr const o, object_ptr const m)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_metadatable)
    {
      return bs.with_meta(o, m);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not metadatable: {}", bs.to_string(o)) };
    }
  }

  object_ptr reset_meta(object_ptr const o, object_ptr const m)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_metadatable)
    {
      return bs.set_meta(o, m);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not metadatable: {}", bs.to_string(o)) };
    }
  }

  obj::persistent_string_ptr subs(object_ptr const s, object_ptr const start)
  {
    return try_object<obj::persistent_string>(s)->substring(to_int(start)).expect_ok();
  }

  obj::persistent_string_ptr subs(object_ptr const s, object_ptr const start, object_ptr const end)
  {
    return try_object<obj::persistent_string>(s)->substring(to_int(start), to_int(end)).expect_ok();
  }

  native_integer first_index_of(object_ptr const s, object_ptr const m)
  {
    return try_object<obj::persistent_string>(s)->first_index_of(m);
  }

  native_integer last_index_of(object_ptr const s, object_ptr const m)
  {
    return try_object<obj::persistent_string>(s)->last_index_of(m);
  }

  native_bool is_named(object_ptr const o)
  {
    return object_behaviors(o).is_named;
  }

  native_persistent_string name(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(o->type == object_type::persistent_string)
    {
      return bs.to_string(o);
    }
    else if(bs.is_named)
    {
      return bs.get_name(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not nameable: {}", bs.to_string(o)) };
    }
  }

  object_ptr namespace_(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_named)
    {
      auto const ns(bs.get_namespace(o));
      return (ns.empty() ? obj::nil::nil_const() : make_box<obj::persistent_string>(ns));
    }
    else
    {
      throw std::runtime_error{ fmt::format("not nameable: {}", bs.to_string(o)) };
    }
  }

  object_ptr keyword(object_ptr const ns, object_ptr const name)
  {
    return __rt_ctx->intern_keyword(runtime::to_string(ns), runtime::to_string(name)).expect_ok();
  }

  native_bool is_keyword(object_ptr const o)
  {
    return o->type == object_type::keyword;
  }

  native_bool is_simple_keyword(object_ptr const o)
  {
    return o->type == object_type::keyword && expect_object<obj::keyword>(o)->sym->ns.empty();
  }

  native_bool is_qualified_keyword(object_ptr const o)
  {
    return o->type == object_type::keyword && !expect_object<obj::keyword>(o)->sym->ns.empty();
  }

  native_bool is_callable(object_ptr const o)
  {
    return object_behaviors(o).is_callable;
  }

  native_hash to_hash(object_ptr const o)
  {
    return object_behaviors(o).to_hash(o);
  }

  object_ptr macroexpand1(object_ptr const o)
  {
    return __rt_ctx->macroexpand1(o);
  }

  object_ptr macroexpand(object_ptr const o)
  {
    return __rt_ctx->macroexpand(o);
  }

  object_ptr gensym(object_ptr const o)
  {
    return make_box<obj::symbol>(__rt_ctx->unique_symbol(to_string(o)));
  }

  object_ptr atom(object_ptr const o)
  {
    return make_box<obj::atom>(o);
  }

  object_ptr swap_atom(object_ptr const atom, object_ptr const fn)
  {
    return try_object<obj::atom>(atom)->swap(fn);
  }

  object_ptr swap_atom(object_ptr const atom, object_ptr const fn, object_ptr const a1)
  {
    return try_object<obj::atom>(atom)->swap(fn, a1);
  }

  object_ptr
  swap_atom(object_ptr const atom, object_ptr const fn, object_ptr const a1, object_ptr const a2)
  {
    return try_object<obj::atom>(atom)->swap(fn, a1, a2);
  }

  object_ptr swap_atom(object_ptr const atom,
                       object_ptr const fn,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const rest)
  {
    return try_object<obj::atom>(atom)->swap(fn, a1, a2, rest);
  }

  object_ptr swap_vals(object_ptr const atom, object_ptr const fn)
  {
    return try_object<obj::atom>(atom)->swap_vals(fn);
  }

  object_ptr swap_vals(object_ptr const atom, object_ptr const fn, object_ptr const a1)
  {
    return try_object<obj::atom>(atom)->swap_vals(fn, a1);
  }

  object_ptr
  swap_vals(object_ptr const atom, object_ptr const fn, object_ptr const a1, object_ptr const a2)
  {
    return try_object<obj::atom>(atom)->swap_vals(fn, a1, a2);
  }

  object_ptr swap_vals(object_ptr const atom,
                       object_ptr const fn,
                       object_ptr const a1,
                       object_ptr const a2,
                       object_ptr const rest)
  {
    return try_object<obj::atom>(atom)->swap_vals(fn, a1, a2, rest);
  }

  object_ptr
  compare_and_set(object_ptr const atom, object_ptr const old_val, object_ptr const new_val)
  {
    return try_object<obj::atom>(atom)->compare_and_set(old_val, new_val);
  }

  object_ptr reset(object_ptr const atom, object_ptr const new_val)
  {
    return try_object<obj::atom>(atom)->reset(new_val);
  }

  object_ptr reset_vals(object_ptr const atom, object_ptr const new_val)
  {
    return try_object<obj::atom>(atom)->reset_vals(new_val);
  }

  object_ptr deref(object_ptr const o)
  {
    auto const bs(object_behaviors(o));
    if(bs.is_derefable)
    {
      return bs.deref(o);
    }
    else
    {
      throw std::runtime_error{ fmt::format("not derefable: {}", bs.to_string(o)) };
    }
  }

  object_ptr volatile_(object_ptr const o)
  {
    return make_box<obj::volatile_>(o);
  }

  native_bool is_volatile(object_ptr const o)
  {
    return o->type == object_type::volatile_;
  }

  object_ptr vswap(object_ptr const v, object_ptr const fn, object_ptr const args)
  {
    auto const v_obj(expect_object<obj::volatile_>(v));
    return v_obj->reset(apply_to(fn, make_box<obj::cons>(v_obj->deref(), args)));
  }

  object_ptr vreset(object_ptr const v, object_ptr const new_val)
  {
    return expect_object<obj::volatile_>(v)->reset(new_val);
  }

  void push_thread_bindings(object_ptr const o)
  {
    __rt_ctx->push_thread_bindings(o).expect_ok();
  }

  void pop_thread_bindings()
  {
    __rt_ctx->pop_thread_bindings().expect_ok();
  }

  object_ptr get_thread_bindings()
  {
    return __rt_ctx->get_thread_bindings();
  }

  object_ptr force(object_ptr const o)
  {
    if(o->type == object_type::delay)
    {
      return expect_object<obj::delay>(o)->deref();
    }
    return o;
  }

  object_ptr tagged_literal(object_ptr const tag, object_ptr const form)
  {
    return make_box<obj::tagged_literal>(tag, form);
  }

  native_bool is_tagged_literal(object_ptr const o)
  {
    return o->type == object_type::tagged_literal;
  }

}
