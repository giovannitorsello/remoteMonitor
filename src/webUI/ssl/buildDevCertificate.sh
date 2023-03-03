######################
# Become a Certificate Authority
######################

# Generate private key
#openssl genrsa -des3 -out wifinetcomMicroDevCA.key 2048
# Generate root certificate
#openssl req -x509 -new -nodes -key wifinetcomMicroDevCA.key -sha256 -days 825 -out wifinetcomMicroDevCA.pem

######################
# Create CA-signed certs
######################

NAME=iot.wifinetcom.net
# Use your own domain name
# Generate a private key
#openssl genrsa -out $NAME.key 2048
# Create a certificate-signing request
# Create a config file for the extensions
openssl req -new -key $NAME.key -out $NAME.csr >$NAME.ext cat <<-EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = iot.wifinetcom.net
DNS.2 = iot.wfn.net
DNS.3 = embedded.wifinetcom.net
DNS.4 = embedded.wfn.net
DNS.5 = powerstation.wifinetcom.net
DNS.6 = powerstation.wfn.net
DNS.7 = remotemonitor.wifinetcom.net
DNS.8 = remotemonitor.wfn.net
DNS.9 = ipcontroller.wfn.net
DNS.10 = ipcontroller.wifinetcom.net
DNS.11 = domotics.wifinetcom.net
DNS.12 = domotics.wfn.net
EOF
# Create the signed certificate
openssl x509 -req -in $NAME.csr -CA wifinetcomMicroDevCA.pem -CAkey wifinetcomMicroDevCA.key -CAcreateserial -out $NAME.crt -days 825 -sha256 -extfile $NAME.ext
