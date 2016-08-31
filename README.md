### p2p-host

Handle WiFi P2P connections via wpa_supplicant

####Build:

We depend on libwpa_client.a in wpa_supplicant tree.

```
$ rm -r wpa_supplicant-2.x # if previous version installed
$ curl -sLO http://w1.fi/releases/wpa_supplicant-2.5.tar.gz # or later release
$ tar -xf wpa_supplicant-2.5.tar.gz
$ rm wpa_supplicant-2.5.tar.gz

$ make
```

####Usage:

Start a wpa_supplicant instance configured for your host's P2P interface, e.g.

```
$ sudo cp p2p0.conf /etc/wpa_supplicant/
$ sudo wpa_supplicant -i p2p0 -D nl80211 -c /etc/wpa_supplicant/p2p0.conf
```

Listen for 75ms every 1.5s. Accept 11111111 as PIN code from connecting device:

```
$ ./p2phostd /run/wpa_supplicant/p2p0 75 1500 11111111
```
