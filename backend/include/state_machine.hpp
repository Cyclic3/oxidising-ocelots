#pragma once

#include <queue>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <array>
#include <functional>

#include "card.hpp"
#include "player.hpp"

namespace oxidisingocelots {
  enum class clearup {
    Reset,
    Keep
  };

  // Turing-complete flow, because screw it
  class flow {
  public:
    size_t n_players;

    size_t pos = 0;
    size_t goes_left = 1;
    ssize_t last_step = 1;

  public:
    decltype(pos) random();

  private:
    // Takes in the last step size
    //
    // returns the next player goes, and whether the flow_modifier should be kept
    std::function<std::pair<ssize_t, clearup>(ssize_t)> _step;
    static std::pair<ssize_t, clearup> default_step(size_t last) {
      return { last, clearup::Keep };
    }
  public:
    void reset_step(std::function<std::pair<ssize_t, clearup>(ssize_t)> f = default_step) {
      _step = std::move(f);
    }
  private:
    void step() {
      auto t = _step(last_step);
      if (t.second == clearup::Reset)
        reset_step();
      last_step = t.first;
    }

  private:
    // returns the next player goes, and whether the flow_modifier should be kept
    std::function<std::pair<size_t, clearup>()> _next_player_goes;
    static std::pair<card, clearup> default_draw(std::deque<card>& cs) {
      auto ret = cs.front();
      cs.pop_front();
      return { ret, clearup::Keep };
    }
  public:
    void reset_next_player_goes(std::function<std::pair<size_t, clearup>()> f = default_player_goes) {
      _next_player_goes = std::move(f);
    }
  private:
    size_t next_player_goes() {
      auto t = _next_player_goes();
      if (t.second == clearup::Reset)
        reset_next_player_goes();
      return t.first;
    }

  private:
    // returns the card, and whether the flow_modifier should be kept
    std::function<std::pair<card, clearup>(std::deque<card>&)> _draw_next;
    static std::pair<size_t, clearup> default_player_goes() {
      return { 1, clearup::Keep };
    }
  public:
    void reset_draw(std::function<std::pair<card, clearup>(std::deque<card>&)> f = default_draw) {
      _draw_next = std::move(f);
    }
    card draw_next(std::deque<card>& cs) {
      auto t = _draw_next(cs);
      if (t.second == clearup::Reset)
        reset_draw();
      return t.first;
    }
    void finish();

  public:
    inline flow(size_t n) : n_players{n} {
      reset_draw();
      reset_next_player_goes();
      reset_step();
    }
  };

  class state {
  private:
    std::deque<card> deck;
  public:
    std::vector<player> players = {};
  public:
    flow f;

  public:
    inline player& current_player() { return players[f.pos]; }
    inline player& random_player() { return players[f.random()]; }
  private:
    inline card _take_top() {
      auto ret = deck.front();
      deck.pop_front();
      return ret;
    }
    inline card _take_bottom() {
      auto ret = deck.back();
      deck.pop_back();
      return ret;
    }
    inline void _push_front(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.begin() + dist, std::move(c));
    }
    inline void _push_back(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.end() - dist, std::move(c));
    }
    void _oxidise(player_id id);

  public:
    player kill(player_id);
    void shuffle();
    void play(card&& c, const c3::nu::obj_struct& params = {});
    void deal(card&& c, player_id id) {
      if (c == card::OxidisingOcelot)
        _oxidise(id);
      else
        get_player(id).pick_up(std::move(c));
    }
    void finish() {
      // Required as f.finish will modify pos
      auto id = players[f.pos].id;
      f.finish();
      deal(f.draw_next(deck), id);
    }
    player& get_player(player_id id) {
      auto iter = std::find(players.begin(), players.end(), id);
      if (iter == players.end())
        throw std::runtime_error("Invalid player id");
      else
        return *iter;
    }

  public:
    state(std::vector<player> _players,
          int _n_extra_defuses = 2,
          int _n_extra_ocelots = 0,
          std::vector<card> _deck = initial_deck,
          int _n_dealt_cards = initial_n_dealt_cards);
  };
}

