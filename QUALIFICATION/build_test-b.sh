cd ../FW
BOARD=BLUEPILL make clean
BOARD=BLUEPILL make
cp BLUEPILL/PHiLIP.bin /media/${USER}/${FLASH_PORT}/
sleep 5
cd ../IF/philip_pal
python3 setup.py test
echo FINISHED
