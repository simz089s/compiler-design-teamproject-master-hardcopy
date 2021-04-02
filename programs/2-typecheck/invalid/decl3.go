package decl3

// Use of undeclared function (not declared before)
func main() {
	f() // Error: (line 5) f is not declared
}

func f() int {
	return 0
}
