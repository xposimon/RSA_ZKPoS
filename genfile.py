import os

with open("./inputdata/file.out", "wb") as f:
    f.write(os.urandom(500*1024*1024))
