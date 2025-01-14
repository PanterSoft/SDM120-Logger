#!/bin/bash

# Load environment variables from .env file
if [ -f ../SDM120-Logger-Arduino/.env ]; then
    export $(cat ../SDM120-Logger-Arduino/.env | xargs)
else
    echo ".env file not found. Please create it and try again."
    exit 1
fi

# Update and install necessary packages
sudo apt update
sudo apt install -y hostapd dnsmasq

# Stop services while configuring
sudo systemctl stop hostapd
sudo systemctl stop dnsmasq

# Configure static IP for wlan0 using systemd-networkd
cat <<EOT | sudo tee /etc/systemd/network/10-wlan0.network
[Match]
Name=wlan0

[Network]
Address=192.168.4.1/24
EOT

# Restart systemd-networkd to apply the static IP
sudo systemctl restart systemd-networkd

# Configure DHCP server (dnsmasq) for the AP
cat <<EOT | sudo tee /etc/dnsmasq.conf
interface=wlan0      # Use interface wlan0
dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h
EOT

# Configure hostapd for AP mode
cat <<EOT | sudo tee /etc/hostapd/hostapd.conf
interface=wlan0
driver=nl80211
ssid=${WIFI_SSID}
hw_mode=g
channel=6
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=${WIFI_PASSWORD}
wpa_key_mgmt=WPA-PSK
rsn_pairwise=CCMP
EOT

# Secure hostapd configuration file
sudo chown root:root /etc/hostapd/hostapd.conf
sudo chmod 600 /etc/hostapd/hostapd.conf

# Point hostapd to the configuration file
sudo sed -i 's|#DAEMON_CONF=""|DAEMON_CONF="/etc/hostapd/hostapd.conf"|' /etc/default/hostapd

# Enable and start services
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl enable systemd-networkd
sudo systemctl start hostapd
sudo systemctl start dnsmasq

echo "Access Point setup complete. SSID: '${WIFI_SSID}' (default password: '${WIFI_PASSWORD}')"
