const fs = require('fs');
const path = require('path');


const decoder = new TextDecoder("utf-8");
const encoder = new TextEncoder("utf-8");
let wasmInstance;

function get_memory() {
	return new Uint8Array(wasmInstance.memory.buffer);
}

function charPtrToString(str) {
	const memory = get_memory();
	let length=0;
	for (; memory[str + length] !== 0 ;++length) {}
	return decoder.decode(memory.subarray(str, str + length));
}

const STATES = {10: "WIN", 11: "TIE", 12: "LOST"}

const importObject = {
		           env: {
	                      print_string: function (str) {
		                console.log(charPtrToString(str));
	                      },
			      print_int: function (str) {
		                console.log("PRINT INT", str);
	                      },
			      notify_to_view: (state) => console.log("NOTIFICATION ", STATES[state])
		           }
		        };

describe("TicTacToe Test", () => {
	beforeAll(async () => {
		const wasmPath = path.resolve(__dirname, 'public', "tictactoe.wasm");
		const buffer = fs.readFileSync(wasmPath);
		const results = await WebAssembly.instantiate(buffer, importObject);
		wasmInstance = results.instance.exports;
	});
	test('it should be next turn and call the state WIN', () => {
		wasmInstance.start_match(false);
		expect(wasmInstance.next(0,0)).toBe(1);
		expect(wasmInstance.next(1,0)).toBe(1);
		expect(wasmInstance.next(0,1)).toBe(1);
		expect(wasmInstance.next(1,1)).toBe(1);
		expect(wasmInstance.next(0,2)).toBe(5);
	});
	test('it should be next turn and call the state LOST', () => {
		wasmInstance.start_match(true);
		expect(wasmInstance.next(0,0)).toBe(1);
		expect(wasmInstance.next(1,0)).toBe(1);
		expect(wasmInstance.next(0,1)).toBe(1);
		expect(wasmInstance.next(1,1)).toBe(1);
		expect(wasmInstance.next(0,2)).toBe(5);
	});
});
