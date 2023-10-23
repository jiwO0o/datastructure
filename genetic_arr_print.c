#include <stdio.h>



void print_int(void *elem) {
	
	printf("%d ", *(int *)elem);
}

void print_float(void *elem) {
	printf("%f ", *(float *)elem);
}

void genetic_print_arr(void *base, size_t nmemb, size_t size, void (*print)(void *)) {
	for (int i = 0; i < nmemb; i++)
		print(base + i * size);
		
	printf("\n");
}

/*int main(void) {
	
	int arr[10] = {3, 5, 54, 23, 4, 6, 78, 94, 100, 10};
	
	genetic_print_arr(arr, 10, sizeof(int), print_int);
	
	return 0;
}*/

int main(void) {
	
	float arr[10] = {3.1, 5.2, 5.4, 2.3, 4.5, 6.8, 7.8, 9.4, 10.2, 1.9};
	
	genetic_print_arr(arr, 10, sizeof(float), print_float);
	
	return 0;
}