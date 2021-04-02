//~0 0 0 0
//~0 0 0 0
//~0 1 1 0
//~1 1 1 1
//~0 2 2 0
//~1 2 2 1

package main

var a [5] int
var b []int 
var c struct { f int; e []int; }

func  foo() [5]int { 
        return a; 
}
func  bar() []int { 
        return b; 
}

func  baz()  struct{ f int; e []int; } { 
        return c; 
}

func ah(a [5]int, b []int, c struct{f int; e[] int; }){
        a[0], b[0], c.e[0], c.f = 2,2,2,2
} 

func  main() {
        b = append(b, 0)
        c.e = append(c.e, 0)
		var d, e, f = foo(), bar(), baz()
		
		println(a[0], b[0], c.e[0], c.f)
		println(d[0], e[0], f.e[0], f.f)
		
        d[0], e[0], f.e[0], f.f = 1, 1, 1, 1
        println(a[0], b[0], c.e[0], c.f)
		println(d[0], e[0], f.e[0], f.f)
		
		ah(d,e,f)
		println(a[0], b[0], c.e[0], c.f)
        println(d[0], e[0], f.e[0], f.f)
}
