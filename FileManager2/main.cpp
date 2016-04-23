#include "fileman.h"
#include "other.h"

int main() {
	// Только русские регионы
	setlocale(0, "rus");
	Console console;
	console.work();
	return 0;
}