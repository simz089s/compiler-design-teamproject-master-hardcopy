package decl7

// Short declaration with only already declared variables
func main() {
	var x,y,z int
	x,y,z := 0,0,0 // Error: (line 5) short declaration contains no new variables
}
