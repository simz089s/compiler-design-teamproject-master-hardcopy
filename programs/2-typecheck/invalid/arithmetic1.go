package arithmetic1

// Invalid (binary) operation on strings (arithmetic)
var s string = "hello" - "lo" + "p" // Error: (line 4) incompatible type in arithmetic op - [received string, expected numeric (int, rune, float64)]
