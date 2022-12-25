#include "matrix_compressor/matrix_compressor.h"

#include <streamvbyte.h>

namespace matrix_compressor {

void compress(const blaze::CompressedVector<float>& vector) {
  std::vector<uint32_t> indexes;
  indexes.reserve(vector.nonZeros());

  std::vector<float> values;
  values.reserve(vector.nonZeros());

  for (auto it = vector.begin(); it != vector.end(); ++it) {
    indexes.push_back(static_cast<uint32_t>(it->index()));
    values.push_back(it->value());
  }

  std::vector<uint8_t> compressed;
  compressed.resize(streamvbyte_max_compressedbytes(indexes.size()));
  size_t compressed_size =
      streamvbyte_encode(indexes.data(), indexes.size(), compressed.data());
}

}  // namespace matrix_compressor
