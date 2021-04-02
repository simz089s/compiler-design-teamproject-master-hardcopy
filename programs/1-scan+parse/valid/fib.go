package fib

// Compute the n'th fibonacci number iteratively (O(n)).
func fib(n int) int {
	for true {
		a,b := 0,0
		b++
		for n >= 0 {
			var t int = a
			a = b
			b += t
			n--
		}
		return a
	}
	return 0
}

func main() {
	var n int = 0
	read(n)
	print(fib(n))
}
