################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/MotionEstimation/RecME.cpp \
../src/MotionEstimation/TestRecMEMain2.cpp \
../src/MotionEstimation/Utilities.cpp 

OBJS += \
./src/MotionEstimation/RecME.o \
./src/MotionEstimation/TestRecMEMain2.o \
./src/MotionEstimation/Utilities.o 

CPP_DEPS += \
./src/MotionEstimation/RecME.d \
./src/MotionEstimation/TestRecMEMain2.d \
./src/MotionEstimation/Utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/MotionEstimation/%.o: ../src/MotionEstimation/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -lrt -I"../inc/MotionEstimationHeaders" -I"../inc/OperatorHeaders" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


