#!/usr/bin/env python

import serial, time, sys
 
if __name__ == '__main__':
	if len(sys.argv) < 3:
		print 'Usage: %s [COMn] [BAUD]' % sys.argv[0]
		sys.exit(1)

	s = serial.Serial(sys.argv[1], int(sys.argv[2]))
	s.setDTR(True)
	time.sleep(1)
	s.setDTR(False)

