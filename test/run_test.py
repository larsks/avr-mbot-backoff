import click
import sys
import time

from pysimavr.avr import Avr
from pysimavr.firmware import Firmware

# These are going to be specific to the atmega328p
GPIOR0 = 0x3E
GPIOR1 = 0x4A


@click.command()
@click.option('-f', '--f_cpu', default=16000000)
@click.option('-m', '--mcu', default='atmega328p')
@click.option('-t', '--timeout', default=10)
@click.argument('testfile')
def run_test(f_cpu, mcu, timeout, testfile):
    avr = Avr(mcu=mcu, f_cpu=f_cpu)
    fw = Firmware(testfile)
    avr.load_firmware(fw)

    t_start = time.time()
    while avr.peek(GPIOR0) != 255:
        if time.time() - t_start > timeout:
            raise click.ClickException('Timeout')
        avr.step(100)

    if avr.uart.buffer:
        print(''.join(avr.uart.buffer))
    sys.exit(0 if avr.peek(GPIOR1) == 255 else 1)


if __name__ == '__main__':
    run_test()
