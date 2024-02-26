from PIL import Image

def rgb_to_hex_mask(rgb):
    # Extract RGB components
    red, green, blue = rgb
    
    # Convert RGB to hexadecimal format and combine them into a single string
    hex_mask = f"0x{red:02X}{green:02X}{blue:02X}"  # Use f-string formatting
    
    return hex_mask
    
img = Image.open("somepic.jpg")

pix_val = list(img.getdata())

hex_mask = []
for i in range(len(pix_val)):
    hex_mask.append(rgb_to_hex_mask(pix_val[i]))

print(hex_mask)