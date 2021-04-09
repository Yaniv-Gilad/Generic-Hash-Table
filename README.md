Generic Hash Table

=== Description ===

Program files:
GenericHashTable.h - header file of the generic hash table.
GenericHashTable.c - file that implementaion a generic hash table and its Actions.

functions:
	createTable - initializes the Table struct members.
	freeTable - The function release all the allocated members of struct Table.
	add - The function adds data to the hashtable.
	removeObj - The function removes the Object which its data equals to data, if there are more than one, it removes the first one.
	search - The function search for an object that its data is equal to data and returns a pointer to that object.
	printTable - The function print the table.
	createObject - This function creates an object and return the pointer to it or NULL if there is some error.
	freeObject - This function frees an object, the data of the object should also be freed.
	isEqual - check the equality of the data of two objects.
	intHashFun - returns the hash value of an integer, which is key mod origSize.
	strHashFun - returns the hash value of an string.
    
    numOfLetters - return number of letters in text.
    isAvailbePlace - check if there is availale place in the the table.
    doubleTableSize - double the size fo the table.

	




