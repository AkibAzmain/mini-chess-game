#include "chess.h"
#include <stdexcept>
#include <sstream>
#include <cmath>

using namespace std;

// Namespace containing chess class
namespace chess {

    // This function checks whether a move a valid for a specific piece
    bool chess::validate(color color, int x1, int y1, int x2, int y2, char piece) {

        // Check if move is valid for king
        if (piece == 'K') {
            if (abs(x1 - x2) == 1 || abs(y1 - y2) == 1) {
                return true;
            }

        // Check if move is valid for queen
        } else if (piece == 'Q') {

            // If the move is valid for either rook or bishop, it is also valid for queen
            if (validate(color, x1, y1, x2, y2, 'R') || validate(color, x1, y1, x2, y2, 'B')) {
                return true;
            }

        // Check if move is valid for bishop
        } else if (piece == 'B') {

            // Check if bishop is moving diagonally
            if (abs(x1 - x2) == abs(y1 - y2)) {

                // Check if all squares between bishop's current position and destination are empty, if not, move isn't valid
                int distance = abs(x1 - x2), x_increment = -(distance / (x1 - x2)), y_increment = -(distance / (y1 - y2));
                for (int x = x_increment, y = y_increment; abs(x) < abs(distance); x += x_increment, y += y_increment) {
                    if (square[y1 + y][x1 + x] != ' ') {
                        return false;
                    }
                }
                return true;
            }

        // Check if move is valid for knight
        } else if (piece == 'N') {
            if ((abs(x1 - x2) == 2 && abs(y1 - y2) == 1) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 2)) {
                return true;
            }

        // Check if move is valid for rook
        } else if (piece == 'R') {

            // Check if rook is moving either side by side or lengthwise, but not both
            if (!(x1 - x2 && y1 - y2) && (x1 - x2 || y1 - y2)) {

                // Distance between rook's current position and destination
                int distance = -((x1 - x2) + (y1 - y2));

                // Check if all squares between rook's current position and destination are empty, if not, move isn't valid
                for (int i = abs(distance) / distance; abs(i) < abs(distance); i += (abs(distance) / distance)) {

                    // If rook is moving side by side
                    if (x1 - x2) {
                        if (square[y1][x1 + i] != ' ') {
                            return false;
                        }

                    // If rook is moving lengthwise
                    } else {
                        if (square[y1 + i][x1] != ' ') {
                            return false;
                        }
                    }
                }

                // Every square between rook's current position and destination is empty, so move is valid
                return true;
            }

        // Check if move is valid for pawn
        } else if (piece == 'P') {

            // Check if pawn moving to next square
            if ((color == color::white && y2 - y1 == 1) || (color == color::black && y1 - y2 == 1)) {

                // Check if pawn isn't leaving it's current file
                if (x1 - x2 == 0 && square[y2][x2] == ' ') {
                    return true;

                // Check if pawn is moving to a file next to the current file
                } else if (abs(x1 - x2) == 1) {

                    // Check if pawn if capturing
                    if (square[y2][x2] != ' ') {
                        return true;

                    // Check if move is an en passant
                    } else if (last_move[0] == y1 && last_move[1] == x2) {
                        if ((color == color::white && square[y1][x2] == 'p') || (color == color::black && square[y1][x2] == 'P')) {
                            return (en_passant = true);
                        }
                    }
                }

            // Check if pawn is moving two squares, not leaving current file and this is the first move of the pawn
            } else if ((color == color::white && y2 - y1 == 2 && y1 == 1) || (color == color::black && y1 - y2 == 2 && y1 == 6)) {

                // Check if destination square and the square between destination and pawn's current place is empty
                if (!(x1 - x2) && square[(y1 + y2) / 2][(x1 + x2) / 2] == ' ' && square[y2][x2] == ' ') {
                    return true;
                }
            }
        }

