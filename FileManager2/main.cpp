#include "fileman.h"

Console console;

void ErrorMessage(std::wstring text)
{
	console.showError(text);
}

int main() {
	console.work();
}