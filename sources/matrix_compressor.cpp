#include "matrix_compressor/matrix_compressor.h"

#include <fpzip.h>
#include <streamvbyte.h>

#include <iostream>

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

  std::cout << "Compressed " << indexes.size() << " integers down to "
            << compressed_size << " bytes." << std::endl;

  size_t N = values.size();
  std::vector<uint8_t> compressed_values(sizeof(float) * N + 1024);
  FPZ* fpz =
      fpzip_write_to_buffer(reinterpret_cast<void*>(compressed_values.data()),
                            sizeof(float) * N + 1024);
  fpz->type = FPZIP_TYPE_FLOAT;
  fpz->prec = 0;
  fpz->nx = N;
  fpz->ny = 1;
  fpz->nz = 1;
  fpz->nf = 1;
  size_t hs = fpzip_write_header(fpz);
  size_t ds = fpzip_write(fpz, static_cast<void*>(values.data()));
  fpzip_write_close(fpz);
  std::cout << "Compressed " << N << " floats down to " << hs + ds << " bytes."
            << std::endl;
}

}  // namespace matrix_compressor
