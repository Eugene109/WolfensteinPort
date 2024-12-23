
string = """palettes:
  - name: global_palette
    fixed-entries:
      - color: {index: 0, r: 0, g: 0, b: 0}
      - color: {index: 1, r: 255, g: 255, b: 255}
    images:"""

for i in range(1,64+1):
    string += """
      - brick_wall/brick_wall-""" + str(i)+"_x_"+str(i)+".png"

string += """

converts:
  - name: sprites
    palette: global_palette
    transparent-color-index: 0
    images:
      - brick_wall.png

outputs:
  - type: c
    include-file: gfx.h
    palettes:
      - global_palette
    converts:
      - sprites

"""

print(string);