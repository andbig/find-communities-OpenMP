int increase_size_policy(int currentSize, int neededSize) {
	int size;

	size = currentSize;

	while(size < neededSize){
		size *= 2;
	}

	return size;
}
