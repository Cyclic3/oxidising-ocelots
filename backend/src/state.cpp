#include "state_machine.hpp"

namespace oxidisingocelots {
  void flow::finish() {
    if (--goes_left != 0)
      return;

    step();

    if (last_step == 0)
      return;
    else if (last_step > 0) {
      pos += n_players;
    }
    else { // (step < 0)
      if (pos == 0)
        pos = n_players - 1;
      else
        --pos;
    }

    if (pos >= n_players)
      // Provide helpful and thoughtful error message
      throw std::runtime_error("LUKSHAN YOU MESSED UP "
                               "THEY'RE TRYING TO HACK ME WITH PLAYER BOUNDS");

    // Check the next player actually has some goes
    if ((goes_left = next_player_goes()) == 0)
      finish();
  }
  player state::kill(player_id id) {
    auto iter = std::find(players.begin(), players.end(), id);
    if (iter != players.end())
      throw std::out_of_range("Invalid player id");
    auto target = std::move(*iter);
    // Reset the flow, and go to the next player
    f.reset_draw();
    f.reset_next_player_goes();
    f.reset_step();
    f.last_step = 1;

    // Skip the current players go
    f.finish();

    // Kill the player
    players.erase(iter);

    return target;
  }

  state::state(std::vector<player> _players,
               int _n_extra_defuses,
               int _n_extra_ocelots,
               std::vector<card> _deck,
               int _n_dealt_cards) :
      deck{_deck.begin(), _deck.end()}, f{_players.size()} {
    if (_players.size() == 0)
      throw std::runtime_error("Need at least 1 player!");

    shuffle();

    // Then we act on the players
    for (auto& i : _players) {
      // Add the player
      auto& player = players.emplace_back(i);
      // Deal the defuse
      player.pick_up(card::Defuse);
      // Deal n unforced cards
      for (auto i = 0; i < _n_dealt_cards; ++i)
        player.pick_up(_take_top());
    }

    // Then add in the ocelots and the defuses
    deck.insert(deck.end(), players.size() - 1 + _n_extra_ocelots, card::OxidisingOcelot);
    deck.insert(deck.end(), _n_extra_defuses, card::Defuse);
    // Shuffle the final cards
    shuffle();
  }
}
