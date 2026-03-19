#include "history.h"

using namespace std;

optional<Match> History::rewind() {
   if (matches.empty() || cursor == 0)
     return nullopt;

   auto match = matches[--cursor];
   return match;
}

optional<Match> History::restore() {
  if (matches.empty() || cursor >= matches.size())
    return nullopt;

   auto match = matches[cursor++];
   return match;
}

std::size_t History::size() const {
  // Each match accounts for two tiles
  return cursor * 2;
}

Match::Match(Position a_pos, Tile* a_tile, Position b_pos, Tile* b_tile)
  : a(make_pair(a_pos, a_tile)),
    b(make_pair(b_pos, b_tile))
{}
