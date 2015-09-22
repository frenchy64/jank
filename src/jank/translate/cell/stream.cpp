#include <jank/parse/cell/stream.hpp>
#include <jank/translate/cell/stream.hpp>
#include <jank/translate/cell/trait.hpp>
#include <jank/translate/function/argument/call.hpp>
#include <jank/detail/stream/indenter.hpp>

namespace jank
{
  using indenter = detail::stream::indenter;
  namespace translate
  {
    namespace cell
    {
      //static int indent_level{ -1 };

      template <typename T>
      static std::ostream& operator <<(std::ostream &os, T const &)
      { return std::operator<<(os, "(unimplemented) "); }

      std::ostream& operator <<(std::ostream &os, cell const &c)
      {
        switch(trait::to_enum(c))
        {
          case type::function_body:
            os << boost::get<function_body>(c).data;
            break;
          case type::function_definition:
            os << boost::get<function_definition>(c).data;
            break;
          case type::native_function_declaration:
            os << boost::get<native_function_declaration>(c).data;
            break;
          case type::function_call:
            os << boost::get<function_call>(c).data;
            break;
          case type::indirect_function_call:
            os << boost::get<indirect_function_call>(c).data;
            break;
          case type::native_function_call:
            os << boost::get<native_function_call>(c).data;
            break;
          case type::function_reference:
            os << boost::get<function_reference>(c).data;
            break;
          case type::native_function_reference:
            os << boost::get<native_function_reference>(c).data;
            break;
          case type::binding_definition:
            os << boost::get<binding_definition>(c).data;
            break;
          case type::binding_reference:
            os << boost::get<binding_reference>(c).data;
            break;
          case type::literal_value:
            //os << boost::get<literal_value>(c).data;
            break;
          case type::return_statement:
            os << boost::get<return_statement>(c).data;
            break;
          case type::if_statement:
            os << boost::get<if_statement>(c).data;
            break;
          case type::do_statement:
            os << boost::get<do_statement>(c).data;
            break;
          default:
            //os << "??? ";
            break;
        }

        return os;
      }
    }
  }
}
