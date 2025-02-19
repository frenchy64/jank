#include <jank/coz_native.hpp>
#include <jank/runtime/coz.hpp>

#include <jank/runtime/convert.hpp>
#include <jank/runtime/context.hpp>
#include <jank/runtime/core/munge.hpp>
#include <jank/runtime/obj/native_function_wrapper.hpp>
#include <jank/runtime/obj/persistent_hash_map.hpp>
#include <jank/runtime/obj/keyword.hpp>

jank_object_ptr jank_load_jank_coz_native()
{
  using namespace jank;
  using namespace jank::runtime;

  auto const ns(__rt_ctx->intern_ns("jank.coz-native"));

  auto const intern_fn([=](native_persistent_string const &name, auto const fn) {
    ns->intern_var(name)->bind_root(
      make_box<obj::native_function_wrapper>(convert_function(fn))
        ->with_meta(obj::persistent_hash_map::create_unique(std::make_pair(
          __rt_ctx->intern_keyword("name").expect_ok(),
          make_box(obj::symbol{ __rt_ctx->current_ns()->to_string(), name }.to_string())))));
  });
  intern_fn("progress", &coz::progress);
  intern_fn("progress-named", &coz::progress_named);
  intern_fn("begin", &coz::begin);
  intern_fn("end", &coz::end);

  return erase(obj::nil::nil_const());
}
