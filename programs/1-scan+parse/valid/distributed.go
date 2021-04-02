package tc

var (
	a int
	b int // skipped in printing
)

type error string

// everything below is also skipped in printing
type ( // supposed to not print

)

var ( // supposed to not print

)

type ( // ???
	num int
	point struct {
		x, y float64
	}
)

var ( // ???
	x1, x2 int
	y1, y2 = 42, 43
	z1, z2 int = 1, 2
)