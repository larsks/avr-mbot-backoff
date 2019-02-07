import click
import sys
import time

from pysimavr.avr import Avr
from pysimavr.firmware import Firmware
from pysimavr.swig.simavr import avr_gdb_init, cpu_Stopped

# These are going to be specific to the atmega328p
GPIOR0 = 0x3E
GPIOR1 = 0x4A


@click.command()
@click.option('-f', '--f_cpu', default=16000000)
@click.option('-m', '--mcu', default='atmega328p')
@click.option('-t', '--timeout', default=10)
@click.option('-g', '--gdb', is_flag=True)
@click.argument('testfile')
def run_test(f_cpu, mcu, timeout, gdb, testfile):
    avr = Avr(mcu=mcu, f_cpu=f_cpu)
    fw = Firmware(testfile)
    avr.load_firmware(fw)

    if gdb:
        avr.gdb_port = 1234
        avr_gdb_init(avr.backend)
        avr.state = cpu_Stopped

    t_start = time.time()
    while avr.peek(GPIOR0) != 255:
        if timeout and time.time() - t_start > timeout:
            raise click.ClickException('Timeout')
        avr.step(100)

    if avr.uart.buffer:
        print(''.join(avr.uart.buffer))

    sys.exit(0 if avr.peek(GPIOR1) == 255 else 1)


if __name__ == '__main__':
    run_test()
