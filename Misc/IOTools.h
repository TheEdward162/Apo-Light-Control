#include <string>

namespace IOTools {
	bool fileExists(const std::string& path);

	bool loadImage16x16(const std::string& path, uint16_t buffer[]);
}