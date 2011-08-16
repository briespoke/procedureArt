#include <stdlib.h>
float randRange(float lower, float upper)
{
	if (lower > upper)
	{
		float tmp;

		tmp = upper;
		upper = lower;
		lower = tmp;
	}
	float value = rand() * 1.0;
	float valueFloat = value / RAND_MAX;
	return (upper - lower) * valueFloat + lower;
}
