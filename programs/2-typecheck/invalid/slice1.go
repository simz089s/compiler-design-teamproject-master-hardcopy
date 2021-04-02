package slice1

// Appending string to int slice
func main() {
	var sl []int
	sl = append(sl, "pineapple pizza should be illegal") // Error: (line 6) append expression slice type is incompatible with element type [int != string]
}
