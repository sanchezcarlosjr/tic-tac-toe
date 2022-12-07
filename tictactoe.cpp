#include "clang-wasm/nanolibc/libc.h"
#include "clang-wasm/nanolibc/libc_extra.h"
#include "game.h"

#define WASM_EXPORT __attribute__((visibility("default"))) extern "C"

Game* game;

WASM_EXPORT void debug_dump_memory() {
	dump_memory();
}

WASM_EXPORT void start_match(bool isPassive) {
	Player* players[PLAYERS];
	players[0] = isPassive ? new PassivePlayer(X) : new Player(X);
	players[1] = isPassive ? new Player(O) : new PassivePlayer(O);
	game = new Game(new Board, players);
	game->next();
}


WASM_EXPORT int next(int row, int column) {
	globalCoordinate.row = row;
	globalCoordinate.column = column;
	return game->next();
}
