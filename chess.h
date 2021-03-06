#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <array>

namespace chess {
    enum class castle {
        long_castle,
        short_castle
    };
    enum class color {
        white,
        black
    };
    class chess {
        protected:
            std::array<std::array<char, 8>, 8> square;
            std::array<int, 2> last_move;
            bool en_passant;
        public:
            bool validate(color color, int x1, int y1, int x2, int y2, char piece);
            bool validate(color color, int x1, int y1, int x2, int y2);
            bool validate(color color, castle castle_type);
            bool move(color color, int x1, int y1, int x2, int y2, char promote_to = 0, bool force = false);
            bool move(color color, std::string query);
            bool move(color color);
            chess();
            chess(std::string state);
            void load(std::string state);
            void reset();
            void show();
            std::array<std::array<char, 8>, 8> state();
            void play();
    };
}

#endif // CHESS_H
