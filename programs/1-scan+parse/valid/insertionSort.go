package insertionSort

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
    slc = append(slc, 2)
    slc = append(slc, 3)
    slc = append(slc, 1)
    insertionSort(slc)
}
