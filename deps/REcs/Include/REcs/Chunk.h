#pragma once
#include <array>

namespace REcs
{
	struct alignas(128) Chunk
	{
	public:
		using byte = uint8_t;
		static_assert(sizeof(byte) == 1);
		static constexpr size_t size = 16 * 1024;
		byte* Data() noexcept { return buffer.data(); }

	private:
		std::array<byte, size> buffer;
	};
	static_assert(sizeof(Chunk) == Chunk::size);
}
