import click
import logging
import serial
import sys
import time


LOG = logging.getLogger(__name__)


@click.command()
@click.option('-o', '--output')
@click.argument('port')
def livelogger(output, port):
    while True:
        try:
            s = serial.Serial(port, 9600)
            LOG.info('reading from port %s', port)
            break
        except serial.SerialException:
            LOG.error('failed to open port %s, retrying...', port)
            time.sleep(1)

    with open(output, 'ab', 0) if output else sys.stdout.buffer as fd:
        while True:
            fd.write(s.readline())


if __name__ == '__main__':
    logging.basicConfig(level='INFO')
    livelogger()
