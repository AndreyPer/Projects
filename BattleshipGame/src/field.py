from geometry import Geometry
from constants import *
import globals
import pygame

font_size = int(block_size / 1.5)
nice_col = (202, 203, 122)
rose = (190, 38, 76)
dist = 15
angle = 5
delta = 4
num = 10
length = 25
thickness = 4


class Field:
    def rects(self):
        x1 = left_margin - 1
        x2 = num * block_size + delta
        y1 = upper_margin - 1
        y2 = num * block_size + delta
        Geometry().fill(0, 0, 0)
        Geometry().rectangle(rose, x1, y1, x2, y2, 0, angle)
        Geometry().rectangle(rose, x1 + block_size * dist, y1, x2, y2, 0, angle)
        Geometry().rectangle(nice_col, x1, y1, x2, y2, thickness, angle)
        Geometry().rectangle(nice_col, x1 + block_size * dist, y1, x2, y2,
                             thickness, angle)

    def grids(self):
        Field().rects()
        for i in range(num + 1):
            if i != 0 and i != num:
                y1 = upper_margin
                y2h = upper_margin + i * block_size
                y2v = upper_margin + num * block_size
                # Hor grid1
                x1 = left_margin
                x2 = left_margin + num * block_size
                Geometry().line(nice_col, x1, y1 + i * block_size, x2, y2h,
                                thickness)
                # Vert grid1
                x1 = left_margin + i * block_size
                x2 = left_margin + i * block_size
                Geometry().line(nice_col, x1, y1, x2, y2v, thickness)
                # Hor grid2
                x1 = left_margin + dist * block_size
                x2 = left_margin + length * block_size
                Geometry().line(nice_col, x1, y1 + i * block_size, x2, y2h,
                                thickness)
                # Vert grid2
                x1 = left_margin + (i + dist) * block_size
                x2 = left_margin + (i + dist) * block_size
                Geometry().line(nice_col, x1, y1, x2, y2v, thickness)
                Geometry().update()

    def two_fields(self):
        Field().grids()
        for i in range(num + 1):
            if i < num:
                num_ver = pygame.font.SysFont('notosans',
                                              font_size).render(str(i + 1),
                                                                True,
                                                                (221, 160, 221))
                letters_hor = pygame.font.SysFont('notosans', font_size).render(
                    letters[i], True, (221, 160, 221))
                x1v = pygame.font.SysFont('notosans', font_size).render(
                    str(i + 1), True, (221, 160, 221))
                x1h = pygame.font.SysFont('notosans', font_size).render(
                    letters[i], True, (221, 160, 221))

                self.draw_lines(num_ver, letters_hor, x1v, x1h, i)

        Geometry().update()
        globals.placement = globals.screen.copy()
        globals.screen_cond = [globals.screen.copy(), globals.screen.copy()]

    def draw_lines(self, num_ver, letters_hor, x1v, x1h, i):
        num_ver_width = num_ver.get_width()
        num_ver_height = num_ver.get_height()
        letters_hor_width = letters_hor.get_width()
        letters_hor_height = letters_hor.get_height()
        new_delta = 10
        dist_v = upper_margin + i * block_size + (
                block_size // 2 - num_ver_height // 2)
        dist_h = upper_margin - letters_hor_height - new_delta

        y1 = left_margin - (block_size // 2 + num_ver_width // 2)
        Geometry().building(x1v, y1, dist_v)
        y1 = left_margin + i * block_size + (
                block_size // 2 - letters_hor_width // 2)
        Geometry().building(x1h, y1, dist_h)
        y1 = left_margin - (block_size // 2 + num_ver_width // 2) + (
                dist * block_size)
        Geometry().building(x1v, y1, dist_v)
        y1 = left_margin + i * block_size + (
                block_size // 2 - letters_hor_width // 2) + (
                     dist * block_size)
        Geometry().building(x1h, y1, dist_h)
