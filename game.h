extern "C" {
    // Will be provided by Javascript.
    void notify_to_view(int state);
    void print_int(int x);
}

#define FULL_BOARD -286331154
#define WIN  10
#define TIE  11
#define LOST 12
#define EMPTY_COORDINATE ' '
#define X 'x'
#define O 'o'
#define RESET 0 
#define NEXT_TURN 1
#define TRY_AGAIN 2
#define NEW_PLAYER 3
#define MAX_PLAYER 4
#define NEW_GAME 5
#define DONE 6
#define NO_PLAYER 7
#define PLAYERS 2
#define ERROR (-1)

struct Coordinate {
	int row=-1;
	int column=-1;
	char symbol;
} globalCoordinate;

long long int BOARD[3][3] = {{0x80080080, 0x40008000, 0x20000808}, {0x08040000, 0x04004044, 0x02000400}, {0x00820002, 0x00402000, 0x00200220}};

class Player {
	protected:
		long long int board = 0x0;
		Coordinate coordinate{};
	public:
		explicit Player(char symbol=' ') {
			coordinate.symbol=symbol;
		}
		bool isPlayable(Player* player, Coordinate coordinate) {
			return ((board | player->board) & BOARD[coordinate.row][coordinate.column]) == 0;
		}
		bool isBoardFull(Player* player) {
			return (board | player->board) == FULL_BOARD;
		}
		void registerCoordinate(Coordinate coordinate) {
			this->coordinate = coordinate;
		}
		void save() {
			board |= BOARD[coordinate.row][coordinate.column];
		}
		void start() {}
		bool hasWon() const {
			return (board & (board >> 1) & (board << 1)) != 0;
		}
		virtual void lose(Coordinate coordinate) {
			notify_to_view(LOST);
		}
		virtual Coordinate win() {
			notify_to_view(WIN);
			return coordinate;
		}
		virtual Coordinate tie() {
			notify_to_view(TIE);
			return coordinate;
		}
		virtual Coordinate tie(Coordinate coordinate) {
			notify_to_view(TIE);
			return coordinate;
		}
		bool askIfContinueMatch() {
			return true;
		}
		Coordinate move(Coordinate coordinate) {
			this->coordinate.row = globalCoordinate.row;
			this->coordinate.column = globalCoordinate.column;
			return this->coordinate;
		}
};


class PassivePlayer: public Player {
	public:	
		PassivePlayer(char symbol): Player(symbol) {}
		Coordinate win() override {
		       notify_to_view(LOST);
		       return coordinate;
		}
		void lose(Coordinate coordinate) override {
		       notify_to_view(WIN);
		}
};




class Board {
	private:
		char board[3][3];
		Coordinate coordinate;
	public:
		void reset() {
			for(auto & row : board)
				for(char & column : row)
					column = EMPTY_COORDINATE;
		}
		int save(Player* player) {
			coordinate = player->move(coordinate);
			if (0 > coordinate.row || 0 > coordinate.column  || coordinate.row+coordinate.column > 4  || board[coordinate.row][coordinate.column] != EMPTY_COORDINATE)
				return ERROR;
			player->save();
			board[coordinate.row][coordinate.column] = coordinate.symbol;
			if (player->hasWon())
				return WIN;
			return NEXT_TURN;
		}
		void draw() {}
		int size() {
			return 9;
		}
};




class Game {
	private:
		Board* board;
		Player** players;
		int turn = 0;
		int current_state = RESET;
		Player* currentPlayer() {
			return players[turn%PLAYERS];
		}
		
		int transit(int state) {
			if(state == NEW_GAME) {
				return currentPlayer()->askIfContinueMatch() ? RESET : DONE;
			}
			if(state == RESET) {
				board->reset();
				board->draw();
				turn = 0;
				return NEXT_TURN;
			}
			int result = 0;
			if ((result = board->save(currentPlayer())) == ERROR)
				return TRY_AGAIN;
			board->draw();
			if(result == WIN) {
				Coordinate coordinate = currentPlayer()->win();
				turn++;
				currentPlayer()->lose(coordinate);
				return NEW_GAME;
			}
			bool isTie = ++turn == board->size();
			if (isTie) {
				Coordinate coordinate = currentPlayer()->tie();
				turn++;	
				currentPlayer()->tie(coordinate);
				return NEW_GAME;
			}
			return NEXT_TURN;
		}
	public:
		Game(Board* board, Player** players): board(board), players(players){}
		int next() {
			current_state = transit(current_state);
			return current_state;
		}
};
