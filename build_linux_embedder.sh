
./tools/gn                              \
    --target-sysroot /home/chuyi/kraken_v2/gcc-toolchain-arm-standard/sysroot-rk-debian9       \
    --target-toolchain /home/chuyi/kraken_v2/gcc-toolchain-arm-standard/arm32-gcc8.3           \
    --target-triple arm-linux-gnueabihf \
    --linux-cpu arm                     \
    --runtime-mode debug                \
    --embedder-for-target               \
    --no-lto                            \
    --target-os linux                   \
    --arm-float-abi                     \