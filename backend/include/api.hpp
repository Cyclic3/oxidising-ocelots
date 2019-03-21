#include "state_machine.hpp"

#include <c3/nu/data/encoders/json.hpp>

namespace oxidisingocelots {
  class state_machine_api {
  public:
    std::unique_ptr<state> s;
    bool debug = false;

  public:
    static std::map<std::string,
                    std::function<c3::nu::obj_struct(const c3::nu::obj_struct&, state_machine_api& s)>> funcs;

  public:
    std::string handle(std::string_view);
  };
}
