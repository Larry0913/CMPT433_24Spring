#Makefile for ARM architecture
#Copy the executable to public/myApps folder

CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
#CFLAGS =  -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

APP_NAME = $(shell basename "$(CURDIR)")
SRC_FILES = src/*.c

#Build for target, then copy to shared folder
build:
	$(CC_C) $(CFLAGS) $(SRC_FILES) -o $(APP_NAME).o
	cp $(APP_NAME).o ~/cmpt433/public/myApps/

#clean the binary file
clean:
	rm *.o
	cd ~/cmpt433/public/myApps/;rm *.o; cd -

#
#$(CC_C) $(CFLAGS) hello-world.c -o $(APP_NAME).o
	#readelf -h $(APP_NAME).o
