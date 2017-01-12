# payloader

## How to install

It is recommended to install this project in Ubuntu 14.04 

- Install requirements

  ```
	sudo apt-get update
	sudo apt-get install yasm gcc g++ pkg-config make cmake git libgomp1 libglib2.0-dev libboost-regex-dev libboost-thread-dev libboost-system-dev libboost-test-dev libssl-dev liblzma-dev libbz2-dev liblog4cxx10-dev
  ```

- Install ffmpeg version with LHE codec. Detailed instructions [here](https://github.com/magonzalezc/LHE/tree/lhe_develop_video)

  ```
	git clone https://github.com/magonzalezc/LHE.git
	cd LHE
	git checkout -b lhe_develop origin/lhe_develop
	./configure --extra-cflags=-fopenmp --extra-ldflags=-fopenmp 
	sudo make
	sudo make install
  ```

- Download and compile the software

  ```
	git clone https://github.com/ging/payloader.git
	cd payloader
	./compile.sh
  ```

## How to run

- Prototype 1

Gets a MP4 file and outputs it in other MP4 file

<img src="img/prototype1.png" width="450">

```
./prototype1 input_file output_file

```


- Prototype 2

Gets a MP4 file and outputs it in other MP4 file with decoding - encoding

<img src="img/prototype2.png" width="450">

```
./prototype2 input_file output_file

```


- Prototype 3

Gets a MP4 file and outputs it in a MLHE file

<img src="img/prototype2.png" width="450">

```
./prototype1 input_file output_file

```
