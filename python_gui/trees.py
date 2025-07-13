import subprocess

def run_quadtree():
    subprocess.Popen(["QuadTree.exe", "0"], creationflags=subprocess.CREATE_NEW_CONSOLE)

def run_octree():
    subprocess.Popen(["QuadTree.exe", "1"], creationflags=subprocess.CREATE_NEW_CONSOLE)