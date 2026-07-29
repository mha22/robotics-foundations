from PIL import Image

width, height = 120, 80

img = Image.new("L", (width, height), 255)  # white background
pixels = img.load()

# draw 1-pixel black border
for x in range(width):
    pixels[x, 0] = 0
    pixels[x, height - 1] = 0

for y in range(height):
    pixels[0, y] = 0
    pixels[width - 1, y] = 0

img.save("simple_room.pgm")
