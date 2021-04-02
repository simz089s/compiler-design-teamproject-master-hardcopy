package func2

// Wrong return type (returning string for int function)
func f() int {
	return "penguins are pretty great" // Error: (line 5) int is not assignment compatible with string in return statement
}
