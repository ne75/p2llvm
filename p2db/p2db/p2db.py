import argparse
import serial
import time
import logging
from colorama import Fore

from . import screen
from . import p2tools
from . import p2db_server

verbose = False

def main():
    log = logging.getLogger('main')
    log.info("=======================")
    log.info("==== Starting P2DB ====")
    log.info("=======================")

    parser = argparse.ArgumentParser(description='P2DB Debugger')
    parser.add_argument('port', type=str)
    parser.add_argument('app', type=str)
    parser.add_argument('--verbose', nargs='?', const=True, default=False);

    args = parser.parse_args()

    verbose = args.verbose

    server = p2db_server.P2DBServer('');
    server.start()

    # app_bin = p2tools.gen_bin(args.app)

    # if not app_bin:
    #     return

    # objdata = p2tools.get_objdump_data(args.app)

    if not p2tools.load(args.port, args.app, server.ser.baudrate, verbose):
        return

    # open("p2db.log", 'w').close()

    # main_screen = screen.Screen(ser, objdata)
    # main_screen.run()
    while(1):
        time.sleep(1)

if __name__ == "__main__":
    main()