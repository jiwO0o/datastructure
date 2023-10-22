#include <stdio.h>
#include <string.h>

void get_array(int arr[], int n) {
	
	int i;
	srand(time(NULL));
	
	for (i = 0; i < n; i++)
		arr[i] = rand() % 100 + 1;
}

void print_array( {
	
	int i;
	int *p;
	p = arr;
	
	for (i = 0; i < n; i++, p++) {
		printf("%d", 
}

void bubble (int arr[], ) {
	
	int i, j;
	int temp;
	
	for (i = 0; i < n-1; i++) {
		for (j = 0; j < nmemb-i-1; j++) {
			if (base[j] > base[j+1]) {
				temp = base[j];
				base[j] = base[j+1];
				base[j+1] = temp;
			}
		}
	}
	
}

int compare_int(const void *e1, const void *e2) {
	
	return *(int *)e1 - *(int *)e2; // 이렇게 return할 수도 있음
}

void bsort(void *base, size_t nmemb, size_t size), int (*compar)(const void *, const void *)) {
	
	int i, j;
	void *temp = malloc(sizeof(size));
		
	for (i = 0; i < n-1; i++) {
		for (j = 0; j < nmemb-i-1; j++) {
			if (compar(base+j*size, base+(j+1)*size) > 0) {
				
				//temp = base[j];
				memmove(temp, base+j*size, size); 
				
				//base[j] = base[j+1];
				memmove(base+j*size, base+(j+1)*size, size); 
				
				//base[j+1] = temp;
				memmove(base+(j+1)*size, temp, size); 
			}
		}
	}
	free(temp);
}


int main(void) {

	int arr[10];

	get_array(arr, 10);
	
	print_arary(arr, 10);
	
	//bubble(arr,10);
	bsort(arr, 10, sizeof(int), compare_int);
	
	print_array(arr, 10);
	
	return 1;
}