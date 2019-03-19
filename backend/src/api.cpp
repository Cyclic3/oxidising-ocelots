#include "api.hpp"

#define OCELOT_HANDLER(ACTION, DESC, STRUCT_VAR, STATE_VAR) \
  c3::nu::obj_struct ocelot_action_##ACTION(const c3::nu::obj_struct&, std::unique_ptr<state>&); \
  static auto _ocelot_action_##ACTION##_funcs = state_machine_api::funcs.emplace(#ACTION, ocelot_action_##ACTION); \
  static auto _ocelot_action_##ACTION##_descs = descs.emplace(#ACTION, DESC); \
  c3::nu::obj_struct ocelot_action_##ACTION(const c3::nu::obj_struct& STRUCT_VAR, std::unique_ptr<state>& STATE_VAR)

namespace oxidisingocelots {
  decltype(state_machine_api::funcs) state_machine_api::funcs;

  std::map<std::string, std::string> descs;

  OCELOT_HANDLER(draw, "Draw a card, with an optional `player`", obj, s) {
    if (auto id_ptr = obj.try_get_child("player"))
      s->draw(id_ptr->as<c3::nu::obj_struct::int_t>());
    else
      s->draw();

    return {};
  }

  OCELOT_HANDLER(init, "Reset the state, with given `players`", obj, s) {
    std::vector<player> players;
    for (auto i : obj.get_child("players").as<c3::nu::obj_struct::arr_t>())
      players.emplace_back(i.as<c3::nu::obj_struct::int_t>());
    s = std::make_unique<state>(std::move(players));
    return {};
  }

  OCELOT_HANDLER(dump, "Dump the current state", _, s) {
    c3::nu::obj_struct ret;
    ret["current_player"] = s->current_player().id;
    ret["goes_left"] = s->n_goes_left();
    {
      auto& players = ret["players"];
      for (auto& i : s->players) {
        auto& p = players[std::to_string(i.id)];
        auto& priv = p["hand"].as<c3::nu::obj_struct::arr_t>();
        for (auto card : i.hand)
          priv.push_back(static_cast<int>(card));

        auto& pub = p["revealed_hand"].as<c3::nu::obj_struct::arr_t>();
        for (auto card : i.revealed_hand)
          pub.push_back(static_cast<int>(card));
      }
    }

    return ret;
  }

  OCELOT_HANDLER(play, "Play `card`, with an optional `player`", obj, s) {
    card c = static_cast<card>(obj.get_child("card").as<c3::nu::obj_struct::int_t>());
    player_id id;
    if (auto id_ptr = obj.try_get_child("player"))
      id = id_ptr->as<c3::nu::obj_struct::int_t>();
    else
      id = s->current_player().id;

    auto& player = s->get_player(id);

    // Try to play a revealed card first
    if (auto iter = player.revealed_hand.find(c); iter != player.revealed_hand.end())
      s->play(std::move(c));
    // Try to play a hidden card
    else if (auto iter = player.hand.find(c); iter != player.hand.end())
      s->play(std::move(c));
    else
      throw std::runtime_error("Player does not have the requested card");

    return {};
  }

  OCELOT_HANDLER(card_name, "Get the name of `card`", obj, _1) {
    card c = static_cast<card>(obj.get_child("card").as<c3::nu::obj_struct::int_t>());

    return card_names[c];
  }

  OCELOT_HANDLER(cards, "Get a list of cards names and their id", _1, _2) {
    c3::nu::obj_struct ret;

    for (auto i : card_names)
      ret.emplace_child(i.second, static_cast<c3::nu::obj_struct::int_t>(i.first));

    return ret;
  }

  OCELOT_HANDLER(help, "List all the valid actions", _1, _2) {
    c3::nu::obj_struct ret;
    for (auto& i : descs)
      ret.emplace_child(i.first, i.second);
    return ret;
  }

  c3::nu::obj_struct state_machine_api::handle(const c3::nu::obj_struct& input) {
    const std::string& action = input.get_child("action").as<std::string>();
    if (auto iter = funcs.find(action); iter != funcs.end()) {
      return iter->second(input, s);
    }
    else
      throw std::runtime_error("Requested action not found");
  }
}
