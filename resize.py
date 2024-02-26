from PIL import Image

base_width= 32
img = Image.open('digitnumbering1-300x300.jpg')
wpercent = (base_width / float(img.size[0]))
hsize = int((float(img.size[1]) * float(wpercent)))
img = img.resize((base_width, hsize), Image.Resampling.LANCZOS)
img.save('somepic.jpg')