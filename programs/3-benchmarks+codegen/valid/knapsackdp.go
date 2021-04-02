//~23306

package main

var n int = 36 //number of items

func max(a,b int) int{
	if (a<b) {
		return b;
	}
	return a;
}

func knapsack(maxWeight int, weightArray [36]int, valueArray [36]int, x int) int{

	if(x==0 || maxWeight==0){
		return 0;
	}

	if(weightArray[x-1] > maxWeight){
		return knapsack(maxWeight, weightArray, valueArray, x-1);
	} else{
		a := valueArray[x-1] + knapsack(maxWeight-weightArray[x-1],weightArray, valueArray, x-1)
		b := knapsack(maxWeight, weightArray, valueArray, x-1)
		var retval int = max(a,b);
		return retval;
	}
	

}

func populateArray() [36]int {
	var array [36]int;
	var x int = 1
	array[0] = 69;
	for x<n {
		array[x] = (10*(x%3)+ array[x-1]*x)%3000;
		x++;
	}
	
	return array
}
func populateArray2() [36]int {
	var array [36]int;
	var x int = 1
	array[0] = 20;
	for x<n {
		array[x] = (10*(x%7)+ 6*array[x-1]*x%3)%100;
		x++;
	}
	
	return array
}

func main(){
	
	var maxW int = 200
	
	v := populateArray();
	w := populateArray2();

	x := knapsack(maxW, w, v, n);
	println(x)



}