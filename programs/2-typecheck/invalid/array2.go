package array2

// Attempting to index a non-array variable
func main() {
	var A int
	A[0] = 1 // Error: (line 6) indexing target expects list target (slice, array) [received int]
}
