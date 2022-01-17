#include <stdio.h>
#include <stdlib.h>


//Declaring functions
void sort(int* array, int size);
int min(int* array);
int max(int* array, int size);
int arraysum(int* array, int size);
int arrayaverage(int* array, int sum, int count);
int median(int* array, int size);
void gradecount(int* array, int size, int* A, int* B, int* C, int* D, int* F);

int main() {

	int grades[] = {0,86,37,47,14,94,69,25,54,10,1,24,91,82,5,41,5,50,48,60,39,51,58,58,72}; //Please enter grades here

	int size = sizeof(grades) / sizeof(grades[0]), minimum_grade, maximum_grade, average, median_grade, sum, A = 0, B = 0, C = 0, D = 0, F = 0;

	sort(grades, size); //sorting grades
	minimum_grade = min(grades); //minimum grade
	maximum_grade = max(grades, size); //maximum grade
	sum = arraysum(grades, size); //used for finding average
	average = arrayaverage(grades, sum, size); //average of grades
	median_grade = median(grades, size); //median grade
	gradecount(grades, size, &A, &B, &C, &D, &F);

	return 0;
}

void sort(int* array, int size) { //this function sorts the grades
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size - 1; j++) {
			if (array[j] > array[j + 1]) {
				array[j] += array[j + 1];
				array[j + 1] = array[j] - array[j + 1];
				array[j] -= array[j + 1];
			}
		}
	}
}

int min(int* array) { //min holds the lowest grade
	return array[0];
}

int max(int* array, int size) { //max holds the highest grade
	return array[size - 1];
}

int arraysum(int* array, int size) { //finding the sum of the grades
	int sum = 0;
	for (int i = 0; i < size; i++){
		sum += array[i];
	}
	return sum;
}

int arrayaverage(int* array, int sum, int count) { //finding the average of the grades
	return sum / count;
}

int median(int* array, int size) { //finding median
	if (size % 2 == 0) {
		return (array[size / 2 - 1] + array[size / 2]) / 2;
	} else {
		return array[size / 2];
	}
}

void gradecount(int* array, int size, int* A, int* B, int* C, int* D, int* F) {
	for (int i = 0; i < size; i++) {
		if (array[i] > 89) {
			*A += 1;
		} else if (array[i] > 79) {
			*B += 1;
		} else if (array[i] > 69) {
			*C += 1;
		} else if (array[i] > 59) {
			*D += 1;
		} else {
			*F += 1;
		}
	}
}