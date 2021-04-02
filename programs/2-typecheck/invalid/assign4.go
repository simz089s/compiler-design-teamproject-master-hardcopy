package assign4

// Incompatible assignment operation and variable type
func main() {
	var s string = "o"
	s /= "k" // Error: (line 6) incompatible type in assign op /= [received string, expected numeric (int, rune, float64)]
}
