'''
Setup Basic Logging for Demo

Most of the time, you will want to use the default logging configuration.
This will set up a simple logger that outputs to the console and a file 'logs/main.log'.

Example from https://docs.python.org/3/howto/logging-cookbook.html
'''
import logging

main_logger = logging.getLogger('main')
main_logger.setLevel(logging.DEBUG)

# create file handler which logs even debug messages
fh = logging.FileHandler('p2db.log')
fh.setLevel(logging.DEBUG)

# create console handler with a higher log level
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)

# create formatter and add it to the handlers

formatter = logging.Formatter('%(asctime)s - [%(filename)s:%(lineno)d] - %(levelname)s - %(message)s')
ch.setFormatter(formatter)
fh.setFormatter(formatter)

# add the handlers to logger
# main_logger.addHandler(ch)
main_logger.addHandler(fh)