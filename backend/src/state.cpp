#include "state_machine.hpp"

namespace oxidisingocelots {
  void state::play(card&& c) {
    switch (c) {
      case (card::Skip): next_player(); break;
      case (card::Attack): next_player(); ++goes_left; break;
      case (card::Shuffle): shuffle(); break;
      case (card::Reverse): is_forward = !is_forward; break;

      default:
        throw std::runtime_error("Invalid card");
    }
  }
  player state::kill(player_id id) {
    auto iter = std::find(players.begin(), players.end(), id);
    if (iter != players.end())
      throw std::out_of_range("Invalid player id");
    auto target = std::move(*iter);
    players.erase(iter);
    // Now we fix the array
    if (is_forward)
      pos %= players.size();
    else
      pos != 0 ? --pos : pos = players.size() - 1;

    return target;
  }

  void state::_oxidise(player_id id) {
    // Check if the player has a defuse, and if so, play it
    if (get_player(id).defuse())
      return;

    // Otherwise, they will die
    // Steal the player

    // Now kill them
    throw oxidation(state::kill(id));
  }
  state::state(std::vector<player> _players,
               int _n_extra_defuses,
               int _n_extra_ocelots,
               std::vector<card> _deck,
               int _n_dealt_cards) :
      deck{_deck.begin(), _deck.end()} {
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
