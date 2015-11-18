#clang++ -o test_dct chenidct.cpp test_dct.cpp -I/usr/local/packages/Xilinx/Vivado_HLS/2014.2/include
#./test_dct
#clang++ -o test_color colorconvert.cpp test_colorconvert.cpp -I/usr/local/packages/Xilinx/Vivado_HLS/2014.2/include
#./test_color



#clang++ -o test_all chenidct.cpp colorconvert.cpp test_dct_color.cpp -I/usr/local/packages/Xilinx/Vivado_HLS/2014.2/include
#./test_all
all: 
    g++ -o output main.cpp -I/usr/local/packages/Xilinx/Vivado_HLS/2014.2/include