        // Tests failed, so return false
        return false;
    }

    // This function checks if castling is possible
    bool chess::validate(color color, castle castle_type) {
        int rank, rook_pos;
        char king, rook;

        // Set the color of king and rook
        if (color == color::white) {
            rank = 0;
            king = 'K';
            rook = 'R';
        } else {
            rank = 7;
            king = 'k';
            rook = 'r';
        }

        // Check if king is in correct place
        if (square[rank][4] != king) {
            return false;
        }

        // Set the value of rook_pos variable, containing rook's current position
        if (castle_type == castle::long_castle) {
            rook_pos = 0;
        } else {
            rook_pos = 7;
        }

        // Check if rook is in correct place
        if (square[rank][rook_pos] == rook) {

            // Check if every squares between king and rook is empty, return false if not
            for (int i = 5; i < rook_pos; i++) {
                if (square[rank][i] != ' ') {
                    return false;
                }
            }
        }

        // All tests passed, so castling is legal
        return true;
    }

    // This function checks if move is valid (except castling)
    bool chess::validate(color color, int x1, int y1, int x2, int y2) {

        // Declare some required variables for validating
        bool valid = false, correct_piece, capture_own_piece;

        // Check if square is in range
        if ((x1 >= 0 && x1 < 8) && (y1 >= 0 && y1 < 8) && (x2 >= 0 && x2 < 8) && (y2 >= 0 && y2 < 8)) {

            // Check if the square from where piece will be moved is not empty
            if (square[y1][x1] != ' ') {

                // Prepare for checking if the player is moving his own piece
                if (color == color::white) {
                    correct_piece = square[y1][x1] != tolower(square[y1][x1]);
                } else {
                    correct_piece = square[y1][x1] != toupper(square[y1][x1]);
                }

                // Check if the player is moving his own piece
                if (correct_piece) {

                    // Check if two squares, from where piece will move and where piece will placed, are not same
                    if (y1 != y2 || x1 != x2) {

                        // Prepare for checking if player is capturing his own piece
                        if (color == color::white) {
                            capture_own_piece = square[y2][x2] != tolower(square[y2][x2]);
                        } else {
                            capture_own_piece = square[y2][x2] != toupper(square[y2][x2]);
                        }

                        // Check if player is capturing his own piece
                        if (!capture_own_piece) {

                            // Do piece specific tests
                            valid = validate(color, x1, y1, x2, y2, toupper(square[y1][x1]));
                        }
                    }
                }
            }
        }
        return valid;
    }

    // This function works with the board directly
    bool chess::move(color color, int x1, int y1, int x2, int y2, char promote_to, bool force) {

        // Do an essensial test, check if move is forced, if yes, check if square is out of range
        // Program will fail if skipped this test, and square is out of range
        if (force) {
            for (int value : {x1, y1, x2, y2}) {
                if (value > 7 || value < 0) {
                    throw out_of_range("Square is out of range!"); // Throw exception
                }
            }
        }

        // Check if move is either forced or valid and check if it is a pawn move and it can be promoted
        if ((force || validate(color, x1, y1, x2, y2)) && !(!promote_to && toupper(square[y1][x1]) == 'P' && (y2 == 7 || y2 == 1))) {

            // Check if pawn is going to promoted, and if yes, check if the piece is correct
            bool promote = false;
            if (promote_to) {
                for (char piece : {'Q', 'R', 'B', 'N'}) {
                    if (toupper(promote_to) == piece) {
                        promote = true;
                        break;
                    }
                }

                // If piece isn't correct, move isn't valid
                if (!promote) {
                    return false;
                }
            }

            // Move the piece
            square[y2][x2] = square[y1][x1];
            square[y1][x1] = ' ';

            // Set the last_move variable to the position of destination square,
            // it is needed to check if en passant is valid at function
            // validate(color color, int x1, int y1, int x2, int y2, char piece)
            last_move = {y2, x2};

            // Check if piece is going to promoted, if yes, promote it
            if (promote) {
                if (color == color::white) {
                    square[y2][x2] = toupper(promote_to);
                } else {
                    square[y2][x2] = tolower(promote_to);
                }

            // Check if move is en passant, if yes, capture (remove) the pawn
            } else if (en_passant) {
                square[y1][x2] = ' ';
                en_passant = false;
            }
        } else {
            return false;
        }

        // Moved successfully, inform it
        return true;
    }

    // This function take the move as std::string and moves
    bool chess::move(color color, string query) {

        // Prepare for checking if move is castling
        int rank;
        if (color == color::white) {
            rank = 0;
        } else {
            rank = 7;
        }

        // Check if move is short castling
        if (query == "O-O" || query == "o-o" || query == "0-0") {
            if (!validate(color, castle::short_castle)) {
                return false;
            }
            move(color, 4, rank, 6, rank, true);
            move(color, 7, rank, 5, rank, true);
            return true;

        // Check if move is long castling
        } else if (query == "O-O-O" || query == "o-o-o" || query == "0-0-0") {
            if (!validate(color, castle::long_castle)) {
                return false;
            }
            move(color, 4, rank, 2, rank, true);
            move(color, 0, rank, 3, rank, true);
            return true;
        }

        // Declare two strings, a char and a stringstream to extract square information and move properly
        stringstream squares(query);
        string src, dest;
        char promote_to = 0;

        // Extract square information from string
        squares >> src >> dest >> promote_to;
        int x1 = tolower(src[0])-'a';
        int y1 = src[1]-'1';
        int x2 = tolower(dest[0])-'a';
        int y2 = dest[1]-'1';

        // Call move function with all informations, and return the value it returned
        return move(color, x1, y1, x2, y2, promote_to);
    }

    // This function takes input input from user with standard input and moves
    bool chess::move(color color) {

        // Declare string where move query will be stored
        string query = "";
        int i = 0;

        // Ask for move until move is valid
        do {

            // Show that move was invalid
            if (i && query != "") {
                cout << "Invaild move!" << endl;
            }
            i++;

            // Show prompt
            if (color == color::white) {
                cout << "white >>> ";
            } else {
                cout << "black >>> ";
            }

            // Get move
            getline(cin, query);

            // Return false if user entered exit
            if (query == "exit") {
                return false;
            }
        } while (!move(color, query));
        return true;
    }

    // Setup the board
    chess::chess() {
        reset();
    }

    // Setup board with state from string
    chess::chess(string state) {
        load(state);
    }

    // This function loads board state from string
    void chess::load(string state) {

        // Prepare for loading board state
        stringstream board(state);
        string line;
        bool correct_piece;

        // Load board state
        for (int y = 7; y >= 0; y--) {
            getline(board, line);
            for (int x = 0; x < 8; x++) {

                // Check if correct piece are given
                correct_piece = false;
                for (char piece : {'K', 'Q', 'R', 'B', 'N', 'P'}) {
                    if (correct_piece = (toupper(line[x]) == piece || line[x] == ' ')) break;
                }

                // If piece is correct add it
                if (correct_piece) {
                    square[y][x] = line[x];

                // If not, throw an exception
                } else {
                    throw std::logic_error("input contains invalid pieces");
                }
            }
        }
    }

    // This function will reset the board
    void chess::reset() {

        // Place white piece, except pawns
        square[0][0] = square[0][7] = 'R';
        square[0][1] = square[0][6] = 'N';
        square[0][2] = square[0][5] = 'B';
        square[0][3] = 'Q';
        square[0][4] = 'K';

        // Place black piece, except pawns
        square[7][0] = square[7][7] = 'r';
        square[7][1] = square[7][6] = 'n';
        square[7][2] = square[7][5] = 'b';
        square[7][3] = 'q';
        square[7][4] = 'k';

        // Place pawns of both colors
        char pawn;
        for (int y : {1, 6}) {

            // Change pawn color, when y = 1, pawn is while, black otherwise
            if (y == 1) {
                pawn = 'P';
            } else {
                pawn = 'p';
            }
            for (int x = 0; x < 8; x++) {
                square[y][x] = pawn;
            }
        }

        // Clear all other squares
        for (int y = 2; y < 6; y++) {
            for (int x = 0; x < 8; x++) {
                square[y][x] = ' ';
            }
        }

        // Set en_passant variable to false, or the game may misbehave
        en_passant = false;
    }

    // This function show the state of board, directly to terminal
    void chess::show() {
        cout << "  +---+---+---+---+---+---+---+---+" << endl;
        for (int y = 7; y >= 0; y--) {
            cout << y+1 << " | ";
            for (int x = 0; x < 8; x++) {
                cout << square[y][x] << " | ";
            }
            cout << endl << "  +---+---+---+---+---+---+---+---+" << endl;
        }
        cout << "    a   b   c   d   e   f   g   h  " << endl;
    }

    // This function will return the array containing board state
    std::array<std::array<char, 8>, 8> chess::state() {
        return square;
    }

    // This function will ask for move infinitely, until user types exit on prompt
    void chess::play() {

        // Declare variable containing if move should be asked again
        bool again = true;

        // Ask moves, until user exits
        while (again) {

            // Ask for move to white, and store return value to again
            again = move(color::white);

            // Check if white moved, if yes, ask for move
            if (again) {

                // Show the board
                show();

                // Black must move, otherwise he will lose his chance
                while (!move(color::black)) {
                    cout << "You must move, otherwise you'll lose your chance" << endl;
                }

                // Show the board
                show();
            }
        }
    }
}
