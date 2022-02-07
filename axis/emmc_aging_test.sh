## Please change SLEEP_TIME if you want to change the time between each test ##
## Please change BLOCK_SIZE and BLOCK_COUNT if you want to change the file size ##
SLEEP_TIME=2
BLOCK_SIZE=4194304
BLOCK_COUNT=256
SRC_FILE=src_file.dat
DST_FILE=dst_file.dat
ITER=0
echo "########### SLEEP_TIME \$SLEEP_TIME ##########"
while [ 1 ]; do
echo "########### \$ITER Test ##########"
echo 1 > /proc/sys/vm/drop_caches
dd if=/dev/urandom of=\$SRC_FILE bs=\$BLOCK_SIZE count=\$BLOCK_COUNT
RETURN=\$?
sync
if [ \$RETURN -eq 0 ];then
echo "Write Test Success"
else
echo "Write Test Fail"
exit \$RETURN
fi
echo 1 > /proc/sys/vm/drop_caches
dd if=\$SRC_FILE of=\$DST_FILE bs=\$BLOCK_SIZE count=\$BLOCK_COUNT
RETURN=\$?
sync
if [ \$RETURN -eq 0 ];then
echo "Read Test Success"
else
echo "Read Test Fail"
exit \$RETURN
fi
busybox diff \$SRC_FILE \$DST_FILE
RETURN=\$?
if [ \$RETURN -eq 0 ];then
echo "Data comparing success"
else
echo "Data Comparing fail"
exit \$RETURN
fi
rm -rf \$SRC_FILE \$DST_FILE
sync
sleep \$SLEEP_TIME
ITER=\$((\$ITER+1))
if [ \$ITER -eq 1000 ];then
echo "Aging Test Success"
exit 0
fi
done
EOF

