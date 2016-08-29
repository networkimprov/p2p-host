
all: libwpa-latest libwpa-latest/wpa_supplicant/libwpa_client.a main.o
	gcc -o p2phostd main.o libwpa-latest/wpa_supplicant/libwpa_client.a

libwpa-latest:
	test -L libwpa-latest || ln -s wpa_supplicant-*.* libwpa-latest

libwpa-latest/wpa_supplicant/.config: wpa_supplicant.config
	cp wpa_supplicant.config libwpa-latest/wpa_supplicant/.config

libwpa-latest/wpa_supplicant/libwpa_client.a: libwpa-latest/wpa_supplicant/.config
	cd libwpa-latest/wpa_supplicant && make libwpa_client.a

main.o: main.c
	gcc -Wall -I libwpa-latest/src/common -c main.c

