// given p, the probability of the event occuring,
//		 n, the number of attempts
//		 k, an integer
// calculate the probability with binomial theorem of P(X=k). 
// = (n choose k) p^k (1-p)^(n-k)


//n choose k = n!/(k!(n-k!))
package binomial;

func binom() {
	var dif,nfact,kfact int = n-k,1,1; //dif,n!,k!
	var diffact int; //(n-k)!

	var tempn,tmpk,tempdif int = n,k,dif;

	for tempn > 0{
		nfact*=tempn;
		tempn--;
	}

	for tempk > 0{
		kfact *= tempk;
		tempk--;
	}


	for tempdif > 0{
		diffact *= tempdif;
		tempdif--;
	}

	nchoosek := nfact*(kfact/diffact);

	var p_to_k float64 = p;
	counter := (1-(0+0))*1/(((((((((((((((((((1)))))))))))))))))))

	for counter < k{
		p_to_k*=p;
		counter++;
	}

	q,q_exp := 1-p,q;
	counter = 1;

	for counter < n-k{
		q_exp*=q;
		counter++;
	}

	var finalAnswer float64 = nchoosek * ((p_to_k) * q_exp);

	print("The answer is ");
	print(finalAnswer);
	println("");
}