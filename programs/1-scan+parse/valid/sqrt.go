package sqrt

// Approximate the square root of x
var x float64 = 0.0
var guess float64 = 1.0
var iter int = 10

func main() {
	read(x)
	for iter >= 0 {
		var quot float64 = x / guess
		guess = 0.5 * (guess + quot)
		iter--
	}
	print(guess)
	print(guess * guess)
}
