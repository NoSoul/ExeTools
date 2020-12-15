#! /bin/sh
pciSlot=$(lspci  | grep Wireless | awk '{print $1}')
echo 1 > /sys/devices/pci0000:00/0000:$pciSlot/remove
echo 1 > /sys/bus/pci/rescan
