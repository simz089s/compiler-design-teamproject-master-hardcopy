//checking if the collatz sequence starting at n will converge to 4-2-1-4 within N iterations
package collatz

func collatz() {
	var counter, m int = 0, 0;

	for n!=4 && n!=2 && n!=1 && counter < N{
		
		remainder := n;

		for remainder >= 2{
			m = remainder;
			remainder -= 2;
		}
		//m is now the value mod 2 

		if m==1{
			n = 3*n+1;
		} else if m==0{
			n = n/2;
		} else{
			print("Error whoever wrote this messed up\n");
		}

		counter++ ;

	}

	if counter >= N{
		print("Did not converge within"+N+" iterations.");
	}
	if n==4 || n==2 || n==1{
		print("Converged in ");	
		print(counter);	
		print("steps.");	
	}
}
