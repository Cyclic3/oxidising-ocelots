#include "card.hpp"
#include "player.hpp"
#include "state_machine.hpp"

#include <random>

namespace oxidisingocelots {
  std::random_device rng;

  void state::shuffle() {
    std::shuffle(deck.begin(), deck.end(), rng);
  }

  card player::take_random_card() {
    if (hand.size() == 0)
      throw std::runtime_error("Empty hand");

    std::uniform_int_distribution<ptrdiff_t> dist(0, hand.size() - 1);

    auto target = hand.begin();
    std::advance(target, dist(rng));

    return hand.extract(target).value();
  }


  void state::_oxidise(player_id id) {
    // Check if the player has a defuse, and if so, play it
    if (get_player(id).defuse()) {
      // Including the last card
      std::uniform_int_distribution<ptrdiff_t> dist(0, deck.size());
      deck.insert(deck.begin() + dist(rng), card::OxidisingOcelot);
    }
    // Otherwise, they will die
    else
      throw oxidation(state::kill(id));
  }

  decltype(flow::pos) flow::random() {
    std::uniform_int_distribution<size_t> dist(0, n_players - 1);
    return dist(rng);
  }

  void state::play(card&& c) {
    switch (c) {
      case (card::Skip): {
        // Skip us
        f.finish();
      } break;
      case (card::Attack): {
        // Attack
        f.reset_next_player_goes([]() { return std::pair{ 2, clearup::Reset }; });

        // Skip us
        f.finish();
      } break;
      case (card::Shuffle): shuffle(); break;
      case (card::Reverse): {
        f.reset_step([](size_t step) { return std::pair{ step * -1, clearup::Reset }; });
      } break;
      case (card::ErraticElephant): {
        f.reset_step([=](size_t) {
          std::uniform_int_distribution<ptrdiff_t> dist(0, f.n_players - 1);
          return std::pair{ dist(rng), clearup::Reset };
        });
        f.reset_next_player_goes([]() { return std::pair{ 2, clearup::Reset }; });
      } break;
      case (card::Steal): {
        current_player().pick_up(random_player().take_random_card());
      } break;
      default:
        throw std::runtime_error("Invalid card");
    }
  }
}
