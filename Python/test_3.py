import hid

print("Checking for xArm device...")
devices = hid.enumerate(0x0483, 0x5750)

if devices:
    print("xArm device found:")
    for device in devices:
        print(f"  Path: {device['path']}")
        print(f"  Serial Number: {device['serial_number']}")
        print(f"  Manufacturer: {device['manufacturer_string']}")
        print(f"  Product: {device['product_string']}")
else:
    print("xArm device not found. Listing all HID devices:")
    all_devices = hid.enumerate()
    for device in all_devices:
        print(f"VID: 0x{device['vendor_id']:04x}, PID: 0x{device['product_id']:04x}, "
              f"Product: {device['product_string']}")