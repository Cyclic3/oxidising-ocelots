#pragma once

#include <cstdint>
#include <set>

#include "card.hpp"

namespace oxidisingocelots {

  using player_id = uint32_t;

  class player {
  public:
    player_id id;
    std::string name;
    std::multiset<card> hand = {};

  public:
    void pick_up(card&& c) {
      hand.emplace(std::move(c));
    }

  public:
    player(player_id id, std::string name) : id{id}, name{name} {}
  };
}
