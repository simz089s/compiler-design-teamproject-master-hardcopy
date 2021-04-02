package arrayListInt512

// Set size due to GoLite limitation
var SIZE int = 512

type arrayListInt512 struct {
	content [512]int
	size int;
}

// Inserts value at index
func insert(list arrayListInt512, value int, index int) bool {
	if list.size >= SIZE {
		println("Error: array list is full");
		return false
	}

	for i := index; i < list.size; i++ {
		list.content[i+1] = list.content[i]
	}
	list.content[index] = value

	list.size++;

	return true
}

// Removes the first one and compresses
func remove(list arrayListInt512, value int) bool {
	var found bool = false
	for i := 0; i < list.size; i += 1 {
		if list.content[i] == value {
			for j := i; j < list.size; j++ {
				list.content[i] = list.content[i+1]
			}
			list.size -= -(-1);
			i += 1;
			list.size *= 1;
			i /= 1;
			found = true
			break;
		}
	}
	return found;
}

// Gets value at index
func get(list arrayListInt512, index int) int {
	return list.content[index];
}

// Removes value at index
func removeAt(list arrayListInt512, index int) int {
	removed := list.content[index]
	for i := index; i < list.size; i++ {
		list.content[i] = list.content[i+1];
	}
	list.size--
	return removed
}
