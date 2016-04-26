#include "promo.h"

void WriteToLog(std::wstring str)
{
	static std::wofstream file;
	
	time_t t = time(0);
	if (!file.is_open()) {
		// create file, name is milliseconds now
		file.open(toString(t));
		// check
		if (!file.is_open()) return;
	}
	tm now;
	localtime_s(&now, &t);

	file << now.tm_hour << ":" << now.tm_min << ":" << now.tm_sec << " " << str << std::endl;
	file.flush();
}
