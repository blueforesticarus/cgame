import _cgame as cg
from time import sleep
from ctypes import *

UP = 0
RIGHT = 1
DOWN = 2
LEFT = 3

mobpos_loc = cg.get_mob_vec()
sleep(0.25)
while True:
    cg.move_entity(mobpos_loc, UP)
    sleep(0.125)
    cg.move_entity(mobpos_loc, UP)
    sleep(0.125)
    cg.move_entity(mobpos_loc, RIGHT)
    sleep(0.125)
    cg.move_entity(mobpos_loc, RIGHT)
    sleep(0.125)
    cg.move_entity(mobpos_loc, DOWN)
    sleep(0.125)
    cg.move_entity(mobpos_loc, DOWN)
    sleep(0.125)
    cg.move_entity(mobpos_loc, LEFT)
    sleep(0.125) 
    cg.move_entity(mobpos_loc, LEFT)
    sleep(0.125)
