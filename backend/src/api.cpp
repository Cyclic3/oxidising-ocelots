#include "api.hpp"

#define OCELOT_HANDLER(ACTION, DESC, STRUCT_VAR, STATE_VAR) \
  c3::nu::obj_struct ocelot_action_##ACTION(const c3::nu::obj_struct&, state_machine_api&); \
  static auto _ocelot_action_##ACTION##_funcs = state_machine_api::funcs.emplace(#ACTION, ocelot_action_##ACTION); \
  static auto _ocelot_action_##ACTION##_descs = descs.emplace(#ACTION, DESC); \
  c3::nu::obj_struct ocelot_action_##ACTION(const c3::nu::obj_struct& STRUCT_VAR, state_machine_api& STATE_VAR)

#define REQUIRE_STATE(STATE) \
  if (!STATE.s) throw std::runtime_error("State not initialised!");

namespace oxidisingocelots {
  decltype(state_machine_api::funcs) state_machine_api::funcs;

  std::map<std::string, std::string> descs;

  OCELOT_HANDLER(finish, "Finish the current player's go", _, s) {
    (void)_;
    REQUIRE_STATE(s);
    s.s->finish();

    return {};
  }

  OCELOT_HANDLER(debug, "Set debug mode to `on` (default true)", obj, s) {
    if (auto ptr = obj.try_get_child("on"))
      s.debug = ptr->as<bool>();
    else
      s.debug = true;

    return {};
  }

  OCELOT_HANDLER(kill, "Remove `player`", obj, s) {
    REQUIRE_STATE(s);
    s.s->kill(static_cast<player_id>(obj.get_child("player").as<c3::nu::obj_struct::int_t>()));

    return {};
  }

  OCELOT_HANDLER(init, "Reset the state, with given `players` as an array of player ids", obj, s) {
    std::vector<player> players;
    for (auto i : obj.get_child("players").as<c3::nu::obj_struct::arr_t>())
      players.emplace_back(i.as<c3::nu::obj_struct::int_t>());
    s.s = std::make_unique<state>(std::move(players));
    return {};
  }

  OCELOT_HANDLER(dump, "Dump the current state", _, s) {
    (void)_;
    REQUIRE_STATE(s);
    c3::nu::obj_struct ret;
    ret["current_player"] =s.s->current_player().id;
    ret["goes_left"] = s.s->f.goes_left;
    {
      auto& players = ret["players"];
      for (auto& i :s.s->players) {
        auto& p = players[std::to_string(i.id)];
        auto& priv = p["hand"].as<c3::nu::obj_struct::arr_t>();
        for (auto card : i.hand)
          priv.push_back(static_cast<int>(card));
        auto& fut = p["last_grab"];
        if (i.last_grab) {
          fut["go"] = i.last_grab->go;
          auto& fut_obs = fut["observed"].as<c3::nu::obj_struct::arr_t>();;
          for (auto card : i.last_grab->observed)
            fut_obs.push_back(static_cast<int>(card));
        }
      }
    }

    return ret;
  }

  OCELOT_HANDLER(play, "Play `card`, with an optional `player`, defaulting to the current player", obj, s) {
    REQUIRE_STATE(s);
    card c = static_cast<card>(obj.get_child("card").as<c3::nu::obj_struct::int_t>());
    player_id id;
    if (auto id_ptr = obj.try_get_child("player"))
      id = id_ptr->as<c3::nu::obj_struct::int_t>();
    else
      id =s.s->current_player().id;

    auto& player = s.s->get_player(id);

    if (auto iter = player.hand.find(c); iter != player.hand.end()) {
      if (auto params = obj.try_get_child("params"))
        s.s->play(c, *params);
      else
        s.s->play(c);
    }
    else
      throw std::runtime_error("Player does not have the requested card");

    return {};
  }

  OCELOT_HANDLER(card_name, "Get the name of `card`", obj, _1) {
    (void)_1;

    card c = static_cast<card>(obj.get_child("card").as<c3::nu::obj_struct::int_t>());

    return card_names[c];
  }

  OCELOT_HANDLER(cards, "Get a list of cards names, their ids, and their parameters", _1, _2) {
    (void)_1,(void)_2;
    c3::nu::obj_struct ret;

    for (auto i : card_names) {
      auto& c = ret[i.second];
      c["id"] = static_cast<c3::nu::obj_struct::int_t>(i.first);
      auto& params = c["params"];
      for (auto param : card_params.at(i.first))
        params[param.first] = param.second;
    }

    return ret;
  }

  OCELOT_HANDLER(help, "List all the valid actions", _1, _2) {
    (void)_1,(void)_2;
    c3::nu::obj_struct ret;
    for (auto& i : descs)
      ret.emplace_child(i.first, i.second);
    return ret;
  }

  std::string state_machine_api::handle(std::string_view str) {
    c3::nu::obj_struct ret;

//    try {
      c3::nu::obj_struct input = c3::nu::json_decode(str);
      const std::string& action = input.get_child("action").as<std::string>();
      if (auto iter = funcs.find(action); iter != funcs.end()) {
        ret["result"] = iter->second(input, *this);
        ret["succeeded"] = true;
      }
      else
        throw std::runtime_error("Requested action not found");
//    }
//    catch(std::exception& exn) {
//      ret["succeeded"] = false;
//      ret["result"] = std::string(exn.what());
//      if (debug)
//        throw;
//    }


    return c3::nu::json_encode(ret);
  }
}
