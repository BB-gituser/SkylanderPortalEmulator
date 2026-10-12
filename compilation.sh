(
    mkdir build
    cd build
    rm -rf *
    clear && cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w -DTFT_OPTIONS="TFT_ENABLE_RED;TFT_ENABLE_RESET;TFT_ENABLE_TEXT;TFT_ENABLE_SHAPES;TFT_ENABLE_ROTATE" .. && clear && make -j$(nproc)
)
