all:
	g++ -O3 -o encoder *.cpp
test_roki: encoder test_image/roki_320_240.ppm
	./encoder test_image/roki_320_240.ppm roki.jpg 320 240 0 20
test_audi: encoder test_image/audi_1280_720.ppm
	./encoder test_image/audi_1280_720.ppm audi_1280_720.jpg 1280 720 1 30
test_audi_small: encoder test_image/audi_1280_720.ppm
	./encoder test_image/audi_640_360.ppm audi_640_360.jpg 640 360 3 45
test_lena: encoder test_image/lena_512_512.ppm
	./encoder test_image/lena_512_512.ppm lena_512_512.jpg 512 512 1 45
test: test_roki test_audi test_audi_small test_lena

clean:
	rm encoder *.jpg 
