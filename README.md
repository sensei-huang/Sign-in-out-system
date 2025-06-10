# Sign-in-out-system
Sign-in-out system code

Openssl command for generating certificate and key:
```cmd
openssl req -x509 -newkey rsa:1024 -sha256 -keyout key.txt -out cert.txt -days 3650 -nodes -subj "/C=NZ/ST=Otago/L=Dunedin/O=Logan Park High School/OU=Samuel Huang/CN=info.local" -addext subjectAltName=DNS:info.local
```
