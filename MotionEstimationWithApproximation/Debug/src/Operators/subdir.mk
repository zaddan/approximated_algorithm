################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Operators/analysis.cpp \
../src/Operators/bam.cpp \
../src/Operators/bta.cpp \
../src/Operators/btm.cpp \
../src/Operators/compare_bmp.cpp \
../src/Operators/error_model.cpp \
../src/Operators/error_model_old.cpp \
../src/Operators/eta1.cpp \
../src/Operators/eta2.cpp \
../src/Operators/etm.cpp \
../src/Operators/fft.cpp \
../src/Operators/hw_ac.cpp \
../src/Operators/io_stat.cpp \
../src/Operators/jpeg_helper.cpp \
../src/Operators/loa.cpp \
../src/Operators/loadjpg.cpp \
../src/Operators/loadjpg_ac.cpp \
../src/Operators/operand_gen.cpp \
../src/Operators/savejpg.cpp \
../src/Operators/tb_ac.cpp \
../src/Operators/tb_ac2.cpp \
../src/Operators/tb_fft.cpp \
../src/Operators/tb_fft_bt.cpp \
../src/Operators/tb_fft_v2.cpp \
../src/Operators/tb_fft_v3.cpp \
../src/Operators/tb_fft_v4.cpp \
../src/Operators/tb_jpg.cpp \
../src/Operators/tb_mac_chernoff.cpp \
../src/Operators/tb_mac_example.cpp 

OBJS += \
./src/Operators/analysis.o \
./src/Operators/bam.o \
./src/Operators/bta.o \
./src/Operators/btm.o \
./src/Operators/compare_bmp.o \
./src/Operators/error_model.o \
./src/Operators/error_model_old.o \
./src/Operators/eta1.o \
./src/Operators/eta2.o \
./src/Operators/etm.o \
./src/Operators/fft.o \
./src/Operators/hw_ac.o \
./src/Operators/io_stat.o \
./src/Operators/jpeg_helper.o \
./src/Operators/loa.o \
./src/Operators/loadjpg.o \
./src/Operators/loadjpg_ac.o \
./src/Operators/operand_gen.o \
./src/Operators/savejpg.o \
./src/Operators/tb_ac.o \
./src/Operators/tb_ac2.o \
./src/Operators/tb_fft.o \
./src/Operators/tb_fft_bt.o \
./src/Operators/tb_fft_v2.o \
./src/Operators/tb_fft_v3.o \
./src/Operators/tb_fft_v4.o \
./src/Operators/tb_jpg.o \
./src/Operators/tb_mac_chernoff.o \
./src/Operators/tb_mac_example.o 

CPP_DEPS += \
./src/Operators/analysis.d \
./src/Operators/bam.d \
./src/Operators/bta.d \
./src/Operators/btm.d \
./src/Operators/compare_bmp.d \
./src/Operators/error_model.d \
./src/Operators/error_model_old.d \
./src/Operators/eta1.d \
./src/Operators/eta2.d \
./src/Operators/etm.d \
./src/Operators/fft.d \
./src/Operators/hw_ac.d \
./src/Operators/io_stat.d \
./src/Operators/jpeg_helper.d \
./src/Operators/loa.d \
./src/Operators/loadjpg.d \
./src/Operators/loadjpg_ac.d \
./src/Operators/operand_gen.d \
./src/Operators/savejpg.d \
./src/Operators/tb_ac.d \
./src/Operators/tb_ac2.d \
./src/Operators/tb_fft.d \
./src/Operators/tb_fft_bt.d \
./src/Operators/tb_fft_v2.d \
./src/Operators/tb_fft_v3.d \
./src/Operators/tb_fft_v4.d \
./src/Operators/tb_jpg.d \
./src/Operators/tb_mac_chernoff.d \
./src/Operators/tb_mac_example.d 


# Each subdirectory must supply rules for building sources it contributes
src/Operators/%.o: ../src/Operators/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -lrt -I"../inc/OperatorHeaders" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


