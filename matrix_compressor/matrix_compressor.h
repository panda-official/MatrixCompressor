#pragma once

#include <blaze/Blaze.h>

namespace matrix_compressor {
void compress(const blaze::CompressedVector<float>& vector);
}  // namespace matrix_compressor
