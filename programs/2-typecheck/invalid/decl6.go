package decl6

// Using non-bool in logical operation
var x int = 1 && 1 // Error: (line 4) incompatible type in logical op && [received int, expected bool]
