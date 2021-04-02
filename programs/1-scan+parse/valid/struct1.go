package struct1

type num int

type point struct {
	x, y float64
	a,b num
}

type (
	num2 int
	point2 struct {
		x, y float64
	}
	p point
)

type st struct {
	x,y float
	a,b num2
	c,d point2
	x,y,z p
}

type (
	natural int
	positives []int
	rational float64
	decimals [5]float64
)

type st2 struct {
	q natural
	w positives
	r rational
	t decimals
	
	sts []st
	decimalss [3]decimals
	positivess []positives
}

func main() {
	var thing st2
}
