//line 9: myint not type compatible with int
package test

type myint int

func s(){
	var a myint
	var b myint
	var c int = (a+b)
}
