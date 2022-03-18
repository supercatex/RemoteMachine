import serial
from datetime import datetime

now = datetime.today().strftime("%Y%m%d_%H%M%S")
fname = "GPS_%s.txt" % now

gps = serial.Serial(
    "/dev/ttyUSB0",
    timeout=None,
    baudrate=4800,
    xonxoff=False,
    rtscts=False,
    dsrdtr=False
)

records = []
while True:
    line = gps.readline()
    s = line.decode('utf-8')
    a = s.split(",")
    if "GPRMC" in a[0]:
        print(a)
        isValid = a[2] == 'A'
        if isValid:
            dateUTC, timeUTC = a[9], float(a[1])
            x, y = float(a[3]), float(a[5])
            NS, EW = a[4], a[6]
            speed = float(a[7])
            heading = float(a[8])
            records.append(['A', dateUTC, timeUTC, x, y, NS, EW, speed, heading])
        else:
            records.append(['V', datetime.today().strftime("%Y-%m-%d_%H:%M:%S")])

    if len(records) > 5:
        f = open(fname, "a+")
        for r in records:
            f.write(",".join(r))
            f.write("\n")
        f.close()
        print("saved.")
        records.clear()
