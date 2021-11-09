import argparse
import serial

import screen
import p2tools

verbose = False

    
def main():
    parser = argparse.ArgumentParser(description='P2DB Debugger')
    parser.add_argument('port', type=str)
    parser.add_argument('app', type=str)
    parser.add_argument('--verbose', nargs='?', const=True, default=False);

    args = parser.parse_args()

    verbose = args.verbose

    ser = serial.Serial()
    ser.baudrate = 3000000
    ser.timeout = 0.1
    ser.port = args.port
    ser.open()

    app_bin = p2tools.gen_bin(args.app)

    if not app_bin:
        return

    objdata = p2tools.get_objdump_data(args.app)

    if not p2tools.load(args.port, app_bin, ser.baudrate, verbose):
        return

    main_screen = screen.Screen(ser, objdata)
    main_screen.run()

if __name__ == "__main__":
    main()