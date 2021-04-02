package array3

// Attempting to append to an array
func main() {
	var A [1]int
	A = append(A, 1) // Error: (line 6) append builtin expects slice type as first argument [received [1]int]
}
