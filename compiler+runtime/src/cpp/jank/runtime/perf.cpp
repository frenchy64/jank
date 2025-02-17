#include <nanobench.h>

#include <fmt/format.h>

#include <jank/native_persistent_string/fmt.hpp>
#include <jank/runtime/perf.hpp>
#include <jank/runtime/behaviors.hpp>
#include <jank/runtime/obj/keyword.hpp>
#include <jank/runtime/core/seq.hpp>
#include <jank/runtime/core/to_string.hpp>
#include <jank/runtime/context.hpp>

namespace jank::runtime::perf
{
  object_ptr benchmark(object_ptr const opts, object_ptr const f)
  {
    auto const label(to_string(get(opts, __rt_ctx->intern_keyword("label").expect_ok())));

    auto const bs(object_behaviors(f));
    if(bs.is_callable)
    {
      ankerl::nanobench::Config config;
      //config.mTimeUnitName = TODO
      config.mOut = &std::cout;

      /* Larger things. */
      config.mTimeUnit = std::chrono::milliseconds{ 1 };
      config.mMinEpochIterations = 20;
      config.mWarmup = 10;

      /* Smaller things. */
      //config.mTimeUnit = std::chrono::nanoseconds{ 1 };
      //config.mMinEpochIterations = 1000000;
      //config.mWarmup = 1000;

      ankerl::nanobench::Bench().config(config).run(label, [&] {
        auto const res(bs.call0(f));
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    }
    else
    {
      throw std::runtime_error{ fmt::format("not callable: {}", bs.to_string(f)) };
    }
    return obj::nil::nil_const();
  }
}
