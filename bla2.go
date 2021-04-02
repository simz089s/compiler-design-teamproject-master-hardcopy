package sqrt
var x float64  = 0.000000

var guess float64  = 1.000000

var iter int  = 10

func main() {
	read(x)
	for (iter >= 0) {
		var quot float64  = (x / guess)
		guess = (0.500000 * (guess + quot))
		iter--
	}
	print(guess)
	print((guess * guess))

}
