### p2p-host

Handle WiFi P2P connections via wpa_supplicant

####Build:

We depend on libwpa_client.a in wpa_supplicant release.
Download and unpack the lastest release, last seen at:

  http://w1.fi/releases/wpa_supplicant-2.5.tar.gz

```
$ make
```

####Usage:

This assumes a wpa_supplicant instance configured for P2P, e.g.

```
ctrl_interface=/run/wpa_supplicant
ap_scan=1
device_name=mwifiex-p2p-device
device_type=1-0050F204-1
p2p_listen_reg_class=81
p2p_listen_channel=1
p2p_oper_reg_class=81
p2p_oper_channel=1
p2p_no_group_iface=1
p2p_go_intent=15
```

Listen for 75ms every 1.5s. Accept 11111111 as PIN code from connecting device:

```
./p2phostd /run/wpa_supplicant/p2p0 75 1500 11111111
```
