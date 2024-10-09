import hid

print("Listing all HID devices:")
for device in hid.enumerate():
    print(f"Vendor ID: 0x{device['vendor_id']:04x}")
    print(f"Product ID: 0x{device['product_id']:04x}")
    #print(f"Serial Number: {device['serial_number']}")
    #print(f"Manufacturer: {device['manufacturer_string']}")
    #print(f"Product: {device['product_string']}")
    #print(f"Path: {device['path'].decode()}")
    #print("-" * 50)