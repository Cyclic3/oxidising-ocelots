#pragma once

#include <cstdint>
#include <set>

#include "card.hpp"

namespace oxidisingocelots {
  using player_id = uint32_t;

  struct future_grab {
    std::vector<card> observed;
    size_t go;

    inline future_grab(decltype(observed) observed, decltype(go) go) :
      observed{std::move(observed)}, go{go} {}
  };

  class player {
  public:
    player_id id;
    std::multiset<card> hand;

    std::optional<future_grab> last_grab;


  private:
    /*inline void _reveal_card(card&& c) {
      revealed_hand.emplace(std::move(c));
    }*/
    void _oxidise();

  public:
    inline void pick_up(card&& c) {
      hand.emplace(std::move(c));
    }
    card take_random_card();
    bool defuse();

  public:
    inline bool operator==(player_id pid) { return id == pid; }
    inline bool operator!=(player_id pid) { return id != pid; }

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
