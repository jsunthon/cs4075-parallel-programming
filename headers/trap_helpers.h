double Trap(double, double, int, double);
double f(double);

//Compute the trapezoidal area
double Trap(
	double left_endpt,
	double right_endpt, 
	int trap_count, 
	double base_len) {

	double estimate, x;
	int i;

	estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
	for (i = 1; i <= trap_count - 1; i++) {
		x = left_endpt + i*base_len;
		estimate += f(x);
	}
	estimate = estimate*base_len;
	return estimate;
}

//Graph of y = x^2
double f(double x) {
	return x*x;
}