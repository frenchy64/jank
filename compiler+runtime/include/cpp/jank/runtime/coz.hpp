#pragma once

#include <jank/runtime/object.hpp>

namespace jank::runtime::coz
{
  object_ptr progress();
  object_ptr progress_named(object_ptr const);
  object_ptr begin(object_ptr const);
  object_ptr end(object_ptr const);
}
