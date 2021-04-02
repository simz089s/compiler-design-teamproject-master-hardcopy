//line 13: mybool not assignment comptaible with bool
package test

type mybool bool

var a struct{
	field bool
}

func s(){
	var b mybool
	var c mybool
	a.field = b
}
