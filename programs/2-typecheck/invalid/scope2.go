//line 9 b is not declared (out of scope)
package test

func s(){
	var x int
	if x<3 {
		var b int
	} else{
		b = 3
	}
}