package slice3

// Attempting to append a slice to an int slice (instead of an int)
func main() {
	var sl []int
	sl = append(sl, sl) // Error: (line 6) append expression slice type is incompatible with element type [int != []int]
}
