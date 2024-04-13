import serial
import time
import ctypes

class Dv3kPacket(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("start_byte", ctypes.c_ubyte),
        ("length", ctypes.c_ushort),
        ("packet_type", ctypes.c_ubyte),
        ("field_identifier", ctypes.c_ubyte)
    ]

debug = False
def parseResponse(response, remaining_data):
    packet = Dv3kPacket.from_buffer_copy(response)

    # Print parsed data
    if debug:
        print("start_byte:", hex(packet.start_byte))
        print("length:", hex(packet.length))
        print("packet_type:", hex(packet.packet_type))
        print("field_identifier:", hex(packet.field_identifier))
        print("remaining_data:", " ".join(hex(byte) for byte in remaining_data))    

    return (packet, remaining_data)

def ambeCommand(cmdarray):
    if debug: print(''.join('{:02x}'.format(x) for x in cmdarray))

    numout = ser.write(cmdarray)
    while (True): 
        if (ser.inWaiting() > 0):
            data_str = ser.readline(ser.inWaiting())
            if debug: print('Response: %s' % ''.join('{:02x}'.format(x) for x in data_str))
            remaining_data = data_str[5:]
            return parseResponse(data_str, remaining_data)

        time.sleep(0.01) 

# packet definitions
pkt_resetsoftcfg = bytearray.fromhex('61 00 07 00 34 05 00 00 07 00 10 61 00 03 00 05 10 40') # page 77
pkt_ratep =        bytearray.fromhex('61 00 0d 00 0a 01 30 07 63 40 00 00 00 00 00 00 48') # SPEECH RATE 2400bps FEC 1200bps RCW 0 0x0130 RCW 1 0x0763 RCW 2 0x4000 RCW 3 0x0000 RCW 4 0x0000 RCW 5 0x0048 (PAGE 95)
pkt_ready =        bytearray.fromhex('61 00 01 00 39') # ready packet as the resetsoftcfg doesn't return a specific response, just a ready
pkt_prodid =       bytearray.fromhex('61 00 01 00 30') # page 65
pkt_verstring =    bytearray.fromhex('61 00 01 00 31') # page 65

# define the serial port
ser = serial.Serial('/dev/ttyUSB0', 460800)

# perform a soft reset
(response, remaining_data) = ambeCommand(pkt_resetsoftcfg)
if response.field_identifier != 0x39:   # look for ready packet
    raise ValueError("AMBE Dongle did not return a ready packet.")

(response, remaining_data) = ambeCommand(pkt_verstring)
version_string = remaining_data.decode()

(response, remaining_data) = ambeCommand(pkt_prodid)
product_id_string = remaining_data.decode()

(response, remaining_data) = ambeCommand(pkt_ratep)

print(f"Version: {version_string}")
print(f"Product: {product_id_string}")
