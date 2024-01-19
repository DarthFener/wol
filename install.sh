#!/bin/bash

gcc main.c -o wol_server.elf
chmod +x wol_server.elf

sudo mv wol_server.service /etc/systemd/system/
sudo systemctl daemon-reload

sudo systemctl enable wol_server.service
sudo systemctl start wol_server.service

sudo systemctl status wol_server.service

journalctl -xeu wol_server.service

echo "Installato"
exit 0