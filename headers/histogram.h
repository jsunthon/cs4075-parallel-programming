void print_bin_maxes(int*, int);
void print_data(double*, int);

void print_bin_maxes(int* bin_maxes, int bin_count) {
	int i;
	for (i = 0; i < bin_count; i++) {
		printf("%d\n", *(bin_maxes + i));
	}
}

void print_data(double* bin_maxes, int bin_count) {
	int i;
	for (i = 0; i < bin_count; i++) {
		printf("%lf\n", *(bin_maxes + i));
	}
}