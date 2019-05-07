cd ../FW
BOARD=NUCLEOF103RB make clean
BOARD=NUCLEOF103RB make
cp NUCLEOF103RB/PHiLIP.bin /media/${USER}/NODE_F103RB/
sleep 5
cd ../IF/philip_pal
python3 setup.py test
echo FINISHED
