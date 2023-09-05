import pygame
import constants

condition = [[[0 for i in range(12)] for j in range(12)],
             [[0 for i in range(12)] for j in range(12)]]
change_pos = [0, 0, 0, 0]
shoot = 0
hits = [0, 0]
screen = pygame.display.set_mode(constants.size)
placement = screen.copy()
screen_cond = [screen.copy(), screen.copy()]
counter = [[0, 0], [0, 0], [0, 0], [0, 0]]
accuracy = 0
all_ships = [[], []]
x = 0
y = 0
