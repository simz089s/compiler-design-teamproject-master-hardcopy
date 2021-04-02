package gcd

// Calculate the greatest common divisor of m and n
func gcd(m,n int) int {
  if m == 0 && n == 0 { return 0; } else {
    if m == 0  { return n; } else {
      if m < 0 { m = -m; }
      if n < 0 { n *= -1; }
      if n < m { m,n = n,m; }

      var divisor,a int = 1,1

      for a <= m {
        var m_mod_a_rem,n_mod_a_rem int
        n_mod_a_rem,m_mod_a_rem = 0,0

        m_mod_a_rem = m % a
        n_mod_a_rem = n % a

        if m_mod_a_rem == 0 && n_mod_a_rem == 0 { divisor = a; }
        a++
      }

      return divisor
    }
  }
}

func main() {
  var m,n int
  read(m)
  read(n)
  print(gcd(m, n))
}
