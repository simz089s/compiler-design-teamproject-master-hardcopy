package decl5

// Creating a type based on a variable
func main() {
	var x int
	type t x // Error: (line 6) x is not a type
}
