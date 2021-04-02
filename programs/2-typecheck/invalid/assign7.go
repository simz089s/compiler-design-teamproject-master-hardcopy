package assign7

// Comparing different type variables lhs and rhs in binary expression (for an assignment)
type num int
type num2 int
var x num
var y num2
func main() {
	b := x == y // Error: (line 9) assigment lhs type is incompatible with rhs type [num != num2]
}