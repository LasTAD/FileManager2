#include "app.h"
#include "files.h"
#include <ctime>

int main() {
	srand(static_cast<int64>(time(0)));
	Console console;
	console.work();
}