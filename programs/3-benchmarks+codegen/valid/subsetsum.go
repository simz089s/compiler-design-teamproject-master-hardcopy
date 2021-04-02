//~false
//~true
//~true
//~true
//~true


//given an set of n numbers and a target T, is there a subset that sums to T? 

package main

var n int = 50


func populateArray() [50]int {
	var array [50]int;
	var x int = 1
	array[0] = 33;
	for x<n {
		array[x] = (10*(x%3)+ array[x-1]*x+(x<<1)*x)%3000;
		x++;
	}
	
	return array
}

func subsetSum(set [50]int, T int, numNums int) bool{
	

	if(T==0) { return true; }
	if(numNums==0 && T !=0 ) { return false; }

	if(set[numNums-1] > T) { return subsetSum(set, T, numNums-1); }

	return subsetSum(set, T, numNums-1) || subsetSum(set, T-set[numNums-1], numNums-1);

}

func main(){

	x := populateArray();
	println(subsetSum(x, 13, n))
	println(subsetSum(x, 930, n))
	println(subsetSum(x, 1111, n))
	println(subsetSum(x, 27391, n))
	println(subsetSum(x, 33877, n))

}

