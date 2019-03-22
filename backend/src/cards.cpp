#include "card.hpp"
#include "state_machine.hpp"

#include "rng.hpp"

#define OCELOT_ADD_CARD(CARD, NAME, COUNT, DESC) \
  inline int _##CARD##_init() { \
    card_names.emplace(card::CARD, NAME); \
    initial_deck.insert(initial_deck.end(), COUNT, card::CARD); \
    card_params[card::CARD] = {}; \
    return 0; \
  } \
  static int _##CARD##_inited = _##CARD##_init();

#define OCELOT_ADD_PARAM(CARD, PARAM, DESC) \
  static auto _##CARD##_param##__COUNTER__ = card_params[card::CARD].emplace(PARAM, DESC);

namespace oxidisingocelots {
  std::map<card, std::string> card_names;
  std::vector<card> initial_deck;
  std::map<card, std::map<std::string, std::string>> card_params;

  OCELOT_ADD_CARD(OxidisingOcelot, "Oxidising Ocelot", 0, "Kills you")
  OCELOT_ADD_CARD(Defuse, "Defuse", 0, "Replaces the ocelot into a random positiondeck")
  OCELOT_ADD_CARD(Skip, "Skip", 4, "Finishes your turn without drawing");
  OCELOT_ADD_CARD(Attack, "Attack", 4, "Makes the next player have two goes")
  OCELOT_ADD_CARD(Reverse, "Reverse", 4, "Flips the order of play")
  OCELOT_ADD_CARD(ErraticElephant, "Erratic Elephant", 4, "Finishes your turn without drawing, and play jumps to a random point")
  OCELOT_ADD_CARD(Steal, "Steal", 4, "Steals a random card from a random player");
  OCELOT_ADD_CARD(Shuffle, "Shuffle", 4, "Shuffles the deck");

  OCELOT_ADD_CARD(Defer, "Defer", 4, "Moves all of your remaining goes to the next player, and finishes your turn without drawing")

  OCELOT_ADD_CARD(Mug, "Mug", 4, "Steals a random card from a specific player");
  OCELOT_ADD_PARAM(Mug, "target", "The player ID of the targeted player")

  OCELOT_ADD_CARD(Scry, "Scry", 4, "For each player still playing, view the next top card");

  void state::play(card c, const c3::nu::obj_struct& params) {
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
        player* target;
        do target = &random_player();
        while (target->hand.size() == 0);

        current_player().pick_up(target->take_random_card());
      } break;
      case (card::Mug): {
        auto& target = get_player(params.get_child("target").as<std::string>());
        current_player().pick_up(target.take_random_card());
      } break;
      case (card::Defer): {
        f.reset_next_player_goes([=]() { return std::pair{ f.goes_left, clearup::Reset }; });
        f.finish();
      } break;
      case (card::Scry): {
        current_player().last_grab = grab(f.n_players);
      } break;
      default:
        throw std::runtime_error("Invalid card");
    }
  }
}
