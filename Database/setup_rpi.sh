#!/bin/bash

# Load environment variables from .env file
if [ -f ../SDM120-Logger-Arduino/.env ]; then
    export $(cat ../SDM120-Logger-Arduino/.env | xargs)
else
    echo ".env file not found. Please create it and try again."
    exit 1
fi

# Check if the script is running with sudo
if [ "$EUID" -ne 0 ]; then
    echo "This script must be run as root. Re-running with sudo..."
    sudo "$0" "$@"
    exit $?
fi

# Add /boot/dietpi to PATH
export PATH=$PATH:/boot/dietpi

# Update and upgrade DietPi
echo "Updating system..."
sudo apt update && sudo apt upgrade -y && sudo apt install -y netcat-traditional

# Install InfluxDB and Grafana using dietpi-software
echo "Installing InfluxDB and Grafana..."

useradd grafana
mkdir /etc/grafana
cat <<EOL > /etc/grafana/grafana.ini
[server]
# Protocol (http, https, h2, socket)
;protocol = http

# The ip address to bind to, empty will bind to all interface
;http_addr =

# The http port to use
;http_port = 3000

# The public facing domain name used to access grafana from a browser
domain = ${GRAFANA_DOMAIN}

# Redirect to correct domain if host header does not match domain
# Prevents DNS rebinding attacks
;enforce_domain = false

# The full public facing url you use in browser, used for redirects and emails
# If you use reverse proxy and sub path specify full url (with sub path)
root_url = %(protocol)s://%(domain)s/
EOL

dietpi-software install 74  | tee -a install.log # InfluxDB (DietPi-Software ID: 74)
dietpi-software install 77 | tee -a install.log # Grafana (DietPi-Software ID: 77)

# Start and enable Grafana
echo "Starting and enabling Grafana..."
sudo systemctl enable grafana-server
sudo systemctl start grafana-server

# Start and enable InfluxDB
echo "Starting and enabling InfluxDB..."
sudo systemctl start influxdb
sudo systemctl enable influxdb

# Create database in InfluxDB for energy data
echo "Creating '${INFLUXDB_BUCKET}' database in InfluxDB..."
influx -execute "CREATE DATABASE ${INFLUXDB_BUCKET}" 2>/dev/null || echo "Failed to create database, please check InfluxDB installation."

# Create a user for InfluxDB
NEW_INFLUX_PASSWORD="${INFLUXDB_PASSWORD}" # Use the password from .env
echo "Creating InfluxDB user..."
influx -execute "CREATE USER admin WITH PASSWORD '$NEW_INFLUX_PASSWORD' WITH ALL PRIVILEGES" 2>/dev/null || echo "Failed to create InfluxDB user, please check InfluxDB installation."

# Wait for Grafana to fully start by checking if the port is open
echo "Waiting for Grafana to start..."
while ! nc -z localhost 3001; do
        echo "Waiting for Grafana to start..."
        sleep 5 # wait for 1 second before checking again
done
echo "Grafana is up and running."

# Change Grafana admin password
NEW_GRAFANA_PASSWORD="${GRAFANA_PASSWORD}" # Use the password from .env
echo "Changing Grafana admin password..."
sudo grafana-cli admin reset-admin-password $NEW_GRAFANA_PASSWORD

# Add InfluxDB as a Grafana data source using the Grafana API
echo "Configuring Grafana to use InfluxDB data source..."
GRAFANA_API_URL="http://localhost:3001/api/datasources"
GRAFANA_ADMIN_USER="admin"
GRAFANA_ADMIN_PASSWORD="$NEW_GRAFANA_PASSWORD" # Use the new password

# Data source configuration payload
read -r -d '' INFLUXDB_PAYLOAD << EOM
{
        "name": "InfluxDB",
        "type": "influxdb",
        "access": "proxy",
        "url": "${INFLUXDB_URL}",
        "database": "${INFLUXDB_BUCKET}",
        "user": "",
        "password": "",
        "jsonData": {
                "httpMode": "GET"
        }
}
EOM

# Send configuration payload to Grafana
curl -s -X POST -H "Content-Type: application/json" -d "$INFLUXDB_PAYLOAD" \
        -u $GRAFANA_ADMIN_USER:$GRAFANA_ADMIN_PASSWORD $GRAFANA_API_URL \
        || echo "Failed to add InfluxDB as Grafana data source. Please check Grafana credentials and try again."

RPI_ZERO_IP=$(hostname -I | awk '{print $1}')
echo "Installation complete. You can now access Grafana at http://$RPI_ZERO_IP:3001"
echo "Default login is admin / $NEW_GRAFANA_PASSWORD. Remember to change the password after logging in."