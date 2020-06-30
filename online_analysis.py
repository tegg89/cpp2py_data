#! /usr/bin/env python3

import zmq
import struct

topic = "fancyhw_data".encode('ascii')

print("Reading messages with topic: {}".format(topic))

with zmq.Context() as context:
    socket = context.socket(zmq.SUB)

    socket.connect("tcp://127.0.0.1:5555")
    socket.setsockopt(zmq.SUBSCRIBE, topic)

    i = 0

    try:
        while True:
            binary_topic, data_buffer = socket.recv().split(b' ', 1)

            topic = binary_topic.decode(encoding = 'ascii')

            print("Message {:d}:".format(i))
            print("\ttopic: '{}'".format(topic))

            packet_size = len(data_buffer) // struct.calcsize("h")

            print("\tpacket size: {:d}".format(packet_size))

            struct_format = "{:d}h".format(packet_size)

            data = struct.unpack(struct_format, data_buffer)

            print("\tdata: {}".format(data))

            i += 1

    except KeyboardInterrupt:
        socket.close()
    except Exception as error:
        print("ERROR: {}".format(error))
        socket.close()
