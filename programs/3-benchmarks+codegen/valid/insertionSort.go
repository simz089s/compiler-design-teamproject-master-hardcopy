//~37000, 36999, 36998, 36997, 36996, 36995, 36994, 36993, 36992, 36991, ..., 0
//~0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ..., 37000
//~
package main

// Sorts slice
func insertionSort(arr []int) {
    size := len(arr)
    for i := 1; i < size; i++ {
        j := i
        for j > 0 {
            if arr[j-1] > arr[j] {
                arr[j-1],arr[j] = arr[j],arr[j-1]
            }
            j--
        }
    }
}

func main() {
    var slc []int
    for i := 37000; i >= 0; i-- {
        slc = append(slc, i)
    }
    for i := 0; i < 10; i++ {
        print(slc[i], ", ")
    }
    println("...,", slc[37000])
    insertionSort(slc) // Around 5s according to bash command "time" on my laptop (Go playground seems to cut-off aroud 3s)
    for i := 0; i < 10; i++ {
        print(slc[i], ", ")
    }
    println("...,", slc[37000])
}
