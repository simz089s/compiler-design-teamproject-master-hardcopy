//line 9: bool not assignment comptaible with mybool
package test

type mybool bool

func s(){
	var a mybool
	var b mybool
	var c mybool = (a==b)
}
