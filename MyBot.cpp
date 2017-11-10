#include "hlt/hlt.hpp"
#include "bot/BotAlgo.h"

int main() {
    const hlt::Metadata metadata = hlt::initialize("ocdy1001");
    const hlt::PlayerId player_id = metadata.player_id;
    hlt::Map initial_map = metadata.initial_map;

    std::vector<hlt::Move> moves;
    bot::BotAlgo bot(player_id);
    while(true) {
        hlt::Map map = hlt::in::get_map();
        moves.clear();

        bot.Moves(map, moves);
        if (!hlt::out::send_moves(moves)) {
            hlt::Log::log("send_moves failed; exiting");
            break;
        }
    }
}