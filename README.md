# payloader

## How to install

It is recommended to install this project in Ubuntu 14.04 

- Install requirements

	sudo apt-get update
	sudo apt-get install yasm gcc g++ pkg-config make cmake git libgomp1 libglib2.0-dev libboost-regex-dev libboost-thread-dev libboost-system-dev libboost-test-dev libssl-dev liblzma-dev libbz2-dev liblog4cxx10-dev

- Install ffmpeg version with LHE codec (detailed instructions [https://github.com/magonzalezc/LHE/tree/lhe_develop_video here])

	git clone https://github.com/magonzalezc/LHE.git
	cd LHE
	git checkout -b lhe_develop origin/lhe_develop
	./configure --extra-cflags=-fopenmp --extra-ldflags=-fopenmp 
	sudo make
	sudo make install

- Download and compile the software

	git clone https://github.com/ging/payloader.git
	cd payloader
	./compile.sh

## How to run

- Prototype 1

	./prototype1 input_file output_file
