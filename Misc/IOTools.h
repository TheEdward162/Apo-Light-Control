#include <string>

/**
 * @brief Tools connected to IO..
 *
 */
namespace IOTools {
	/**
	 * @brief Checks whether a file exists.
	 * @param path Path to file.
	 * @return True if the file exists, false otherwise.
	 * */
	bool fileExists(const std::string& path);

    /**
	 * @brief Loads a ppm 16x16 image.
	 * @param path Path to file.
     * @param buffer An array to store the image into.
	 * @return True if the image was loaded successfully, false otherwise.
	 * */
	bool loadImage16x16(const std::string& path, uint16_t buffer[]);
}
/* namespace IOTools */