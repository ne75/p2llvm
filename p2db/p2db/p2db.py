import argparse
import serial
import time
import logging
from colorama import Fore

from . import loggers
from . import p2tools
from . import p2db_server
from . import ui

verbose = False

log = logging.getLogger('main')
log.info("=======================")
log.info("==== Starting P2DB ====")
log.info("=======================")

def main():
    parser = argparse.ArgumentParser(description='P2DB Debugger')
    parser.add_argument('port', type=str)
    parser.add_argument('app', type=str)
    parser.add_argument('--verbose', nargs='?', const=True, default=False);

    args = parser.parse_args()

    verbose = args.verbose

    app_bin = p2tools.gen_bin(args.app)
    if not app_bin:
        return

    objdata = p2tools.get_objdump_data(args.app)

    server = p2db_server.P2DBServer(args.port);
    front_end = ui.UI(server, objdata)
    server.set_objdata(objdata)

    if not server.load_app(args.port, app_bin, verbose):
        return

    front_end.update_log("Loaded " + args.app + "\n")
    server.start()
    front_end.run()

if __name__ == "__main__":
    main()