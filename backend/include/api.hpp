#include "state_machine.hpp"

#include <c3/nu/data/encoders/json.hpp>

namespace oxidisingocelots {
  class ipc_channel {
  public:
    virtual void send(const c3::nu::obj_struct& msg) = 0;
    virtual c3::nu::obj_struct receive() = 0;

  public:
    virtual ~ipc_channel() = default;
  };

  class state_machine_api {
  public:
    state s = std::vector<player>();

  public:
    static std::map<std::string,
                    std::function<c3::nu::obj_struct(const c3::nu::obj_struct&, state& s)>> funcs;

  public:
    std::string handle(std::string_view);
  };
}
