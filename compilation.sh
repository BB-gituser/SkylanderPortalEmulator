(
    mkdir build
    cd build
    rm -rf *
    clear && cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico -DTFT_OPTIONS="TFT_ENABLE_RED;TFT_ENABLE_RESET;TFT_ENABLE_TEXT;TFT_ENABLE_SHAPES;TFT_ENABLE_ALL;TFT_ENABLE_ROTATE;TFT_ENABLE_SCROLL;" .. && make -j$(nproc)
)
