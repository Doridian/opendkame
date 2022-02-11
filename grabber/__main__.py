import RPi.GPIO as GPIO
from time import sleep
from rfstuff import get_packet, setup_radio, decode_packet, format_packet
from signal import signal, SIGUSR1

REPEATS_NEEDED = 2
SIGNAL_DONE_REPEATS_NEEDED = 10
RELAY_PIN = 7  # GPIO 4
VALID_SIGNAL_SETS = set([pow(2, 16), pow(2, 17), pow(2, 18)])
GPIO.setmode(GPIO.BOARD)

signal_sequence = None
signal_set = None


def load_data():
    global signal_sequence
    global signal_set

    fh = open("data.txt", "r")
    signal_sequence = [int(x.strip()) for x in fh.readlines() if x.strip()]
    fh.close()
    signal_set = set(signal_sequence)


load_data()

data_file = open("data.txt", "a")


def append_signal(pkt):
    is_repeat = pkt in signal_set
    signal_sequence.append(pkt)
    signal_set.add(pkt)
    data_file.write("%d\n" % pkt)
    data_file.flush()
    return is_repeat


GPIO.setup(RELAY_PIN, GPIO.OUT)


def set_relay(on):
    GPIO.output(RELAY_PIN, on)


def grab_signal():
    set_relay(True)

    repeats = 0
    previous_data = None
    while repeats < REPEATS_NEEDED:
        data = get_packet()
        if data == previous_data:
            repeats += 1
        else:
            repeats = 0
            previous_data = data

    set_relay(False)

    return decode_packet(previous_data)


setup_radio()
set_relay(False)
sleep(1)

should_run = True


def sigusr1_handler(signal, frame):
    global should_run
    should_run = False


signal(SIGUSR1, sigusr1_handler)

signal_done_repeats = 0
while should_run and ((signal_done_repeats < SIGNAL_DONE_REPEATS_NEEDED) or (len(signal_set) not in VALID_SIGNAL_SETS)):
    sleep(0.1)
    pkt = grab_signal()
    print("Probe %d: %s" % (len(signal_sequence), format_packet(pkt)))
    if append_signal(pkt):
        print("Repeat!")
        signal_done_repeats += 1
    else:
        signal_done_repeats = 0

GPIO.cleanup()
