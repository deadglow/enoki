#include <chrono>

using int8   = std::int8_t;
using int16  = std::int16_t;
using int32  = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using MillisecondsU32 = std::chrono::duration<uint32, std::chrono::milliseconds::period>;
using MillisecondsS32 = std::chrono::duration<int32,  std::chrono::milliseconds::period>;
using MillisecondsU64 = std::chrono::duration<uint64, std::chrono::milliseconds::period>;
using MillisecondsS64 = std::chrono::duration<int64,  std::chrono::milliseconds::period>;
using MillisecondsF32 = std::chrono::duration<float,  std::chrono::milliseconds::period>;
using MillisecondsF64 = std::chrono::duration<double, std::chrono::milliseconds::period>;

using SecondsU32 = std::chrono::duration<uint32, std::chrono::seconds::period>;
using SecondsS32 = std::chrono::duration<int32,  std::chrono::seconds::period>;
using SecondsU64 = std::chrono::duration<uint64, std::chrono::seconds::period>;
using SecondsS64 = std::chrono::duration<int64,  std::chrono::seconds::period>;
using SecondsF32 = std::chrono::duration<float,  std::chrono::seconds::period>;
using SecondsF64 = std::chrono::duration<double, std::chrono::seconds::period>;
