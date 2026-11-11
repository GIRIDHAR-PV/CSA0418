
echo "===== Q6: DISK MANAGEMENT ====="

echo
echo "===== BLOCK DEVICES ====="
lsblk

echo
echo "===== PARTITION INFORMATION ====="
fdisk -l 2>&1 | head -n 25

echo
echo "===== SWAP INFORMATION ====="
swapon --show 2>&1

echo
echo "===== MEMORY INFORMATION ====="
free -h

echo
echo "===== SWAP FILE INFORMATION ====="
cat /proc/swaps

echo
echo "===== DISK READ SPEED TEST ====="
if command -v hdparm >/dev/null 2>&1; then
    echo "hdparm is installed."
    echo "Disk device details:"
    lsblk -o NAME,TYPE,SIZE,MODEL
else
    echo "hdparm is not installed in this online environment."
    echo "Disk speed test must be performed on a Linux computer."
fi

echo
echo "===== RAID CAPACITY CALCULATIONS ====="
echo "Number of disks = 6"
echo "Each disk = 4 TB"
echo "Total raw capacity = 24 TB"
echo
echo "RAID 0  = 24 TB (No redundancy)"
echo "RAID 1  = 12 TB (Mirroring)"
echo "RAID 5  = 20 TB (One disk redundancy)"
echo "RAID 10 = 12 TB (Mirroring and striping)"

echo
echo "===== RAID DATA LOSS PROBABILITY ====="
echo "Let p = probability of one disk failing during rebuild."
echo "RAID 1 = approximately p"
echo "RAID 5 = 1 - (1-p)^5 approximately 5p"
echo "RAID 6 = C(5,2)p^2 approximately 10p^2"

echo
echo "===== Q6 TESTS COMPLETED ====="
