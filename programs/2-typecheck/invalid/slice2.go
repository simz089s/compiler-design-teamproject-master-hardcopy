package slice2

// Attempting to append to a non-slice variable
func main() {
	var sl int
	sl = append(sl, 1) // Error: (line 6) append builtin expects slice type as first argument [received int]
}
