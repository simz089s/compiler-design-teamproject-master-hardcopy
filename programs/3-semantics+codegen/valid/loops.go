//~3 0
//~4 2
//~6 6
//~10 14
//~3
//~4
//~2
//~1
package main

//scope rules: the new a inside the for loop should not be visible outside.
func another(){

	var a int = 2;

	for a,b:=3,0;a<5;a,b = a+1,b+1 {
		println(a)
	}

	println(a)
	a = 1
	println(a)


}

func main(){

	var a,b int = 3,0
	for ;a<10;a,b = a+1,b+2 {
		if(b>a){
			continue;
		}
		println(a,b)
	}
	println(a,b)

	for c:=3; c<4; c++ {
	}

	//c shortdec in for loop should not be visible in current scope.
	//this shortdec will only succeed if c is not defined in this scope (it should succeed)
	c:=1
	c = c+1

	another();

}

