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
    std::unique_ptr<state> s;

  public:
    static std::map<std::string,
                    std::function<c3::nu::obj_struct(const c3::nu::obj_struct&, std::unique_ptr<state>& s)>> funcs;

  public:
    c3::nu::obj_struct handle(const c3::nu::obj_struct&);
  };
}
