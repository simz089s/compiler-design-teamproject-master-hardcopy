package lcm

// Calculate the least common multiple of m and n
func calc_lcm(m,n int) int {
    var temp_m int = m
    var temp_n int = n
    
    for temp_m != temp_n {
        if temp_m > temp_n {
            temp_n += n
        } else {
            temp_m = temp_m + m
        }
    }

    return temp_m;
}

func main() {
    print("LCM is ")
    var m,n int
    read(m)
    read(n)
    print(calc_lcm(m, n))
}
