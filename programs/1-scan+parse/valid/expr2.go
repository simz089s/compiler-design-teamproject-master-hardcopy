package expr2

// valid operators, integer
func int_op_test() {
	var x int;
	x = 2 + 3;
	x = 2 - 3;
	x = 2 * 3;
	x = 2 / 3;
}

// valid floating point operations
func float_op_test() {
	var y float64;
	y = 0.0 + 0.0;
	y = 0.0 - 0.0;
	y = 0.0 * 0.0;
	y = 0.0 / 0.0;
}

// valid string operations
func string_op_test() {
	var s string;
	s = "string" + "string";
	s = "string" * 3;
}

// valid logical operators
func logical_op_test() {
	var b bool;
	var z int = 0;
	b = z == 0;
	b = z != 0;
	b = b && z == 0;
	b = b || z != 0;
	b = z > 0;
	b = z >= 0;
	b = z <= 0;
	b = z < 0;
}
