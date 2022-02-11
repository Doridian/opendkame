#!/usr/bin/env python
from rflib import RfCat, MOD_ASK_OOK, ChipconUsbTimeoutException, SYNCM_16_of_16, BSCFG_BS_LIMIT_12

d = None


def setup_radio():
    global d

    d = RfCat()
    d.ping()
    d.setFreq(317_940_000)
    d.setMdmModulation(MOD_ASK_OOK)
    d.setMdmDRate(2500)
    d.makePktFLEN(25)
    d.setMaxPower()
    d.setBSLimit(BSCFG_BS_LIMIT_12)
    d.setEnablePktCRC(False)
    d.setEnableMdmFEC(False)
    d.setEnablePktDataWhitening(False)
    d.setPktPQT(3)
    d.setMdmSyncMode(SYNCM_16_of_16)
    d.setMdmSyncWord(0b10101010_00000000)
    d.ping()

    d.printRadioConfig()


def get_packet():
    while True:
        try:
            data, t = d.RFrecv(1)
            return data
        except ChipconUsbTimeoutException:
            pass


def decode_packet(data_bytes):
    data = int.from_bytes(data_bytes, "big")
    data >>= 1
    out = 0
    for i in range(0, 66):
        bit = (data >> (i * 3)) & 0b111
        if bit == 0b110:
            pass
        elif bit == 0b100:
            out |= 1 << i
        else:
            raise ValueError("Invalid bit: %d" % bit)
    return out


def format_packet(pkt):
    return '{:066b}'.format(pkt)
