package iffor1

var x int = 0

func main() {
	// valid if-for, nested
	if x > 0 {
		for x != 0 {
		}
	}
	
	// valid for-if, nested
	for x < 0 {
		if x != 0 {
		}
	}
}
