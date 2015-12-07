#include <stdexcept>

// Parse string in format NUMBER[SUFFIX] as number of bytes
// where suffix is [KMG] for kibibyte, mebibyte and gibibyte respectively
size_t ParseSize(std::string str) {
  const size_t kKibi = 1 << 10;
  const size_t kMebi = 1 << 20;
  const size_t kGibi = 1 << 30;

  size_t suffix;
  size_t max_memory;

  max_memory = std::stoi(str, &suffix);

  switch (str[suffix]) {
    case '\0':
      break;

    case 'k':
    case 'K':
      max_memory *= kKibi;
      break;

    case 'm':
    case 'M':
      max_memory *= kMebi;
      break;

    case 'g':
    case 'G':
      max_memory *= kGibi;
      break;

    default:
      throw std::invalid_argument("max_memory");
      break;
  }

  return max_memory;
}
