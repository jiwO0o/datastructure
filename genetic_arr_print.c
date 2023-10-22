#include <stdio.h>

void print_arr(int *base, int size) {
	for (int i = 0; i < size; i++)
		printf("%d ", *(base+i));
	printf("\n");
}

void print_int(void *elem) {
	printf("%d ", *(int *)elem);
}

/* void print_float(void *elem) {
	printf("%f ", *(float *)elem);
} */

void genetic_print_arr(void *base, size_t nmemb, size_t size, void (*print)(void *)) {
	for (int i = 0; i < memb; i++)
		print(base+i*size);
		
	printf("\n");
}

int main(void) {
	
	int arr[10] = {3, 5, 54, 23, 4, 6, 78, 94, 100, 10};
	
	genetic_print_arr(arr, 10, sizeof(int), print_int);
	
	return 0;
}

/* void print_arr(int base[], int size) {
	for (int i = 0; i < size; i++)
		printf("%d ", base[i]);
	printf("\n");
}

int main(void) {
	
	int arr[10] = {3, 5, 54, 23, 4, 6, 78, 94, 100, 10};
	
	print_arr(arr, 10);
	
	return 0;
} 위에랑 같은 의미 */