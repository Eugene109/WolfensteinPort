from PIL import Image
import numpy as np

palette_file = open("src/gfx/global_palette.c", "r")
palette_lines = palette_file.read().split('\n')
palette = dict()
index = 0

for i in range(2, len(palette_lines)- 2):
    current_line = palette_lines[i]

    r = int(current_line.split("rgb(")[1].split(", ")[0])
    g = int(current_line.split("rgb(")[1].split(", ")[1])
    b = int(current_line.split("rgb(")[1].split(", ")[2].split(")")[0])
    palette[index] = (r,g,b)
    index += 1

# print(palette)

size = 2;
for i in range (1, 64+1):
    size += i*i

string = """
unsigned char brick_wall_arr_data["""+str(size)+"""] =
{
    """

for i in range(1, 64+1):
    im_frame = Image.open('src/gfx/brick_wall/brick_wall-'+str(i)+'_x_'+str(i)+'.png')
    np_frame = np.array(im_frame.getdata())
    # palette_indices = []
    for j in range(0, i*i):
        # np_frame[j] is pixel in form [r,g,b,a]
        # must now search through the palette for best matching palette entry
        r = np_frame[j][0]
        g = np_frame[j][1]
        b = np_frame[j][2]
        offset = 100
        current_palette_index = 0
        for c in range(0, len(palette)):
            new_offset = abs(palette[c][0] - r)+abs(palette[c][1] - g)+abs(palette[c][2]-b)
            if(new_offset < offset):
                current_palette_index = c
                offset = new_offset
        
        # palette_indices.append(current_palette_index)
        if(j % 15 == 0):
            string += "\n    "
        string += (hex(current_palette_index)+",")
    print(i)

string += """
};
"""

out = open("src/gfx/brick_wall.c", "w")
out.write(string)

print("\n--------------------------------\n")
print(string)