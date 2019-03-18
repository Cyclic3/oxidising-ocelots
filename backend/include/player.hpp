#pragma once

#include <cstdint>
#include <set>

#include "card.hpp"

namespace oxidisingocelots {

  using player_id = uint32_t;

  class player {
  public:
    player_id id;
    std::multiset<card> hand = {};
    std::multiset<card> public_hand = {};

  private:
    card _take_random_card();
    inline void _reveal_card(card&& c) {
      public_hand.emplace(std::move(c));
    }
    void _oxidise();

  public:
    inline void pick_up(card&& c) {
      hand.emplace(std::move(c));
    }
    inline void reveal_random_card() {
      _reveal_card(_take_random_card());
    }
    bool defuse();

  public:
    player(player_id id) : id{id} {}
  };

  class oxidation {
  public:
    player oxidised;

  public:
    oxidation(player oxidised) : oxidised(std::move(oxidised)) {}
  };
}
